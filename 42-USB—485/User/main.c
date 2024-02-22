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
#include "bsp_485.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"




static void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	char *pbuf;
	
	uint16_t len;
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	/* Debug_usart �˿ڳ�ʼ�� */
	Debug_USART_Config();
	
	/*��ʼ��485ʹ�õĴ��ڣ�ʹ���ж�ģʽ����*/
	RS485_Config();
	
	LED_BLUE;
	
	/* ������ʼ�� */
	Key_GPIO_Config();
	

	
	printf("\r\n ��ӭʹ��Ұ�� F103-�Ե� STM32 �����塣\r\n");
  printf("\r\n Ұ��F103-�Ե� 485ͨѶ�ػ�ʵ������\r\n");
	
	printf("\r\n ʵ�鲽�裺\r\n");

	printf("\r\n 1.ʹ�õ������Ӻ�����485ͨѶ�豸\r\n");
	printf("\r\n 2.ʹ������ñ���Ӻ�:�Ե�V1�װ壺5V <---> C/4-5V  485-D <-----> PA2 485-R <-----> PA3  \r\n");
 	printf("\r\n                   �Ե�V2�װ壺3V3 <---> CAN/485_3V3 485TX <-----> PA2 485RX <-----> PA3 \r\n");   
	printf("\r\n 3.��ʹ������Ұ�𿪷������ʵ�飬�����������嶼���ر����򼴿ɡ�\r\n");
	printf("\r\n 4.׼���ú󣬰�������һ���������KEY1������ʹ��485���ⷢ��0-255������ \r\n");
	printf("\r\n 5.���������485���յ�256���ֽ����ݣ����������16������ʽ��ӡ������ \r\n");
	

	while (1)
	{
		/* ��һ�ΰ�������һ������ */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
		{
			uint16_t i;
			LED_BLUE;
			
			RS485_TX_EN();
			
			for(i=0; i<0xff;i++)
			{
				RS485_SendByte(i);//��������
			}
			/*�Ӷ�����ʱ����֤485�����������*/
			Delay(0xFFFF);
			
			RS485_RX_EN();
			LED_GREEN;
			
			printf("\r\n �������ݳɹ��� \r\n");//ʹ�õ��Դ��ڴ�ӡ������Ϣ���ն�
			
			
		}
		else
		{
			LED_BLUE;
			
			pbuf = get_rebuff(&len);
			if(len>=256)
			{
				LED_GREEN;
				printf("\r\n ���ճ���Ϊ%d������ \r\n",len);
				RS485_DEBUG_ARRAY((uint8_t *)pbuf,len);
				clean_rebuff();
			}
		}

	}
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
