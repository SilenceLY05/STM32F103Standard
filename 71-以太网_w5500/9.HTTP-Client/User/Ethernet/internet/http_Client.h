#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "types.h"
#include "stm32f10x.h"
#define yeelink_id               "wildfire"             //yeelink ID
#define yeelink_password         "wildfire123"          //yeelink PassWord


extern uint8 yeelink_server_ip[];
extern uint8 yeelink_port;

void HTTP_ClientTest(void);



#endif /* __HTTP_CLIENT_H__ */

