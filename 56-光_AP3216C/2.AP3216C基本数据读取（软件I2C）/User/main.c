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
#include <string.h>
#include "bsp_ap3216c.h"
#include "stm32f10x_it.h"


/* ��ȡԭʼ���� */
uint16_t ALS_RAW;
uint16_t PS_RAW;
uint16_t IR_RAW;



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	float ALSValue;
	
	
	LED_BLUE;
	
	USART_Config();
	
	printf("����һ������һ���մ�������дԴ���ݲ�������\r\n");
	
	
	printf("\r\n оƬ��ʼ���С����� \r\n");
	
	ap3216c_init();
	

	
	while (1)
	{	
		/* ÿ 1000 ����ִ�У���תLED�����ж���ִ�У� */
    
    /* ÿ 500 ����ִ�У���ȡ���ݣ����ж����ȡ����Ȼ�������ﴦ����ӡ���� */
		if(task_readdata_finish == 1)
		{
			ALSValue = ALS_RAW *0.36;
			printf("�����⣺%.2flux",ALSValue);
			printf("�ӽ�ֵ��%d",PS_RAW);
			printf("����⣺%d",IR_RAW);
			
			task_readdata_finish = 0;
		}
	}
	
}



/*********************************************END OF FILE**********************/
