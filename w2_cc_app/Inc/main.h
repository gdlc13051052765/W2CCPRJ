/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/


#define MAIN_LED_Pin GPIO_PIN_13
#define MAIN_LED_GPIO_Port GPIOC

#define BEEP_Pin GPIO_PIN_14
#define BEEP_GPIO_Port GPIOC
#define LOAD_ACT0_Pin GPIO_PIN_0
#define LOAD_ACT0_GPIO_Port GPIOA
#define LOAD_ACT1_Pin GPIO_PIN_1
#define LOAD_ACT1_GPIO_Port GPIOA
#define LOAD_ACT2_Pin GPIO_PIN_2
#define LOAD_ACT2_GPIO_Port GPIOA
#define LOAD_ACT3_Pin GPIO_PIN_3
#define LOAD_ACT3_GPIO_Port GPIOA
#define CS_FLASH_Pin GPIO_PIN_4
#define CS_FLASH_GPIO_Port GPIOA
#define LOAD_ACT4_Pin GPIO_PIN_0
#define LOAD_ACT4_GPIO_Port GPIOB
#define LOAD_ACT5_Pin GPIO_PIN_1
#define LOAD_ACT5_GPIO_Port GPIOB
#define LOAD_ACT6_Pin GPIO_PIN_10
#define LOAD_ACT6_GPIO_Port GPIOB
#define LOAD_ACT7_Pin GPIO_PIN_11
#define LOAD_ACT7_GPIO_Port GPIOB
#define LOAD_ACT8_Pin GPIO_PIN_15
#define LOAD_ACT8_GPIO_Port GPIOB
#define LOAD_ACT9_Pin GPIO_PIN_6
#define LOAD_ACT9_GPIO_Port GPIOC
#define LOAD_ACT10_Pin GPIO_PIN_7
#define LOAD_ACT10_GPIO_Port GPIOC
#define LOAD_ACT11_Pin GPIO_PIN_8
#define LOAD_ACT11_GPIO_Port GPIOC
#define SPI3_NSS2_Pin GPIO_PIN_8
#define SPI3_NSS2_GPIO_Port GPIOA
#define SPI3_NSS1_Pin GPIO_PIN_15
#define SPI3_NSS1_GPIO_Port GPIOA
#define RUN_SIGNAL_Pin GPIO_PIN_6
#define RUN_SIGNAL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
typedef enum
{
  BOX_SUCCESS     = 0,
  BOX_FAIL      = 0X02,
  BOX_UNKNOW_CMD, 
  BOX_UNKNOW_PARAM,

  Android_SUCCESS   = 0X80,
  Android_FAIL    = 0X82,
  Android_UNKNOW_CMD, 
  Android_UNKNOW_PARAM,
}ret_msg_t;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
