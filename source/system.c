/*################## Инициализация системной периферии #########################
Порядок инициализации:
1.
################################################################################*/

#include "system.h"

typedef struct
{
  GPIO* LED;
  uint16_t period;
}LedTaskParam;

LedTaskParam sysLedTaskParam;

xSemaphoreHandle xLedTaskSemaphore;

void vSysTask(void *pvParameters)
{
  volatile LedTaskParam *pxTaskParam;
  pxTaskParam = (LedTaskParam *) pvParameters;

  while(1)
  {
    xSemaphoreTake(xLedTaskSemaphore, portMAX_DELAY);
    if(pxTaskParam->LED->baseGPIO->IDR & (GPIO_IDR_IDR_0 << pxTaskParam->LED->pin)) pxTaskParam->LED->baseGPIO->BSRRH |= GPIO_BSRR_BS_0 << pxTaskParam->LED->pin;
    else pxTaskParam->LED->baseGPIO->BSRRL |= GPIO_BSRR_BS_0 << pxTaskParam->LED->pin;

    sys.Clock->hours = (((RTC->TR >> 20) & 0x03) * 10);
    sys.Clock->hours += ((RTC->TR >> 16) & 0x0F);
    sys.Clock->minutes = (((RTC->TR >> 12) & 0x07) * 10);
    sys.Clock->minutes += ((RTC->TR >> 8) & 0x0F);
    sys.Clock->seconds = (((RTC->TR >> 4) & 0x07) * 10);
    sys.Clock->seconds += (RTC->TR  & 0x0F);

    sys.Clock->year = 10 * ((RTC->DR >> 20) & 0x0F);
    sys.Clock->year += (RTC->DR >> 16) & 0x0F;
    sys.Clock->year += 2000;
    sys.Clock->month = 10 * ((RTC->DR >> 12) & 0x01);
    sys.Clock->month += (RTC->DR >> 8) & 0x0F;
    sys.Clock->day = 10 * ((RTC->DR >> 4) & 0x03);
    sys.Clock->day += RTC->DR & 0x0F;
    sys.Clock->weekDay = (RTC->DR >> 13) & 0x07;
  }
  vTaskDelete(NULL);
}

void System_Init(System* system)
{
  system->i2cAddress = 0x01;
  vSemaphoreCreateBinary(xLedTaskSemaphore);

  if(xLedTaskSemaphore != NULL)
  {
    sysLedTaskParam.LED = system->LED;
    sysLedTaskParam.period = 500;

    xTaskCreate(vSysTask, "sysLedTask", configMINIMAL_STACK_SIZE, (void*)&sysLedTaskParam, tskIDLE_PRIORITY + 2, NULL);
  }

  if(system->Clock)
  {
    RTC_Init();
  }

  if(system->LED)
  {
    system->LED->baseGPIO = (GPIO_TypeDef*)GPIOD;
    system->LED->pin = 12;
    system->LED->mode = GPIO_MODE_OUTPUT;
    system->LED->pull = GPIO_PULL_PULLUP;
    system->LED->speed = GPIO_SPEED_HIGH;
    system->LED->type = GPIO_TYPE_PUSHPULL;
    userGPIO_Init(system->LED);
  }

  if(system->Button)
  {
    system->Button->interruptFalling = EXTI_FALLING_DISABLE;
    system->Button->interruptRising = EXTI_RISING_ENABLE;
    system->Button->baseGPIO = (GPIO_TypeDef*)GPIOA_BASE;
    system->Button->pin = 0;
    system->Button->mode = GPIO_MODE_INPUT;
    system->Button->pull = GPIO_PULL_NOPULL;
    system->Button->speed = GPIO_SPEED_HIGH;
    system->Button->type = GPIO_TYPE_PUSHPULL;
    userGPIO_Init(system->Button);
    userEXTI_Init(system->Button);
  }

  if(system->IWatchDog)
  {
    system->IWatchDog->frequancy = 32000;
    system->IWatchDog->watchPeriod = 3000;
    system->IWatchDog->debugMode = IWDG_DEBUGMODE_STOP;
    IWDG_Init(system->IWatchDog);
    IWDG_Enable();
  }

  if(system->sleepMode)
  {
    system->sleepMode->debugMode = SLEEPMODE_DEBUGMODE_SLEEP_RUN | SLEEPMODE_DEBUGMODE_STOP_RUN | SLEEPMODE_DEBUGMODE_STANDBY_RUN;
    system->sleepMode->deepSleepModeEn = SLEEPMODE_SLEEPDEEP_ON;
    system->sleepMode->sleepOnExitMode = SLEEPMODE_SLEEPONEXIT_OFF;
    system->sleepMode->deepSleepMode = SLEEPMODE_DEEPSLEEP_STANDBY;
    system->sleepMode->wakeUpPinEn = SLEEPMODE_WKUPPIN_ON;
    sleepMode_Init(system->sleepMode);
  }

  system->timer->baseTimer = (TIM_TypeDef*)TIM6_BASE;
  system->timer->mode = TIMER_MODE_COUNTER;
  system->timer->timerAPBFreq = system->pclk1 * 2;
  system->timer->timerOneCount = 500;
  system->timer->timerPeriod = 500000;
  userTimer_Init(system->timer);
  userTimer_Enable(system->timer);
}

uint8_t bouncingHandling(GPIO* GPIO)
{
  uint8_t tmp = 100;
  while((tmp > 0) && (tmp < 200))
  {
    if(GPIO->baseGPIO->IDR &(GPIO_IDR_IDR_0 << GPIO->pin)) tmp++;
    else tmp--;
    DelayUs(200);
  }
  return(tmp);
}

void DelayMs(uint16_t ms)
{
  uint32_t clockCycle = ms * (sys.hclk / 1000);
  clockCycle /= 5;
  for(uint32_t i = 0; i < clockCycle; i++);
}

void DelayUs(uint16_t us)
{
  uint32_t clockCycle = us * (sys.hclk / 1000000);
  clockCycle /= 5;
  for(uint32_t i = 0; i < clockCycle; i++);
}

void InterruptsPrioritySet()
{
  NVIC_SetPriority(EXTI1_IRQn, 10);
  NVIC_SetPriority(TIM6_DAC_IRQn, 15);          //Таймер системный
}

void EXTI0_IRQHandler()
{
  EXTI->PR |= EXTI_PR_PR0;

  if(bouncingHandling(sys.Button))
  {
    sys.timer->baseTimer->CR1 |= TIM_CR1_UDIS;
    sys.timer->baseTimer->ARR /= 2;
    if(sys.timer->baseTimer->ARR < 100) sys.timer->baseTimer->ARR = 9999;
    sys.timer->baseTimer->CR1 &= ~TIM_CR1_UDIS;

    sys.timer->baseTimer->CR1 |= TIM_CR1_CEN;
    sys.sleepMode->sleepMode_ON = 0;
  }
}

void TIM6_DAC_IRQHandler()
{
  sys.timer->baseTimer->SR &= ~TIM_SR_UIF;

  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(xLedTaskSemaphore, &xHigherPriorityTaskWoken);
  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}