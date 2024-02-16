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
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_sdio_test.h"
#include "bsp_sdio_sdcard.h"


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* ��ʼ���������� */
	KEY_GPIO_Config();
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	printf("\r\n��ӭʹ��Ұ��  STM32 �����塣\r\n");

  printf("�ڿ�ʼ����SD����������ǰ��������������32G���ڵ�SD��\r\n");
  printf("��������SD������ ���ļ�ϵͳ ��ʽ��д����ɾ��SD�����ļ�ϵͳ\r\n");
  printf("ʵ����ͨ�����Ը�ʽ����ʹ��SD���ļ�ϵͳ�����ָ̻�SD���ļ�ϵͳ\r\n");
  printf("\r\n ��sd���ڵ�ԭ�ļ����ɻָ���ʵ��ǰ��ر���SD���ڵ�ԭ�ļ�������\r\n");

  printf("\r\n ����ȷ�ϣ��밴�������KEY1��������ʼSD������ʵ��....\r\n");
	


	while (1)
	{
		/* ���°�����ʼ����SD����дʵ�飬����SD��ԭ�ļ� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("\r\n ��ʼ����SD����дʵ�� \r\n");
			SD_Test();
		}
	}
}


/*********************************************END OF FILE**********************/
