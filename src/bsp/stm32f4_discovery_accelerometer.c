/**
  ******************************************************************************
  * @file    stm32f4_discovery_accelerometer.c
  * @author  MCD Application Team
  * @version V2.0.2
  * @date    26-June-2014
  * @brief   This file provides a set of functions needed to manage the
  *          MEMS accelerometers available on STM32F4-Discovery Kit.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "bsp/stm32f4_discovery_accelerometer.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */ 

/** @addtogroup STM32F4_DISCOVERY_ACCELEROMETER
  * @brief  This file includes the motion sensor driver for ACCELEROMETER motion sensor 
  *         devices.
  * @{
  */

/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_Macros
  * @{
  */
/**
  * @}
  */ 
  
/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_Variables
  * @{
  */ 
static ACCELERO_DrvTypeDef *AcceleroDrv;
/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F4_DISCOVERY_ACCELEROMETER_Private_Functions
  * @{
  */

/**
  * @brief  Setx Accelerometer Initialization.
  * @param  None
  * @retval ACCELERO_OK if no problem during initialization
  */
uint8_t BSP_ACCELERO_Init(void)
{ 
  uint8_t ret = ACCELERO_ERROR;
  uint16_t ctrl = 0x0000;

  LIS3DSH_InitTypeDef          l1s3dsh_InitStruct;

   if(Lis3dshDrv.ReadID() == I_AM_LIS3DSH)
  {
    /* Initialize the accelerometer driver structure */
    AcceleroDrv = &Lis3dshDrv;

    /* Set configuration of LIS3DSH MEMS Accelerometer **********************/
    l1s3dsh_InitStruct.Output_DataRate = LIS3DSH_DATARATE_100;
    l1s3dsh_InitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;
    l1s3dsh_InitStruct.SPI_Wire = LIS3DSH_SERIALINTERFACE_4WIRE;
    l1s3dsh_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
    l1s3dsh_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_8;
    l1s3dsh_InitStruct.Filter_BW = LIS3DSH_FILTER_BW_200;
    
    /* Configure MEMS: power mode(ODR) and axes enable */
    ctrl = (uint16_t) (l1s3dsh_InitStruct.Output_DataRate | \
                       l1s3dsh_InitStruct.Axes_Enable);
    
    /* Configure MEMS: full scale and self test */
    ctrl |= (uint16_t) ((l1s3dsh_InitStruct.SPI_Wire    | \
                         l1s3dsh_InitStruct.Self_Test   | \
                         l1s3dsh_InitStruct.Full_Scale  | \
                         l1s3dsh_InitStruct.Filter_BW) << 8);

    /* Configure the accelerometer main parameters */
    AcceleroDrv->Init(ctrl);
    
    ret = ACCELERO_OK;
  }

  else
  {
    ret = ACCELERO_ERROR;
  }
  return ret;
}

/**
  * @brief  Read ID of Accelerometer component.
  * @param  None
  * @retval ID
  */
uint8_t BSP_ACCELERO_ReadID(void)
{
  uint8_t id = 0x00;

  if(AcceleroDrv->ReadID != NULL)
  {
    id = AcceleroDrv->ReadID();
  }  
  return id;
}

/**
  * @brief  Reboot memory content of Accelerometer.
  * @param  None
  * @retval None
  */
void BSP_ACCELERO_Reset(void)
{
  if(AcceleroDrv->Reset != NULL)
  {
    AcceleroDrv->Reset();
  }
}

/**
  * @brief  Configure Accelerometer click IT. 
  * @param  None
  * @retval None
  */
void BSP_ACCELERO_Click_ITConfig(void)
{
  if(AcceleroDrv->ConfigIT != NULL)
  {
    AcceleroDrv->ConfigIT();
  }
}

/**
  * @brief  Clear Accelerometer click IT.
  * @param  None
  * @retval None
  */
void BSP_ACCELERO_Click_ITClear(void)
{
  if(AcceleroDrv->ClearIT != NULL)
  {
    AcceleroDrv->ClearIT();
  }
}

/**
  * @brief  Get XYZ axes acceleration.
  * @param  pDataXYZ: Pointer to 3 angular acceleration axes.  
  *                   pDataXYZ[0] = X axis, pDataXYZ[1] = Y axis, pDataXYZ[2] = Z axis
  * @retval None
  */
void BSP_ACCELERO_GetXYZ(int16_t *pDataXYZ)
{
  int16_t SwitchXY = 0;
  
  if(AcceleroDrv->GetXYZ != NULL)
  {   
    AcceleroDrv->GetXYZ(pDataXYZ);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
