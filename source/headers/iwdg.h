#ifndef __IWDG_H__
#define __IWDG_H__

#include "stm32f4xx.h"

//Debug mode
#define IWDG_DEBUGMODE_RUN      0x00
#define IWDG_DEBUGMODE_STOP     0x01


typedef struct
{
  uint8_t debugMode;            // ��������� ������� � ������ �������
  uint16_t frequancy;           // ������� ������������ �������
  uint16_t watchPeriod;         // ������ ������������ ������� � ��
}IndependentWatchDog;           // ��������� ����������� �������

extern IndependentWatchDog IWatchDog;

void IWDG_Init(IndependentWatchDog*);
void IWDG_Enable(void);
void IWDG_Reload(void);

#endif