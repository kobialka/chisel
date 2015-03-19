/*
 * lib_LIS3DSH.h
 *
 *  Created on: 7 sty 2015
 *      Author: mike
 */

#ifndef LIB_LIS3DSH_H_
#define LIB_LIS3DSH_H_

#define ACC_INFO1_ADDR 	(0x0D)
#define ACC_INFO2 		(0x0E)
#define ACC_WHO_AM_I 	(0x0F)

/* CTRL_REG3 */
#define ACC_CTRL3_ADDR			(0x23)
#define ACC_CTRL3_DR_EN 	(1<<7)
#define ACC_CTRL3_IEA		(1<<6)
#define ACC_CTRL3_IEL		(1<<5)
#define ACC_CTRL3_INT2_EN	(1<<4)
#define ACC_CTRL3_INT1_EN	(1<<3)
#define ACC_CTRL3_VFILT	(1<<2)
#define ACC_CTRL3_STRT	(1)


/* CTRL_REG4 */
#define ACC_CTRL4_ADDR			(0x20)
#define ACC_CTRL4_ODR_OFFSET	(4)
#define ACC_CTRL4_ODR_PDOWN		(0 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_3			(1 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_6			(2 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_12		(3 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_25		(4 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_50 		(5 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_100		(6 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_400		(7 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_800		(8 << ACC_ODR_OFFSET)
#define ACC_CTRL4_ODR_1600		(9 << ACC_ODR_OFFSET)
#define ACC_CTRL4_BDU			(1 << 3)
#define ACC_Z_CTRL4_ENABLE		(1 << 2)
#define ACC_Y_CTRL4_ENABLE		(1 << 1)
#define ACC_X_CTRL4_ENABLE		(1 << 0)

/* CTRL_REG5 */
#define ACC_CTRL5_ADDR			(0x24)
#define ACC_CTRL5_BW_OFFSET		(6)
#define ACC_CTRL5_BW_800		(0 << ACC_BW_OFFSET)
#define ACC_CTRL5_BW_400		(1 << ACC_BW_OFFSET)
#define ACC_CTRL5_BW_200		(2 << ACC_BW_OFFSET)
#define ACC_CTRL5_BW_50			(3 << ACC_BW_OFFSET)

#define ACC_CTRL5_FSCALE_OFFSET	(3)
#define ACC_CTRL5_FSCALE_2G		(0 << ACC_FSCALE_OFFSET)
#define ACC_CTRL5_FSCALE_4G		(1 << ACC_FSCALE_2G)
#define ACC_CTRL5_FSCALE_6G		(2 << ACC_FSCALE_OFFSET)
#define ACC_CTRL5_FSCALE_8G		(3 << ACC_FSCALE_OFFSET)
#define ACC_CTRL5_FSCALE_16G		(4 << ACC_FSCALE_OFFSET)

#define ACC_CTRL5_ST_OFFSET			(1)
#define ACC_CTRL5_ST_NORMAL_MODE	(0 << ACC_ST_OFFSET)
#define ACC_CTRL5_ST_POS_SIGN		(1 << ACC_ST_OFFSET)
#define ACC_CTRL5_ST_NEG_SIGN		(2 << ACC_ST_OFFSET)
#define ACC_CTRL5_ST_NOT_ALLOWED	(3 << ACC_ST_OFFSET)

/* CTRL_REG_6 */
#define ACC_CTRL6_ADDR			(0x25)
#define ACC_CTRL6_BOOT			(1 << 7)
#define ACC_CTRL6_FIFO_EN		(1 << 6)
#define ACC_CTRL6_WTM_EN		(1 << 5)
#define ACC_CTRL6_ADD_INC		(1 << 4)
#define ACC_CTRL6_P1_EMPTY		(1 << 3)
#define ACC_CTRL6_P1_WTM		(1 << 2)
#define ACC_CTRL6_P1_OVERRUN	(1 << 1)
#define ACC_CTRL6_P2_BOT		(1)

/* STATUS REGISTER */
#define ACC_STATUS_ADDR		(0x27)
// OVERRUN
#define ACC_STATUS_ZYXOR 	(1 << 7)
#define ACC_STATUS_ZOR		(1 << 6)
#define ACC_STATUS_YOR		(1 << 5)
#define ACC_STATUS_XOR		(1 << 4)
// DATA AVAILABLE
#define ACC_STATUS_ZYXDA	(1 << 3)
#define ACC_STATUS_ZDA		(1 << 2)
#define ACC_STATUS_YDA		(1 << 1)
#define ACC_STATUS_XDA		(1 << 0)

/* OUT_T - temperature output register */
#define ACC_OUT_T_ADDR	 (0x0C)

/* OFFSET CORRECTION - signed values */
#define ACC_OFF_X_ADDR	(0x10)
#define ACC_OFF_Y_ADDR	(0x11)
#define ACC_OFF_Z_ADDR	(0x12)


/* INTERRUPT STATUS REGISTER */
#define ACC_STAT_ADDR		(0x18)


/* OUTPUT REGITER */
#define ACC_OUT_XL (0x28)
#define ACC_OUT_XH (0x29)
#define ACC_OUT_YL (0x2A)
#define ACC_OUT_YH (0x2B)
#define ACC_OUT_ZL (0x2C)
#define ACC_OUT_ZH (0x2D)

/* CTRL_REG1 */
#define ACC_CTRL_REG1_ADDR	(0x21)




/* Jest po 24, zadużo tych rejestrów zostało żeby kończyć w nocy */



#endif /* LIB_LIS3DSH_H_ */




