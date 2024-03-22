#ifndef __PING_H__
#define __PING_H__

#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "types.h"
#include "utility.h"

#define BUFF_LEN        128
#define PING_REQUEST    8
#define PING_REPLY      0
#define CODE_ZERO       0

#define SOCKET_ERROR    1
#define TIMEOUT_ERROR   2
#define SUCCESS         3
#define REPLY_ERROR     4

extern uint8 req;



typedef struct pingmsg
{
	uint8 Type;                //0-Ping 回复  8-Ping 请求
	uint8 Code;                //Always 0
	uint16 CheckSum;           //Check sum
	uint16 ID;                 //如果code是0，则帮助匹配回显和应答的标识符可能是零
	uint16 SeqNum;
	int8_t Data[BUFF_LEN];     //Ping Data:1452 = IP RAW MTU - sizeof (Type + Code + CheckSum + ID + SeqNum)
}PINGMSG;



uint8 Ping_Request(uint8 s,uint8 *addr);
uint8 Ping_Reply(uint8 s,uint8 *addr,uint16 rlen);
void Ping_Auto(uint8 s,uint8 *addr);
void Ping_Count(uint8 s,uint16 pCount,uint8 *addr);
void Ping_Test(void);


#endif /* __PING_H__ */

