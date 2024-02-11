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
#include "bsp_AdvanceTim.h" 
#include "bsp_GeneralTim.h" 
#include "bsp_usart.h"


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/*ͨ�ö�ʱ����ʼ��*/
	GENERAL_TIM_Init();
	
	/*�߼���ʱ����ʼ��*/
	ADVANCE_TIM_Init();	

	while (1)
	{

	}
}


/*********************************************END OF FILE**********************/
