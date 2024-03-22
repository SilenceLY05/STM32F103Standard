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
#include "weibo.h"


#define weibo_id           "3196855541@qq.com"             //΢��ID
#define weibo_pwd          "wildfire123"                   //΢������



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
	
	printf("�����ʼ�� NTPʵ��\r\n");
	
	W5500_GPIO_Config();                     /*��ʼ��MCU�������*/
	
	W5500_Reset();                           /*Ӳ��λW5500*/
	Set_W5500_MAC();                         /*����MAC��ַ*/
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);          /*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	printf("�ҵ�΢��IDΪ��%s\r\n",weibo_id);
	printf("�ҵ�΢������Ϊ:%s\r\n",weibo_pwd);
	printf("�����ڵ�����Ϣ��rev255:OK ˵���ϴ��ɹ�����¼����΢���˺žͿ��Կ�������΢����Ϣ\r\n");
	
	socket_buf_init(txsize,rxsize);
	printf("Ӧ�ó���ִ����....\r\n");
	
	while(1)
	{
		WEIBO_Test();
	}

	
}



/*********************************************END OF FILE**********************/
