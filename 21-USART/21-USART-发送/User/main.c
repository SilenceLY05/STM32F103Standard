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



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint8_t a[10] = {1,2,3,4,5,6,7,8,9,10};
	USART_Config();
	
	USART_SendByte(DEBUG_USARTx,'A');
	USART_SendHalfWord(DEBUG_USARTx,0xff56);
	USART_SendArray(DEBUG_USARTx,a,10);
	USART_SendString(DEBUG_USARTx,"��������\n");
	
	printf("����AJ\n");
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
