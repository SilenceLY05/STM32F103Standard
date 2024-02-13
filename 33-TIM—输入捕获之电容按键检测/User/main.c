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
#include "bsp_tpad.h"
#include "bsp_usart.h"
#include "bsp_beep.h"
#include "bsp_systick.h"



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* ��������ʼ�� */
	Beep_Init();
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	printf ( "\r\nҰ��STM32 ���벶����ݰ������ʵ��\r\n" );
	printf ( "\r\n�������ݰ����������������\r\n" );
	
	

	//TPAD_TIM_Init();	
	while(TPAD_Init());

	while (1)
	{
		if(TPAD_Scan() == TPAD_ON)
		{
			BEEP_ON();
			SysTick_Delay_Ms(25);
			BEEP_OFF();
		}
	}
}


/*********************************************END OF FILE**********************/
