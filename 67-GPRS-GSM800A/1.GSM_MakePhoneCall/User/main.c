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
#include "bsp_sdfs_app.h"
#include "bsp_SysTick.h"
#include "bsp_gsm_usart.h"
#include "bsp_gsm_gprs.h"
#include <string.h>

char testbuff[100];
uint8_t len;
char *rebuff;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	static uint8_t timecount = 0;
	char num[20] = {0};
	
	/* ��ʼ���������� */
	KEY_GPIO_Config();
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	SysTick_Init();
	
	printf("\r\n��ӭʹ��Ұ��  STM32 �����塣\r\n");
	
	//���ģ���Ƿ���Ӧ����
	while(gsm_init() != GSM_TRUE)
	{
		printf("\r\n ģ����Ӧ���Բ��������� \r\n");
		printf("\r\n ��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ���� \r\n");
		GSM_DELAY(500);
	}
	printf("\r\n ͨ����ģ����Ӧ���ԣ�5���ʼ���Ų���.... \r\n");
	
	/* ��ʱ5���ٷ������ģ�� */
	Delay_ms(5000);
	
	//�������
	gsm_call("112");              //����112�绰����
	rebuff = gsm_waitask(0);      
	
	if(strstr(rebuff,"ATD")!= NULL)   //��ӦOK����ʾģ���������յ�����
	{
		printf("\r\n ���ں��� \r\n");
		GSM_CLEAN_RX();
		
		rebuff = gsm_waitask(0);       //���µȴ���Ϣ
		
		if(strstr(rebuff,"NO CARRIER") != NULL)       //��ӦNO CARRIER,ͨ������
		{
			printf("\r\n ͨ������ \r\n");
		}
		else if(strstr(rebuff,"NO ANSWER") != NULL)   //��ӦNO ANSWER�����˽���
		{
			printf("\r\n ������ĵ绰��ʱ���˽��������Ժ��ٲ� \r\n");
		}
	}
	
	GSM_CLEAN_RX();         //������ջ�����
	
	
	while (1)
	{
		if(timecount >=100)
		{
			if(IsRing(num) == GSM_TRUE)
			{
				printf("Ringling:\nFrom:%s\n�밴��KEY2�������߰���KEY1�Ҷ�\r\n",num);
			}
			timecount =0;
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			GSM_HANGON();
		}
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			GSM_HANGOFF();
		}
		timecount++;
		GSM_DELAY(10);
	}
}


/*********************************************END OF FILE**********************/
