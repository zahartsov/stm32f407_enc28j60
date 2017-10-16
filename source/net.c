#include "net.h"

uint8_t netBufRx[ENC28J60_MAXFRAME];
uint8_t netBufTx[ENC28J60_MAXFRAME];

/*################################################
* Function: ip_filter
* Description: обрабатывает полученный ip-пакет
##################################################*/
void ip_filter(EthMsgStruct *ethMsg, uint16_t length)
{
  IPMsgStruct *msg = (void*)(ethMsg->data);

  if((msg->verHeadLen == 0x45) && (ipAddr(msg->destAddr[0],msg->destAddr[1],msg->destAddr[2],msg->destAddr[3]) == enc28j60.ipAddr))
  {
    switch(msg->protocol)
    {
    case 1:
      icmp_filter(ethMsg, length);
      break;

    case 6:
      //udp_filter();
      break;
    }
  }
}

/*################################################
* Function: arp_filter
* Description: обрабатывает полученный  ARP-пакет
##################################################*/
void arp_filter(EthMsgStruct *ethMsg, uint16_t length)
{
  ARPMsgStruct *msg = (void*)(ethMsg->data);

  if(length > sizeof(ARPMsgStruct))
  {
    if((msg->hwType == fromBigEndian16(ARP_HW_TYPE_ETH)) && (msg->netType == fromBigEndian16(ARP_PROTOCOL_TYPE_IP)))
    {
      if((msg->type == fromBigEndian16(ARP_TYPE_REQUEST)) && (ipAddr(msg->destIPAddr[0],msg->destIPAddr[1],msg->destIPAddr[2],msg->destIPAddr[3]) == enc28j60.ipAddr))
      {
        msg->type = toBigEndian16(ARP_TYPE_RESPONSE);
        for(uint8_t i = 0; i < 4; i++) msg->destIPAddr[i] = msg->senderIPAddr[i];
        for(uint8_t i = 0; i < 4; i++) msg->senderIPAddr[i] = (enc28j60.ipAddr >> i*8);
        for(uint8_t i = 0; i < 6; i++) msg->destMACAddr[i] = msg->senderMACAddr[i];
        for(uint8_t i = 0; i < 6; i++) msg->senderMACAddr[i] = enc28j60.macadr[i];

        ethReply(ethMsg, sizeof(ARPMsgStruct));
      }
    }
  }
}

/*################################################
* Function: icmp_filter
* Description: обрабатывает ICMP-пакет
##################################################*/
void icmp_filter(EthMsgStruct *ethMsg, uint16_t length)
{
  IPMsgStruct *msg = (void*)ethMsg->data;
  __NOP();
  __NOP();
  ICMPEchoMsgStruct *echoMsg = (void*)msg->data;

  if(echoMsg->type == ICMP_TYPE_ECHO_REQUEST)
  {
    echoMsg->type = ICMP_TYPE_ECHO_REPLY;
    echoMsg->checkSum += 8;

    ipReply(ethMsg, length);
  }
}

/*################################################
* Function: ethReply
* Description: отправка ответа на Ethernet-фрейм
##################################################*/
void ethReply(EthMsgStruct *ethMsg, uint16_t length)
{
  for(uint8_t i = 0; i < 6; i++) ethMsg->destAddr[i] = ethMsg->senderAddr[i];
  for(uint8_t i = 0; i < 6; i++) ethMsg->senderAddr[i] = enc28j60.macadr[i];
  ethMsg->type = toBigEndian16(ethMsg->type);
  uint8_t *buf = (void*)(ethMsg);
  SpiOpParam nextSpiOperation;
  portBASE_TYPE xStatus;
  nextSpiOperation.opType = OP_WRITE_BUF;
  nextSpiOperation.bytesNumber = length + sizeof(EthMsgStruct);
  nextSpiOperation.bufData = &netBufTx[0];
  for(uint8_t i = 0; i < 14; i++) nextSpiOperation.bufData[i] = buf[i];
  for(uint8_t i = 14; i < nextSpiOperation.bytesNumber; i++) nextSpiOperation.bufData[i] = netBufRx[i-14];
  xStatus = xQueueSendToBack(xSpiMsgQueue,&nextSpiOperation,0);
}

/*################################################
* Function: ip_filter
* Description: отправка ответа на IP-пакет
##################################################*/
void ipReply(EthMsgStruct *ethMsg, uint16_t length)
{
  IPMsgStruct *msg = (void*)(ethMsg->data);

  msg->totalLen = toBigEndian16(length + sizeof(IPMsgStruct) - 14);
  //msg->fragmentId = 0;
  msg->flagsFramgentOffset = 0;
  msg->ttl = IP_PACKET_TTL;
  for(uint8_t i = 0; i < 4; i ++) msg->destAddr[i] = msg->senderAddr[i];
  for(uint8_t i = 0; i < 4; i ++) msg->senderAddr[i] = (enc28j60.ipAddr >> i*8);
  msg->checkSum = checkSum((void*)msg,sizeof(IPMsgStruct));

  ethReply(ethMsg, length-1);
}

/*################################################
* Function: checkSum
* Description: расчёт контрольной суммы
##################################################*/
uint16_t checkSum(uint8_t *buf, uint16_t length)
{
  uint32_t sum = 0;
  while(length >= 2)
  {
    sum = (((uint16_t)*buf << 8) | *(buf+1));
    buf += 2;
    length -= 2;
  }
  if(length) sum += (uint16_t)*buf << 8;
  while(sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
  return ~toBigEndian16((uint16_t)sum);
}