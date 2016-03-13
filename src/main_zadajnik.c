/* main_zadajnik.c */

/*	autor:		Michal Kobialka, mmkobialka@gmail.com
 * 	data: 		23.07.2015
 *
 */


// =======================================================================================================
// PLIKI NAGŁÓWKOWE
#include <stdint.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "lib_LIS3DSH.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"
//#include "lib_S1D15705_m.h"
#include "string.h"
#include "command_decoder.h"
#include "uart.h"
#include "mpu9250_m.h"

// =======================================================================================================
// DEFINICJE
#define ACC_XYZ_BUFF_SIZE	9
#define BUTTON_PIN			GPIO_PIN_0
#define BUTTON_PORT			GPIOA


// =======================================================================================================
// ZMIENNE GLOBALNE:
extern	tToken				asToken[MAX_TOKEN_NR];	// tablica tokenów.
extern unsigned char		ucTokenNr;		// ilosc nalezionych tokenow.

uint8_t 					Hal_RxBuff[1];
uint8_t						Hal_TxBuff[1];
uint8_t						pSPI_RxBuff[1];
uint8_t						pcMessageBuffer[UART_RECIEVER_SIZE];
int16_t						pACC_XYZ_BUFF[ACC_XYZ_BUFF_SIZE];
tToken 						*psToken = asToken;

UART_HandleTypeDef			huart4;
TIM_HandleTypeDef			hTimer6;
HAL_StatusTypeDef 			HAL_Status;
SPI_HandleTypeDef			hspi3_MPU9250;

uint8_t						fCalc 				= 0;
uint8_t						fId 				= 0;
uint8_t						fOk					= 0;
uint8_t						fUnknownCommand 	= 0;
uint8_t						fProvideData 		= 0;
uint8_t						fAccGotXYZ 			= 0;
uint8_t						fTest				= 0;



// =======================================================================================================
// FUNKCJE PRYWATNE
static void Error_Handler(void);
static void SystemClock_Config(void);
static void TIMER6_Base_Init(void);
static void SendPendingString(void);
static void ExecuteCommand(void);
static void BoardButton_Init(void);
static void LED_Init(void);
static void LED_StartSignal(void);
static void ACC_Init(void);
static void MPU9250_Init_9D(void);



// =======================================================================================================
// GŁÓWNA PĘTLA
int main(void){
	// 110, 150, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600

	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();
	HAL_InitTick(0);
	LED_Init();
	ACC_Init();
	MPU9250_Init_9D();
	UART_InitWithInt(115200);
	TIMER6_Base_Init();
	BoardButton_Init();

	LED_StartSignal();
	BSP_LED_On(GREEN);

	while(1){
		SendPendingString();
		if(eReciever_GetStatus() == READY){
			Reciever_GetStringCopy(pcMessageBuffer);
			DecodeMsg(pcMessageBuffer);
			ExecuteCommand();
		}
	}
}


// =======================================================================================================
// DEFINICJE FUNKCJI
static void BoardButton_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = BUTTON_PIN;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

static void SendPendingString(void){
	if(eTransmiter_GetStatus() == FREE){
		if(1 == fId){
			Transmiter_SendString("stm32f407VG\n");
			fId = 0;
		}
		else if(1 == fOk){
			Transmiter_SendString("OK\n");
			fOk = 0;
		}
		else if(1 == fCalc){
			char pcTempString[17] = "calc ";
			uint16_t uiTempVar = (psToken+1)->uValue.u32_Number* 2;

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
		else if(1 == fTest){
			char pcTempString[50] = "";

			AppendHexIntToString(pACC_XYZ_BUFF[0],pcTempString);
			AppendString(";",pcTempString);
			AppendHexIntToString(pACC_XYZ_BUFF[1],pcTempString);
			AppendString(";",pcTempString);
			AppendHexIntToString(pACC_XYZ_BUFF[2],pcTempString);
			AppendString(";",pcTempString);
			Transmiter_SendString(pcTempString);
			fTest = 0;
		}
	}
}

static void ExecuteCommand(void){
	uint8_t u8_TempAccReg;


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
					BSP_ACCELERO_GetXYZ(pACC_XYZ_BUFF);
					BSP_LED_Toggle(ORANGE);
					fTest = 1;
					break;

				case LIS3DSH_START:
					HAL_NVIC_EnableIRQ(EXTI0_IRQn);
					ACCELERO_IO_Read(&u8_TempAccReg, LIS3DSH_CTRL3_ADDR, 1);
					u8_TempAccReg |= LIS3DSH_CTRL3_INT1_EN_BIT;					// enable Data Ready Int1
					ACCELERO_IO_Write(&u8_TempAccReg, LIS3DSH_CTRL3_ADDR, 1);
					ACCELERO_IO_Read(&u8_TempAccReg, LIS3DSH_CTRL4_ADDR, 1);
					u8_TempAccReg |= LIS3DSH_CTRL4_XYZ_ENABLE_BIT;				// turn on xyz axis.
					ACCELERO_IO_Write(&u8_TempAccReg, LIS3DSH_CTRL4_ADDR, 1);
					fOk = 1;
					break;

				case LIS3DSH_STOP:
					ACCELERO_IO_Read(&u8_TempAccReg, LIS3DSH_CTRL4_ADDR, 1);
					u8_TempAccReg &= ~(LIS3DSH_CTRL4_XYZ_ENABLE_BIT);				// turn off xyz axis.
					ACCELERO_IO_Write(&u8_TempAccReg, LIS3DSH_CTRL4_ADDR, 1);
					ACCELERO_IO_Read(&u8_TempAccReg, LIS3DSH_CTRL3_ADDR, 1);
					u8_TempAccReg &= ~(LIS3DSH_CTRL3_INT1_EN_BIT);					// disable Data Ready Int1
					ACCELERO_IO_Write(&u8_TempAccReg, LIS3DSH_CTRL3_ADDR, 1);
					HAL_NVIC_DisableIRQ(EXTI0_IRQn);
					fOk = 1;
					break;

				case TEST:
//					BSP_ACCELERO_GetXYZ(pACC_XYZ_BUFF);
//					MPU9250_ReadGyro(pACC_XYZ_BUFF);
//					MPU9250_ReadAcc(pACC_XYZ_BUFF);
					MPU9250_ReadMeas9D(pACC_XYZ_BUFF);
					fTest = 1;
					break;

				default:
					Error_Handler();
				    break;
			}
		}
		else {
			fUnknownCommand = 1;
		}
}

static void MPU9250_Init_9D(){
	tsMPU9250_InitTypedef sMPU9250_Init;

	// KONFIGURACJA. NIEISTOTNE PARAMETRY USTAWIĆ NA 0.
	sMPU9250_Init.Clock_Source                              =       MPU9250_CONF_CLOCK_SOURCE_AUTOSELECT;
	sMPU9250_Init.Sample_Rate_Divider                       =       20;
	sMPU9250_Init.External_Sync                             =       MPU9250_CONF_EXT_SYNC_DISABLED;

	sMPU9250_Init.sI2C.I2C_IF_DIS                           =       MPU9250_CONF_I2C_SLAVE_INTERFACE_DISABLE_YES;
	sMPU9250_Init.sI2C.I2C_MST_EN                           =       MPU9250_CONF_I2C_MASTER_MODE_YES;
	sMPU9250_Init.sI2C.Bypass_Enable                        =       MPU9250_CONF_I2C_BYPASS_PIN_YES;				//  W przypadku używania i2c w trybie slave, ustawia piny pocnicze i2c w stan wysokiej impedancji lub łączy logicznie z pinami głównego i2c
	sMPU9250_Init.sI2C.Ext_Sens_Data_Shadowing				=		MPU9250_CONF_I2C_DELAY_CTRL_SHADOWING_ON;
	sMPU9250_Init.sI2C.Ext_Sens_Read_Delay_Enable			=		0;
	sMPU9250_Init.sI2C.Next_Slave_Mode						=		MPU9250_CONF_I2C_NEXT_SLV_AFTER_STOP;
	sMPU9250_Init.sI2C.Multimaster_Enable					=		MPU9250_CONF_I2C_MULTIMASTER_MODE_NO;
	sMPU9250_Init.sI2C.Master_Clock							=		MPU9250_CONF_I2C_MST_CLOCK_400kHz;


	// TODO: clean up MAG configuration
	sMPU9250_Init.sI2C_Slave_0.Data_Lenght					=		8;
	sMPU9250_Init.sI2C_Slave_0.Byte_Swap					=		0;
	sMPU9250_Init.sI2C_Slave_0.Grouping						=		0;
	sMPU9250_Init.sI2C_Slave_0.No_Write						=		0;
	sMPU9250_Init.sI2C_Slave_0.Physical_Address				=		AK8963_I2C_ADDR | I2C_READ;
	sMPU9250_Init.sI2C_Slave_0.Readout_Enable				=		1;

	sMPU9250_Init.sMAG.Resolution							=		AK8963_CONF_RESOLUTION_16bit;
	sMPU9250_Init.sMAG.OperationalMode						=		AK8963_CONF_MODE_CONTINUOUS_100Hz;

	sMPU9250_Init.sACC.Axis_Disable                         =       MPU9250_CONF_ACC_AXIS_ALL;
	sMPU9250_Init.sACC.Full_Scale                           =       MPU9250_CONF_ACC_FULLSCALE_4g;
	sMPU9250_Init.sACC.Filter_Bypass                        =       MPU9250_CONF_ACC_FILTER_BYPASS_NO;
	sMPU9250_Init.sACC.Filter_BW                            =       MPU9250_CONF_ACC_FILTER_BW_460Hz;
	sMPU9250_Init.sACC.WakeOn_Threshold                     =       0;

	sMPU9250_Init.sGYRO.Axis_Disable                        =       MPU9250_CONF_GYRO_AXIS_ALL_ON;
	sMPU9250_Init.sGYRO.Full_Scale                          =       MPU9250_CONF_GYRO_FULLSCALE_250dps;
	sMPU9250_Init.sGYRO.Filter_Bypass                       =       MPU9250_CONF_GYRO_FILTER_BYPASS_NO;
	sMPU9250_Init.sGYRO.Filter_BW                           =       MPU9250_CONF_GYRO_FILTER_BW_5Hz;

	sMPU9250_Init.sFIFO.Enable                              =       MPU9250_CONF_FIFO_ENABLE_NO;
	sMPU9250_Init.sFIFO.Overlap                             =       0;
	sMPU9250_Init.sFIFO.Source_Select                       =       0;

	sMPU9250_Init.sINTERRUPTS.INT_Pin_Logic_Level           =       MPU9250_CONF_INT_PIN_LOGIC_ACTIVE_LVL_HIGH;
	sMPU9250_Init.sINTERRUPTS.INT_Pin_PP_OD                 =       MPU9250_CONF_INT_PIN_PP;

	sMPU9250_Init.sINTERRUPTS.Data_Ready_Waits_For_Ext_Sens	=		MPU9250_CONF_WAIT_FOR_EXT_SENS_DATA_YES;
	sMPU9250_Init.sINTERRUPTS.INT_Pin_Latch                 =       MPU9250_CONF_INT_PIN_LATCH_NO_PULSE_50us;
	sMPU9250_Init.sINTERRUPTS.Clear_Status_On_Read          =       MPU9250_CONF_INT_CLEAR_STATUS_RX_ANY;
	sMPU9250_Init.sINTERRUPTS.FSYNC_Logic_Level             =       0;
	sMPU9250_Init.sINTERRUPTS.FSYNC_INT_MODE_Enable         =       0;
	sMPU9250_Init.sINTERRUPTS.Enable_Wake_Up_Int            =       0;
	sMPU9250_Init.sINTERRUPTS.Enable_FIFO_Overflow_Int      =       0;
	sMPU9250_Init.sINTERRUPTS.Enable_FSYNC_Int              =       0;
	sMPU9250_Init.sINTERRUPTS.Enable_Raw_Data_Enable_Int	=       0;


	MPU9250_Init(&sMPU9250_Init);

	// prawdzenie działania magnetometru: inicjalizacja I2C
}

static void TIMER6_Base_Init(void){
	TIM_Base_InitTypeDef 	Timer6_InitStruct;

	Timer6_InitStruct.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	Timer6_InitStruct.CounterMode   	= TIM_COUNTERMODE_UP;
	Timer6_InitStruct.Prescaler			= 16800;		// 168000000/16800 = 10000
	Timer6_InitStruct.Period			= 5000;
	Timer6_InitStruct.RepetitionCounter	= 0;

	hTimer6.Init 		= Timer6_InitStruct;
	hTimer6.Instance	= TIM6;
	hTimer6.Lock		= HAL_UNLOCKED;
	HAL_TIM_Base_Init(&hTimer6);
	HAL_TIM_Base_Start_IT(&hTimer6);
}

static void ACC_Init(void){
	//GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t AccControlDataTemp = 0;

	BSP_ACCELERO_Init();			// Data rate - 12.5[Hz]; BW filter - 40[Hz]
	ACCELERO_IO_Read(&AccControlDataTemp,LIS3DSH_CTRL3_ADDR,1);

	//  ===  Disable interrupts from ACC.
	AccControlDataTemp &= 	~(LIS3DSH_CTRL3_DR_EN_BIT	| \
							  LIS3DSH_CTRL3_INT1_EN_BIT | \
							  LIS3DSH_CTRL3_INT2_EN_BIT);

	ACCELERO_IO_Write(&AccControlDataTemp, LIS3DSH_CTRL_REG3_ADDR, 1);

	//  ===  Set updating data all the time
	ACCELERO_IO_Read(&AccControlDataTemp,LIS3DSH_CTRL4_ADDR,1);
	AccControlDataTemp &= ~LIS3DSH_CTRL4_BDU_BIT;
	ACCELERO_IO_Write(&AccControlDataTemp,LIS3DSH_CTRL4_ADDR,1);

	/* Enable INT1 GPIO clock and configure GPIO PIN PE.00 to detect Interrupts */
	/*
	ACCELERO_INT_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	*/

	/* Enable and set EXTI0 Accelerometer INT1 to the lowest priority */
	//HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
}

static void LED_Init(void){
	BSP_LED_Init(BLUE);
	BSP_LED_Init(ORANGE);
	BSP_LED_Init(GREEN);
	BSP_LED_Init(RED);
}

static void SystemClock_Config(void){
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

static void LED_StartSignal(void){
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

static void Error_Handler(void){
	BSP_LED_Off(GREEN);
	BSP_LED_On(RED);
	while(1){
	}
}

