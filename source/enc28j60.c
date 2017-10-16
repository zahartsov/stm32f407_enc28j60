#include "enc28j60.h"


/*################################################
* Task: vEncPollTask
* Description: Опрашивает ключевые регистры микросхемы для контроля состояния.
##################################################*/
void vEncPollTask(void *pvParameters)
{
  portBASE_TYPE xStatus;
  SpiOpParam nextSpiOperation;

  while(1)
  {
      nextSpiOperation.opType = OP_READ_REG;
      nextSpiOperation.bytesNumber = 1;
      nextSpiOperation.regAddr = ESTAT;
      nextSpiOperation.addr = &enc28j60.ESTAT_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = EIR;
      nextSpiOperation.addr = &enc28j60.EIR_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = ECON1;
      nextSpiOperation.addr = &enc28j60.ECON1_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = EPKTCNT;
      nextSpiOperation.addr = &enc28j60.EPKTCNT_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = ERXWRPT;
      nextSpiOperation.addr = &enc28j60.ERXWRPTL_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = ERXWRPT+1;
      nextSpiOperation.addr = &enc28j60.ERXWRPTH_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

      nextSpiOperation.regAddr = ERDPT;
      nextSpiOperation.addr = &enc28j60.ERDPTL_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      nextSpiOperation.regAddr = ERDPT+1;
      nextSpiOperation.addr = &enc28j60.ERDPTH_state;
      xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);


      if((enc28j60.EIR_state & 0x01) && (enc28j60.EPKTCNT_state > 0))
      {
        nextSpiOperation.opType = OP_READ_BUF;
        xStatus = xQueueSendToBackFromISR(xSpiMsgQueue,&nextSpiOperation,0);
      }
      else if (enc28j60.EIR_state & 0x01)
      {
        nextSpiOperation.opType = OP_BFC;
        nextSpiOperation.bytesNumber = 1;
        nextSpiOperation.regAddr = EIR;
        xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
      }

      vTaskDelay(50/portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}


/*################################################
* Task: vEthFilterTask
* Description: Определяет тип полученного сообщения и передаёт его соотвествующей функции.
##################################################*/
void vEthFilterTask(void *pvParameters)
{
  portBASE_TYPE xStatus;
  MsgStruct currMsg;

  while(1)
  {
    xStatus = xQueueReceive(xEthMsgQueue,&currMsg,portMAX_DELAY);
    if(xStatus == pdPASS)
    {
      switch(currMsg.ethMsgStruct.type)
      {
      case ETH_TYPE_ARP:
          arp_filter(&currMsg.ethMsgStruct, enc28j60.RxLength - sizeof(EthMsgStruct));
          break;

      case ETH_TYPE_IP:
          ip_filter(&currMsg.ethMsgStruct, enc28j60.RxLength - sizeof(EthMsgStruct));
          break;
      }

      enc28j60.TxBuffer[0] = EIE;
      enc28j60.TxBuffer[1] = EIE_INTIE;
      ENC_BFS(&enc28j60);

    }
  }
  vTaskDelete(NULL);
}


/*################################################
* Task: vSpiTask
* Description: Осуществляет связь с микросхемой по SPI.
##################################################*/
void vSpiTask(void *pvParameters)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  SpiOpParam currOperation;
  portBASE_TYPE xStatus;

  while(1)
  {
    xStatus = xQueueReceive(xSpiMsgQueue,&currOperation,portMAX_DELAY);
    if (xStatus == pdPASS)
    {
      if(currOperation.opType == OP_WRITE_REG)
      {
        for(uint8_t i = 0; i < currOperation.bytesNumber; i++)
        {
          enc28j60.TxBuffer[0] = currOperation.regAddr + i;
          enc28j60.TxBuffer[1] = currOperation.data[i];
          ENC_WriteReg(&enc28j60);
        }
      }
      else if(currOperation.opType == OP_READ_REG)
      {
        for(uint8_t i = 0; i < currOperation.bytesNumber; i++)
        {
          enc28j60.RxLength = i;
          enc28j60.TxBuffer[0] = currOperation.regAddr + i;
          ENC_ReadReg(&enc28j60);
          *currOperation.addr = enc28j60.RxBuffer[0];
          enc28j60.lastReadReg = enc28j60.RxBuffer[0];
        }
      }
      else if(currOperation.opType == OP_SOFT_RESET)
      {
        ENC_SoftReset(&enc28j60);
        vTaskDelayUntil(&xLastWakeTime, (2/portTICK_RATE_MS));
      }
      else if(currOperation.opType == OP_READ_MII_REG)
      {
        enc28j60.TxBuffer[0] = currOperation.regAddr;
        ENC_ReadMIIReg(&enc28j60);
        *currOperation.addr = enc28j60.RxBuffer[0];
        enc28j60.lastReadReg = enc28j60.RxBuffer[0];
      }
      else if(currOperation.opType == OP_WRITE_PHY_REG)
      {
        enc28j60.TxBuffer[0] = currOperation.regAddr;
        enc28j60.TxBuffer[1] = currOperation.data[0];
        enc28j60.TxBuffer[2] = currOperation.data[1];
        ENC_WritePhyReg(&enc28j60);
      }
      else if(currOperation.opType == OP_READ_PHY_REG)
      {
        enc28j60.TxBuffer[0] = currOperation.regAddr;
        ENC_ReadPhyReg(&enc28j60);
        *currOperation.addr = enc28j60.RxBuffer[0];
        enc28j60.lastReadReg = enc28j60.RxBuffer[0];
      }
      else if(currOperation.opType == OP_BFS)
      {
        enc28j60.TxBuffer[0] = currOperation.regAddr;
        enc28j60.TxBuffer[1] = currOperation.data[0];
        ENC_BFS(&enc28j60);
      }
      else if(currOperation.opType == OP_BFC)
      {
        enc28j60.TxBuffer[0] = currOperation.regAddr;
        enc28j60.TxBuffer[1] = currOperation.data[0];
        ENC_BFC(&enc28j60);
      }
      else if(currOperation.opType == OP_WRITE_BUF)
      {
        enc28j60.TxLength = currOperation.bytesNumber + 1;
        for(uint8_t i = 0; i < enc28j60.TxLength-1; i++) enc28j60.TxBuffer[i+1] = currOperation.bufData[i];
        ENC_SendPackage(&enc28j60);
      }
      else if(currOperation.opType == OP_READ_BUF)
      {
        ENC_ReceivePackage(&enc28j60);
      }
    }
    vTaskDelayUntil(&xLastWakeTime, (1/portTICK_RATE_MS));

  }
  vTaskDelete(NULL);
}

/*################################################
* Function: ENC_Init
* Description: инициализация микросхемы enc28j60.
##################################################*/
void ENC_Init(ENC28J60* enc28j60)
{
  portBASE_TYPE xStatus;
  xSpiMsgQueue = xQueueCreate(20,sizeof(SpiOpParam));
  if (xSpiMsgQueue != NULL)
  {
    xTaskCreate(vSpiTask,"SpiOpTask",configMINIMAL_STACK_SIZE*5, NULL, tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate(vEncPollTask,"EncPollTask",configMINIMAL_STACK_SIZE*5, NULL, tskIDLE_PRIORITY + 2, NULL);
  }
  xEthMsgQueue = xQueueCreate(10,sizeof(EthMsgStruct));
  if(xEthMsgQueue != NULL)
  {
    xTaskCreate(vEthFilterTask,"EthFilterTask",configMINIMAL_STACK_SIZE*5, NULL, tskIDLE_PRIORITY + 3, NULL);
  }

  enc28j60->ipAddr = ip_addr(172,16,12,59);

  SpiOpParam nextSpiOperation;
  nextSpiOperation.opType = OP_SOFT_RESET;
  nextSpiOperation.bytesNumber = 1;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
  //if(xStatus != pdPASS)

  enc28j60->currentBank = 0x00;
  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = ECON1;
  nextSpiOperation.data[0] = 0x00;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 2;
  nextSpiOperation.regAddr = ERXST;
  nextSpiOperation.data[0] = ENC28J60_RXSTART & 0xFF;
  nextSpiOperation.data[1] = ENC28J60_RXSTART >> 8;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 2;
  nextSpiOperation.regAddr = ERXND;
  nextSpiOperation.data[0] = ENC28J60_RXEND & 0xFF;
  nextSpiOperation.data[1] = ENC28J60_RXEND >> 8;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  enc28j60->rxrdpt = ENC28J60_RXSTART;
  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 2;
  nextSpiOperation.regAddr = ERXRDPT;
  nextSpiOperation.data[0] = ENC28J60_RXSTART & 0xFF;
  nextSpiOperation.data[1] = ENC28J60_RXSTART >> 8;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = MACON2;
  nextSpiOperation.data[0] = 0x00;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = MACON1;
  nextSpiOperation.data[0] = MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = MACON3;
  nextSpiOperation.data[0] = MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 2;
  nextSpiOperation.regAddr = MAMXFL;
  nextSpiOperation.data[0] = ENC28J60_MAXFRAME & 0xFF;
  nextSpiOperation.data[1] = ENC28J60_MAXFRAME >> 8;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = MABBIPG;
  nextSpiOperation.data[0] = 0x15;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 2;
  nextSpiOperation.regAddr = MAIPG;
  nextSpiOperation.data[0] = 0x12;
  nextSpiOperation.data[1] = 0x0C;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  enc28j60->macadr[5] = 0xCD;
  enc28j60->macadr[4] = 0x17;
  enc28j60->macadr[3] = 0x01;
  enc28j60->macadr[2] = 0x4E;
  enc28j60->macadr[1] = 0x8D;
  enc28j60->macadr[0] = 0x00;

  nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 6;
  nextSpiOperation.regAddr = MAADR1;
  nextSpiOperation.data[0] = 0x17;
  nextSpiOperation.data[1] = 0xCD;
  nextSpiOperation.data[2] = 0x4E;
  nextSpiOperation.data[3] = 0x01;
  nextSpiOperation.data[4] = 0x00;
  nextSpiOperation.data[5] = 0x8D;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  /*nextSpiOperation.opType = OP_WRITE_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = ERXFCON;
  nextSpiOperation.data[0] = 0x00;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);*/

  nextSpiOperation.opType = OP_WRITE_PHY_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.phyRegMask = PHCON1_MASK;
  nextSpiOperation.regAddr = PHCON1;
  nextSpiOperation.data[0] = 0x00;
  nextSpiOperation.data[1] = 0x01;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_PHY_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.phyRegMask = PHCON2_MASK;
  nextSpiOperation.regAddr = PHCON2;
  nextSpiOperation.data[0] = 0x00;
  nextSpiOperation.data[1] = 0x01;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_WRITE_PHY_REG;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.phyRegMask = PHLCON_MASK;
  nextSpiOperation.regAddr = PHLCON;
  nextSpiOperation.data[0] = PHLCON_LBCFG2 | PHLCON_LFRQ0 | PHLCON_STRCH;
  nextSpiOperation.data[1] = (PHLCON_LACFG2 >> 8) | (PHLCON_LACFG1 >> 8) | (PHLCON_LACFG0 >> 8);
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_BFS;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = EIE;
  nextSpiOperation.data[0] = EIE_PKTIE | EIE_INTIE;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);

  nextSpiOperation.opType = OP_BFS;
  nextSpiOperation.bytesNumber = 1;
  nextSpiOperation.regAddr = ECON1;
  nextSpiOperation.data[0] = ECON1_RXEN;
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
}

/*################################################
* Function: ENC_ReadReg
* Description: чтение регистра микросхемы enc28j60.
##################################################*/
void ENC_ReadReg(ENC28J60* enc28j60)
{
  ENC_BankCheck(enc28j60);

  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, (OP_READ_REG | (enc28j60->TxBuffer[0] & ADDR_MASK)));
  userSPI_WriteByte(enc28j60->SPI, 0xFF);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
  while(enc28j60->SPI->baseSPI->SR & SPI_SR_RXNE);
}

/*################################################
* Function: ENC_ReadMIIReg
* Description: чтение MII регистра микросхемы enc28j60.
##################################################*/
void ENC_ReadMIIReg(ENC28J60* enc28j60)
{
  ENC_BankCheck(enc28j60);

  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, (OP_READ_REG | (enc28j60->TxBuffer[0] & ADDR_MASK)));
  userSPI_WriteByte(enc28j60->SPI, 0xFF);
  userSPI_WriteByte(enc28j60->SPI, 0xFF);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
  while(enc28j60->SPI->baseSPI->SR & SPI_SR_RXNE);
}

/*################################################
* Function: ENC_WriteReg
* Description: запись регистра микросхемы enc28j60.
##################################################*/
void ENC_WriteReg(ENC28J60* enc28j60)
{
  ENC_BankCheck(enc28j60);

  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, (OP_WRITE_REG | (enc28j60->TxBuffer[0] & ADDR_MASK)));
  userSPI_WriteByte(enc28j60->SPI, enc28j60->TxBuffer[1]);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
}

/*################################################
* Function: ENC_ReadPhyReg
* Description: чтение PHY регистра микросхемы enc28j60.
##################################################*/
void ENC_ReadPhyReg(ENC28J60* enc28j60)
{
  //Записывем адрес физ регистра в регистр MIREGADR
  enc28j60->TxBuffer[1] = enc28j60->TxBuffer[0];
  enc28j60->TxBuffer[0] = MIREGADR;
  ENC_WriteReg(enc28j60);
  DelayUs(1);

  //Устанавливаме бит MIIRD в регистре MICMD
  enc28j60->TxBuffer[0] = MICMD;
  enc28j60->TxBuffer[1] = 0x01;
  ENC_WriteReg(enc28j60);
  DelayUs(2);

  //Читаем статус чтения регистра
  enc28j60->RxBuffer[0] = 0x01;
  while((enc28j60->RxBuffer[0] & 0x01) != 0)
  {
    enc28j60->TxBuffer[0] = MISTAT;
    ENC_ReadMIIReg(enc28j60);
  }

  //Сбрасываем бит MIIRD в регистре MICMD
  enc28j60->TxBuffer[0] = MICMD;
  enc28j60->TxBuffer[1] = 0x00;
  ENC_WriteReg(enc28j60);

  //Когда запись физ регистра в MII регистры завершена - читаем их
  enc28j60->TxBuffer[0] = MIRD;
  ENC_ReadMIIReg(enc28j60);
  enc28j60->phyRegValue = enc28j60->RxBuffer[0];
  enc28j60->TxBuffer[0] = MIRD+1;
  ENC_ReadMIIReg(enc28j60);
  enc28j60->phyRegValue |= enc28j60->RxBuffer[0] << 8;
}

/*################################################
* Function: ENC_WritePhyReg
* Description: запись PHY регистра микросхемы enc28j60.
##################################################*/
void ENC_WritePhyReg(ENC28J60* enc28j60)
{
  uint16_t phyNewRegValue = enc28j60->TxBuffer[1] | (enc28j60->TxBuffer[2] << 8);
  uint8_t phyAdr = enc28j60->TxBuffer[0];
  ENC_ReadPhyReg(enc28j60);

  /*uint16_t tmp = (enc28j60->phyRegValue & ~(enc28j60->phyRegMask)) ^ (phyNewRegValue & ~(enc28j60->phyRegMask));
  enc28j60->phyRegValue = enc28j60->phyRegValue & ~(tmp);
  phyNewRegValue = enc28j60->phyRegValue | (phyNewRegValue & ~(enc28j60->phyRegMask));*/

  enc28j60->phyRegValue = (enc28j60->phyRegValue & enc28j60->phyRegMask) | (phyNewRegValue & ~(enc28j60->phyRegMask));

  //Записывем адрес физ регистра в регистр MIREGADR
  enc28j60->TxBuffer[1] = phyAdr;
  enc28j60->TxBuffer[0] = MIREGADR;
  ENC_WriteReg(enc28j60);
  DelayUs(2);

  //Записывем данные в регистр MIWR
  enc28j60->TxBuffer[0] = MIWR;
  enc28j60->TxBuffer[1] = enc28j60->phyRegValue & 0xFF;
  ENC_WriteReg(enc28j60);
  enc28j60->TxBuffer[0] = MIWR+1;
  enc28j60->TxBuffer[1] = enc28j60->phyRegValue >> 8;
  ENC_WriteReg(enc28j60);
  DelayUs(10);
}

/*################################################
* Function: ENC_BFS
* Description: установка определённых битов регистра микросхемы enc28j60.
##################################################*/
void ENC_BFS(ENC28J60* enc28j60)
{
  ENC_BankCheck(enc28j60);

  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, (OP_BFS | (enc28j60->TxBuffer[0] & ADDR_MASK)));
  userSPI_WriteByte(enc28j60->SPI, enc28j60->TxBuffer[1]);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
}


/*################################################
* Function: ENC_BFC
* Description: сброс определённых битов регистра микросхемы enc28j60.
##################################################*/
void ENC_BFC(ENC28J60* enc28j60)
{
  ENC_BankCheck(enc28j60);

  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, (OP_BFC | (enc28j60->TxBuffer[0] & ADDR_MASK)));
  userSPI_WriteByte(enc28j60->SPI, enc28j60->TxBuffer[1]);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
}

/*################################################
* Function: ENC_WriteBuffer
* Description: запись в буфер микросхемы enc28j60.
##################################################*/
void ENC_WriteBuffer(ENC28J60* enc28j60)
{
  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, OP_WRITE_BUF);
  for(uint8_t i = 0; i < enc28j60->TxLength; i++) userSPI_WriteByte(enc28j60->SPI, enc28j60->TxBuffer[i]);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
}

/*################################################
* Function: ENC_ReadBuffer
* Description: чтение буфера микросхемы enc28j60.
##################################################*/
void ENC_ReadBuffer(ENC28J60* enc28j60)
{
  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, OP_READ_BUF);
  /*enc28j60->RxCounter = 0;
  while(enc28j60->RxLength--)
  {
    userSPI_WriteByte(enc28j60->SPI, 0xFF);
    while(enc28j60->SPI->baseSPI->SR & SPI_SR_RXNE);
  }*/
  for(enc28j60->RxCounter = 0; enc28j60->RxCounter < enc28j60->RxLength; enc28j60->RxCounter++)
  {
    userSPI_WriteByte(enc28j60->SPI, 0xFF);
    while(enc28j60->SPI->baseSPI->SR & SPI_SR_RXNE);
  }
  enc28j60->RxCounter = 0;
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
  enc28j60->RxLength = 0;
}

/*################################################
* Function: ENC_SendPackge
* Description: отправка пакета данных микросхеме enc28j60 для отправки сетевому адресату.
##################################################*/
void ENC_SendPackage(ENC28J60* enc28j60)
{
  enc28j60->RxBuffer[0] = ECON1_TXRTS;
  while((enc28j60->RxBuffer[0] & ECON1_TXRTS) != 0)
  {
    enc28j60->TxBuffer[0] = EIR;
    ENC_ReadReg(enc28j60);
    if(enc28j60->RxBuffer[0] & EIR_TXERIF)
    {
      enc28j60->TxBuffer[0] = ECON1;
      enc28j60->TxBuffer[1] = ECON1_TXRST;
      ENC_BFS(enc28j60);
      ENC_BFC(enc28j60);
    }

    enc28j60->TxBuffer[0] = ECON1;
    ENC_ReadReg(enc28j60);
  }

  enc28j60->TxBuffer[0] = EWRPT;
  enc28j60->TxBuffer[1] = ENC28J60_TXSTART & 0xFF;
  ENC_WriteReg(enc28j60);
  enc28j60->TxBuffer[0] = EWRPT+1;
  enc28j60->TxBuffer[1] = ENC28J60_TXSTART >> 8;
  ENC_WriteReg(enc28j60);
  DelayUs(1);

  enc28j60->TxBuffer[0] = 0x00;
  enc28j60->TxBuffer[1] = 0x00;
  ENC_WriteBuffer(enc28j60);
  DelayUs(1);

  enc28j60->TxBuffer[0] = ETXST;
  enc28j60->TxBuffer[1] = ENC28J60_TXSTART & 0xFF;
  ENC_WriteReg(enc28j60);
  enc28j60->TxBuffer[0] = ETXST+1;
  enc28j60->TxBuffer[1] = ENC28J60_TXSTART >> 8;
  ENC_WriteReg(enc28j60);

  enc28j60->TxBuffer[0] = ETXND;
  enc28j60->TxBuffer[1] = (ENC28J60_TXSTART + enc28j60->TxLength) & 0xFF;
  ENC_WriteReg(enc28j60);
  enc28j60->TxBuffer[0] = ETXND+1;
  enc28j60->TxBuffer[1] = (ENC28J60_TXSTART + enc28j60->TxLength)  >> 8;
  ENC_WriteReg(enc28j60);

  enc28j60->TxBuffer[0] = ECON1;
  enc28j60->TxBuffer[1] = ECON1_TXRTS;
  ENC_BFS(enc28j60);
}

/*################################################
* Function: ENC_ReceivePackge
* Description: приём пакета данных от микросхемы enc28j60.
##################################################*/
void ENC_ReceivePackage(ENC28J60* enc28j60)
{
  enc28j60->TxBuffer[0] = EPKTCNT;
  ENC_ReadReg(enc28j60);
  if(enc28j60->RxBuffer[0] > 0)
  {
    enc28j60->TxBuffer[0] = ERDPT;
    enc28j60->TxBuffer[1] = enc28j60->rxrdpt & 0xFF;
    ENC_WriteReg(enc28j60);
    enc28j60->TxBuffer[0] = ERDPT+1;
    enc28j60->TxBuffer[1] = enc28j60->rxrdpt >> 8;
    ENC_WriteReg(enc28j60);

    enc28j60->RxLength = 2;
    enc28j60->RxCounter = 0;
    ENC_ReadBuffer(enc28j60);
    enc28j60->rxrdpt = enc28j60->RxBuffer[0] | (enc28j60->RxBuffer[1] << 8);

    enc28j60->RxLength = 2;
    enc28j60->RxCounter = 0;
    ENC_ReadBuffer(enc28j60);
    uint16_t len = enc28j60->RxBuffer[0] | (enc28j60->RxBuffer[1] << 8);

    enc28j60->RxLength = 2;
    enc28j60->RxCounter = 0;
    ENC_ReadBuffer(enc28j60);
    uint16_t status = enc28j60->RxBuffer[0] | (enc28j60->RxBuffer[1] << 8);

    if(status & 0x80)
    {
      enc28j60->RxLength = (len - 4) & 0xFF;
      enc28j60->RxCounter = 0;
      ENC_ReadBuffer(enc28j60);

      portBASE_TYPE xStatus;
      MsgStruct nextMsg1;
      MsgStruct nextMsg2;
      MsgStruct nextMsg;
      for(uint16_t i = 0; i < 6; i++) nextMsg.ethMsgStruct.destAddr[i] = enc28j60->RxBuffer[i];
      for(uint16_t i = 6; i < 12; i++) nextMsg.ethMsgStruct.senderAddr[i-6] = enc28j60->RxBuffer[i];
      nextMsg.ethMsgStruct.type = (enc28j60->RxBuffer[12] << 8) | enc28j60->RxBuffer[13];
      nextMsg.ethMsgStruct.data = &netBufRx[0];
      for(uint16_t i = 14; i < len; i++) nextMsg.ethMsgStruct.data[i-14] = enc28j60->RxBuffer[i];
      nextMsg.length = len;
      enc28j60->RxLength = len - 4;
      xStatus = xQueueSendToBack(xEthMsgQueue,&nextMsg,0);

      /*for(uint16_t i = 0; i < len; i++)
      {
        enc28j60->log[enc28j60->logCounter] = enc28j60->RxBuffer[i];
        enc28j60->logCounter++;
      }*/

      enc28j60->TxBuffer[0] = ECON1;
      enc28j60->TxBuffer[1] = ECON1_RXEN;
      ENC_BFC(enc28j60);
    }

    enc28j60->TxBuffer[0] = ERXRDPT;
    enc28j60->TxBuffer[1] = (enc28j60->rxrdpt-1) & 0xFF;
    ENC_WriteReg(enc28j60);
    enc28j60->TxBuffer[0] = ERXRDPT+1;
    enc28j60->TxBuffer[1] = (enc28j60->rxrdpt-1) >> 8;
    ENC_WriteReg(enc28j60);

    enc28j60->TxBuffer[0] = ECON2;
    enc28j60->TxBuffer[1] = ECON2_PKTDEC;
    ENC_BFS(enc28j60);

    enc28j60->TxBuffer[0] = ECON1;
    enc28j60->TxBuffer[1] = ECON1_RXEN;
    ENC_BFS(enc28j60);
  }
  DelayUs(100);
  /*enc28j60->TxBuffer[0] = EIE;
  enc28j60->TxBuffer[1] = EIE_INTIE;
  ENC_BFS(enc28j60);*/
}

/*################################################
* Function: ENC_SoftReset
* Description: программный сброс микросхемы enc28j60.
##################################################*/
void ENC_SoftReset(ENC28J60* enc28j60)
{
  userSPI_SSselect(enc28j60->SPI);
  DelayUs(2);
  userSPI_WriteByte(enc28j60->SPI, 0xFF);
  DelayUs(2);
  userSPI_SSdeselect(enc28j60->SPI);
  //DelayMs(1);
}

/*################################################
* Function: ENC_BankCheck
* Description: проверка текущего банка и установка нового при несоответствии.
##################################################*/
void ENC_BankCheck(ENC28J60* enc28j60)
{
  if((enc28j60->TxBuffer[0] & ADDR_MASK) < 0x1A)
  {
    if(enc28j60->currentBank != ((enc28j60->TxBuffer[0] & BANK_MASK) >> 5))
    {
      enc28j60->currentBank = (enc28j60->TxBuffer[0] & BANK_MASK) >> 5;

      userSPI_SSselect(enc28j60->SPI);
      userSPI_WriteByte(enc28j60->SPI, (OP_BFC | (ECON1 & ADDR_MASK)));
      userSPI_WriteByte(enc28j60->SPI, 0x03);
      DelayUs(1);
      userSPI_SSdeselect(enc28j60->SPI);

      userSPI_SSselect(enc28j60->SPI);
      userSPI_WriteByte(enc28j60->SPI, (OP_BFS | (ECON1 & ADDR_MASK)));
      userSPI_WriteByte(enc28j60->SPI, ((enc28j60->TxBuffer[0] & BANK_MASK) >> 5));
      DelayUs(1);
      userSPI_SSdeselect(enc28j60->SPI);
    }
  }
}

/*################################################
* Function: SPI1_IRQHandler
* Description: обработчик прерывания SPI интерфейса, работающего с микросхемой.
##################################################*/
void SPI1_IRQHandler()
{
  if((SPI1->SR & SPI_SR_RXNE) && (SPI1->CR2 & SPI_CR2_RXNEIE))
  {
    /*enc28j60.RxBuffer[enc28j60.RxCounter] = enc28j60.SPI->baseSPI->DR;
    enc28j60.RxCounter++;
    if(enc28j60.RxLength == 0) enc28j60.RxCounter = 0;*/
    enc28j60.RxBuffer[enc28j60.RxCounter] = enc28j60.SPI->baseSPI->DR;
  }

  if((SPI1->SR & SPI_SR_TXE) && (SPI1->CR2 & SPI_CR2_TXEIE))
  {
    SPI1->CR2 &= ~SPI_CR2_TXEIE;
  }
}


/*################################################
* Function: EXTI1_IRQHandler
* Description: обработчик прерывания по приёму сообщения от микросхемы.
##################################################*/
void EXTI1_IRQHandler()
{
  EXTI->PR |= EXTI_PR_PR1;

  enc28j60.TxBuffer[0] = EIE;
  enc28j60.TxBuffer[1] = EIE_INTIE;
  ENC_BFC(&enc28j60);

  portBASE_TYPE xStatus;
  SpiOpParam nextSpiOperation;
  nextSpiOperation.opType = OP_READ_BUF;
  xStatus = xQueueSendToBackFromISR(xSpiMsgQueue,&nextSpiOperation,0);
}