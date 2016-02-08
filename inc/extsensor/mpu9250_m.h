/*
 * mpu9250_m.h
 *
 *  Created on: 27 sty 2016
 *      Author: mike
 */

#ifndef EXTSENSOR_MPU9250_M_H_
#define EXTSENSOR_MPU9250_M_H_

// =======================================================================================================
// Deklaracje zmiennych globalnych
extern SPI_HandleTypeDef			hspi3_MPU9250;


// =======================================================================================================
// Definicje


/*
 *	SPI3
 * 	SCK 		PB3	(I2S3_CK)
 * 	MISO 		PB4	(JTAG NJTRST)
 * 	MOSI 		PB5	(I2S3_SD)
 * 	SS (CS) 	PA4	(I2S3_WS)
 */

#define MPU9250_SPI_CLK_ENABLE()                 	__SPI3_CLK_ENABLE()
#define MPU9250_SPI_CLK_DISABLE()                	__SPI3_CLK_DISABLE()
#define MPU9250_SPI	                             	SPI3
#define MPU9250_SPI_AF                           	GPIO_AF6_SPI3

#define MPU9250_SPI_GPIO_CLK_ENABLE()            	__GPIOB_CLK_ENABLE()
#define MPU9250_SPI_GPIO_CLK_DISABLE()           	__GPIOB_CLK_DISABLE()
#define MPU9250_SPI_SCK_PIN                      	GPIO_PIN_3
#define MPU9250_SPI_MISO_PIN                     	GPIO_PIN_4
#define MPU9250_SPI_MOSI_PIN                     	GPIO_PIN_5
#define MPU9250_SPI_GPIO_PORT                    	GPIOB

#define MPU9250_SPI_CS_GPIO_CLK_ENABLE()          	__GPIOA_CLK_ENABLE()
#define MPU9250_SPI_CS_GPIO_CLK_DISABLE()         	__GPIOA_CLK_DISABLE()
#define MPU9250_SPI_CS_PIN							GPIO_PIN_4
#define MPU9250_SPI_CS_GPIO_PORT                 	GPIOA


/*
 *	SPI3
 * 	SCK 		PC10
 * 	MISO 		PC11
 * 	MOSI 		PC12
 * 	SS (CS) 	PA4	(I2S3_WS)
 */

//#define MPU9250_SPI	                             	SPI3
//#define MPU9250_SPI_CLK_ENABLE()                 	__SPI3_CLK_ENABLE()
//#define MPU9250_SPI_CLK_DISABLE()                	__SPI3_CLK_DISABLE()
//#define MPU9250_SPI_AF                           	GPIO_AF6_SPI3
//#define MPU9250_SPI_GPIO_PORT                    	GPIOC
//#define MPU9250_SPI_GPIO_CLK_ENABLE()            	__GPIOC_CLK_ENABLE()
//#define MPU9250_SPI_GPIO_CLK_DISABLE()           	__GPIOC_CLK_DISABLE()
//#define MPU9250_SPI_SCK_PIN                      	GPIO_PIN_10
//#define MPU9250_SPI_MISO_PIN                     	GPIO_PIN_11
//#define MPU9250_SPI_MOSI_PIN                     	GPIO_PIN_12
//#define MPU9250_SPI_CS_PIN							GPIO_PIN_4
//#define MPU9250_SPI_CS_GPIO_PORT                 	GPIOA
//#define MPU9250_SPI_CS_GPIO_CLK_ENABLE()          	__GPIOA_CLK_ENABLE()
//#define MPU9250_SPI_CS_GPIO_CLK_DISABLE()         	__GPIOA_CLK_DISABLE()


#define MPU9250_CS_LOW()       HAL_GPIO_WritePin(MPU9250_SPI_CS_GPIO_PORT, MPU9250_SPI_CS_PIN, GPIO_PIN_RESET)
#define MPU9250_CS_HIGH()      HAL_GPIO_WritePin(MPU9250_SPI_CS_GPIO_PORT, MPU9250_SPI_CS_PIN, GPIO_PIN_SET)




// MAPA REJESTRÓW MPU9250
#define MPU9250_ADDR_SELF_TEST_X_GYRO        0x00
#define MPU9250_ADDR_SELF_TEST_Y_GYRO        0x01
#define MPU9250_ADDR_SELF_TEST_Z_GYRO        0x02

#define MPU9250_ADDR_SELF_TEST_X_ACCEL       0x0D
#define MPU9250_ADDR_SELF_TEST_Y_ACCEL       0x0E
#define MPU9250_ADDR_SELF_TEST_Z_ACCEL       0x0F

#define MPU9250_ADDR_XG_OFFSET_H             0x13
#define MPU9250_ADDR_XG_OFFSET_L             0x14
#define MPU9250_ADDR_YG_OFFSET_H             0x15
#define MPU9250_ADDR_YG_OFFSET_L             0x16
#define MPU9250_ADDR_ZG_OFFSET_H             0x17
#define MPU9250_ADDR_ZG_OFFSET_L             0x18
#define MPU9250_ADDR_SMPLRT_DIV              0x19
#define MPU9250_ADDR_CONFIG                  0x1A
#define MPU9250_ADDR_GYRO_CONFIG             0x1B
#define MPU9250_ADDR_ACCEL_CONFIG            0x1C
#define MPU9250_ADDR_ACCEL_CONFIG_2          0x1D
#define MPU9250_ADDR_LP_ACCEL_ODR            0x1E
#define MPU9250_ADDR_WOM_THR                 0x1F

#define MPU9250_ADDR_FIFO_EN                 0x23
#define MPU9250_ADDR_I2C_MST_CTRL            0x24
#define MPU9250_ADDR_I2C_SLV0_ADDR           0x25
#define MPU9250_ADDR_I2C_SLV0_REG            0x26
#define MPU9250_ADDR_I2C_SLV0_CTRL           0x27
#define MPU9250_ADDR_I2C_SLV1_ADDR           0x28
#define MPU9250_ADDR_I2C_SLV1_REG            0x29
#define MPU9250_ADDR_I2C_SLV1_CTRL           0x2A
#define MPU9250_ADDR_I2C_SLV2_ADDR           0x2B
#define MPU9250_ADDR_I2C_SLV2_REG            0x2C
#define MPU9250_ADDR_I2C_SLV2_CTRL           0x2D
#define MPU9250_ADDR_I2C_SLV3_ADDR           0x2E
#define MPU9250_ADDR_I2C_SLV3_REG            0x2F
#define MPU9250_ADDR_I2C_SLV3_CTRL           0x30
#define MPU9250_ADDR_I2C_SLV4_ADDR           0x31
#define MPU9250_ADDR_I2C_SLV4_REG            0x32
#define MPU9250_ADDR_I2C_SLV4_DO             0x33
#define MPU9250_ADDR_I2C_SLV4_CTRL           0x34
#define MPU9250_ADDR_I2C_SLV4_DI             0x35
#define MPU9250_ADDR_I2C_MST_STATUS          0x36
#define MPU9250_ADDR_INT_PIN_CFG             0x37
#define MPU9250_ADDR_INT_ENABLE              0x38

#define MPU9250_ADDR_INT_STATUS              0x3A
#define MPU9250_ADDR_ACCEL_XOUT_H            0x3B
#define MPU9250_ADDR_ACCEL_XOUT_L            0x3C
#define MPU9250_ADDR_ACCEL_YOUT_H            0x3D
#define MPU9250_ADDR_ACCEL_YOUT_L            0x3E
#define MPU9250_ADDR_ACCEL_ZOUT_H            0x3F
#define MPU9250_ADDR_ACCEL_ZOUT_L            0x40
#define MPU9250_ADDR_TEMP_OUT_H              0x41
#define MPU9250_ADDR_TEMP_OUT_L              0x42
#define MPU9250_ADDR_GYRO_XOUT_H             0x43
#define MPU9250_ADDR_GYRO_XOUT_L             0x44
#define MPU9250_ADDR_GYRO_YOUT_H             0x45
#define MPU9250_ADDR_GYRO_YOUT_L             0x46
#define MPU9250_ADDR_GYRO_ZOUT_H             0x47
#define MPU9250_ADDR_GYRO_ZOUT_L             0x48
#define MPU9250_ADDR_EXT_SENS_DATA_00        0x49
#define MPU9250_ADDR_EXT_SENS_DATA_01        0x4A
#define MPU9250_ADDR_EXT_SENS_DATA_02        0x4B
#define MPU9250_ADDR_EXT_SENS_DATA_03        0x4C
#define MPU9250_ADDR_EXT_SENS_DATA_04        0x4D
#define MPU9250_ADDR_EXT_SENS_DATA_05        0x4E
#define MPU9250_ADDR_EXT_SENS_DATA_06        0x4F
#define MPU9250_ADDR_EXT_SENS_DATA_07        0x50
#define MPU9250_ADDR_EXT_SENS_DATA_08        0x51
#define MPU9250_ADDR_EXT_SENS_DATA_09        0x52
#define MPU9250_ADDR_EXT_SENS_DATA_10        0x53
#define MPU9250_ADDR_EXT_SENS_DATA_11        0x54
#define MPU9250_ADDR_EXT_SENS_DATA_12        0x55
#define MPU9250_ADDR_EXT_SENS_DATA_13        0x56
#define MPU9250_ADDR_EXT_SENS_DATA_14        0x57
#define MPU9250_ADDR_EXT_SENS_DATA_15        0x58
#define MPU9250_ADDR_EXT_SENS_DATA_16        0x59
#define MPU9250_ADDR_EXT_SENS_DATA_17        0x5A
#define MPU9250_ADDR_EXT_SENS_DATA_18        0x5B
#define MPU9250_ADDR_EXT_SENS_DATA_19        0x5C
#define MPU9250_ADDR_EXT_SENS_DATA_20        0x5D
#define MPU9250_ADDR_EXT_SENS_DATA_21        0x5E
#define MPU9250_ADDR_EXT_SENS_DATA_22        0x5F
#define MPU9250_ADDR_EXT_SENS_DATA_23        0x60

#define MPU9250_ADDR_I2C_SLV0_DO             0x63
#define MPU9250_ADDR_I2C_SLV1_DO             0x64
#define MPU9250_ADDR_I2C_SLV2_DO             0x65
#define MPU9250_ADDR_I2C_SLV3_DO             0x66
#define MPU9250_ADDR_I2C_MST_DELAY_CTRL      0x67
#define MPU9250_ADDR_SIGNAL_PATH_RESET       0x68
#define MPU9250_ADDR_MOT_DETECT_CTRL         0x69
#define MPU9250_ADDR_USER_CTRL               0x6A
#define MPU9250_ADDR_PWR_MGMT_1              0x6B
#define MPU9250_ADDR_PWR_MGMT_2              0x6C

#define MPU9250_ADDR_FIFO_COUNTH             0x72
#define MPU9250_ADDR_FIFO_COUNTL             0x73
#define MPU9250_ADDR_FIFO_R_W                0x74
#define MPU9250_ADDR_WHO_AM_I                0x75
#define MPU9250_ADDR_XA_OFFSET_H             0x77
#define MPU9250_ADDR_XA_OFFSET_L             0x78

#define MPU9250_ADDR_YA_OFFSET_H             0x7A
#define MPU9250_ADDR_YA_OFFSET_L             0x7B

#define MPU9250_ADDR_ZA_OFFSET_H             0x7D
#define MPU9250_ADDR_ZA_OFFSET_L             0x7E
//
#define MPU9250_ADDR_I2C_READ 0x80

//Magnetometer register maps
#define MPU9250_ADDR_AK8963_WIA                 0x00
#define MPU9250_ADDR_AK8963_INFO                0x01
#define MPU9250_ADDR_AK8963_ST1                 0x02
#define MPU9250_ADDR_AK8963_XOUT_L              0x03
#define MPU9250_ADDR_AK8963_XOUT_H              0x04
#define MPU9250_ADDR_AK8963_YOUT_L              0x05
#define MPU9250_ADDR_AK8963_YOUT_H              0x06
#define MPU9250_ADDR_AK8963_ZOUT_L              0x07
#define MPU9250_ADDR_AK8963_ZOUT_H              0x08
#define MPU9250_ADDR_AK8963_ST2                 0x09
#define MPU9250_ADDR_AK8963_CNTL                0x0A
#define MPU9250_ADDR_AK8963_CNTL2               0x0B
#define MPU9250_ADDR_AK8963_RSV                 0x0B //DO NOT ACCESS <MPU9250_AK8963_CNTL2>
#define MPU9250_ADDR_AK8963_ASTC                0x0C
#define MPU9250_ADDR_AK8963_TS1                 0x0D //DO NOT ACCESS
#define MPU9250_ADDR_AK8963_TS2                 0x0E //DO NOT ACCESS
#define MPU9250_ADDR_AK8963_I2CDIS              0x0F
#define MPU9250_ADDR_AK8963_ASAX                0x10
#define MPU9250_ADDR_AK8963_ASAY                0x11
#define MPU9250_ADDR_AK8963_ASAZ                0x12


// ==============================================================
// I2C MASTER Podział wewnętrznego zagara 8MHz
#define MPU9250_CONF_I2C_MST_CLOCK_400kHz				((uint8_t)0x0d)

#define MPU9250_CONF_I2C_DISABLE_MODULE_YES				((uint8_t)0x10)
#define MPU9250_CONF_I2C_DISABLE_MODULE_NO				((uint8_t)0x00)
#define MPU9250_CONF_I2C_MASTER_MODE_YES				((uint8_t)0x20)
#define MPU9250_CONF_I2C_MASTER_MODE_NO					((uint8_t)0x00)
#define MPU9250_CONF_I2C_BYPASS_PIN_YES					((uint8_t)0x02)
#define MPU9250_CONF_I2C_BYPASS_PIN_NO					((uint8_t)0x00)

// FIFO aktywacja modułów
#define	MPU9250_CONF_FIFO_SOURCE_TEMP_SENS				((uint8_t)0x80)
#define MPU9250_CONF_FIFO_SOURCE_GYRO_X					((uint8_t)0x40)
#define MPU9250_CONF_FIFO_SOURCE_GYRO_Y					((uint8_t)0x20)
#define MPU9250_CONF_FIFO_SOURCE_GYRO_Z					((uint8_t)0x10)
#define MPU9250_CONF_FIFO_SOURCE_ACC_ALL				((uint8_t)0x08)
#define MPU9250_CONF_FIFO_SOURCE_I2C_SLAVE_2			((uint8_t)0x04)
#define	MPU9250_CONF_FIFO_SOURCE_I2C_SLAVE_1			((uint8_t)0x02)
#define MPU9250_CONF_FIFO_SOURCE_I2C_SLAVE_0			((uint8_t)0x01)	// See I2C_MST_CTRL register to enable this feature for SLAVE_3.

// FIFO nadpisywanie najstarszych wartości
#define	MPU9250_CONF_FIFO_OVERLAP_YES					((uint8_t)0x00)
#define	MPU9250_CONF_FIFO_OVERLAP_NO					((uint8_t)0x40)
#define	MPU9250_CONF_FIFO_ENABLE_YES					((uint8_t)0x40)
#define	MPU9250_CONF_FIFO_ENABLE_NO						((uint8_t)0x00)
// GYRO zakres
#define	MPU9250_CONF_GYRO_FULLSCALE_250dps				((uint8_t)0x00)
#define	MPU9250_CONF_GYRO_FULLSCALE_500dps				((uint8_t)0x08)
#define	MPU9250_CONF_GYRO_FULLSCALE_1000dps				((uint8_t)0x10)
#define	MPU9250_CONF_GYRO_FULLSCALE_2000dps				((uint8_t)0x18)
#define	MPU9250_CONF_GYRO_FULLSCALE_MASK				((uint8_t)0x18)

#define MPU9250_GYRO_SENSITIVITY_FOR_250dps_SCALE		(((float)2*250)/65536*1000)	// 1[mdps]/LSB
#define MPU9250_GYRO_SENSITIVITY_FOR_500dps_SCALE		(((float)2*500)/65536*1000)
#define MPU9250_GYRO_SENSITIVITY_FOR_1000dps_SCALE		(((float)2*1000)/65536*1000)
#define MPU9250_GYRO_SENSITIVITY_FOR_2000dps_SCALE		(((float)2*2000)/65536*1000)


// GYRO częstotliwość graniczna cyfrowego filtru dolnoprzepustowego
#define	MPU9250_CONF_GYRO_FILTER_BW_3600Hz				((uint8_t)0x07)
#define MPU9250_CONF_GYRO_FILTER_BW_250Hz				((uint8_t)0x00)
#define	MPU9250_CONF_GYRO_FILTER_BW_184Hz				((uint8_t)0x01)
#define MPU9250_CONF_GYRO_FILTER_BW_92Hz				((uint8_t)0x02)
#define	MPU9250_CONF_GYRO_FILTER_BW_41Hz				((uint8_t)0x03)
#define	MPU9250_CONF_GYRO_FILTER_BW_20Hz				((uint8_t)0x04)
#define	MPU9250_CONF_GYRO_FILTER_BW_10Hz				((uint8_t)0x05)
#define	MPU9250_CONF_GYRO_FILTER_BW_5Hz					((uint8_t)0x06)

// GYRO obejście filtru DP FCHOICE_B (odwrócony FCHOICE)
#define MPU9250_CONF_GYRO_FILTER_BYPASS_NO				((uint8_t)0x00)
#define MPU9250_CONF_GYRO_FILTER_BYPASS_YES_BW_8800Hz	((uint8_t)0x01)
#define MPU9250_CONF_GYRO_FILTER_BYPASS_YES_BW_3600Hz	((uint8_t)0x02)

// GYRO aktywacja osi

// ACC zakres
#define MPU9250_CONF_ACC_FULLSCALE_2g					((uint8_t)0x00)
#define MPU9250_CONF_ACC_FULLSCALE_4g					((uint8_t)0x08)
#define MPU9250_CONF_ACC_FULLSCALE_8g					((uint8_t)0x10)
#define MPU9250_CONF_ACC_FULLSCALE_16g					((uint8_t)0x18)
#define MPU9250_CONF_ACC_FULLSCALE_MASK					((uint8_t)0x18)

// CZUŁOŚĆ ACC W [mG]
#define MPU9250_ACC_SENSITIVITY_FOR_2g_SCALE			(((float)2*2)/65536*1000)		// 1[mg]/LSB
#define MPU9250_ACC_SENSITIVITY_FOR_4g_SCALE			(((float)2*4)/65536*1000)
#define MPU9250_ACC_SENSITIVITY_FOR_8g_SCALE			(((float)2*8)/65536*1000)
#define MPU9250_ACC_SENSITIVITY_FOR_16g_SCALE			(((float)2*16)/65536*1000)

//#define MPU9250_ACC_SENSITIVITY_FOR_2g_SCALE			(((float)2*2)/65536*1000000)		// 1[ug]/LSB
//#define MPU9250_ACC_SENSITIVITY_FOR_4g_SCALE			(((float)2*4)/65536*1000000)
//#define MPU9250_ACC_SENSITIVITY_FOR_8g_SCALE			(((float)2*8)/65536*1000000)
//#define MPU9250_ACC_SENSITIVITY_FOR_16g_SCALE			(((float)2*16)/65536*1000000)


// ACC częstotliwość graniczna cyfrowego filtru dolnoprzepustowego
#define MPU9250_CONF_ACC_FILTER_BW_460Hz				((uint8_t)0x00)
#define	MPU9250_CONF_ACC_FILTER_BW_184Hz				((uint8_t)0x01)
#define MPU9250_CONF_ACC_FILTER_BW_92Hz					((uint8_t)0x02)
#define	MPU9250_CONF_ACC_FILTER_BW_41Hz					((uint8_t)0x03)
#define	MPU9250_CONF_ACC_FILTER_BW_20Hz					((uint8_t)0x04)
#define	MPU9250_CONF_ACC_FILTER_BW_10Hz					((uint8_t)0x05)
#define	MPU9250_CONF_ACC_FILTER_BW_5Hz					((uint8_t)0x06)
#define	MPU9250_CONF_ACC_FILTER_BW_460Hzb				((uint8_t)0x07)

// ACC obejście filtru DP
#define MPU9250_CONF_ACC_FILTER_BYPASS_NO				((uint8_t)0x00)
#define MPU9250_CONF_ACC_FILTER_BYPASS_YES				((uint8_t)0x08)

// ACC osie
#define MPU9250_CONF_ACC_AXIS_X							((uint8_t)0x20)
#define MPU9250_CONF_ACC_AXIS_Y							((uint8_t)0x10)
#define MPU9250_CONF_ACC_AXIS_Z							((uint8_t)0x08)
#define MPU9250_CONF_ACC_AXIS_ALL						((uint8_t)0x00)

#define MPU9250_CONF_GYRO_AXIS_X						((uint8_t)0x04)
#define MPU9250_CONF_GYRO_AXIS_Y						((uint8_t)0x02)
#define MPU9250_CONF_GYRO_AXIS_Z						((uint8_t)0x01)
#define MPU9250_CONF_GYRO_AXIS_ALL_ON					((uint8_t)0x00)

// RESET
#define MPU9250_CONF_HARD_RESET							((uint8_t)0x80)

// ZEGAR GŁÓWNY
#define MPU9250_CONF_CLOCK_SOURCE_INTERNAL_20MHz		((uint8_t)0x00)
#define MPU9250_CONF_CLOCK_SOURCE_AUTOSELECT			((uint8_t)0x01)
#define MPU9250_CONF_CLOCK_SOURCE_STOP					((uint8_t)0x07)

// ZEWNĘTRZNA SYNCHRONIZACJA
#define MPU9250_CONF_EXT_SYNC_DISABLED					((uint8_t)0x00)
// dopisać

// PRZERWANIA
#define MPU9250_CONF_INT_PIN_LOGIC_ACTIVE_LVL_HIGH		((uint8_t)0x00)
#define MPU9250_CONF_INT_PIN_LOGIC_ACTIVE_LVL_LOW		((uint8_t)0x80)
#define MPU9250_CONF_INT_PIN_PP							((uint8_t)0x00)
#define MPU9250_CONF_INT_PIN_OD							((uint8_t)0x40)
#define MPU9250_CONF_INT_PIN_LATCH_YES					((uint8_t)0x20)
#define MPU9250_CONF_INT_PIN_LATCH_NO_PULSE_50us		((uint8_t)0x00)
#define MPU9250_CONF_INT_CLEAR_STATUS_RX_ANY			((uint8_t)0x10)
#define MPU9250_CONF_INT_CLEAR_STATUS_RX_STATUS			((uint8_t)0x00)
#define MPU9250_CONF_INT_FSYNC_LOGIC_ACTIVE_LVL_LOW		((uint8_t)0x08)
#define MPU9250_CONF_INT_FSYNC_LOGIC_ACTIVE_LVL_HIGH	((uint8_t)0x00)
#define MPU9250_CONF_INT_FSYNC_INT_ENABLE_YES			((uint8_t)0x04)
#define MPU9250_CONF_INT_FSYNC_INT_ENABLE_NO			((uint8_t)0x00)


// =======================================================================================================
// typy zmiennych


typedef struct{

	uint8_t Sample_Rate_Divider;
	uint8_t	Clock_Source;
	uint8_t External_Sync;

	struct{
		uint8_t Master_Clock;
		uint8_t	I2C_MST_EN;
		uint8_t	I2C_IF_DIS;
		uint8_t Bypass_Enable;
		uint8_t I2C_MST_DELAY_CTRL;
	}sI2C;

	struct{
		uint8_t	INT_Pin_Logic_Level;
		uint8_t	INT_Pin_PP_OD;
		uint8_t	INT_Pin_Latch;
		uint8_t Clear_Status_On_Read;
		uint8_t FSYNC_Logic_Level;
		uint8_t FSYNC_INT_MODE_Enable;
		uint8_t Enable_Wake_Up_Int;
		uint8_t	Enable_FIFO_Overflow_Int;
		uint8_t	Enable_FSYNC_Int;
		uint8_t	Enable_Raw_Data_Enable_Int;
	}sINTERRUPTS;

	struct{
		uint8_t Overlap;
		uint8_t	Enable;
		uint8_t	Source_Select;
	}sFIFO;


	struct{
		uint8_t Full_Scale;
		uint8_t Filter_BW;
		uint8_t Filter_Bypass;
		uint8_t	Axis_Disable;
		uint8_t	WakeOn_Threshold;		//0-1020[mg]
	}sACC;

	struct{
		uint8_t Full_Scale;
		uint8_t Filter_BW;
		uint8_t Filter_Bypass;
		uint8_t	Axis_Disable;
	}sGYRO;

	struct{
		uint8_t DummyParam;

	}sMAG;

//	struct{
//		uint8_t Addres;
//		uint8_t Readout_Enable;
//	}sI2C_Slave_0;
//
//	struct{
//		uint8_t Addres;
//	}sI2C_Slave_1;
//
//	struct{
//		uint8_t Addres;
//	}sI2C_Slave_2;
//
//	struct{
//		uint8_t Addres;
//	}sI2C_Slave_3;
}tsMPU9250_InitTypedef;




// =======================================================================================================
// deklaracje zmiennych




// =======================================================================================================
// funkcje
void Sensor_MPU9250_Init();
void MPU9250_ReadGyro(int16_t *pi16DataXYZ);
void MPU9250_ReadAcc(int16_t *pDataXYZ);
void MPU9250_ReadMag(void);
uint8_t MPU9250_WhoAmI(void);

#endif /* EXTSENSOR_MPU9250_M_H_ */
