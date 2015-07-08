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
#include <stdint.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "lib_LIS3DSH.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"
#include "lib_S1D15705_m.h"
#include "string.h"
#include "command_decoder.h"
#include "uart.h"

/* Defines	------------------------------------------------------------------*/
#define ACC_XYZ_BUFF_SIZE 3


//___________________________________
// ZMIENNE GLOBALNE:
extern	tToken						asToken[MAX_TOKEN_NR];	// tablica tokenów.
extern unsigned char 				ucTokenNr;		// ilosc nalezionych tokenow.

uint8_t 							Hal_RxBuff[1];
uint8_t								Hal_TxBuff[1];
uint8_t								pcMessageBuffer[UART_RECIEVER_SIZE];
volatile int16_t					pACC_XYZ_BUFF[ACC_XYZ_BUFF_SIZE];
tToken 								*psToken = asToken;

UART_HandleTypeDef					huart4;
TIM_HandleTypeDef					hTimer6;
HAL_StatusTypeDef 					HAL_Status;

uint8_t						fCalc = 0, fId = 0, fOk = 0, fUnknownCommand = 0,fProvideData = 0, fAccGotXYZ = 0;



//___________________________________
static void Error_Handler(void);

static void SystemClock_Config(void);
static void LED_Init(void);
static void LED_StartSignal(void);
static void ACC_Init(void);
static void TIMER6_Base_Init(void);
static void SendPendingString(void);
static void ExecuteCommand(void);




// =======================================================================================================
int main(void){

	//___________________________________
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();

	//___________________________________
	LCD_Init();
	LCD_BUFF_Wrv_U32Dec(CHAR_WIDTH*8,7,SystemCoreClock);
	LCD_Update();
	LED_Init();
	ACC_Init();
	UART_InitWithInt(9600);
	TIMER6_Base_Init();
//	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
	//___________________________________
	LED_StartSignal();
	BSP_LED_On(GREEN);

	LCD_BUFF_Wrs(0,0, "Zadajnik_v0.0.1 ");
	LCD_BUFF_Wrs(160-9*6,1,"CMSIS 4.2");
	LCD_BUFF_Wrs(160-11*6,2,"STM32F407VG");

//	LCD_BUFF_Wrs(85,5,"1. acc_get");
//	LCD_BUFF_Wrs(85,6,"2. acc_start");
//	LCD_BUFF_Wrs(85,7,"2. acc_stop");
	LCD_Update();

//	LCD_BUFF_Wrs(0,3,"X = ");
//	LCD_BUFF_Wrs(0,4,"Y = ");
//	LCD_BUFF_Wrs(0,5,"Z = ");


//  UART4->DR = 0xaf;  // DZIAŁA
//	huart4.Instance->DR = 0xaf;

	//___________________________________
	while(1){
//		LCD_BUFF_Wrv_S16Dec(18,3,pACC_XYZ_BUFF[0]);
//		LCD_BUFF_Wrv_S16Dec(18,4,pACC_XYZ_BUFF[1]);
//		LCD_BUFF_Wrv_S16Dec(18,5,pACC_XYZ_BUFF[2]);
//		LCD_Update();

		SendPendingString();
		if(eReciever_GetStatus() == READY){
			Reciever_GetStringCopy(pcMessageBuffer);
			DecodeMsg(pcMessageBuffer);
			ExecuteCommand();
		}
	}
}
// =======================================================================================================
static void TIMER6_Base_Init(void){
	TIM_Base_InitTypeDef 	Timer6_InitStruct;
//	TIM_ClockConfigTypeDef	Timer6_ClockConfigStruct;

	SystemCoreClockUpdate();

	Timer6_InitStruct.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	Timer6_InitStruct.CounterMode   	= TIM_COUNTERMODE_UP;
	Timer6_InitStruct.Prescaler			= 16800;		// 168000000/16800 = 10000
	Timer6_InitStruct.Period			= 5000;
	Timer6_InitStruct.RepetitionCounter	= 0;

//	Timer6_ClockConfigStruct.ClockSource 	= TIM_CLOCKSOURCE_INTERNAL;
//	Timer6_ClockConfigStruct.ClockPolarity	= TIM_CLOCKPOLARITY_RISING;
//	Timer6_ClockConfigStruct.ClockPrescaler	= TIM_CLOCKPRESCALER_DIV1;
//	Timer6_ClockConfigStruct.ClockFilter 	= 0;
//	HAL_TIM_ConfigClockSource(hTimer6, Timer6_ClockConfigStruct);

	hTimer6.Init 		= Timer6_InitStruct;
	hTimer6.Instance	= TIM6;
	hTimer6.Lock		= HAL_UNLOCKED;
	HAL_TIM_Base_Init(&hTimer6);
	HAL_TIM_Base_Start_IT(&hTimer6);
}

static void ACC_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t AccControlDataTemp[1] = {0};

	BSP_ACCELERO_Init();			// Data rate - 25[Hz]; BW filter - 40[Hz]

	/* Enable LIS3DSH 'Data ready' interrupt (INT1), INT1 active HIGH, INT1 pulsed */
	ACCELERO_IO_Read(AccControlDataTemp,LIS3DSH_CTRL3_ADDR,1);
	AccControlDataTemp[0] |= LIS3DSH_CTRL3_DR_EN_BIT | LIS3DSH_CTRL3_IEL_BIT | LIS3DSH_CTRL3_INT1_EN_BIT | LIS3DSH_CTRL3_IEA_BIT;
	ACCELERO_IO_Write((uint8_t*)AccControlDataTemp, LIS3DSH_CTRL_REG3_ADDR, 1);


	/* LIS3DSH enable 'block data update', won't updatedata regs if not read */
	AccControlDataTemp[0] = 0;
	ACCELERO_IO_Read(AccControlDataTemp,LIS3DSH_CTRL4_ADDR,1);
	AccControlDataTemp[0] |= LIS3DSH_CTRL4_BDU_BIT;
	ACCELERO_IO_Write((uint8_t*)AccControlDataTemp,LIS3DSH_CTRL4_ADDR,1);

	/* Enable INT1 GPIO clock and configure GPIO PIN PE.00 to detect Interrupts */
	ACCELERO_INT_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);


	/* Enable and set EXTI0 Accelerometer INT1 to the lowest priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}



static void LED_Init(void){
	BSP_LED_Init(BLUE);
	BSP_LED_Init(ORANGE);
	BSP_LED_Init(GREEN);
	BSP_LED_Init(RED);
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
	RCC_OscInitStruct.PLL.PLLM = 8;									// Wejscie VCO ma byc 1-2 MHz. 8/8=1MHz.
	RCC_OscInitStruct.PLL.PLLN = 336;								// wyjsie VCO ma byc 192-432 MHz. 1*336 = 336MHz.
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;						// Dzielki wyjscia PLL 336/2 = 168.
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;					// wlaczyc PLL.
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}


	RCC_ClkInitStruct.ClockType 		= RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;			// 24Mhz
	RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV1;			// 24MHz
	RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV1;			// 24MHz
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
		Error_Handler();
	}

	// Wyprowadzenie sygnalow zegarowych na piny zewnetrzne.
	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK,RCC_MCODIV_4);
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI,RCC_MCODIV_1);

}


//___________________________________
void LED_StartSignal(void){
	uint32_t u32WaitTime_ms = 50;

	BSP_LED_On(GREEN);
	HAL_Delay(u32WaitTime_ms);
	BSP_LED_Off(GREEN);

	BSP_LED_On(ORANGE);
	HAL_Delay(u32WaitTime_ms);
	BSP_LED_Off(ORANGE);

	BSP_LED_On(RED);
	HAL_Delay(u32WaitTime_ms);
	BSP_LED_Off(RED);

	BSP_LED_On(BLUE);
	HAL_Delay(u32WaitTime_ms);
	BSP_LED_Off(BLUE);

	BSP_LED_On(GREEN);
	HAL_Delay(u32WaitTime_ms);
	BSP_LED_Off(GREEN);
}


void SendPendingString(void){
	if(eTransmiter_GetStatus() == FREE){
		if(1 == fId){
			Transmiter_SendString("id stm32f407VG\n");
			fId = 0;
		}
		else if(1 == fOk){
			Transmiter_SendString("OK\n");
			fOk = 0;
		}
		else if(1 == fCalc){
			char pcTempString[17] = "calc ";
			unsigned int uiTempVar = (psToken+1)->uValue.uiNumber * 2;

			AppendUIntToString(uiTempVar,pcTempString);
			AppendString(" \n",pcTempString);
			Transmiter_SendString(pcTempString);
			fCalc = 0;
		}
		else if(1 == fProvideData){
			Transmiter_SendString("provide correct number\n");
			fProvideData = 0;
		}
		else if(1 == fUnknownCommand ){
			Transmiter_SendString("unknown command\n");
			fUnknownCommand = 0;
		}
	}
}

void ExecuteCommand(void){
	if((0 != ucTokenNr) && (KEYWORD == psToken->eType) ){
			switch(psToken->uValue.eKeyword){
				case CALC:
					if( NUMBER == (psToken+1)->eType ){
						fCalc = 1;
					}
					else {
						fProvideData = 1;
					}
					break;
				case ID:
					fId = 1;
					break;
				case LIS3DSH_GETXYZ:
					BSP_LED_On(BLUE);
					BSP_ACCELERO_GetXYZ(pACC_XYZ_BUFF);
					HAL_Delay(100);
					BSP_LED_Off(BLUE);
					LCD_BUFF_Wrv_S16Dec(18,3,pACC_XYZ_BUFF[0]);
					LCD_BUFF_Wrv_S16Dec(18,4,pACC_XYZ_BUFF[1]);
					LCD_BUFF_Wrv_S16Dec(18,5,pACC_XYZ_BUFF[2]);
					LCD_Update();
					fOk = 1;
					break;
				default:
					BSP_LED_Toggle(ORANGE);
				    break;
			}
		}
		else {
			fUnknownCommand = 1;
		}
}


//___________________________________
static void Error_Handler(void){
	BSP_LED_Off(GREEN);
	BSP_LED_On(RED);
	while(1){
	}
}


