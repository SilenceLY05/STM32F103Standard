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
#include "bsp_can.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"

__IO uint32_t flag = 0;   //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;       //���ͻ�����
CanRxMsg RxMessage;       //���ջ�����


static void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	/* Debug_usart �˿ڳ�ʼ�� */
	Debug_USART_Config();
	
	/* ������ʼ�� */
	Key_GPIO_Config();
	
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	
	printf("\r\n ��ӭʹ��Ұ�� F103-�Ե� STM32 �����塣\r\n");
  printf("\r\n Ұ��F103-�Ե� CANͨѶ�ػ�ʵ������\r\n");
	
	printf("\r\n ʵ�鲽�裺\r\n");

	printf("\r\n 1.ʹ�ûػ�ģʽ������Ҫ�����ⲿоƬ\r\n");
	printf("\r\n 2.���¿������KEY1������ʹ��CAN���ⷢ��0-7�����ݰ���������չIDΪ0x1314 (���ڻػ�ģʽ�����Լ����͸��Լ�)\r\n");
	printf("\r\n 3.���������CAN���յ���չIDΪ0x1314�����ݰ�����������Դ�ӡ�����ڡ� \r\n");
	printf("\r\n 4.�����е�can������Ϊ1MBps��Ϊstm32��can������ʡ� \r\n");
	

	while (1)
	{
		/* ��һ�ΰ�������һ������ */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx,&TxMessage);
			
			Delay(10000);   //�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
			
			while(CAN_TransmitStatus(CAN1,(uint8_t)CAN_Transmit) == CAN_TxStatus_Failed);
			
			LED_GREEN;
			printf("\r\n ��ʹ��CAN�������ݰ��� \r\n");
			printf("\r\n ���͵ı�������Ϊ�� \r\n");
			printf("\r\n ��չID��ExtId:0x%x \r\n",TxMessage.ExtId);
			CAN_DEBUG_ARRAY(TxMessage.Data,8);
		}
		if(flag == 1)
		{
			LED_GREEN;
			printf("\r\nCAN���յ�������Ϊ:\r\n");
			CAN_DEBUG_ARRAY(RxMessage.Data,8);
			flag =0;
		}
	}
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
