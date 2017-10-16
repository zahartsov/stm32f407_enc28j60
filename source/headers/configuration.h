#ifndef __CONF_H__
#define __CONF_H__

#include "stm32f4xx.h"
#include "spi.h"
#include "enc28j60.h"
#include "system.h"

typedef struct
{
  uint8_t opType;
  uint8_t bytesNumber;
  uint8_t regAddr;
  uint8_t *addr;
  uint16_t phyRegMask;
  uint8_t data[5];
  uint8_t *bufData;
}SpiOpParam;

extern xQueueHandle xSpiMsgQueue;
extern xQueueHandle xEthMsgQueue;

void configureIt(void);

void LEDs_Init();

#endif