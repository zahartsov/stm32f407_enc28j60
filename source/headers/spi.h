#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx.h"
#include "gpio.h"
#include "system.h"

//Data frame format
#define SPI_FRAME_8BIT          0x00
#define SPI_FRAME_16BIT         0x01

//Synchronization frequancy
#define SPI_FREQ_FPCLK_DIV2     0x00
#define SPI_FREQ_FPCLK_DIV4     0x01
#define SPI_FREQ_FPCLK_DIV8     0x02
#define SPI_FREQ_FPCLK_DIV16    0x03
#define SPI_FREQ_FPCLK_DIV32    0x04
#define SPI_FREQ_FPCLK_DIV64    0x05
#define SPI_FREQ_FPCLK_DIV128   0x06
#define SPI_FREQ_FPCLK_DIV256   0x07

//Mode
#define SPI_SLAVE               0x00
#define SPI_MASTER              0x01

//Free CLK polarity
#define SPI_CLK_IDLE_LOW        0x00
#define SPI_CLK_IDLE_HIGH       0x01

//Data capture edge
#define SPI_CAPTURE_EDGE_FRONT  0x00
#define SPI_CAPTURE_EDGE_DROOP  0x01

//Frame transition direction
#define SPI_FRAME_TRANS_DIR_MSBFIRST    0x00
#define SPI_FRAME_TRANS_DIR_LSBFIRST    0x01

//Software slave management
#define SPI_SLAVE_MANAGEMENT_HARD       0x00
#define SPI_SLAVE_MANAGEMENT_SOFT       0x01

//Slave slecet
#define SPI_SLAVE_DESELECT              0x00
#define SPI_SLAVE_SELECT                0x01

//Interrupts
#define SPI_NO_INTERRUPTS               0x00
#define SPI_ERRORIE                     0x01
#define SPI_RXNEIE                      0x02
#define SPI_TXEIE                       0x04
#define SPI_EVERYIE                     0x07

//DMA enable
#define SPI_DMADIS                      0x00
#define SPI_RXDMAEN                     0x01
#define SPI_TXDMAEN                     0x02

typedef struct
{
  SPI_TypeDef* baseSPI;
  GPIO* spiMISO;
  GPIO* spiMOSI;
  GPIO* spiSCK;
  GPIO* spiNSS;
  uint8_t RxBuffer[128];
  uint8_t RxCounter;
  uint8_t TxBuffer[128];
  uint8_t TxCounter;
  uint8_t dataFrameFormat;
  uint8_t fpclkDIV;
  uint8_t mode;
  uint8_t idleCLKPolarity;
  uint8_t dataCaptureEdge;
  uint8_t frameTransitionDirection;
  uint8_t softwareSlaveManagement;
  uint8_t slaveSelect;
  uint8_t interrupts;
  uint8_t DMAEnable;
}SPI;     // Параметры таймера

void userSPI_Init(SPI* SPI);
void userSPI_Enable(SPI* SPI);
void userSPI_Disable(SPI* SPI);
void userSPI_SSselect(SPI* SPI);
void userSPI_SSdeselect(SPI* SPI);
void userSPI_WriteByte(SPI*, uint8_t);

#endif