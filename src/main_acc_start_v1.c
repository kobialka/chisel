/* main_clk_start_v3.c */

/*	autor:		Michal Kobialka, mmkobialka@gmail.com
 * 	data: 		05.01.2015
 *
 *	Program napisany w celu zapoznania sie z obsluga przerwan i NVIC. W procedurze obslugi przerwania
 *	EXTI0 zmieniane jest zrodlo sygnaly SYSCLK zamiennie z HSE (8MHz) na PLL (24MHz). Wartosci sa wyswietlane
 *	na LCD oraz zapalane sa odpowiednie diody: HSE - niebieska, PLL - zielona.
 *
 *	Program odczytuje dane z akcelerometru umieszczonego na PCB
 *
 *
 */


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "bsp/stm32f4_discovery.h"
#include "bsp/stm32f4_discovery_accelerometer.h"
#include "lcd/lib_S1D15705_m.h"

#include <stdint.h>


/* Defines	------------------------------------------------------------------*/

#define CYCLES 	(500000UL);


//___________________________________
//	DEKLARACJE FUNKCJI
static void SystemClock_Config(void);
static void LED_StartSignal(void);
static void Error_Handler(void);
static void EXTILine0_Config(void);
static void LED_StartSignal(void);
static void UART_init(uint32_t BaudRate);
//static void ACC_Init(void);

static void GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin		= GPIO_PIN_7;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	GPIO_InitStruct.Speed	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
}

//___________________________________
int main(void){
	int16_t	AccelerationMeas[]={0,0,0};


	/*	1. HAL_Init();
		2. HAL_RCC_OscConfig();
		3. HAL_RCC_ClockConfig();
		4. Add HAL_IncTick() to SysTick_Handler() ISR to enable polling process when using HAL_Delay(); HAL_NVIC_SetPriority().
		5. HW initialization: HAL_ppp_Init();
		6. HW low level initialization: HAL_ppp_MspInit()
		7. User application code.
	*/

	//___________________________________
	HAL_Init();
	SystemClock_Config();


	//___________________________________
	// Wyprowadzenie sygnalow zegarowych na piny zewnetrzne.
	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK,RCC_MCODIV_1);
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI,RCC_MCODIV_1);


	//___________________________________
	BSP_LED_Init(0);
	BSP_LED_Init(1);
	BSP_LED_Init(2);
	BSP_LED_Init(3);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	BSP_ACCELERO_Init();
	LCD_Init();
	GPIO_Init();
	UART_Init(9600);

	/* Configure EXTI Line0 (connected to PA0 pin) in interrupt mode */
	EXTILine0_Config();

	//___________________________________
	LED_StartSignal();
	LCD_BUFF_Wrs(0,0,"ACC: 3-axis measurement");
	LCD_BUFF_Wrs(84,3,"CMSIS 4.2");
	LCD_BUFF_Wrs(0,7,"SYSCLK:");
	LCD_BUFF_Wrs(18*CHAR_WIDTH,7,"MHz");
	SystemCoreClockUpdate();
	LCD_BUFF_Wrv_U32Dec(CHAR_WIDTH*8,7,SystemCoreClock);
	LCD_Update();



	//___________________________________
	while(1){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_SET);
		BSP_ACCELERO_GetXYZ(AccelerationMeas);
		LCD_BUFF_Wrv_S16Dec(0,3,AccelerationMeas[0]);
		LCD_BUFF_Wrv_S16Dec(0,4,AccelerationMeas[1]);
		LCD_BUFF_Wrv_S16Dec(0,5,AccelerationMeas[2]);
		LCD_Update();
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_RESET);
	}
}





void UART_init(uint32_t BaudRate){
	UART_HandleTypeDef UART_MyHandle;
	UART_InitTypeDef UART_InitStructure;

	UART_InitStructure.BaudRate 	= BaudRate;					// Zobaczyć czy nie trzeba tego przeliczyć
	UART_InitStructure.HwFlowCtl	= UART_HWCONTROL_NONE;
	UART_InitStructure.Mode			= UART_MODE_TX_RX;
	UART_InitStructure.OverSampling	= UART_OVERSAMPLING_8;
	UART_InitStructure.Parity		= UART_PARITY_NONE;
	UART_InitStructure.StopBits		= UART_STOPBITS_1;
	UART_InitStructure.WordLength	= UART_WORDLENGTH_8B;

	HAL_UART_Init()


}
// HAL_UART_IRQHandler()
// HAL_UART_TxCpltCallback(),
// HAL_UART_RxCpltCallback()

//___________________________________
static void EXTILine0_Config(void){
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  __GPIOA_CLK_ENABLE();

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_0){
	  RCC_ClkInitTypeDef RCC_ClkInitStruct;
	  if(__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSE){    	// Czy zrodlem SYSCLK jest HSE?
		  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;		// Jezeli zrodlem SYSCLK jest HSE to zmien na PLL.
		  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
			  /* Initialization Error */
			  Error_Handler();
		  }
		  BSP_LED_Off(BLUE);
		  BSP_LED_On(ORANGE);
	  }
	  else{													// Jezeli zrodlem SYSCLK jest PLL to zmien na HSE.
		  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;		// Jezeli zrodlem SYSCLK jest HSE to zmien na PLL.
		  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
		  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
			  /* Initialization Error */
			  Error_Handler();
		  }
		  BSP_LED_On(BLUE);
		  BSP_LED_Off(ORANGE);
	  }
	  SystemCoreClockUpdate();
	  LCD_BUFF_Wrv_U32Dec(CHAR_WIDTH*8,7,SystemCoreClock);
	  LCD_Update();
  }
}


//___________________________________
void SystemClock_Config(void){
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;


	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;			// HSE 8MHz.
	RCC_OscInitStruct.PLL.PLLM = 4;									// Wejscie VCO ma byc 1-2 MHz. 8/4=2MHz.
	RCC_OscInitStruct.PLL.PLLN = 96;								// wyjsie VCO ma byc 192-432 MHz. 2*96 = 192MHz.
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;						// Dzielki wyjscia PLL 192/8=24.
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;					// wlaczyc PLL.
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}


	RCC_ClkInitStruct.ClockType 		= RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV1;
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
		Error_Handler();
	}
}




//___________________________________
void LED_StartSignal(void)
{

	uint32_t TempVar1;

	BSP_LED_On(GREEN);
	TempVar1 = CYCLES;
	while(--TempVar1);
	BSP_LED_Off(GREEN);

	BSP_LED_On(ORANGE);
	TempVar1 = CYCLES;
	while(--TempVar1);
	BSP_LED_Off(ORANGE);

	BSP_LED_On(RED);
	TempVar1 = CYCLES;
	while(--TempVar1);
	BSP_LED_Off(RED);

	BSP_LED_On(BLUE);
	TempVar1 = CYCLES;
	while(--TempVar1);
	BSP_LED_Off(BLUE);

	BSP_LED_On(GREEN);
	TempVar1 = CYCLES;
	while(--TempVar1);
	BSP_LED_Off(GREEN);
}





//___________________________________
static void Error_Handler(void)
{
  /* Turn LED5 (RED) on */
	BSP_LED_On(RED);
	while(1){
	}
}


