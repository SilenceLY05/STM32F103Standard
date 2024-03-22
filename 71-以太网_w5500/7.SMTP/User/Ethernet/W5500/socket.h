#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "stm32f10x.h"
#include "types.h"

/* Socket ¶Ë¿ÚÑ¡Ôñ */
#define SOCK_TCPS              0
#define SOCK_HUMTEM            0
#define SOCK_PING              0
#define SOCK_TCPC              1
#define SOCK_UDPS              2
#define SOCK_WEIBO             2
#define SOCK_DHCP              3
#define SOCK_HTTPS             4
#define SOCK_DNS               5
#define SOCK_SMTP              6
#define SOCK_NTP               7

void Close(SOCKET s);
uint8 socket(SOCKET s,uint8 protocol,uint16 port,uint8 flag);
uint8 Listen(SOCKET s);
uint8 Connect(SOCKET s,uint8 *addr,uint16 port);
void Disconnect(SOCKET s);
uint16 Send(SOCKET s,const uint8 *buff,uint16 len);
uint16 Receive(SOCKET s,uint8 *buff,uint16 len);
uint16 SendTo(SOCKET s,const uint8 *buff,uint16 len,uint8*addr,uint16 port);
uint16 RecvFrom(SOCKET s,uint8*buff,uint16 len,uint8 *addr,uint16 *port);

#ifdef __MACRAW__
void MACRAW_Open(void);
uint16 MACRAW_Send(const uint8*buff,uint16 len);
uint16 MACRAW_Recv(uint8 *buff,uint16 len);

#endif


#endif /* __SOCKET_H__ */

