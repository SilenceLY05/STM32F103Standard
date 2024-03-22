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


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	SysTick_Init(72);
	USART_Config();
	i2c_CfgGpio();
	
	printf("�����ʼ�� PINGʵ��\r\n");
	
	W5500_GPIO_Config();
	
	W5500_Reset();
	Set_W5500_MAC();
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);
	
	printf("��������ɳ�ʼ��...\r\n");
	printf("��DOS������������cmd������ping %d.%d.%d.%d\r\n",ConfigMsg.lip[0],ConfigMsg.lip[1],ConfigMsg.lip[2],ConfigMsg.lip[3]);
	
	while(1)
	{
	}
	
	
	
}



/*********************************************END OF FILE**********************/
