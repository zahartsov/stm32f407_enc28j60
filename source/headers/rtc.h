#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

typedef struct
{
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t weekDay;
}rtc;     // Параметры

void RTC_Init(void);

#endif