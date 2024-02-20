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
#include "bsp_breathing.h"  


//该变量在定时器中断服务函数中使用，用于控制各通道的输出
//修改该变量的值可直接改变呼吸灯的颜色
//变量格式：RGB888

__IO uint32_t rgb_color = 0xFF00FF;
#define SOFT_DELAY()   Delay(0x1FFFFFF);
void Delay(__IO uint32_t nCount);


int main(void)
{	
	/* 初始化呼吸灯 */
	TIMx_Breathing_Init();

	while (1)
	{
					//可动态修改颜色，使用各种颜色的呼吸灯
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
