# 目標
本項目基於STM32F407外接TJA1050，收集CAN匯流排上的資料

# 使用
1. 下載原碼
2. IDE為IAR
3. 外接TJA1050 
4. 接收F103採集DHT20的溫濕度與電壓值
5. 最終以UART方式呈現到電腦

# 注意事項
1.TJA1050 使用5V 需另外供電並與電路共地

2.根據需求，需要幾路收發就需要幾個TJA1050

# 基本設置
Bitrate : 5000000

Sample Point : 87.5%

# 主要程式

#### 中斷接收can 放入Queue
```
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  uint8_t Data[8]={0}; // voltage Humidity Temperature StdId DLC
  
  CAN_RxHeaderTypeDef CAN_RxHeader;
  if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_RxHeader, Data) == HAL_OK)
  {
    BaseType_t xHPW = pdFALSE;  
    Data[6] = CAN_RxHeader.StdId;
    Data[7] = CAN_RxHeader.DLC;
    
    xQueueSendFromISR(Queue_CANHandle, Data, &xHPW); 
    portYIELD_FROM_ISR(Queue_CANHandle);
  }
}
```

#### 處理資料傳入Queue
```
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
      
      snprintf(line, sizeof(line), "ID = %d\n %d\n %.2fV\n %.2f%%\n %.2f\xC2\xB0""C\n\n",
               StdId, DLC, voltage, Humidity, Temperature);
      osMessageQueuePut(Queue_UARTHandle, line, 0, osWaitForever);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
```
#### Binary Semaphore 等待傳輸完成
```
void Task_UART_APP(void *argument)
{

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
}
```
```
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
      osSemaphoreRelease(BinUartTxDoneHandle);
    }
}
```
# 硬體
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/hardware.jpg)

# 最終呈現
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/UART.png)
![image](https://github.com/bmmno-Roy/F407_FreeRTOS_CAN/blob/main/IMG/cangaroo.png)

