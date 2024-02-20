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
#include "bsp_breathing.h"  


//�ñ����ڶ�ʱ���жϷ�������ʹ�ã����ڿ��Ƹ�ͨ�������
//�޸ĸñ�����ֵ��ֱ�Ӹı�����Ƶ���ɫ
//������ʽ��RGB888

__IO uint32_t rgb_color = 0xFF00FF;
#define SOFT_DELAY()   Delay(0x1FFFFFF);
void Delay(__IO uint32_t nCount);


int main(void)
{	
	/* ��ʼ�������� */
	TIMx_Breathing_Init();

	while (1)
	{
					//�ɶ�̬�޸���ɫ��ʹ�ø�����ɫ�ĺ�����
	  rgb_color = 0xFF00FF;
		SOFT_DELAY();			

		rgb_color =0x8080ff;
		SOFT_DELAY();
		
		rgb_color =0xff8000;
		SOFT_DELAY();
		
		rgb_color =0xffc90e;
		SOFT_DELAY();
	}
}

void Delay(__IO uint32_t nCount)
{
	for(;nCount != 0; nCount--);
}



/*********************************************END OF FILE**********************/
