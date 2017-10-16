/*##################################### IWDG ###################################
Порядок инициализации:
1.
################################################################################*/


#include "iwdg.h"

void IWDG_Init(IndependentWatchDog* IWatchDog)
{
  if(IWatchDog->debugMode) DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
  else DBGMCU->APB1FZ &= ~DBGMCU_APB1_FZ_DBG_IWDG_STOP;
  IWDG->KR = 0x5555;
  uint32_t tmp = 0;
  tmp = (IWatchDog->frequancy * IWatchDog->watchPeriod) / 1000;
  uint16_t tmp_prescaler = 0;
  if(tmp < 0xFFF) tmp_prescaler = 0;
  else
  {
    tmp /= 0xFFF;
    uint8_t i = 0;
    while(tmp != 0)
    {
      tmp = tmp >> 1;
      i++;
    }
    tmp_prescaler = i - 2;
  }

  tmp = IWatchDog->frequancy / (0x01 << (tmp_prescaler + 2));
  IWDG->RLR = (tmp * IWatchDog->watchPeriod) / 1000;
  IWDG->PR = tmp_prescaler;
  IWDG->KR = 0x00;
}

void IWDG_Enable()
{
  IWDG->KR = 0xCCCC;
}

void IWDG_Reload()
{
  IWDG->KR = 0xAAAA;
}