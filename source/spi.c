/*##################################### SPI ##################################
������� �������������:
1. ��������� ������������ ������� � ���������������� � RCC->APB�ENR, ��� � - ����� �������� ��� ���������������� ������ SPI.
2. � �������� SPI�->CR1 �����������:
  - ������ ����� ������:
    SPI_CR1_DFF - 16 ���;
    ~SPI_CR1_DFF - 8 ���;
  - ������� �������������:
    SPI_CR1_BR - 000 - fpclk/2;
    SPI_CR1_BR_0 - 001 - fpclk/4;
    SPI_CR1_BR_1 - 010 - fpclk/8;
    SPI_CR1_BR_2 - 100 - fpclk/32;
    ~SPI_CR1_BR - 111 - fpclk/256;
    ������������� �������� - ����������� ����� (��. �������);
  - ����� ������ ������:
    SPI_CR1_MSTR - �������;
    ~SPI_CR1_MSTR - �������;
  - ������� ������� �� ����� ������������� � ������ ��������:
    SPI_CR1_CPOL - �������;
    ~SPI_CR1_CPOL - ������;
  - ����� ��������, �� �������� ���������� ����/�������� ������:
    SPI_CR1_CPHA - �������� �����;
    ~SPI_CR1_CPHA - ������ �����;
  - ������� �������� ������:
    SPI_CR1_LSBFIRST - ������� ����� �����;
    ~SPI_CR1_LSBFIRST - ������� ����� �����;
  - ����� ���������� �������:
    SPI_CR1_SSI - ���. ����������� ����� ��������;
    ~SPI_CR1_SSI - ����. ����������� ����� ��������;
3. � �������� SPI�->CR2 �������� ����������� ����������:
  SPI_CR2_ERRIE - ���������� ��� ������������� ������;
  SPI_CR2_RXNEIE - ���������� ��������;
  SPI_CR2_TXEIE - ���������� �����������;
4. ��������� ������ ���������� ��������������� ������ � NVIC:
  NVIC_EnableIRQ(SPI�_IRQn);
5. �������� ������ SPI � �������� SPI�->CR1 ���������� ���� SPI_CR1_SPE.
################################################################################*/


#include "spi.h"

void userSPI_Init(SPI* SPI)
{
  if(SPI->baseSPI == (SPI_TypeDef*)SPI1)
  {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    NVIC_EnableIRQ(SPI1_IRQn);
  }
  else if(SPI->baseSPI == (SPI_TypeDef*)SPI2)
  {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    NVIC_EnableIRQ(SPI2_IRQn);
  }
  else if(SPI->baseSPI == (SPI_TypeDef*)SPI3)
  {
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    NVIC_EnableIRQ(SPI3_IRQn);
  }

  if(SPI->dataFrameFormat == SPI_FRAME_8BIT) SPI->baseSPI->CR1 &= ~SPI_CR1_DFF;
  else SPI->baseSPI->CR1 |= SPI_CR1_DFF;

  if(SPI->fpclkDIV == SPI_FREQ_FPCLK_DIV2) SPI->baseSPI->CR1 &= ~SPI_CR1_BR;
  else SPI->baseSPI->CR1 |= (SPI->fpclkDIV & 0x07) << 3;

  if(SPI->mode == SPI_SLAVE) SPI->baseSPI->CR1 &= ~SPI_CR1_MSTR;
  else SPI->baseSPI->CR1 |= SPI_CR1_MSTR;

  if(SPI->idleCLKPolarity == SPI_CLK_IDLE_LOW) SPI->baseSPI->CR1 &= ~SPI_CR1_CPOL;
  else SPI->baseSPI->CR1 |= SPI_CR1_CPOL;

  if(SPI->dataCaptureEdge == SPI_CAPTURE_EDGE_FRONT) SPI->baseSPI->CR1 &= ~SPI_CR1_CPHA;
  else SPI->baseSPI->CR1 |= SPI_CR1_CPHA;

  if(SPI->frameTransitionDirection == SPI_FRAME_TRANS_DIR_MSBFIRST) SPI->baseSPI->CR1 &= ~SPI_CR1_LSBFIRST;
  else SPI->baseSPI->CR1 |= SPI_CR1_LSBFIRST;

  if(SPI->softwareSlaveManagement == SPI_SLAVE_MANAGEMENT_HARD) SPI->baseSPI->CR1 &= ~SPI_CR1_SSM;
  else SPI->baseSPI->CR1 |= SPI_CR1_SSM;

  if(SPI->slaveSelect == SPI_SLAVE_DESELECT) SPI->baseSPI->CR1 &= ~SPI_CR1_SSI;
  else SPI->baseSPI->CR1 |= SPI_CR1_SSI;

  if(SPI->interrupts == SPI_NO_INTERRUPTS) SPI->baseSPI->CR2 &= ~(SPI_EVERYIE << 5);
  else SPI->baseSPI->CR2 |= (SPI->interrupts & 0x07) << 5;

  if(SPI->DMAEnable == SPI_DMADIS) SPI->baseSPI->CR2 &= ~(0x03);
  else SPI->baseSPI->CR2 |= SPI->DMAEnable;
}

void userSPI_Enable(SPI* SPI)
{
  SPI->baseSPI->CR1 |= SPI_CR1_SPE;
}

void userSPI_Disable(SPI* SPI)
{
  SPI->baseSPI->CR1 &= ~SPI_CR1_SPE;
}

void userSPI_SSselect(SPI* SPI)
{
  SPI->spiNSS->baseGPIO->BSRRH |= GPIO_BSRR_BS_0 << SPI->spiNSS->pin;
}

void userSPI_SSdeselect(SPI* SPI)
{
  SPI->spiNSS->baseGPIO->BSRRL |= GPIO_BSRR_BS_0 << SPI->spiNSS->pin;
}

void userSPI_WriteByte(SPI* SPI, uint8_t byte)
{
  while(!(SPI->baseSPI->SR & SPI_SR_TXE));
  SPI->baseSPI->DR = byte;
  DelayUs(1);
  while(SPI->baseSPI->SR & SPI_SR_BSY);
}