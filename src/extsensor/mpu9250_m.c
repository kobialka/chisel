/*
 * mpu9250_m.c
 *
 *  Created on: 27 sty 2016
 *      Author: mike
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "mpu9250_m.h"

// ==================================================================================================================================================
// Typy danych




// ==================================================================================================================================================
// Definicje

/* Read/Write command */
#define READWRITE_CMD                     ((uint8_t)0x80)

/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                        ((uint8_t)0x00)

#define SPI_MPU9250_TIMEOUT_MAX                 	(0x1000)

#define AK8963RxBuff_ST1					((uint8_t)0x00)
#define AK8963RxBuff_XL						((uint8_t)0x01)
#define AK8963RxBuff_XH						((uint8_t)0x02)
#define AK8963RxBuff_YL						((uint8_t)0x03)
#define AK8963RxBuff_YH						((uint8_t)0x04)
#define AK8963RxBuff_ZL						((uint8_t)0x05)
#define AK8963RxBuff_ZH						((uint8_t)0x06)
#define AK8963RxBuff_ST2					((uint8_t)0x07)





// ==================================================================================================================================================
// zmienne globalne





// ==================================================================================================================================================
// zmienne prywatne
static uint32_t u32SpiMPU9250Timeout = SPI_MPU9250_TIMEOUT_MAX;
static uint8_t pfAK8963_CallibrationData[3] = {0.0, 0.0, 0.0};



// ==================================================================================================================================================
// funkcje prywatne
static void		Init_MPU9250_SPI(void);
static uint8_t	MPU9250_SPI_TxRx(uint8_t u8Byte);
static void		MPU9250_SPI_Error(void);
static void		MPU9250_Rx(uint8_t *pu8RxBuff, uint8_t u8RxAddr, uint8_t u8ByteNr);
static void 	MPU9250_Tx(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint8_t u8ByteNr);
static uint8_t	MPU9250_Rx_FromAK8963(uint8_t *pu8RxBuff, uint8_t u8RxAddr, uint8_t u8ByteNr);
static uint8_t 	MPU9250_Tx_ToAK8963(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint8_t u8ByteNr);
static void 	MPU9250_ReadGyro(int16_t *pi16DataXYZ);
static void 	MPU9250_ReadAcc(int16_t *pDataXYZ);
static void 	MPU9250_ReadMag(int16_t *pDataXYZ);




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
		 * 	 Przy pierwszym transferze po inicjalizacji sciągnięcie lini zegarowej do poziomu niskiego
		 * 	 jest widocznie opóźnione w stosunku do aktywacji odbiornika (sciągnięcia CS).
		 *
		 * 	 Dekoder w moim analizatorze logicznym zgłasza błąd. Być może interfejs SPI
		 * 	 w odbiorniku też będzie widział jakiś problem.
		 *
		 * 	 Tego problemu nie ma w przypadku wykorzystania kodu z biblioteki STM do obsługi acc
		 * 	 na płytce discovery. Do sprawdzenia oscyloskopem.
		 */

		MPU9250_CS_HIGH();
		HAL_SPI_TransmitReceive(&hspi3_MPU9250, (uint8_t*)&u32Temp, (uint8_t*)&u32Temp, 1, u32SpiMPU9250Timeout);		//
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


static void MPU9250_Rx(uint8_t *pu8RxBuff, uint8_t u8RxAddr, uint8_t u8ByteNr){
	uint32_t u32Cycles;
	u8RxAddr |= (uint8_t)READWRITE_CMD;

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8RxAddr);
	while(u8ByteNr > 0){
		*pu8RxBuff = MPU9250_SPI_TxRx(DUMMY_BYTE);
		u8ByteNr--;
		pu8RxBuff++;
	}
	MPU9250_CS_HIGH();
	u32Cycles = (uint32_t)SystemCoreClock/1000000/7;

	while(u32Cycles--){
		__asm("nop;");
	};
}

// pu8TxBuff	- dane do wysłania
// u8TxAddr		- adre pod który mają być zapisane
// u8ByteNr		- ilość bajtów do wysłania

static void MPU9250_Tx(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint8_t u8ByteNr){
	uint32_t u32Cycles;													// liczba taktów zegara na 1[us] podzielona  następnie ze wględu na narzut od ''nop'' i pętli.

	MPU9250_CS_LOW();
	MPU9250_SPI_TxRx(u8TxAddr);
	while(u8ByteNr >= 1){
		MPU9250_SPI_TxRx(*pu8TxBuff);
		u8ByteNr--;
		pu8TxBuff++;
	}
	MPU9250_CS_HIGH();

	// Opóźnienie wprowadzone aby zapewnić minimalną szerokość impulsu pomiędzy transakcjami. Kiedy impuls trwa mniej niż 1[us] MPU9250 gubi się i dane odczytywane często nie są poprawne.
	u32Cycles = (uint32_t)SystemCoreClock/1000000/7;	// liczba taktów zegara na 1[us] podzielona  następnie ze wględu na narzut od ''nop'' i pętli.

	while(u32Cycles--){
		__asm("nop;");
	};
}


// pu8TxBuff	- dane do wysłania
// u8TxAddr		- adre pod który mają być zapisane
// u8ByteNr		- ilość bajtów do wysłania


static uint8_t	MPU9250_Rx_FromAK8963(uint8_t *pu8RxBuff, uint8_t u8RxRegAddr, uint8_t u8ByteNr){

	uint8_t u8DataCounter;
	uint8_t u8Timeout = 0;
	uint8_t u8TempData = 0;

	if (0 == u8ByteNr){
		return -3;
	}

	// 1. Zapis adresu i2c AK8963 do rejestru I2C_SLV4_ADDR
	u8TempData = (AK8963_I2C_ADDR | I2C_READ);
	MPU9250_Tx(&u8TempData, MPU9250_ADDR_I2C_SLV4_ADDR, 1);
//	HAL_Delay(1);

	u8DataCounter = 0;
	while(u8DataCounter < u8ByteNr){
		// 2. Zapis adresu czytanego rejestru AK8963 do I2C_SLV4_REG
		u8TempData = u8RxRegAddr + u8DataCounter;
		MPU9250_Tx(&u8TempData, MPU9250_ADDR_I2C_SLV4_REG, 1);
//		HAL_Delay(1);

		// 3. Aktywacja SLV4 w I2C_SLV4_EN
		u8TempData = MPU9250_BM_I2C_SLV4_CTRL_I2C_SLV4_EN;
		MPU9250_Tx(&u8TempData, MPU9250_ADDR_I2C_SLV4_CTRL, 1);
//		HAL_Delay(1);

		do {
			if (u8Timeout++ > 50){
				return -1;
			}
			// Odczyt statusu
			MPU9250_Rx(&u8TempData, MPU9250_ADDR_I2C_MST_STATUS, 1);
			HAL_Delay(1);
		} while (0 == (u8TempData & MPU9250_BM_I2C_MST_STATUS_I2C_SLV4_DONE));

		if ((u8TempData & MPU9250_BM_I2C_MST_STATUS_I2C_SLV4_NACK) ){
			return -2;
		}

		MPU9250_Rx( (pu8RxBuff + u8DataCounter) , MPU9250_ADDR_I2C_SLV4_DI, 1);
		u8DataCounter++;
	}
	return 0;
}

static uint8_t 	MPU9250_Tx_ToAK8963(uint8_t *pu8TxBuff, uint8_t u8TxAddr, uint8_t u8ByteNr){
	uint8_t u8Timeout = 0;
	uint8_t u8TempTxData;
	uint8_t u8DataCounter;

	// 1. Zapis adresu i2c AK8963 do rejestru I2C_SLV4_ADDR
	u8TempTxData = (AK8963_I2C_ADDR | I2C_WRITE);
	MPU9250_Tx(&u8TempTxData, MPU9250_ADDR_I2C_SLV4_ADDR, 1);
	HAL_Delay(1);

	u8DataCounter = 0;
	while(u8DataCounter < u8ByteNr){
		// 2. Zapis adresu rejestru AK8963 do I2C_SLV4_REG
		u8TempTxData = u8TxAddr + u8DataCounter;
		MPU9250_Tx(&u8TempTxData, MPU9250_ADDR_I2C_SLV4_REG, 1);
		HAL_Delay(1);

		// 3. Zapis danej do I2C_SLV4_DO
		u8TempTxData = pu8TxBuff[u8DataCounter];
  		MPU9250_Tx(&u8TempTxData, MPU9250_ADDR_I2C_SLV4_DO, 1);
		HAL_Delay(1);

		// 4. Aktywacja SLV4 w I2C_SLV4_EN
		u8TempTxData = MPU9250_BM_I2C_SLV4_CTRL_I2C_SLV4_EN;
		MPU9250_Tx(&u8TempTxData, MPU9250_ADDR_I2C_SLV4_CTRL, 1);
		HAL_Delay(1);
		MPU9250_Rx(&u8TempTxData, MPU9250_ADDR_I2C_SLV4_CTRL, 1);


		do {
			if (u8Timeout++ > 50){
				return -1;
			}
			MPU9250_Rx(&u8TempTxData, MPU9250_ADDR_I2C_MST_STATUS, 1);
			HAL_Delay(1);
		} while (0 == (u8TempTxData & MPU9250_BM_I2C_MST_STATUS_I2C_SLV4_DONE));

		if (u8TempTxData & MPU9250_BM_I2C_MST_STATUS_I2C_SLV4_NACK){
			return -2;
		}
		u8DataCounter++;
	}
	return 0;
}


static void MPU9250_SPI_Error(void){
  /* De-initialize the SPI comunication bus */
  HAL_SPI_DeInit(&hspi3_MPU9250);

  /* Re-Initiaize the SPI comunication bus */
  Init_MPU9250_SPI();
}




// ==================================================================================================================================================
// funkcje publiczne

	/*
	 *  s.26 W trybie SPI nie ma dostępu do trybu passtrough AUX iic
	 * s.25 uC musi skonfigurować mag (Aux iic w trybie passtrough)
	 * s. 32 W trybie SPI należy ustawić bit I2C_IF_DIS w celu wyłączenia interfejsu I2C. Należy to zrobić w określonym momencie (pkt 6.3)
	 *
	 */






// ==================================================================================================================================================
// ===  ODCZYT DANYCH POMIAROWYCH
static void MPU9250_ReadGyro(int16_t *pi16DataXYZ){
	uint8_t		pu8Buffer[6];
	uint8_t 	u8RegTempVal	=	0;
	uint8_t 	u8ValueCounter	=	0;
	float		fSensitivity	=	1;
	float 		fTempValue 		=	0;

	MPU9250_Rx(&u8RegTempVal, MPU9250_ADDR_GYRO_CONFIG, 1);
	u8RegTempVal &= MPU9250_CONF_GYRO_FULLSCALE_MASK;	// FULL SCALE

	MPU9250_Rx(&pu8Buffer[1], MPU9250_ADDR_GYRO_XOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[0], MPU9250_ADDR_GYRO_XOUT_L, 1);
	MPU9250_Rx(&pu8Buffer[3], MPU9250_ADDR_GYRO_YOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[2], MPU9250_ADDR_GYRO_YOUT_L, 1);
	MPU9250_Rx(&pu8Buffer[5], MPU9250_ADDR_GYRO_ZOUT_H, 1);
	MPU9250_Rx(&pu8Buffer[4], MPU9250_ADDR_GYRO_ZOUT_L, 1);


	switch (u8RegTempVal) {
		case MPU9250_CONF_GYRO_FULLSCALE_250dps:
			fSensitivity = MPU9250_GYRO_SENSITIVITY_FOR_250dps_SCALE;
			break;
		case MPU9250_CONF_GYRO_FULLSCALE_500dps:
			fSensitivity = MPU9250_GYRO_SENSITIVITY_FOR_500dps_SCALE;
			break;
		case MPU9250_CONF_GYRO_FULLSCALE_1000dps:
			fSensitivity = MPU9250_GYRO_SENSITIVITY_FOR_1000dps_SCALE;
			break;
		case MPU9250_CONF_GYRO_FULLSCALE_2000dps:
			fSensitivity = MPU9250_GYRO_SENSITIVITY_FOR_2000dps_SCALE;
			break;
		default:
			fSensitivity = MPU9250_GYRO_SENSITIVITY_FOR_250dps_SCALE;
			break;
	}

	for(u8ValueCounter = 0; u8ValueCounter < 3; u8ValueCounter++){
		fTempValue = (((int8_t)pu8Buffer[2*u8ValueCounter+1] << 8) | (0xff & (int8_t)pu8Buffer[2*u8ValueCounter]))*fSensitivity; // rzutowanie na float zachowuje znak!
		pi16DataXYZ[u8ValueCounter] =(int16_t)fTempValue;
	}

}


static void MPU9250_ReadAcc(int16_t *pi16DataXYZ){
	uint8_t		pu8Buffer[6];
	uint8_t 	u8RegTempVal	=	0;
	uint8_t 	u8ValueCounter	=	0;
	float		fSensitivity	=	1;
	float 		fTempValue 		=	0;

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

static void MPU9250_ReadMag(int16_t *pi16DataXYZ){
	uint8_t 	pu8TempRxBuff[8] 	= 	{0,0,0,0,0,0,0,0};
	uint8_t 	u8ValueCounter		=	0;
	float		fSensitivity		=	1;
	float 		fTempValue 			=	0;



	MPU9250_Rx(pu8TempRxBuff, MPU9250_ADDR_EXT_SENS_DATA_00, 8);

	// sprawdzić czy nowe dane gotowe
	if(1 == (pu8TempRxBuff[AK8963RxBuff_ST1] & AK8963_BM_ST1_DRDY)){

	}
	// sprawdź czy nie zgubiono pomiaru
	if(1 == (pu8TempRxBuff[AK8963RxBuff_ST1] & AK8963_BM_ST1_DOR)){

	}

	// sprawdzić czy pomiar poprawny

	if(1 == (pu8TempRxBuff[AK8963RxBuff_ST2] & AK8963_BM_ST2_HOFL) ){

	}
	else{
	// przeliczyć wartości

		switch (pu8TempRxBuff[AK8963RxBuff_ST2] >> 4) {
			case (AK8963_CONF_RESOLUTION_14bit >> 4):
				fSensitivity = MPU9250_MAG_SENSITIVITY_14bit;
				break;
			case (AK8963_CONF_RESOLUTION_16bit >> 4):
							fSensitivity = MPU9250_MAG_SENSITIVITY_16bit;
							break;
			default:
				fSensitivity = MPU9250_MAG_SENSITIVITY_14bit;
				break;
		}

		for(u8ValueCounter = 0; u8ValueCounter < 3; u8ValueCounter++){
			fTempValue = (((int8_t)pu8TempRxBuff[2*u8ValueCounter+1] << 8) | (0xff & (int8_t)pu8TempRxBuff[2*u8ValueCounter])) * fSensitivity * pfAK8963_CallibrationData[u8ValueCounter]; // rzutowanie na float zachowuje znak!
			pi16DataXYZ[u8ValueCounter] =(int16_t)fTempValue;
		}
	}
}




void MPU9250_ReadMeas9D(int16_t *pDataXYZ){
	uint8_t u8TestData = 0xaf;

//	MPU9250_ReadGyro(pDataXYZ);
	MPU9250_ReadMag(pDataXYZ);
//	MPU9250_Rx_FromAK8963(&u8TestData, AK8963_ADDR_WIA, 1);

//	u8TestData = 0xAF;
//	MPU9250_Tx_ToAK8963(&u8TestData, AK8963_ADDR_I2CDIS, 1);
//	MPU9250_Rx_FromAK8963(&u8TestData, AK8963_ADDR_CNTL1, 1);
//	u8TestData = 2;
//	MPU9250_Tx_ToAK8963(&u8TestData, AK8963_ADDR_CNTL1, 1);
//	MPU9250_Rx_FromAK8963(&u8TestData, AK8963_ADDR_CNTL1, 1);
//	u8TestData = 3;
//	MPU9250_Tx_ToAK8963(&u8TestData, AK8963_ADDR_CNTL1, 1);
//	MPU9250_Rx_FromAK8963(&u8TestData, AK8963_ADDR_CNTL1, 1);
}


void MPU9250_Init(tsMPU9250_InitTypedef * sMPU9250_Init){
	uint8_t		u8AK8963TxResult;
	uint8_t		u8TempInitReg;


	Init_MPU9250_SPI();

	// RESET UKŁADU
	u8TempInitReg = MPU9250_BM_PWR_MGMT_1_HARD_RESET;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);
	HAL_Delay(100);


	//  === 0x19 - SMPLRT_DIV
	u8TempInitReg =		sMPU9250_Init->Sample_Rate_Divider;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_SMPLRT_DIV, 1);


	// === 0x1A - CONFIG
	u8TempInitReg = (	sMPU9250_Init->sFIFO.Overlap		|	\
						sMPU9250_Init->External_Sync		|	\
						sMPU9250_Init->sGYRO.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_CONFIG, 1);


	//  === 0x18 - GYRO_CONFIG
	u8TempInitReg = (	sMPU9250_Init->sGYRO.Full_Scale		|	\
						sMPU9250_Init->sGYRO.Filter_Bypass);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_GYRO_CONFIG, 1);


	// === 0x1C - ACCEL_CONFIG
	u8TempInitReg = 	sMPU9250_Init->sACC.Full_Scale;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG, 1);


	// === 0x1D - ACCEL_CONFIG_2
	u8TempInitReg = (	sMPU9250_Init->sACC.Filter_Bypass	|	\
						sMPU9250_Init->sACC.Filter_BW);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_ACCEL_CONFIG_2, 1);


	// === 0x1E - LP_ACCEL_ODR

	// === 0x1F - WOM_THR
	u8TempInitReg = 	sMPU9250_Init->sACC.WakeOn_Threshold;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_WOM_THR, 1);


	// === 0x23 - FIFO_EN
	u8TempInitReg = 	sMPU9250_Init->sFIFO.Source_Select;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_FIFO_EN, 1);

	// === 0x24 I2C_MST_CTRL
	u8TempInitReg = (	sMPU9250_Init->sI2C.Multimaster_Enable		|	\
//						sMPU9250_Init->sFIFO.SLV3
						sMPU9250_Init->sI2C.Next_Slave_Mode			|	\
						sMPU9250_Init->sINTERRUPTS.Data_Ready_Waits_For_Ext_Sens	|	\
						sMPU9250_Init->sI2C.Master_Clock);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_MST_CTRL, 1);



	// === 0x37 - INT_PIN_CFG
	u8TempInitReg = (	sMPU9250_Init->sINTERRUPTS.INT_Pin_Logic_Level 		|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_PP_OD			|	\
						sMPU9250_Init->sINTERRUPTS.INT_Pin_Latch			|	\
						sMPU9250_Init->sINTERRUPTS.Clear_Status_On_Read		|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_Logic_Level		|	\
						sMPU9250_Init->sINTERRUPTS.FSYNC_INT_MODE_Enable	|	\
						sMPU9250_Init->sI2C.Bypass_Enable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_PIN_CFG, 1);


	// === 0x38 - INT_ENABLE
	u8TempInitReg =(	sMPU9250_Init->sINTERRUPTS.Enable_Wake_Up_Int		|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FIFO_Overflow_Int	|	\
						sMPU9250_Init->sINTERRUPTS.Enable_FSYNC_Int			|	\
						sMPU9250_Init->sINTERRUPTS.Enable_Raw_Data_Enable_Int);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_INT_ENABLE, 1);


	// === 0x67 - I2C_MST_DELAY_CTRL
	u8TempInitReg = (	sMPU9250_Init->sI2C.Ext_Sens_Data_Shadowing		|	\
						sMPU9250_Init->sI2C.Ext_Sens_Read_Delay_Enable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_MST_DELAY_CTRL, 1);

	// === 0x6A - USER CONT
	u8TempInitReg = (	sMPU9250_Init->sI2C.I2C_IF_DIS 		|	\
						sMPU9250_Init->sI2C.I2C_MST_EN		|	\
						sMPU9250_Init->sFIFO.Enable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_USER_CTRL, 1);


	// === 0x6B - PWR_MGMT_1
	u8TempInitReg = sMPU9250_Init->Clock_Source;

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_1, 1);	// Rejestr PWR_MGMT_1 poza wyborem zegara zawiera jescze kilka bitów konfiguracyjnych,


	// === 0x6C - PWR_MGMT_2
	u8TempInitReg = (	sMPU9250_Init->sACC.Axis_Disable 	|	\
						sMPU9250_Init->sGYRO.Axis_Disable);

	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_PWR_MGMT_2, 1);

	// MAG MAG


//	reset
	u8TempInitReg = AK8963_BM_CNTL2_SRST;
	MPU9250_Tx_ToAK8963(&u8TempInitReg, AK8963_ADDR_CNTL2, 1);
	HAL_Delay(100);

//	Dostęp i odczyt danych kalibracyjnych
	u8TempInitReg = AK8963_CONF_MODE_POWER_DOWN;
	MPU9250_Tx_ToAK8963(&u8TempInitReg,AK8963_ADDR_CNTL1, 1);

	u8TempInitReg = AK8963_CONF_MODE_FUSE_ROM_ACCESS;
	MPU9250_Tx_ToAK8963(&u8TempInitReg,AK8963_ADDR_CNTL1, 1);

	MPU9250_Rx_FromAK8963(pfAK8963_CallibrationData, AK8963_ADDR_ASAX, 3);
	pfAK8963_CallibrationData[0] = (((pfAK8963_CallibrationData[0] - 128) * 0.5) / 128) + 1;
	pfAK8963_CallibrationData[1] = (((pfAK8963_CallibrationData[1] - 128) * 0.5) / 128) + 1;
	pfAK8963_CallibrationData[2] = (((pfAK8963_CallibrationData[2] - 128) * 0.5) / 128) + 1;

	u8TempInitReg = AK8963_CONF_MODE_POWER_DOWN;
	MPU9250_Tx_ToAK8963(&u8TempInitReg,AK8963_ADDR_CNTL1, 1);

	u8TempInitReg = AK8963_CONF_MODE_CONTINUOUS_100Hz | sMPU9250_Init->sMAG.Resolution;
	MPU9250_Tx_ToAK8963(&u8TempInitReg, AK8963_ADDR_CNTL1, 1);


//	7. write SLV0_ADDR -  AK8963 | read
	u8TempInitReg = AK8963_I2C_ADDR | I2C_READ;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_SLV0_ADDR, 1);

//	8. write SLV0_REG AK8963_ST1
	u8TempInitReg = AK8963_ADDR_ST1;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_SLV0_REG, 1);

//	9. write SLV0_CTRL
	u8TempInitReg = 0x80 | 0x08;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_SLV0_CTRL, 1);

//	11. write slv4_CTRL 0x09 - master delay
	u8TempInitReg = 0x09;
	MPU9250_Tx(&u8TempInitReg, MPU9250_ADDR_I2C_SLV4_CTRL, 1);




}

uint8_t MPU9250_WhoAmI(void){
	uint8_t pu8Buffer;

	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	MPU9250_Rx((uint8_t*)&pu8Buffer, MPU9250_ADDR_WHO_AM_I, 1);
	return pu8Buffer;
}

// ==================================================================================================================================================
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
