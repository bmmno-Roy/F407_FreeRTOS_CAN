// Fixed IAR v9.x printf problem 250914
// retarget for IAR DLIB
#include <yfuns.h>          // IAR low I/O interface
#include "usart.h"          // huart1
#include "stm32f4xx_hal.h" 
#include "stdio.h"

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

#pragma module_name = "?__write"
// output：printf/puts/putchar
size_t __write(int handle, const unsigned char *buf, size_t size)
{
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
      return _LLIO_ERROR;           // 只處理 stdout/stderr
  }
  HAL_UART_Transmit(&huart1, (uint8_t*)buf, (uint16_t)size, HAL_MAX_DELAY);
  return size;                       // 回傳實際寫入的位元組數
}

//// need open newfile read.c to fixed read
//// input：printf/puts/putcharcanf/gets

//#pragma module_name = "?__read"
//size_t __read(int handle, unsigned char *buf, size_t size)
//{
//    if (handle != _LLIO_STDIN) {
//        return _LLIO_ERROR;
//    }
//    // 阻塞式讀 size 個字元；也可改成逐字元
//    if (HAL_UART_Receive(&huart1, (uint8_t*)buf, (uint16_t)size, HAL_MAX_DELAY) == HAL_OK) {
//        return size;
//    }
//    return _LLIO_ERROR;
//}



