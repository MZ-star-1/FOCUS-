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

SpeedState current_speed_state = SPEED_STATE_SLOW;		// 当前速度状态
uint32_t last_speed_change_time = 0;					// uint32_t：无符号32位整数，跨平台一致性
const uint32_t SPEED_CHANGE_INTERVAL = 2000;  			// 速度改变间隔

char rx_buffer[RX_BUFFER_SIZE];  						// 存放字符，串口收到的字符会按顺序放进去;当收到完整命令，就取出来解析
uint8_t rx_index = 0;            						// 缓冲区指针，指向当前要放字符的位置
uint8_t cmd_ready = 0;           						// 标记是否有完整命令可以处理
Motor_Status motor_status = MOTOR_OFF;  				// 记录电机状态
uint16_t current_duty = 200;     						// 当前PWM占空比

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {  // 确保是USART1的中断
        char received_char = rx_buffer[rx_index];  // 取出刚收到的字符
        
        // 如果收到回车或换行符（命令结束标志）
        if(received_char == '\r' || received_char == '\n') {
            if(rx_index > 0) {  // 避免空命令
                rx_buffer[rx_index] = '\0';  // 在字符串末尾加结束符
                cmd_ready = 1;              // 设置命令就绪标志
            }
            rx_index = 0;  // 重置索引，准备接收下一条命令
        }
        // 如果是普通字符，且缓冲区未满
        else if(rx_index < RX_BUFFER_SIZE - 1) {
            rx_index++;  // 移动到下一个位置
        }
        // 缓冲区满了，从头开始（防止溢出）
        else {
            rx_index = 0;
        }
        
        // 重新启动接收，等待下一个字符
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_buffer[rx_index], 1);
    }
}


void Motor_PWM_Control(uint16_t duty, uint8_t direction)
{
    if(duty > 999) duty = 999;  // 限制占空比范围0-999
    
    switch(direction) {
        case 1:  // 正转：PA0输出PWM，PA1输出0
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            break;
        case 2:  // 反转：PA0输出0，PA1输出PWM
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);
            break;
        case 0:  // 停止：PA0和PA1都输出0
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
    
    // 跳过字符串开头的空格
    while(str[i] == ' ') i++;
    
    // 逐个字符转换为数字
    for(; str[i] != '\0'; i++) {
        if(str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            return -1;  // 遇到非数字字符，返回错误
        }
    }
    
    return result;
}

void Parse_Command(const char* cmd)
{
    char response[32];  // 存放要返回的信息
    
    // 复制命令到临时变量（避免修改原命令）
    char temp_cmd[RX_BUFFER_SIZE];
    strcpy(temp_cmd, cmd);
    
    // 转换为大写（实现命令不区分大小写）
    for(int i = 0; temp_cmd[i]; i++) {
        temp_cmd[i] = toupper(temp_cmd[i]);
    }
    
    // 解析ON命令
    if(strcmp(temp_cmd, CMD_ON) == 0) {
        motor_status = MOTOR_ON;  // 设置状态为运行
        Motor_PWM_Control(current_duty, 1);  // 以当前占空比正转启动
        sprintf(response, "MOTOR ON (Duty: %d%%)\r\n", current_duty/10);
    }
    
    // 解析OFF命令
    else if(strcmp(temp_cmd, CMD_OFF) == 0) {
        motor_status = MOTOR_OFF;  // 设置状态为停止
        Motor_PWM_Control(0, 0);   // 停止电机
        sprintf(response, "MOTOR OFF\r\n");
    }
    
    // 解析SPEED命令
    else if(strncmp(temp_cmd, CMD_SPEED, strlen(CMD_SPEED)) == 0) {
        // 查找空格，分离命令和参数
        char* space_pos = strchr(temp_cmd, ' ');
        if(space_pos != NULL) {
            int speed = string_to_int(space_pos + 1);  // 转换数字部分
            
            if(speed >= 0 && speed <= 100) {  // 检查范围
                current_duty = speed * 10;  // 0-100转成0-1000
                
                // 如果电机正在运行，立即应用新速度
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
    
    // 未知命令
    else {
        sprintf(response, "ERROR: Unknown command\r\n");
    }
    
    // 通过串口发送响应
    HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 100);
}

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

// 启动PWM输出
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	// 初始化电机状态
	Motor_PWM_Control(0, 0);
	// 启动串口接收中断
	HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_buffer, 1);

	// 欢迎信息
	/*char welcome[] = "\r\n===== DC Motor Control System =====\r\n"
                 "Commands:\r\n"
                 "  ON       - Start motor (20% duty)\r\n"
                 "  OFF      - Stop motor\r\n"
                 "  SPEED XX - Set speed (0-100%)\r\n"
                 "==================================\r\n"
                 "> ";
	HAL_UART_Transmit(&huart1, (uint8_t*)welcome, strlen(welcome), 100);*/
	/* USER CODE END 2 */

  /* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
   
    if(cmd_ready) {
    Parse_Command(rx_buffer);  // 解析命令
    cmd_ready = 0;             // 清除标志
    
    // 发送命令提示符
    HAL_UART_Transmit(&huart1, (uint8_t*)"> ", 2, 100);
}
    
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */

// STM32系统时钟配置函数
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
