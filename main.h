/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE BEGIN Private defines */

#define SPEED_SLOW    200   // 慢速：20%占空比
#define SPEED_MID     500   // 中速：50%占空比  
#define SPEED_FAST    800   // 快速：80%占空比  


typedef enum {
    SPEED_STATE_SLOW = 0,  // 状态0：慢速
    SPEED_STATE_MID,       // 状态1：中速
    SPEED_STATE_FAST,      // 状态2：快速
    SPEED_STATE_MID2,      // 状态3：中速（第二次）
    SPEED_STATE_SLOW2      // 状态4：慢速（第二次）
} SpeedState;

#define RX_BUFFER_SIZE 64      // 串口接收缓冲区大小
#define CMD_ON         "ON"    // 启动命令字符串
#define CMD_OFF        "OFF"   // 停止命令字符串
#define CMD_SPEED      "SPEED" // 调速命令字符串

typedef enum {
    MOTOR_OFF = 0,  // 电机停止
    MOTOR_ON        // 电机运行
} Motor_Status;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
