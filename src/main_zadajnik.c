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
#include "command_decoder.h"
#include "uart.h"

/* Defines	------------------------------------------------------------------*/
#define CYCLES 	(500000UL)


//___________________________________
// ZMIENNE GLOBALNE:
extern	tToken				asToken[MAX_TOKEN_NR];	// tablica tokenów.
extern unsigned char 		ucTokenNr;		// ilosc nalezionych tokenow.

char 						Hal_RxBuff[1];
char						Hal_TxBuff[1];
UART_HandleTypeDef			huart4;
HAL_StatusTypeDef 			HAL_Status;
uint16_t 					u16Counter = 0;
char 						acMessageBuffer[UART_RECIEVER_SIZE];
tToken 						*psToken = asToken;

uint8_t						fCalc = 0, fId = 0, fUnknownCommand = 0,fProvideData = 0;



//___________________________________
static void SystemClock_Config(void);
static void LED_StartSignal(void);
static void Error_Handler(void);
static void LED_StartSignal(void);
//static void ACC_Init(void);
static void GPIO_Init(void);






// =======================================================================================================
int main(void){

	//___________________________________
	HAL_Init();
	SystemClock_Config();
	LCD_Init();
	GPIO_Init();
	UART_InitWithInt(9600);

	//___________________________________
	//HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK,RCC_MCODIV_1);			// Wyprowadzenie sygnalow zegarowych na piny zewnetrzne.
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI,RCC_MCODIV_1);
	LED_StartSignal();
	BSP_LED_On(GREEN);



    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);

//     UART4->DR = 0xaf;  // DZIAŁA
//	huart4.Instance->DR = 0xaf;

	//___________________________________
	while(1){

		if(eTransmiter_GetStatus() == FREE){
			if(1 == fId){
				Transmiter_SendString("id stm32f407VG\n");
				fId = 0;
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
		if(eReciever_GetStatus() == READY){
			Reciever_GetStringCopy(acMessageBuffer);
			DecodeMsg(acMessageBuffer);

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
				}
			}
			else {
				fUnknownCommand = 1;
			}
		}
	}
}
// =======================================================================================================

static void GPIO_Init(void){
	BSP_LED_Init(BLUE);
	BSP_LED_Init(ORANGE);
	BSP_LED_Init(GREEN);
	BSP_LED_Init(RED);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
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


