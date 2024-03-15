/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_esp8266.h"
#include "core_delay.h" 
#include "bsp_dht11.h"
#include "bsp_esp8266_test.h"
#include "bsp_led.h"
#include "bsp_Systick.h"


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
	CPU_TS_TmrInit();
	LED_GPIO_Config();
	ESP8266_Init();
	DHT11_Init();
  
	printf ( "\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n" );                 //��ӡ����������ʾ��Ϣ

	ESP8266_StaTcpClient_Unvarnish_ConfigTest();       //��ESP8266��������
	
	SysTick_Init();
  
  while(1)
	{	
		if(read_dht11_finish)
		{
			ESP8266_SendDHT11DataTest();
			read_dht11_finish =0;
			LED1_TOGGLE;
		}
  }	
}	
/*********************************************END OF FILE**********************/
