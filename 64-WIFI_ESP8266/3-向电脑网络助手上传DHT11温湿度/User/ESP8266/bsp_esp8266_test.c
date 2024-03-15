#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "core_delay.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define LED_CMD_NUMBER   8
char *ledCmd[8] = {"LED_RED","LED_GREEN","LED_BLUE","LED_YELLOW","LED_PURPLE","LED_CYAN","LED_WHITE","LED_RGBOFF"};
DHT11_Data_Typedef DHT11_Data;

volatile uint8_t ucTcpClosedFlag = 0;

/* ��ȡ����������ֺʹ�����ʹ���ַ�������Ϣ */
void Get_ESP8266_Cmd(char * cmd)
{
	uint8_t i;
	
	for(i=0;i<LED_CMD_NUMBER;i++)
	{
		if((bool)strstr(cmd,ledCmd[i]))
			break;
	}
	switch(i)
	{
		case 0:
			LED_RED;
		break;
		
		case 1:
			LED_GREEN;
		break;
		
		case 2:
			LED_BLUE;
		break;
		
		case 3:
			LED_YELLOW;
		break;
		
		case 4:
			LED_PURPLE;
		break;
		
		case 5:
			LED_CYAN;
		break;
		
		case 6:
			LED_WHITE;
		break;
		
		case 7:
			LED_RGBOFF;
		break;
		
		default:
			break;
	}
}

/* ESP8266���ò��Ժ��� */
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void)
{
	printf("\r\n �������� ESP8266...... \r\n");
	printf("\r\n ʹ�� ESP8266 ...... \r\n");
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test());
	while(!ESP8266_DHCP_CUR());
	printf("\r\n �������ù���ģʽ STA ...... \r\n");
	while(!ESP8266_Net_Mode_Choose(STA));
	
	printf("\r\n �������� WIFI ...... \r\n");
	while(!ESP8266_JoinAP(USER_ESP8266_ApSsid,USER_ESP8266_ApPwd));
	
	printf("\r\n ��ֹ������...... \r\n");
	while(!ESP8266_Enable_MultipleId(DISABLE));
	
	printf("\r\n �������� Server...... \r\n");
	while(!ESP8266_Link_Server(enumTCP,USER_ESP8266_TcpServer_IP,USER_ESP8266_TcpServer_Port,Single_ID_0));
	
	printf("\r\n ����͸������ģʽ...... \r\n");
	while(!ESP8266_UnvarnishSend());
	
	printf("\r\n ���� ESP8266 ��� \r\n");
	printf("\r\n ��ʼ͸��...... \r\n");
	
}


/* ESP8266 ����Ƿ���յ������ݣ�������Ӻ͵������� */
void ESP8266_SendDHT11DataTest(void)
{
	uint8_t ucStatus;
	char cStr[100] = {0};
	
	if(read_dht11_finish == 1)
		sprintf(cStr,"\r\n ��ȡDHT11�ɹ� \r\nʪ��Ϊ%d.%d���¶�Ϊ%d.%d",
				DHT11_Data.humi_int,DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci);
	else
		sprintf(cStr,"\r\n Read DHT11 ERROR! \r\n");
	
	printf("%s",cStr);                                //��ӡ��ȡ DHT11 ��ʪ����Ϣ
	ESP8266_SendString(ENABLE,cStr,0,Single_ID_0);    //���� DHT11 ��ʪ����Ϣ�������������
	
	if(ucTcpClosedFlag)                               //����Ƿ�ʧȥ����
	{
		ESP8266_ExitUnvarnishSend();                    //�˳�͸��ģʽ
		do ucStatus = ESP8266_Get_LinkStatus();         //��ȡ����״̬
		while(!ucStatus);
		
		if(ucStatus ==4)                                //ȷ��ʧȥ���Ӻ�����
		{
			printf("\r\n ���������ȵ�ͷ�����...... \r\n");
			while(!ESP8266_JoinAP(USER_ESP8266_ApSsid,USER_ESP8266_ApPwd));
			while(!ESP8266_Link_Server(enumTCP,USER_ESP8266_TcpServer_IP,USER_ESP8266_TcpServer_Port,Single_ID_0));
			printf("\r\n �����ȵ�ͷ������ɹ� \r\n");
		}
		while(!ESP8266_UnvarnishSend());
	}
}
