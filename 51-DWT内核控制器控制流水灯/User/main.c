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
#include "core_delay.h"




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	

	/* �����ں˶�ʱ�� */	
	CPU_TS_TmrInit();

	for(;;)
	{
		LED1(ON);
			Delay_us(1000000);
		LED1(OFF);
		
		LED2(ON);
			Delay_us(1000000);
		LED2(OFF);
		
		LED3(ON);
			Delay_us(1000000);
		LED3(OFF);
		
		LED1(ON);
		LED2(ON);
			Delay_us(1000000);
		LED1(OFF);
		LED2(OFF);
		
		LED1(ON);
		LED3(ON);
			Delay_us(1000000);
		LED1(OFF);
		LED3(OFF);
		
		LED3(ON);
		LED2(ON);
			Delay_us(1000000);
		LED3(OFF);
		LED2(OFF);
	}
}


/*********************************************END OF FILE**********************/
