/*################## Инициализация системной периферии #########################
Порядок инициализации:
1.
################################################################################*/

#include "configuration.h"

System sys;
Timer sysTimer;
GPIO sysLED;
GPIO sysButton;
rtc sysClock;

ENC28J60 enc28j60;
GPIO ethInt;
SPI ethSPI;
GPIO spiMISO;
GPIO spiMOSI;
GPIO spiSCK;
GPIO spiNSS;

xQueueHandle xSpiMsgQueue;
xQueueHandle xEthMsgQueue;


void configureIt()
{
  sys.hclk = 168000000;
  sys.pclk1 = 42000000;
  sys.pclk2 = 82000000;
  sys.timer = &sysTimer;
  sys.LED = &sysLED;
  sys.Button = &sysButton;
  sys.Clock = &sysClock;

  enc28j60.SPI = &ethSPI;
  enc28j60.intPin = &ethInt;

  spiMISO.baseGPIO = (GPIO_TypeDef*)GPIOA;
  spiMISO.pin = 6;
  spiMISO.mode = GPIO_MODE_AF;
  spiMISO.pull = GPIO_PULL_NOPULL;
  spiMISO.speed = GPIO_SPEED_HIGH;
  spiMISO.type = GPIO_TYPE_PUSHPULL;
  spiMISO.AF = AF5;
  userGPIO_Init(&spiMISO);

  spiMOSI.baseGPIO = (GPIO_TypeDef*)GPIOA;
  spiMOSI.pin = 7;
  spiMOSI.mode = GPIO_MODE_AF;
  spiMOSI.pull = GPIO_PULL_NOPULL;
  spiMOSI.speed = GPIO_SPEED_HIGH;
  spiMOSI.type = GPIO_TYPE_PUSHPULL;
  spiMOSI.AF = AF5;
  userGPIO_Init(&spiMOSI);

  spiSCK.baseGPIO = (GPIO_TypeDef*)GPIOA;
  spiSCK.pin = 5;
  spiSCK.mode = GPIO_MODE_AF;
  spiSCK.pull = GPIO_PULL_NOPULL;
  spiSCK.speed = GPIO_SPEED_HIGH;
  spiSCK.type = GPIO_TYPE_PUSHPULL;
  spiSCK.AF = AF5;
  userGPIO_Init(&spiSCK);

  spiNSS.baseGPIO = (GPIO_TypeDef*)GPIOE;
  spiNSS.pin = 3;
  spiNSS.mode = GPIO_MODE_OUTPUT;
  spiNSS.pull = GPIO_PULL_PULLUP;
  spiNSS.speed = GPIO_SPEED_HIGH;
  spiNSS.type = GPIO_TYPE_PUSHPULL;
  userGPIO_Init(&spiNSS);

  ethSPI.spiMISO = &spiMISO;
  ethSPI.spiMOSI = &spiMOSI;
  ethSPI.spiSCK = &spiSCK;
  ethSPI.spiNSS = &spiNSS;

  ethSPI.baseSPI = (SPI_TypeDef*)SPI1_BASE;
  ethSPI.dataCaptureEdge = SPI_CAPTURE_EDGE_FRONT;
  ethSPI.dataFrameFormat = SPI_FRAME_8BIT;
  ethSPI.DMAEnable = SPI_DMADIS;
  ethSPI.fpclkDIV = SPI_FREQ_FPCLK_DIV8;
  ethSPI.frameTransitionDirection = SPI_FRAME_TRANS_DIR_MSBFIRST;
  ethSPI.idleCLKPolarity = SPI_CLK_IDLE_LOW;
  ethSPI.interrupts = SPI_RXNEIE;
  ethSPI.mode = SPI_MASTER;
  ethSPI.softwareSlaveManagement = SPI_SLAVE_MANAGEMENT_SOFT;
  ethSPI.slaveSelect = SPI_SLAVE_SELECT;
  userSPI_Init(&ethSPI);
  userSPI_Enable(&ethSPI);

  ethInt.interruptFalling = EXTI_FALLING_ENABLE;
  ethInt.interruptRising = EXTI_RISING_DISABLE;
  ethInt.baseGPIO = (GPIO_TypeDef*)GPIOA_BASE;
  ethInt.pin = 1;
  ethInt.mode = GPIO_MODE_INPUT;
  ethInt.pull = GPIO_PULL_NOPULL;
  ethInt.speed = GPIO_SPEED_HIGH;
  ethInt.type = GPIO_TYPE_PUSHPULL;
  userGPIO_Init(&ethInt);
  userEXTI_Init(&ethInt);

  //IndependentWatchDog IWatchDog;
  //sys.IWatchDog = &IWatchDog;
  //SleepMode sleepMode;
  //sys.sleepMode = &sleepMode;

  InterruptsPrioritySet();
  ENC_Init(&enc28j60);
  System_Init(&sys);
}