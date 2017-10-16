#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stm32f4xx.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"
#include "iwdg.h"
#include "sleepmode.h"

#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define OFF     0x01;
#define ON      0x00;

typedef struct
{
  rtc* Clock;
  Timer* timer;
  GPIO* LED;
  GPIO* Button;
  SleepMode* sleepMode;
  IndependentWatchDog* IWatchDog;
  uint32_t hclk;        //частота процессора, √ц
  uint32_t pclk1;       //частота первой периферийной шины, √ц
  uint32_t pclk2;       //частота второй периферийной шины, √ц
  uint8_t i2cAddress;
  uint8_t CPUtemperature;
}System;     // ѕараметры системы

extern System sys;
extern rtc sysClock;
extern Timer sysTimer;
extern GPIO sysLED;
extern GPIO greenLED;
extern GPIO blueLED;
extern GPIO redLED;
extern GPIO orangeLED;
extern GPIO sysButton;
extern SleepMode sleepMode;
extern IndependentWatchDog IWatchDog;

void System_Init(System*);
void DelayMs(uint16_t);
void DelayUs(uint16_t);
void InterruptsPrioritySet(void);

#endif