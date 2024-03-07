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
#include "bsp_i2c_ee.h"
#include <string.h>
#include "bsp_ap3216c.h"
#include "stm32f10x_it.h"


/* ��ȡԭʼ���� */
float ALS;
uint16_t PS;
uint16_t IR;
uint8_t IntStatus;



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	float ALSValue;
	
	
	LED_BLUE;
	
	USART_Config();
	
	printf("����һ������һ���մ�������дԴ���ݲ�������\r\n");
	
	
	printf("\r\n оƬ��ʼ���С����� \r\n");
	
	ap3216c_init();
	

	
	while (1)
	{	
		/* ÿ 1000 ����ִ�У���תLED�����ж���ִ�У� */
    
    /* ÿ 500 ����ִ�У���ȡ���ݣ����ж����ȡ����Ȼ�������ﴦ����ӡ���� */
		if(task_readdata_finish == 1)
		{
			printf("\n����ǿ���ǣ�%.2fLux\n����ǿ���ǣ�%d\n", ALS, IR);

      if (PS == 55555)    // IR ̫ǿ PS ������Ч
        printf("IR ̫ǿ PS ������Ч\n");
      else
      {
        printf("�ӽ������ǣ�%d\n", PS & 0x3FF);
      }
      
      if (AP_INT_Read() == 0)
        printf("���жϲ���\n");
      
      if ((PS >> 15) & 1)
        printf("����ӽ�\n");
      else
        printf("����Զ��\n");
      
      if (IntStatus & 0x1)
        printf("ALS �����ж�\n");
      
      if (IntStatus >> 1 & 0x1)
        printf("PS �����ж�\n");
			
			task_readdata_finish = 0;
		}
	}
	
}



/*********************************************END OF FILE**********************/
