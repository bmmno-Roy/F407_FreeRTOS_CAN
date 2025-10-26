/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Task_DataProc */
osThreadId_t Task_DataProcHandle;
const osThreadAttr_t Task_DataProc_attributes = {
  .name = "Task_DataProc",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_UART */
osThreadId_t Task_UARTHandle;
const osThreadAttr_t Task_UART_attributes = {
  .name = "Task_UART",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for Queue_CAN */
osMessageQueueId_t Queue_CANHandle;
const osMessageQueueAttr_t Queue_CAN_attributes = {
  .name = "Queue_CAN"
};
/* Definitions for Queue_UART */
osMessageQueueId_t Queue_UARTHandle;
const osMessageQueueAttr_t Queue_UART_attributes = {
  .name = "Queue_UART"
};
/* Definitions for BinUartTxDone */
osSemaphoreId_t BinUartTxDoneHandle;
const osSemaphoreAttr_t BinUartTxDone_attributes = {
  .name = "BinUartTxDone"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Task_DataProc_APP(void *argument);
void Task_UART_APP(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinUartTxDone */
  BinUartTxDoneHandle = osSemaphoreNew(1, 0, &BinUartTxDone_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Queue_CAN */
  Queue_CANHandle = osMessageQueueNew (4, 8, &Queue_CAN_attributes);

  /* creation of Queue_UART */
  Queue_UARTHandle = osMessageQueueNew (4, 48, &Queue_UART_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Task_DataProc */
  Task_DataProcHandle = osThreadNew(Task_DataProc_APP, NULL, &Task_DataProc_attributes);

  /* creation of Task_UART */
  Task_UARTHandle = osThreadNew(Task_UART_APP, NULL, &Task_UART_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    vTaskDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Task_DataProc_APP */
/**
* @brief Function implementing the Task_DataProc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_DataProc_APP */
void Task_DataProc_APP(void *argument)
{
  /* USER CODE BEGIN Task_DataProc_APP */
  /* Infinite loop */
  for(;;)
  {
    unsigned char Data[8]; // voltage Humidity Temperature StdId DLC
    if(xQueueReceive(Queue_CANHandle, Data, 0) == pdPASS)
    {
      char line[50];
      uint8_t StdId = Data[6];
      uint8_t DLC = Data[7];
      float voltage = ((Data[0] << 8) + Data[1]) * 0.01f;
      float Humidity = ((Data[2] << 8) + Data[3]) * 0.01f;
      float Temperature = ((Data[4] << 8) + Data[5]) * 0.01f;
      
      snprintf(line, sizeof(line), "ID = %d\n DLC = %d\n %.2fV\n %.2f%%\n %.2f\xC2\xB0""C\n\n",
               StdId, DLC, voltage, Humidity, Temperature);
      osMessageQueuePut(Queue_UARTHandle, line, 0, osWaitForever);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  /* USER CODE END Task_DataProc_APP */
}

/* USER CODE BEGIN Header_Task_UART_APP */
/**
* @brief Function implementing the Task_UART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_UART_APP */
void Task_UART_APP(void *argument)
{
  /* USER CODE BEGIN Task_UART_APP */
  /* Infinite loop */
  for(;;)
  {
    char line[48];
    if(osMessageQueueGet(Queue_UARTHandle, line, NULL, osWaitForever) == osOK)
    {
      size_t len = strnlen(line, sizeof(line));
      if (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)line, len) == HAL_OK){
        osSemaphoreAcquire(BinUartTxDoneHandle, 50);
      }
    }
    vTaskDelay(1);
  }
  /* USER CODE END Task_UART_APP */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1) {
    osSemaphoreRelease(BinUartTxDoneHandle);
  }
}
/* USER CODE END Application */

