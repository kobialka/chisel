/* main_zadajnik.c */

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
#include "string.h"


/* Defines	------------------------------------------------------------------*/
#define CYCLES 	(500000UL)
#define TX_BUFF_SIZE 8
#define RX_BUFF_SIZE 8

//___________________________________
// ZMIENNE GLOBALNE:
volatile uint8_t pucTxBuffer[10];
volatile uint8_t pucRxBuffer[10] ={""};

UART_HandleTypeDef huart4;
HAL_StatusTypeDef HAL_Status;
uint16_t u16Counter=0;


//___________________________________
static void SystemClock_Config(void);
static void LED_StartSignal(void);
static void Error_Handler(void);
static void EXTILine0_Config(void);
static void LED_StartSignal(void);
static void ACC_Init(void);
static void GPIO_Init(void);
static void UART_InitWithIT(uint32_t MyBaudRate);

//___________________________________
int main(void){

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
	//HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK,RCC_MCODIV_1);
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI,RCC_MCODIV_1);


	//___________________________________
	BSP_LED_Init(BLUE);
	BSP_LED_Init(ORANGE);
	BSP_LED_Init(GREEN);
	BSP_LED_Init(RED);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	//BSP_ACCELERO_Init();
	LCD_Init();
	GPIO_Init();
	EXTILine0_Config();							/* Configure EXTI Line0 (connected to PA0 pin) in interrupt mode */
	UART_InitWithIT(9600);



	//___________________________________
	LED_StartSignal();
//	LCD_BUFF_Wrs(0,0,"Zadajnik: 3-axis ac");
//	LCD_BUFF_Wrs(84,3,"CMSIS 4.2");
//	LCD_BUFF_Wrs(0,7,"SYSCLK:");
//	LCD_BUFF_Wrs(18*CHAR_WIDTH,7,"MHz");
//	SystemCoreClockUpdate();
//	LCD_BUFF_Wrv_U32Dec(CHAR_WIDTH*8,7,SystemCoreClock);
//	LCD_Update();
	//__enable_irq();

	BSP_LED_On(GREEN);
//	HAL_UART_Receive_IT(&huart4,huart4.pRxBuffPtr,1);
	HAL_UART_Receive_IT(&huart4,pucRxBuffer,1);

	//___________________________________
	while(1){
//		HAL_UART_Transmit_IT(&huart4,huart4.pRxBuffPtr,1);
		LCD_BUFF_Wrv_U16Dec(0,0,u16Counter);
//		LCD_Update();
//		HAL_Status = HAL_UART_Receive_IT(&huart4, huart4.pRxBuffPtr,RX_BUFF_SIZE);
//		HAL_Status = HAL_UART_Receive(&huart4,huart4.pRxBuffPtr,1,10000);

//		HAL_Status = HAL_UART_Transmit(&huart4, huart4.pRxBuffPtr,RX_BUFF_SIZE,10000);
	}
}

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
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}




static void GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin		= GPIO_PIN_7;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	GPIO_InitStruct.Speed	= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
}


static void UART_InitWithIT(uint32_t MyBaudRate){
	huart4.Instance = UART4;
	huart4.Init.BaudRate = MyBaudRate;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;

	huart4.pRxBuffPtr = pucRxBuffer;
	huart4.pTxBuffPtr = pucTxBuffer;
	huart4.RxXferSize = RX_BUFF_SIZE;
	huart4.TxXferSize = TX_BUFF_SIZE;
	HAL_UART_Init(&huart4);
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


