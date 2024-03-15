#ifndef __BSP_ESP8266_TEST_H
#define __BSP_ESP8266_TEST_H

#include "stm32f10x.h"

/********************************** �û���Ҫ���õĲ���**********************************/
#define USER_ESP8266_ApSsid         "HUAWEI-AF1CS8"             //Ҫ���ӵ��ȵ������
#define USER_ESP8266_ApPwd          "123456789"               //Ҫ���ӵ��ȵ����Կ
#define USER_ESP8266_TcpServer_IP   "192.168.3.8"            //Ҫ���ӵķ������� IP
#define USER_ESP8266_TcpServer_Port "8080"                   //Ҫ���ӵķ������Ķ˿�

extern volatile uint8_t ucTcpClosedFlag;
extern int read_dht11_finish;

void Get_ESP8266_Cmd(char * cmd);
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void);
void ESP8266_SendDHT11DataTest(void);


#endif /* __BSP_ESP8266_TEST_H */

