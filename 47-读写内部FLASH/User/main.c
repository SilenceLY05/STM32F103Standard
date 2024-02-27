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
#include "bsp_internal_flash.h"


static void Delay(__IO uint32_t nCount);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	LED_GPIO_Config();

	USART_Config();
	
	LED_BLUE;
	printf("\r\n ��ӭʹ��Ұ�� STM32 ������ \r\n");
	printf("\r\n ���ڽ��ж�д�ڲ�FLASHʵ�飬��ȴ� \r\n");
	
	if(internalFlash_Test() !=FAILED )
	{
		LED_GREEN;
		printf("��д�ڲ�FLASH���Գɹ�");
	}
	else
	{
		LED_RED;
		printf("��д�ڲ�FLASH����ʧ��");
	}
	

	

	
	while (1)
	{	
	}
}

static void Delay(__IO uint32_t nCount)
{
	for(;nCount != 0 ; nCount--);
}


/*********************************************END OF FILE**********************/
