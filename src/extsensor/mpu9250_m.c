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
	uint32_t u32Temp;

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

	u8RxAddr |= (uint8_t)READWRITE_CMD;

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8RxAddr);
	while(u16ByteNr > 0){
		*pu8RxBuff = MPU9250_SPI_TxRx(DUMMY_BYTE);
		u16ByteNr--;
		pu8RxBuff++;
	}
	MPU9250_CS_HIGH();
}

static void MPU9250_Tx(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint16_t u16ByteNr){

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8TxAddr);
	while(u16ByteNr >= 1){
		MPU9250_SPI_TxRx(*pu8TxBuff);
		u16ByteNr--;
		pu8TxBuff++;
	}
	MPU9250_CS_HIGH();
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


void MPU9250_Init(tsMPU9250_InitTypedef * sMPU9250_Init){
	uint8_t					u8TempInitReg;


	Init_MPU9250_SPI();

	// RESET UKŁADU
	u8TempInitReg = MPU9250_CONF_HARD_RESET;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);
	// delay 100ms

	// KONFIGURACJA

	// ===
	u8TempInitReg = sMPU9250_Init->Clock_Source;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sINTERRUPTS.INT_Pin_Logic_Level 	|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_PP_OD			|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_Latch			|	\
						sMPU9250_Init->sINTERRUPTS.Clear_Status_On_Read	|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_Logic_Level		|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_INT_MODE_Enable	|	\
						sMPU9250_Init->sI2C.Bypass_Enable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_PIN_CFG, 1);

	// ===
	u8TempInitReg =(	sMPU9250_Init->sINTERRUPTS.Enable_Wake_Up_Int		|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FIFO_Overflow_Int	|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FSYNC_Int			|	\
						sMPU9250_Init->sINTERRUPTS.Enable_Raw_Data_Enable_Int);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_ENABLE, 1);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sI2C.I2C_MST_DELAY_CTRL;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_MST_DELAY_CTRL, 1);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sACC.Axis_Disable 	|	\
						sMPU9250_Init->sGYRO.Axis_Disable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_2, 1);

	//  ===
	u8TempInitReg =		sMPU9250_Init->Sample_Rate_Divider;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_SMPLRT_DIV, 1);

	//  ===
	u8TempInitReg = (	sMPU9250_Init->sGYRO.Full_Scale		|	\
						sMPU9250_Init->sGYRO.Filter_Bypass);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_GYRO_CONFIG, 1);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sACC.Full_Scale;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG, 1);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sACC.Filter_Bypass	|	\
						sMPU9250_Init->sACC.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG_2, 1);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sACC.WakeOn_Threshold;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_WOM_THR, 1);

	// ===
	u8TempInitReg = 	sMPU9250_Init->sFIFO.Source_Select;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_FIFO_EN, 1);


	// ===
	u8TempInitReg = (	sMPU9250_Init->sFIFO.Overlap		|	\
						sMPU9250_Init->External_Sync		|	\
						sMPU9250_Init->sGYRO.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_CONFIG, 1);

	// ===
	u8TempInitReg = (	sMPU9250_Init->sFIFO.Enable			|	\
						sMPU9250_Init->sI2C.I2C_IF_DIS		|	\
						sMPU9250_Init->sI2C.I2C_MST_EN);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_USER_CTRL, 1);

}






// gyro acc
// mag

// =======================================================================================================
// ===  ODCZYT DANYCH
void MPU9250_ReadGyro(void){

}

void MPU9250_ReadAcc(int16_t *pDataXYZ){
	int8_t pBuffer[6];
	uint8_t u8RegTempVal	=	0;
	uint8_t u8ValueCounter	=	0;
	float	fSensitivity	=	1;
	float fTempValue 		=	0;

	//MPU9250_Rx(&u8RegTempVal, )
	MPU9250_Rx((uint8_t*)&pBuffer[0], MPU9250_ADDR_ACCEL_XOUT_L, 1);
	MPU9250_Rx((uint8_t*)&pBuffer[1], MPU9250_ADDR_ACCEL_XOUT_H, 1);
	MPU9250_Rx((uint8_t*)&pBuffer[2], MPU9250_ADDR_ACCEL_YOUT_L, 1);
	MPU9250_Rx((uint8_t*)&pBuffer[3], MPU9250_ADDR_ACCEL_YOUT_H, 1);
	MPU9250_Rx((uint8_t*)&pBuffer[4], MPU9250_ADDR_ACCEL_ZOUT_L, 1);
	MPU9250_Rx((uint8_t*)&pBuffer[5], MPU9250_ADDR_ACCEL_ZOUT_H, 1);

	for(u8ValueCounter = 0; u8ValueCounter < 3; u8ValueCounter++){
		fTempValue = ((pBuffer[2*u8ValueCounter+1] << 8) | (0x0ff & pBuffer[2*u8RegTempVal]) )*fSensitivity; // razy czułość
		pDataXYZ[u8ValueCounter] =(int16_t)fTempValue;
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
void dummy(uint8_t DUM){
	MPU9250_Tx(&DUM, MPU9250_ADDR_USER_CTRL, 1);
}
