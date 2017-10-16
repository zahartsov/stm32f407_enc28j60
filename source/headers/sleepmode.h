#ifndef __SLEEPMODE_H__
#define __SLEEPMODE_H__

#include "stm32f4xx.h"

//Debug mode
#define SLEEPMODE_DEBUGMODE_STOP        0x00
#define SLEEPMODE_DEBUGMODE_SLEEP_RUN   0x01
#define SLEEPMODE_DEBUGMODE_STOP_RUN    0x02
#define SLEEPMODE_DEBUGMODE_STANDBY_RUN 0x04

//Send event on pending bit

//Deep sleep mode status
#define SLEEPMODE_SLEEPDEEP_OFF         0x00
#define SLEEPMODE_SLEEPDEEP_ON          0x01

//Sleep on exit
#define SLEEPMODE_SLEEPONEXIT_OFF       0x00
#define SLEEPMODE_SLEEPONEXIT_ON        0x01

//Deep sleep mode
#define SLEEPMODE_DEEPSLEEP_STOP        0x00
#define SLEEPMODE_DEEPSLEEP_STANDBY     0x01

//Deep sleep voltage regulator in STOP mode
#define SLEEPMODE_STOPMODE_VR_ON        0x00
#define SLEEPMODE_STOPMODE_VR_LP_MODE   0x01

//Wake up pin status
#define SLEEPMODE_WKUPPIN_OFF           0x00
#define SLEEPMODE_WKUPPIN_ON            0x01

typedef struct
{
  uint8_t debugMode;
  uint8_t deepSleepModeEn;
  uint8_t sleepOnExitMode;
  uint8_t deepSleepMode;
  uint8_t stopModeVRState;
  uint8_t wakeUpPinEn;
  uint8_t sleepMode_ON;
  uint8_t stopMode_ON;
  uint8_t standbyMode_ON;
}SleepMode;           // Параметры сторожевого таймера

extern SleepMode sleepMode;

void sleepMode_Init(SleepMode*);

#endif