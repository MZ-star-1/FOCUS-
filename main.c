/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "ctype.h"
#include "stdio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

SpeedState current_speed_state = SPEED_STATE_SLOW;
uint32_t last_speed_change_time = 0;
const uint32_t SPEED_CHANGE_INTERVAL = 2000;  

char rx_buffer[RX_BUFFER_SIZE];  
uint8_t rx_index = 0;            
uint8_t cmd_ready = 0;           
Motor_Status motor_status = MOTOR_OFF;  
uint16_t current_duty = 200;     

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE BEGIN 0 */

 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {
        char received_char = rx_buffer[rx_index];  
        
        
        if(received_char == '\r' || received_char == '\n') {
            if(rx_index > 0) { 
                rx_buffer[rx_index] = '\0';  
                cmd_ready = 1;              
            }
            rx_index = 0;  
        }
        
        else if(rx_index < RX_BUFFER_SIZE - 1) {
            rx_index++;
        }
        
        else {
            rx_index = 0;
        }
        
        
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_buffer[rx_index], 1);
    }
}


void Motor_PWM_Control(uint16_t duty, uint8_t direction)
{
    if(duty > 999) duty = 999; 
    
    switch(direction) {
        case 1: 
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            break;
        case 2:  
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);
            break;
        case 0: 
        default:
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            break;
    }
}


int string_to_int(const char* str)
{
    int result = 0;
    int i = 0;
    
    
    while(str[i] == ' ') i++;
    
    
    for(; str[i] != '\0'; i++) {
        if(str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            return -1;  
        }
    }
    
    return result;
}


void Parse_Command(const char* cmd)
{
    char response[32];  
    
    
    char temp_cmd[RX_BUFFER_SIZE];
    strcpy(temp_cmd, cmd);
    
   
    for(int i = 0; temp_cmd[i]; i++) {
        temp_cmd[i] = toupper(temp_cmd[i]);
    }
    
    
    if(strcmp(temp_cmd, CMD_ON) == 0) {
        motor_status = MOTOR_ON;
        Motor_PWM_Control(current_duty, 1);  
        sprintf(response, "MOTOR ON (Duty: %d%%)\r\n", current_duty/10);
    }
    
    else if(strcmp(temp_cmd, CMD_OFF) == 0) {
        motor_status = MOTOR_OFF;
        Motor_PWM_Control(0, 0);  
        sprintf(response, "MOTOR OFF\r\n");
    }
    
    else if(strncmp(temp_cmd, CMD_SPEED, strlen(CMD_SPEED)) == 0) {
        
        char* space_pos = strchr(temp_cmd, ' ');
        if(space_pos != NULL) {
            int speed = string_to_int(space_pos + 1);
            
            if(speed >= 0 && speed <= 100) {
                current_duty = speed * 10;  
                
               
                if(motor_status == MOTOR_ON) {
                    Motor_PWM_Control(current_duty, 1);
                }
                
                sprintf(response, "SPEED SET: %d%%\r\n", speed);
            } else {
                sprintf(response, "ERROR: Speed must be 0-100\r\n");
            }
        } else {
            sprintf(response, "ERROR: Invalid SPEED format\r\n");
        }
    }
    
    else {
        sprintf(response, "ERROR: Unknown command\r\n");
    }
    
   
    HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 100);
}
/* USER CODE END 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
 /* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	Motor_PWM_Control(0, 0);

	HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_buffer, 1);

	char welcome[] = "\r\n===== DC Motor Control System =====\r\n"
                 "Commands:\r\n"
                 "  ON       - Start motor (20% duty)\r\n"
                 "  OFF      - Stop motor\r\n"
                 "  SPEED XX - Set speed (0-100%)\r\n"
                 "==================================\r\n"
                 "> ";
	HAL_UART_Transmit(&huart1, (uint8_t*)welcome, strlen(welcome), 100);
	/* USER CODE END 2 */

  /* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
   
    if(cmd_ready) {
        Parse_Command(rx_buffer);  
        cmd_ready = 0;             
        
       
        HAL_UART_Transmit(&huart1, (uint8_t*)"> ", 2, 100);
    }
    
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
