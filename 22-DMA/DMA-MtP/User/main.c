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
#include "bsp_usart_dma.h"


extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO uint32_t nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint16_t i ;
	
	USART_Config();
	
	USARTx_DMA_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
	
	//printf("\r\n USART1 DMA TX���� \r\n");
	
	for(i = 0; i<SENDBUFF_SIZE;i++)
	{
		SendBuff[i] = 'P';
	}
	
	/*Ϊ��ʾDMA�������ж�CPU���ܴ����������飬����ʹ��DMA�������ݣ����ǳ���
  *��ʱ�����п��ܻᵼ�µ��Զ˴��ڵ������ֻῨ��������ҷɵ������
  *���DMA�����е�ѭ��ģʽ��Ϊ����ģʽ*/		
	
	/* USART1��DMA����TX���� */
	USART_DMACmd(DEBUG_USARTx,USART_DMAReq_Tx,ENABLE);
	
	/* ��ʱCPU�ǿ��еģ����Ը��������� */

	while (1)
	{
		LED1_TOGGLE;
		Delay(0xFFFFF);
	}
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
