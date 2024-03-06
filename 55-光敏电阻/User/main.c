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
#include "bsp_led.h"
#include "bsp_ldr.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	LED2_OFF;
	
	/* 光敏电阻初始化 */
	LDR_GPIO_Config();
	
	/* 轮询光敏状态，若有光则点亮灯 */
	while (1)
	{
		if(LDR_Test(LDR_GPIO_PORT,LDR_GPIO_PIN) == LDR_ON)
		{
			LED2_ON;
		}
		else
		{
			LED2_OFF;
		}
	}
}


/*********************************************END OF FILE**********************/
