/*##################################### SleepMode ###################################
Порядок инициализации:
1. В регистре DBGMCU->CR настраиваем режим работы отладчика в спящих режимах:
  - нулевой бит - в режиме Sleep;
  - первый бит - в режиме Stop;
  - второй бит - в режиме Standby.
2. Настройка режима "глубокого сна" в регистре SCB->SCR. Установка второго бита - переход в режим "глубокого сна" при переходе в спящий режим.
3. Настройка режима сна в основном потоке в регистре SCB->SCR (МК засыпает при выходе в основной поток из прерываний). Установка первого бита - переход в режим сна в основном потоке.
4. Настройка режима глубоко сна в регистре PWR->CR. Установка бита PWR_CR_PDDS - режим Standby; сброс - режим Stop.
5. Настройка регулятора напряжения в режиме глубокого сна.
6. Включение пробуждающего пина (wake up pin) PA0 в регистре PWR->CSR. Установка бита PWR_CSR_EWUP включает "пробуждающий пин" и притягивает пин PA0 к нулю. Высокий уровень сигнала на этом пине выводит МК из спящего режима.
################################################################################*/


#include "sleepmode.h"

void sleepMode_Init(SleepMode* sleepMode)
{
  //Разрешаем работу дебаггера во всех спящих режимах (sleep, stop, standby)
  if(sleepMode->debugMode == SLEEPMODE_DEBUGMODE_STOP) DBGMCU->CR &= ~(0x07);
  else DBGMCU->CR |= sleepMode->debugMode;
  
  if(sleepMode->deepSleepModeEn == SLEEPMODE_SLEEPDEEP_OFF) SCB->SCR &= ~(0x04);
  else SCB->SCR |= 0x04;
  
  if(sleepMode->sleepOnExitMode == SLEEPMODE_SLEEPONEXIT_OFF) SCB->SCR &= ~(0x02);
  else SCB->SCR |= 0x02;
  
  if(sleepMode->deepSleepMode == SLEEPMODE_DEEPSLEEP_STOP) PWR->CR &= ~PWR_CR_PDDS;
  else PWR->CR |= PWR_CR_PDDS;
  
  if(sleepMode->stopModeVRState == SLEEPMODE_STOPMODE_VR_ON) PWR->CR &= ~PWR_CR_LPDS;
  else PWR->CR |= PWR_CR_LPDS;
  
  if(sleepMode->wakeUpPinEn == SLEEPMODE_WKUPPIN_OFF) PWR->CSR &= ~PWR_CSR_EWUP;
  else PWR->CSR |= PWR_CSR_EWUP;
}