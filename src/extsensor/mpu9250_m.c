/*
 * mpu9250_m.c
 *
 *  Created on: 27 sty 2016
 *      Author: mike
 */

//#include <stdint.h>

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "mpu9250_m.h"



// =======================================================================================================
// zmienne globalne
extern SPI_HandleTypeDef	hspi3;

// =======================================================================================================
// zmienne prywatne



// =======================================================================================================
// funkcje prywatne

static void Init_SPI3(void){

    if(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_RESET){
    	hspi3.Instance = SPI3;
		hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		hspi3.Init.Direction = SPI_DIRECTION_2LINES;
		hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
		hspi3.Init.CRCPolynomial = 7;
		hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi3.Init.NSS = SPI_NSS_HARD_OUTPUT;
		hspi3.Init.TIMode = SPI_TIMODE_DISABLED;
		hspi3.Init.Mode = SPI_MODE_MASTER;
		HAL_SPI_Init(&hspi3);

    }
}


// =======================================================================================================
// funkcje publiczne




//  ===  KONFIGURACJA


//  ===  INICJALIZACJA

	/*
	 *  s.26 W trybie SPI nie ma dostępu do trybu passtrough AUX iic
	 * s.25 uC musi skonfigurować mag (Aux iic w trybie passtrough)
	 * s. 32 W trybie SPI należy ustawić bit I2C_IF_DIS w celu wyłączenia interfejsu I2C. Należy to zrobić w określonym momencie (pkt 6.3)
	 *
	 */

// SPI
void MPU9250_Init(void){



	// Init SPI
	/*
	 * Który SPI?
	 * 	SPI3
	 * jakie piny?
	 * 	SCK 		PB3	(I2S3_CK)
	 * 	MISO 		PB4	(JTAG NJTRST)
	 * 	MOSI 		PB5	(I2S3_SD)
	 * 	NSS (CS) 	PA4	(I2S3_WS)
	 */
	Init_SPI3();

	// Init Acc
	// Init Gyro
	// Init Mag
}






// gyro acc
// mag

  // =======================================================================================================
//  ===  DANE POMIAROWE

  // =======================================================================================================
//  ===  KALIBRACJA
