#ifndef __NET_H__
#define __NET_H__

#include "stm32f4xx.h"
#include "system.h"
#include "configuration.h"

#define ipAddr(a,b,c,d)    ( ((uint32_t)a) | ((uint32_t)b << 8) | ((uint32_t)c << 16) | ((uint32_t)d << 24) )

// Перекодирование word'а
#define fromBigEndian16(a)      ( (((a) >> 8) & 0xff) | (((a) << 8) & 0xff00) )
#define toBigEndian16(a)        fromBigEndian16(a)

// Перекодирование dword'а
#define fromBigEndian32(a)      ( (((a) >> 24) & 0xff) | (((a) >> 8) & 0xff00) | (((a) << 8) & 0xff0000) | (((a) << 24) & 0xff000000) )
#define toBigEndian32(a)        fromBigEndian32(a)

#define ETH_TYPE_ARP    0x0806
#define ETH_TYPE_IP     0x0800

#define ARP_HW_TYPE_ETH         0x0001
#define ARP_PROTOCOL_TYPE_IP    0x0800

#define ARP_TYPE_REQUEST        0x01
#define ARP_TYPE_RESPONSE       0x02

#define IP_PROTOCOL_ICMP        1
#define IP_PROTOCOL_IP          6
#define IP_PROTOCOL_UDP         17

#define ICMP_TYPE_ECHO_REQUEST  8
#define ICMP_TYPE_ECHO_REPLY    0

#define IP_PACKET_TTL		255


typedef struct
{
  uint8_t destAddr[6];
  uint8_t senderAddr[6];
  uint16_t type;
  uint8_t *data;
}EthMsgStruct;

typedef struct
{
  EthMsgStruct ethMsgStruct;
  uint16_t length;
}MsgStruct;

typedef struct
{
  uint16_t hwType;
  uint16_t netType;
  uint8_t macAddrLen;
  uint8_t ipAddrLen;
  uint16_t type;
  uint8_t senderMACAddr[6];
  uint8_t senderIPAddr[4];
  uint8_t destMACAddr[6];
  uint8_t destIPAddr[4];
}ARPMsgStruct;

typedef struct
{
    uint8_t verHeadLen; // версия и длина заголовка =0x45
    uint8_t tos; //тип сервиса
    uint16_t totalLen; //длина всего пакета
    uint16_t fragmentId; //идентификатор фрагмента
    uint16_t flagsFramgentOffset; //смещение фрагмента
    uint8_t ttl; //TTL
    uint8_t protocol; //код протокола
    uint16_t checkSum; //контрольная сумма заголовка
    uint8_t senderAddr[4]; //IP-адрес отправителя
    uint8_t destAddr[4]; //IP-адрес получателя
    uint8_t data[];
}IPMsgStruct;

// ICMP Echo-пакет
typedef struct
{
    uint8_t type;
    uint8_t code;
    uint16_t checkSum;
    uint16_t id;
    uint16_t seq;
    uint8_t *data;
} ICMPEchoMsgStruct;


void arp_filter(EthMsgStruct*,uint16_t);
void ip_filter(EthMsgStruct*,uint16_t);
void ethReply(EthMsgStruct*,uint16_t);
void icmp_filter(EthMsgStruct*,uint16_t);
void ipReply(EthMsgStruct*,uint16_t);

uint16_t checkSum(uint8_t *buf, uint16_t length);
#endif