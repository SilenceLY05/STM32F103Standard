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
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "bsp_as608.h"
#include "as608_test.h"
#include "rx_data_queue.h"




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ��ʼ��USART ����ģʽΪ115200  8-N-1 �жϽ��� */
	USART_Config();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	

	/* ��ʼ�����λ����� */
	rx_queue_init();
	
	/* ��ʼ��ָ��ģ������ */
	AS608_Config();
	
	/* ����STM32��ָ��ģ���ͨ�� */
	Connect_Test();

	while (1)
	{
		FR_Task();
	}
}


/*********************************************END OF FILE**********************/
