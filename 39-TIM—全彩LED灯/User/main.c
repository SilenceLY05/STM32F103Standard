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
#include "bsp_color_led.h"

#define SOFT_DELAY() Delay(0xFFFFFF);

static void Delay(__IO uint32_t nCount);


int main(void)
{	
	COLOR_TIMx_LED_Init();

	while (1)
	{
		//��ʾ������ɫ
		SetRGBColor(0x8080ff);
		SOFT_DELAY();
		
		SetRGBColor(0xff8000);
		SOFT_DELAY();
		
		SetRGBColor(0xffc90e);
		SOFT_DELAY();
		
		SetColorValue(181,230,28);
		SOFT_DELAY();
		
		SetColorValue(255,128,64);
		SOFT_DELAY();
		
	}
}

static void Delay(__IO uint32_t nCount)
{
	for(;nCount!= 0 ; nCount--);
}

/*********************************************END OF FILE**********************/
