#ifndef __BSP_ESP8266_TEST_H
#define __BSP_ESP8266_TEST_H

#include "stm32f10x.h"
#include "bsp_esp8266.h"


/********************************** �û���Ҫ���õĲ���**********************************/
//#define   BUILTAP_TEST    //���������л���ESP8266�����ã�STAģʽ�� APģʽ
#ifndef BUILTAP_TEST
#define USER_ESP8266_ApSsid         "HUAWEI-AF1CS8"             //Ҫ���ӵ��ȵ������
#define USER_ESP8266_ApPwd          "123456789"               //Ҫ���ӵ��ȵ����Կ
#else
#define USER_ESP8266_BulitApSsid    "BinghuoLink"
#define USER_ESP8266_BulitApEcn     OPEN
#define USER_ESP8266_BulitApPwd     "wildfire"
#endif 

#define USER_ESP8266_TcpServer_IP   "192.168.3.8"           //Ҫ���ӵķ������� IP
#define USER_ESP8266_TcpServer_Port "8080"                   //Ҫ���ӵķ������Ķ˿�

#define USER_ESP8266_TcpServer_OverTime   "1800"              //��������ʱʱ�䣨��λ���룩



void ESP8266_StaTcpServer_ConfigTest(void);
void ESP8266_ApTcpServer_ConfigTest(void);
void ESP8266_CheckRecv_SendDataTest(void);


#endif /* __BSP_ESP8266_TEST_H */

