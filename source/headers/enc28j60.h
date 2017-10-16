#ifndef __ENC28J60_H__
#define __ENC28J60_H__

#include "stm32f4xx.h"
#include "system.h"
#include "spi.h"
#include "net.h"
#include "configuration.h"

#define ip_addr(a,b,c,d)    ( ((uint32_t)a) | ((uint32_t)b << 8) |\
                                ((uint32_t)c << 16) | ((uint32_t)d << 24) )

// Перекодирование word'а
#define htons(a)            ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define ntohs(a)            htons(a)

// Перекодирование dword'а
#define htonl(a)            ( (((a)>>24)&0xff) | (((a)>>8)&0xff00) |\
                                (((a)<<8)&0xff0000) | (((a)<<24)&0xff000000) )
#define ntohl(a)            htonl(a)


//Opcodes
#define OP_READ_REG      0x00
#define OP_READ_BUF      0x3A
#define OP_WRITE_REG     0x40
#define OP_WRITE_BUF     0x7A
#define OP_BFS           0x80
#define OP_BFC           0xA0
#define OP_SOFT_RESET    0xE0
#define OP_WRITE_PHY_REG 0x01
#define OP_READ_MII_REG  0x02
#define OP_READ_PHY_REG  0x03
//--------------------------------------------------
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
//--------------------------------------------------
//All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
//--------------------------------------------------
// Bank 0 registers
#define ERDPT           (0x00|0x00)
#define EWRPT           (0x02|0x00)
#define ETXST           (0x04|0x00)
#define ETXND           (0x06|0x00)
#define ERXST           (0x08|0x00)
#define ERXND           (0x0A|0x00)
#define ERXRDPT         (0x0C|0x00)
#define ERXWRPT         (0x0E|0x00)
#define EDMADST         (0x14|0x00)
//--------------------------------------------------
// Bank 1 registers
#define EPMM0           (0x08|0x20)
#define EPMM1           (0x09|0x20)
#define EPMCS           (0x10|0x20)
#define ERXFCON         (0x18|0x20)
#define EPKTCNT         (0x19|0x20)
//--------------------------------------------------
// Bank 2 registers
#define MACON1          (0x00|0x40)
#define MACON2          (0x01|0x40)
#define MACON3          (0x02|0x40)
#define MACON4          (0x03|0x40)
#define MABBIPG         (0x04|0x40)
#define MAIPG           (0x06|0x40)
#define MAMXFL          (0x0A|0x40)
#define MICMD           (0x12|0x40)
#define MIREGADR        (0x14|0x40)
#define MIWR            (0x16|0x40)
#define MIRD            (0x18|0x40)
//--------------------------------------------------
// Bank 3 registers
#define MAADR0           (0x01|0x60)
#define MAADR1           (0x00|0x60)
#define MAADR2           (0x03|0x60)
#define MAADR3           (0x02|0x60)
#define MAADR4           (0x05|0x60)
#define MAADR5           (0x04|0x60)
#define MISTAT           (0x0A|0x60)
//--------------------------------------------------
//PHY registers
#define PHCON1          0x00
#define PHCON2          0x10
#define PHLCON          0x14
//PHY registers mask
#define PHCON1_MASK     0x480
#define PHCON2_MASK     0x1AFF
#define PHLCON_MASK     0xF001
//--------------------------------------------------
//MICMD
#define MICMD_MIIRD     0x01
//--------------------------------------------------
// ECON1
#define ECON1_RXEN      0x04
#define ECON1_TXRTS     0x08
#define ECON1_TXRST     0x40
//--------------------------------------------------
// ECON2
#define ECON2_AUTOINC   0x80
#define ECON2_PKTDEC    0x40
#define ECON2_PWRSV     0x20
//--------------------------------------------------
// EIR
#define EIR_TXERIF      0x02
#define EIR_PKTIF       0x40
//--------------------------------------------------
// EIE
#define EIE_PKTIE       0x40
#define EIE_INTIE       0x80
//--------------------------------------------------
// MACON1
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
//--------------------------------------------------
// MACON3
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
//--------------------------------------------------
// PHLCON
#define PHLCON_LACFG3  0x0800
#define PHLCON_LACFG2  0x0400
#define PHLCON_LACFG1  0x0200
#define PHLCON_LACFG0  0x0100
#define PHLCON_LBCFG3  0x0080
#define PHLCON_LBCFG2  0x0040
#define PHLCON_LBCFG1  0x0020
#define PHLCON_LBCFG0  0x0010
#define PHLCON_LFRQ1  0x0008
#define PHLCON_LFRQ0  0x0004
#define PHLCON_STRCH  0x0002
//--------------------------------------------------


#define ENC28J60_BUFSIZE        0x2000
#define ENC28J60_RXSIZE         0x1A00
#define ENC28J60_MAXFRAME       1500

#define ENC28J60_RXSTART        0
#define ENC28J60_RXEND          (ENC28J60_RXSIZE-1)
#define ENC28J60_TXSTART        ENC28J60_RXSIZE
#define ENC28J60_BUFEND         (ENC28J60_BUFSIZE-1)



typedef struct
{
  SPI* SPI;
  GPIO* intPin;
  uint8_t currentBank;
  uint8_t RxBuffer[ENC28J60_MAXFRAME];
  uint8_t log[2048];
  uint16_t logCounter;
  uint8_t RxLength;
  uint8_t RxCounter;
  uint8_t TxBuffer[ENC28J60_MAXFRAME];
  uint8_t TxLength;
  uint16_t phyRegValue;
  uint16_t phyRegMask;
  uint8_t macadr[6];
  uint32_t ipAddr;
  uint16_t rxrdpt;
  uint8_t ECON1_state;
  uint8_t ESTAT_state;
  uint8_t EIR_state;
  uint8_t EPKTCNT_state;
  uint8_t ERXWRPTL_state;
  uint8_t ERXWRPTH_state;
  uint8_t ERDPTL_state;
  uint8_t ERDPTH_state;
  uint8_t lastReadReg;
}ENC28J60;           // Параметры АЦП

extern ENC28J60 enc28j60;
extern SPI ethSPI;
extern uint8_t netBufRx[ENC28J60_MAXFRAME];
extern uint8_t netBufTx[ENC28J60_MAXFRAME];

void ENC_Init(ENC28J60*);
void ENC_ReadReg(ENC28J60*);
void ENC_WriteReg(ENC28J60*);
void ENC_BFS(ENC28J60*);
void ENC_BFC(ENC28J60*);
void ENC_BankCheck(ENC28J60*);
void ENC_SoftReset(ENC28J60*);

void ENC_ReadMIIReg(ENC28J60*);
void ENC_ReadPhyReg(ENC28J60*);
void ENC_WritePhyReg(ENC28J60*);
void ENC_ReadBuffer(ENC28J60*);
void ENC_WriteBuffer(ENC28J60*);

void ENC_SendPackage(ENC28J60*);
void ENC_ReceivePackage(ENC28J60*);
#endif