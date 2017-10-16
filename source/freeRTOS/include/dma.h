#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f4xx.h"
#include "vars.h"

//Priority
#define DMA_PRIORITY_LOW        0x00
#define DMA_PRIORITY_MEDIUM     0x01
#define DMA_PRIORITY_HIGH       0x02
#define DMA_PRIORITY_VERYHIGH   0x03

//Memoery/peripheral data size
#define DMA_DATASIZE_BYTE       0x00
#define DMA_DATASIZE_HALFWORD   0x01
#define DMA_DATASIZE_WORD       0x02

//Memory/peripheral increment
#define DMA_ADDRESS_INCREMENT_DISABLE   0x00
#define DMA_ADDRESS_INCREMENT_ENABLE    0x01

//Data transfer direction
#define DMA_PERIPHERAL_TO_MEMORY        0x00
#define DMA_MEMORY_TO_PERIPHERAL        0x01
#define DMA_MEMORY_TO_MEMORY            0x02

//Flow controller
#define DMA_FLOWCONTROLLER_DMA          0x00
#define DMA_FLOWCONTROLLER_PERIPHERAL   0x01

//Interrupts
#define DMA_NOINTERRUPTS                0x00
#define DMA_DIRECTMODEERRORIE           0x01
#define DMA_TRANSFERERRORIE             0x02
#define DMA_HALFTRANSFERIE              0x04
#define DMA_TRANSFERCOMPLETEIE          0x08

//Direct mode
#define DMA_DIRECTMODE_ENABLE           0x00
#define DMA_DIRECTMODE_DISABLE          0x01

//Circular mode
#define DMA_CIRCULAR_MODE_OFF           0x00
#define DMA_CIRCULAR_MODE_ON            0x01

//Double buffer mode
#define DMA_DOUBLE_BUFFER_MODE_OFF      0x00
#define DMA_DOUBLE_BUFFER_MODE_ON       0x01

typedef struct
{
  uint32_t peripheralAddress;
  uint32_t memoryAddress0;
  uint32_t memoryAddress1;
  uint16_t packageSize;
  uint8_t channel;
  uint8_t priority;
  uint8_t peripheralDataSize;
  uint8_t memoryDataSize;
  uint8_t peripheralIncrement;
  uint8_t memoryIncrement;
  uint8_t dataTransferDirection;
  uint8_t flowController;
  uint8_t interrupts;
  uint8_t directMode;
  uint8_t circularMode;
  uint8_t doubleBufferMode;
  DMA_TypeDef* baseDMA;
  DMA_Stream_TypeDef* baseStream;
}DMA;     // Параметры DMA

void userDMA_Init(DMA*);
void userDMA_Enable(DMA*);
void userDMA_Disable(DMA*);

void DMA1_Stream5_Init(void);
void DMA1_Stream6_Init(void);

#endif