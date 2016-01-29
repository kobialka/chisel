/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by MicroXplorer and eventually modified 
    by the user

  @endverbatim
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
#include "stm32f4xx_hal.h"


extern UART_HandleTypeDef huart4;


/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by MicroXplorer and eventually  
            modified by the user
   */ 
}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by MicroXplorer and eventually  
            modified by the user
   */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/* Init the low level hardware : GPIO, CLOCK, NVIC */


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM6){
		/* Enable timer6 clock */
		__TIM6_CLK_ENABLE();
		/* Peripheral interrupt init*/
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	    __HAL_TIM_ENABLE_IT(htim,TIM_IT_UPDATE);
	}

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM6){
		__TIM6_CLK_DISABLE();
		__HAL_TIM_DISABLE_IT(htim,TIM_IT_UPDATE);
		HAL_NVIC_DisableIRQ(UART4_IRQn);
	}
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  if(huart->Instance==UART4)
  {
     /* Peripheral clock enable */
    __UART4_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();

    /**UART4 GPIO Configuration
	PA1     ------> UART4_RX
	PA0     ------> UART4_TX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral interrupt init*/
	HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(UART4_IRQn);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==UART4)
  {
    /* Peripheral clock disable */
    __UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PA0-WKUP     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi){

	if(hspi->Instance==SPI3) {
		GPIO_InitTypeDef   GPIO_InitStructure;

		/* Włącz taktowanie dla używanych pinów. */
		__SPI3_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();

	  	/* Configure SPI3 GPIO: SCK(3) MISO(4) MOSI(5) */
	  	GPIO_InitStructure.Pin = (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	  	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	  	GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
	  	GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
	  	GPIO_InitStructure.Alternate = GPIO_AF6_SPI3;
	  	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	  	/* Configure SPI3 GPIO: NSS/CS (PA4) */
	  	GPIO_InitStructure.Pin = GPIO_PIN_4;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull  = GPIO_NOPULL;        // GPIO_PULLUP ??
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		GPIO_InitStructure.Alternate = GPIO_AF6_SPI3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi){

	if(hspi->Instance==SPI1) {

	}

}
