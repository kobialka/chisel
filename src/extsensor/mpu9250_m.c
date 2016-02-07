/*
 * mpu9250_m.c
 *
 *  Created on: 27 sty 2016
 *      Author: mike
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "mpu9250_m.h"

// =======================================================================================================
// Typy danych


// =======================================================================================================
// Definicje

/* Read/Write command */
#define READWRITE_CMD                     ((uint8_t)0x80)

/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                        ((uint8_t)0x00)

#define SPI_MPU9250_TIMEOUT_MAX                 	0x1000

// =======================================================================================================
// zmienne globalne
uint32_t u32SpiMPU9250Timeout = SPI_MPU9250_TIMEOUT_MAX;

// =======================================================================================================
// zmienne prywatne



// =======================================================================================================
// funkcje prywatne
static void		Init_MPU9250_SPI(void);
static uint8_t	MPU9250_SPI_TxRx(uint8_t u8Byte);
static void		MPU9250_SPI_Error(void);
static void		MPU9250_Rx(uint8_t *pu8RxBuff, uint8_t u8RxAddr, uint16_t u16ByteNr);
static void 	MPU9250_Tx(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint16_t u16ByteNr);




static void Init_MPU9250_SPI(void){
	uint32_t u32Temp = 0;

    if(HAL_SPI_GetState(&hspi3_MPU9250) == HAL_SPI_STATE_RESET){
    	hspi3_MPU9250.Instance = SPI3;
		hspi3_MPU9250.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
		hspi3_MPU9250.Init.Direction = SPI_DIRECTION_2LINES;
		hspi3_MPU9250.Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi3_MPU9250.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi3_MPU9250.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
		hspi3_MPU9250.Init.CRCPolynomial = 7;
		hspi3_MPU9250.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi3_MPU9250.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi3_MPU9250.Init.NSS = SPI_NSS_SOFT;						// Pin chip select sterowany programowo
		hspi3_MPU9250.Init.TIMode = SPI_TIMODE_DISABLED;
		hspi3_MPU9250.Init.Mode = SPI_MODE_MASTER;
		HAL_SPI_Init(&hspi3_MPU9250);

		/*
		 * 	 Ta transakcja jest nieważna.
		 * 	 Przy pierwszym transferze sciągnięcie lini zegarowej do poziomu niskiego
		 * 	 jest opóźnione w stosunku do aktywacji odbiornika (sciągnięcia CS).
		 *
		 * 	 Dekoder w moim analizatorze logicznym zgłasza błąd. Być może interfejs SPI
		 * 	 w odbiorniku też będzie widział jakiś problem.
		 *
		 * 	 Tego problemu nie ma w przypadku wykorzystania kodu z biblioteki STM do obsługi acc
		 * 	 na płytce discovery. Do sprawdzenia oscyloskopem.
		 */

		MPU9250_CS_HIGH();
		HAL_SPI_TransmitReceive(&hspi3_MPU9250, (uint8_t*)&u32Temp, (uint8_t*)&u32Temp, 1, u32SpiMPU9250Timeout);
    }
}

//  ===  SPI odczyt i zapis MPU9250
static uint8_t MPU9250_SPI_TxRx(uint8_t u8Byte){
	uint8_t u8ReceivedByte;

	if(HAL_SPI_TransmitReceive(&hspi3_MPU9250, (uint8_t*) &u8Byte, (uint8_t*) &u8ReceivedByte, 1, u32SpiMPU9250Timeout) != HAL_OK){
		MPU9250_SPI_Error();
	}

	return u8ReceivedByte;
}

static void MPU9250_SPI_Error(void)
{
  /* De-initialize the SPI comunication bus */
  HAL_SPI_DeInit(&hspi3_MPU9250);

  /* Re-Initiaize the SPI comunication bus */
  Init_MPU9250_SPI();
}

static void MPU9250_Rx(uint8_t *pu8RxBuff, uint8_t u8RxAddr, uint16_t u16ByteNr){
	uint32_t u32Cycles;
	u8RxAddr |= (uint8_t)READWRITE_CMD;

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8RxAddr);
	while(u16ByteNr > 0){
		*pu8RxBuff = MPU9250_SPI_TxRx(DUMMY_BYTE);
		u16ByteNr--;
		pu8RxBuff++;
	}
	MPU9250_CS_HIGH();
	u32Cycles = (uint32_t)SystemCoreClock/1000000/7;

	while(u32Cycles--){
		__asm("nop;");
	};
}

static void MPU9250_Tx(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint16_t u16ByteNr){	// liczba taktów zegara na 1[us] podzielona  następnie ze wględu na narzut od ''nop'' i pętli.
	uint32_t u32Cycles;

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8TxAddr);
	while(u16ByteNr >= 1){
		MPU9250_SPI_TxRx(*pu8TxBuff);
		u16ByteNr--;
		pu8TxBuff++;
	}
	MPU9250_CS_HIGH();

	// Opóźnienie wprowadzone aby zapewnić minimalną szerokość impulsu pomiędzy transakcjami. Kiedy impuls trwa mniej niż 1[us] MPU9250 gubi się i dane odczytywane często nie są poprawne.
	u32Cycles = (uint32_t)SystemCoreClock/1000000/7;	// liczba taktów zegara na 1[us] podzielona  następnie ze wględu na narzut od ''nop'' i pętli.

	while(u32Cycles--){
		__asm("nop;");
	};
}

// =======================================================================================================
// funkcje publiczne





//  ===  KONFIGURACJA
//void MPU9250_Conf(void){
//
//}

//  ===  INICJALIZACJA

	/*
	 *  s.26 W trybie SPI nie ma dostępu do trybu passtrough AUX iic
	 * s.25 uC musi skonfigurować mag (Aux iic w trybie passtrough)
	 * s. 32 W trybie SPI należy ustawić bit I2C_IF_DIS w celu wyłączenia interfejsu I2C. Należy to zrobić w określonym momencie (pkt 6.3)
	 *
	 */


static void MPU9250_Init(tsMPU9250_InitTypedef * sMPU9250_Init){
	uint8_t					u8TempInitReg;


	Init_MPU9250_SPI();

	// RESET UKŁADU
	u8TempInitReg = MPU9250_CONF_HARD_RESET;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);
	// delay 100ms
	HAL_Delay(100);

	// KONFIGURACJA

	u8TempInitReg = 0x00;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = sMPU9250_Init->Clock_Source;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sINTERRUPTS.INT_Pin_Logic_Level 	|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_PP_OD			|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_Latch			|	\
						sMPU9250_Init->sINTERRUPTS.Clear_Status_On_Read	|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_Logic_Level		|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_INT_MODE_Enable	|	\
						sMPU9250_Init->sI2C.Bypass_Enable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_PIN_CFG, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_INT_PIN_CFG, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg =(	sMPU9250_Init->sINTERRUPTS.Enable_Wake_Up_Int		|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FIFO_Overflow_Int	|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FSYNC_Int			|	\
						sMPU9250_Init->sINTERRUPTS.Enable_Raw_Data_Enable_Int);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_ENABLE, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_INT_ENABLE, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sI2C.I2C_MST_DELAY_CTRL;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_MST_DELAY_CTRL, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_I2C_MST_DELAY_CTRL, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sACC.Axis_Disable 	|	\
						sMPU9250_Init->sGYRO.Axis_Disable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_2, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_2, 1);
	HAL_Delay(10);

	//  ===
	u8TempInitReg =		sMPU9250_Init->Sample_Rate_Divider;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_SMPLRT_DIV, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_SMPLRT_DIV, 1);
	HAL_Delay(10);

	//  ===
	u8TempInitReg = (	sMPU9250_Init->sGYRO.Full_Scale		|	\
						sMPU9250_Init->sGYRO.Filter_Bypass);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_GYRO_CONFIG, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_GYRO_CONFIG, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sACC.Full_Scale;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sACC.Filter_Bypass	|	\
						sMPU9250_Init->sACC.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG_2, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG_2, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sACC.WakeOn_Threshold;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_WOM_THR, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_WOM_THR, 1);
	HAL_Delay(10);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sFIFO.Source_Select;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_FIFO_EN, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_FIFO_EN, 1);
	HAL_Delay(10);


	// ===
	u8TempInitReg = (	sMPU9250_Init->sFIFO.Overlap		|	\
						sMPU9250_Init->External_Sync		|	\
						sMPU9250_Init->sGYRO.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_CONFIG, 1);
	MPU9250_Rx(&u8TempInitReg, MPU9250_ADDR_CONFIG, 1);
	HAL_Delay(10);


}






// gyro acc
// mag


void Sensor_MPU9250_Init(){
	tsMPU9250_InitTypedef sMPU9250_Init;

	// KONFIGURACJA. NIEISTOTNE PARAMETRY USTAWIĆ NA 0.
		sMPU9250_Init.Clock_Source									=	MPU9250_CONF_CLOCK_SOURCE_AUTOSELECT;
		sMPU9250_Init.Sample_Rate_Divider							=	1;
		sMPU9250_Init.External_Sync									=	MPU9250_CONF_EXT_SYNC_DISABLED;

		sMPU9250_Init.sI2C.Bypass_Enable							=	MPU9250_CONF_I2C_BYPASS_PIN_NO;
		sMPU9250_Init.sI2C.I2C_IF_DIS								=	MPU9250_CONF_I2C_DISABLE_MODULE_YES;
		sMPU9250_Init.sI2C.I2C_MST_EN								=	MPU9250_CONF_I2C_MASTER_MODE_NO;
		sMPU9250_Init.sI2C.I2C_MST_DELAY_CTRL						=	0;

		sMPU9250_Init.sINTERRUPTS.INT_Pin_Logic_Level				=	MPU9250_CONF_INT_PIN_LOGIC_ACTIVE_LVL_HIGH;
		sMPU9250_Init.sINTERRUPTS.INT_Pin_PP_OD						=	MPU9250_CONF_INT_PIN_PP;
		sMPU9250_Init.sINTERRUPTS.INT_Pin_Latch						=	MPU9250_CONF_INT_PIN_LATCH_NO_PULSE_50us;
		sMPU9250_Init.sINTERRUPTS.Clear_Status_On_Read				=	MPU9250_CONF_INT_CLEAR_STATUS_RX_ANY;
		sMPU9250_Init.sINTERRUPTS.FSYNC_Logic_Level					=	0;
		sMPU9250_Init.sINTERRUPTS.FSYNC_INT_MODE_Enable				=	0;
		sMPU9250_Init.sINTERRUPTS.Enable_Wake_Up_Int				=	0;
		sMPU9250_Init.sINTERRUPTS.Enable_FIFO_Overflow_Int			=	0;
		sMPU9250_Init.sINTERRUPTS.Enable_FSYNC_Int					=	0;
		sMPU9250_Init.sINTERRUPTS.Enable_Raw_Data_Enable_Int		=	0x01;

		sMPU9250_Init.sACC.Axis_Disable 							=	MPU9250_CONF_ACC_AXIS_ALL;
		sMPU9250_Init.sACC.Full_Scale								=	MPU9250_CONF_ACC_FULLSCALE_4g;
		sMPU9250_Init.sACC.Filter_Bypass							=	MPU9250_CONF_ACC_FILTER_BYPASS_NO;
		sMPU9250_Init.sACC.Filter_BW								=	MPU9250_CONF_ACC_FILTER_BW_460Hz;
		sMPU9250_Init.sACC.WakeOn_Threshold							=	0;

		sMPU9250_Init.sGYRO.Axis_Disable							=	MPU9250_CONF_GYRO_AXIS_ALL_ON;
		sMPU9250_Init.sGYRO.Full_Scale								=	MPU9250_CONF_GYRO_FULLSCALE_250dps;
		sMPU9250_Init.sGYRO.Filter_Bypass							=	MPU9250_CONF_GYRO_FILTER_BYPASS_NO;
		sMPU9250_Init.sGYRO.Filter_BW								=	MPU9250_CONF_GYRO_FILTER_BW_20Hz;

		sMPU9250_Init.sFIFO.Enable									=	MPU9250_CONF_FIFO_ENABLE_NO;
		sMPU9250_Init.sFIFO.Overlap									=	0;
		sMPU9250_Init.sFIFO.Source_Select							=	0;

		MPU9250_Init(&sMPU9250_Init);

}

// =======================================================================================================
// ===  ODCZYT DANYCH
void MPU9250_ReadGyro(void){

}

uint8_t MPU9250_WhoAmI(void){
	uint8_t pu8Buffer;

	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	return pu8Buffer;
}

void MPU9250_ReadAcc(int16_t *pi16DataXYZ){
	uint8_t		pu8Buffer[6];
	uint8_t 	u8RegTempVal	=	0;
	uint8_t 	u8ValueCounter	=	0;
	float		fSensitivity	=	1;
	float 	fTempValue 			=	0;

	MPU9250_Rx(&u8RegTempVal, MPU9250_ADDR_ACCEL_CONFIG, 1);
	u8RegTempVal &= MPU9250_CONF_ACC_FULLSCALE_MASK;	// FULL SCALE

	MPU9250_Rx(&pu8Buffer[1], MPU9250_ADDR_ACCEL_XOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[0], MPU9250_ADDR_ACCEL_XOUT_L, 1);
	MPU9250_Rx(&pu8Buffer[3], MPU9250_ADDR_ACCEL_YOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[2], MPU9250_ADDR_ACCEL_YOUT_L, 1);
	MPU9250_Rx(&pu8Buffer[5], MPU9250_ADDR_ACCEL_ZOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[4], MPU9250_ADDR_ACCEL_ZOUT_L, 1);


	switch (u8RegTempVal) {
		case MPU9250_CONF_ACC_FULLSCALE_2g:
			fSensitivity = MPU9250_ACC_SENSITIVITY_FOR_2g_SCALE;
			break;
		case MPU9250_CONF_ACC_FULLSCALE_4g:
			fSensitivity = MPU9250_ACC_SENSITIVITY_FOR_4g_SCALE;
			break;
		case MPU9250_CONF_ACC_FULLSCALE_8g:
			fSensitivity = MPU9250_ACC_SENSITIVITY_FOR_8g_SCALE;
			break;
		case MPU9250_CONF_ACC_FULLSCALE_16g:
			fSensitivity = MPU9250_ACC_SENSITIVITY_FOR_16g_SCALE;
			break;
		default:
			fSensitivity = MPU9250_ACC_SENSITIVITY_FOR_2g_SCALE;
			break;
	}

	for(u8ValueCounter = 0; u8ValueCounter < 3; u8ValueCounter++){
		fTempValue = (((int8_t)pu8Buffer[2*u8ValueCounter+1] << 8) | (0xff & (int8_t)pu8Buffer[2*u8ValueCounter]))*fSensitivity; // rzutowanie na float zachowuje znak!
		pi16DataXYZ[u8ValueCounter] =(int16_t)fTempValue;
	}

}

void MPU9250_ReadMag(void){

}


// =======================================================================================================
//  ===  KALIBRACJA

//void MPU9250_Calib(void){
//
//}

//switch(crtl & LIS3DSH__FULLSCALE_SELECTION)
//  {
//    /* FS bit = 000 ==> Sensitivity typical value = 0.06milligals/digit */
//  case LIS3DSH_FULLSCALE_2:
//    sensitivity = LIS3DSH_SENSITIVITY_0_06G;
//    break;
//
//    /* FS bit = 001 ==> Sensitivity typical value = 0.12milligals/digit */
//  case LIS3DSH_FULLSCALE_4:
//    sensitivity = LIS3DSH_SENSITIVITY_0_12G;
//    break;
//
//    /* FS bit = 010 ==> Sensitivity typical value = 0.18milligals/digit */
//  case LIS3DSH_FULLSCALE_6:
//    sensitivity = LIS3DSH_SENSITIVITY_0_18G;
//    break;
//
//    /* FS bit = 011 ==> Sensitivity typical value = 0.24milligals/digit */
//  case LIS3DSH_FULLSCALE_8:
//    sensitivity = LIS3DSH_SENSITIVITY_0_24G;
//    break;
//
//    /* FS bit = 100 ==> Sensitivity typical value = 0.73milligals/digit */
//  case LIS3DSH_FULLSCALE_16:
//    sensitivity = LIS3DSH_SENSITIVITY_0_73G;
//    break;
//
//  default:
//    break;
//  }
//
//  /* Obtain the mg value for the three axis */
//  for(i=0; i<3; i++)
//  {
//	// valueinfloat = ((buffer[2*i+1] << 8) + buffer[2*i]) * sensitivity;
//    valueinfloat = ((buffer[2*i+1] << 8) | (0x0ff & buffer[2*i]) ) * sensitivity;
//    pData[i] = (int16_t)valueinfloat;
//  }
