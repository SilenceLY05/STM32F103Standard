#ifndef __BSP_ESP8266_TEST_H
#define __BSP_ESP8266_TEST_H

#include "stm32f10x.h"

/********************************** 用户需要设置的参数**********************************/
#define USER_ESP8266_ApSsid         "HUAWEI-AF1CS8"             //要连接的热点的名称
#define USER_ESP8266_ApPwd          "123456789"               //要连接的热点的密钥
#define USER_ESP8266_TcpServer_IP   "192.168.3.8"            //要连接的服务器的 IP
#define USER_ESP8266_TcpServer_Port "8080"                   //要连接的服务器的端口

extern volatile uint8_t ucTcpClosedFlag;
extern int read_dht11_finish;

void Get_ESP8266_Cmd(char * cmd);
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void);
void ESP8266_SendDHT11DataTest(void);


#endif /* __BSP_ESP8266_TEST_H */

