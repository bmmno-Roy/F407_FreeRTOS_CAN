#ifndef _Keys_H__
#define _Keys_H__

#include "stm32f4xx_hal.h"
typedef enum{
  KEY_NONE,
  KEY_1,
  KEY_2,
  KEY_3,
}KEYS;

void Keys_Init(void);
KEYS Scan_Keys(void);



#endif

