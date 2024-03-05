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
#include "bsp_exti.h"



void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 

	/* ��ʼ��EXTI�жϣ���Ӧ��������Դ�ͻᴥ���ж� */
	EXTI_SR501_Config();
	
	/* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
	while(1)
	{
		if(GPIO_ReadInputDataBit(SR501_INT_GPIO_PORT,SR501_INT_GPIO_PIN) == 0)
		{
			GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
		}
		
		Delay(0xFFF);
	}
	
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
