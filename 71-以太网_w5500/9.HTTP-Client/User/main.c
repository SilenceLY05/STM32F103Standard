/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
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
#include "bsp_led.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "utility.h"
#include <stdio.h>
#include <string.h>
#include "dhcp.h"
#include "http_Client.h"
#include "httputil.h"
#include "http_server.h"
#include "bsp_dht11.h"


uint8 reboot_flag = 0;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	SysTick_Init(72);                        /*��ʼ��Systick����ʱ��*/
	USART_Config();                          /*��ʼ������ͨ��:115200@8-n-1*/
	i2c_CfgGpio();                           /*��ʼ��eeprom*/
	DHT11_Init();
	
	printf("�����ʼ�� HTTP_Clientʵ��\r\n");
	
	W5500_GPIO_Config();                     /*��ʼ��MCU�������*/
	
	W5500_Reset();                           /*Ӳ��λW5500*/
	Set_W5500_MAC();                         /*����MAC��ַ*/
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);          /*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	printf("Yeelink������IPΪ��%d.%d.%d.%d\r\n",yeelink_server_ip[0],yeelink_server_ip[1],yeelink_server_ip[2],yeelink_server_ip[3]);
	printf("Yeelink�������˿�Ĭ��Ϊ��%d\r\n",yeelink_port);
	printf("�ҵ�Yeelink ID Ϊ��%s\r\n",yeelink_id);
	printf("�ҵ�Yeelink PassWordΪ: %s\r\n",yeelink_password);
	printf("���¼�����˺Ź۲�ʵʱ�¶ȱ仯������Ҳ���Բ鿴��ȡ��Ϣ");
	printf("Ӧ�ó���ִ����......");
	
	while(1)
	{
		HTTP_ClientTest();
	}
	
	
	
}



/*********************************************END OF FILE**********************/
