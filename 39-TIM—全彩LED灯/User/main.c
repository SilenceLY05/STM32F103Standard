/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
		//显示各种颜色
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
