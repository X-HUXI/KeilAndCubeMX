/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <includes.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

int fputc(int ch,FILE *f)
 
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);    
		//等待发送结束	
		while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET){
		}		

    return ch;
}


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//任务控制块
static  OS_TCB   AppTaskStartTCB;
OS_TCB LED1TaskTCB;//PA4
OS_TCB LED2TaskTCB;//PA5
OS_TCB USART1TaskTCB;//串口1

//任务堆栈
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

/* 私有函数原形 --------------------------------------------------------------*/
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  AppTaskStart(void *p_arg);

//任务函数
void LED1_TASK(void *p_arg);
void LED2_TASK(void *p_arg);
void USART1_TASK(void *p_arg);

//任务优先级
#define LED1_TASK_PRIO 5  //PA4优先级
#define LED2_TASK_PRIO 4  //PA5优先级
#define USART1_TASK_PRIO 3 //USART1优先级

//任务栈大小
#define LED1_STK_SIZE 64 //PA4栈大小
#define LED2_STK_SIZE 64 //PA5栈大小
#define USART1_STK_SIZE 64 //串口1栈大小


CPU_STK LED1_TASK_STK[LED1_STK_SIZE];//PA4任务栈
CPU_STK LED2_TASK_STK[LED2_STK_SIZE];//PA5任务栈
CPU_STK USART1_TASK_STK[USART1_STK_SIZE];//串口任务栈

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
	OS_ERR  err;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	
	OSInit(&err);  

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* 创建任务 */
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
				 (CPU_CHAR   *)"App Task Start",
				 (OS_TASK_PTR ) AppTaskStart,
				 (void       *) 0,
				 (OS_PRIO     ) APP_TASK_START_PRIO,
				 (CPU_STK    *)&AppTaskStartStk[0],
				 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
				 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&err);
				 
	OSStart(&err);            /* Start multitasking (i.e. give control to uC/OS-III). */

//  while (1)
//  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * 函数功能: 启动任务函数体。
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppTaskStart (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  //CPU_Init();

  //Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */

	OSTaskCreate((OS_TCB 	* )&LED1TaskTCB,		
				 (CPU_CHAR	* )"pa4 task", 		
				 (OS_TASK_PTR )LED1_TASK, 			
				 (void		* )0,					
				 (OS_PRIO	  )LED1_TASK_PRIO,     
				 (CPU_STK   * )&LED1_TASK_STK[0],	
				 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
				 (CPU_STK_SIZE)LED1_STK_SIZE,		
				 (OS_MSG_QTY  )0,					
				 (OS_TICK	  )0,					
				 (void   	* )0,					
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err);	
				 
	OSTaskCreate((OS_TCB 	* )&LED2TaskTCB,		
				 (CPU_CHAR	* )"pa5 task", 		
				 (OS_TASK_PTR )LED2_TASK, 			
				 (void		* )0,					
				 (OS_PRIO	  )LED2_TASK_PRIO,     
				 (CPU_STK   * )&LED2_TASK_STK[0],	
				 (CPU_STK_SIZE)LED2_STK_SIZE/10,	
				 (CPU_STK_SIZE)LED2_STK_SIZE,		
				 (OS_MSG_QTY  )0,					
				 (OS_TICK	  )0,					
				 (void   	* )0,					
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err);
				 
				 
	OSTaskCreate((OS_TCB 	* )&USART1TaskTCB,		
				 (CPU_CHAR	* )"usart1 task", 		
				 (OS_TASK_PTR )USART1_TASK, 			
				 (void		* )0,					
				 (OS_PRIO	  )USART1_TASK_PRIO,     
				 (CPU_STK   * )&USART1_TASK_STK[0],	
				 (CPU_STK_SIZE)USART1_STK_SIZE/10,	
				 (CPU_STK_SIZE)USART1_STK_SIZE,		
				 (OS_MSG_QTY  )0,					
				 (OS_TICK	  )0,					
				 (void   	* )0,					
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err);				 
				 
	OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);
 
}

//LED1每秒闪烁一次
void  LED1_TASK (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */

  while (DEF_TRUE)
  {
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		//延时1秒
		OSTimeDlyHMSM(0, 0, 1, 0,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err);
  }
}

//LED2每3秒闪烁一次
void  LED2_TASK (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */

  while (DEF_TRUE)
  {
		HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
	  //延时3秒
		OSTimeDlyHMSM(0, 0, 3, 0,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err);
  }
}


//串口1每2秒输出一次
void  USART1_TASK (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */

  while (DEF_TRUE)
  {
		printf("hello uc/OS! 欢迎来到RTOS多任务环境\r\n");
	  //延时2秒
		OSTimeDlyHMSM(0, 0, 2, 0,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err);
  }
}

/**
  * 函数功能: 创建应用任务
  * 输入参数: p_arg 是在创建该任务时传递的形参
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppTaskCreate (void)
{

	  
}

/**
  * 函数功能: uCOSIII内核对象创建
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static  void  AppObjCreate (void)
{
	
}

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
