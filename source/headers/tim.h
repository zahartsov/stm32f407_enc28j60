#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f4xx.h"
#include "gpio.h"

//Mode
#define TIMER_MODE_COUNTER      0x00
#define TIMER_MODE_PWM          0x01
#define TIMER_MODE_CCP          0x02
#define TIMER_MODE_OPM          0x03

typedef struct
{
  TIM_TypeDef* baseTimer;
  GPIO* pin;
  uint8_t pulseWidth;
  uint8_t channel;
  uint8_t mode;
  uint32_t timerAPBFreq;        //Частота тактирования таймера, Гц
  uint32_t timerOneCount;       //one count, us
  uint32_t timerPeriod;         //Период срабатывания таймера, us
}Timer;     // Параметры таймера

void TIM6_Init(void);
void userTimer_Init(Timer*);
void userTimer_Enable(Timer*);
void userTimer_Disable(Timer*);

#endif