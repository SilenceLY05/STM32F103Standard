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
#include "bsp_key.h"
#include "bsp_ad5689.h"
#include "bsp_SysTick.h"
#include "bsp_ad5689_test.h"





/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	
	SysTick_Init();
	
	USART_Config();
	printf("��ӭʹ��Ұ�� F103 AD5689��ͨ��������Ҳ� ����\r\n");
	KEY_GPIO_Config();
	
	AD5689_Init();
	
	
	while (1)
	{	
		AD5689_Sine();
	}
}


/*********************************************END OF FILE**********************/
