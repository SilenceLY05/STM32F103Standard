#ifndef __BSP_ESP8266_TEST_H
#define __BSP_ESP8266_TEST_H

#include "stm32f10x.h"
#include "bsp_esp8266.h"


/********************************** 用户需要设置的参数**********************************/
//#define   BUILTAP_TEST    //这个宏可以切换对ESP8266的配置：STA模式和 AP模式
#ifndef BUILTAP_TEST
#define USER_ESP8266_ApSsid         "HUAWEI-AF1CS8"             //要连接的热点的名称
#define USER_ESP8266_ApPwd          "123456789"               //要连接的热点的密钥
#else
#define USER_ESP8266_BulitApSsid    "BinghuoLink"
#define USER_ESP8266_BulitApEcn     OPEN
#define USER_ESP8266_BulitApPwd     "wildfire"
#endif 

#define USER_ESP8266_TcpServer_IP   "192.168.3.8"           //要连接的服务器的 IP
#define USER_ESP8266_TcpServer_Port "8080"                   //要连接的服务器的端口

#define USER_ESP8266_TcpServer_OverTime   "1800"              //服务器超时时间（单位：秒）



void ESP8266_StaTcpServer_ConfigTest(void);
void ESP8266_ApTcpServer_ConfigTest(void);
void ESP8266_CheckRecv_SendDataTest(void);


#endif /* __BSP_ESP8266_TEST_H */

