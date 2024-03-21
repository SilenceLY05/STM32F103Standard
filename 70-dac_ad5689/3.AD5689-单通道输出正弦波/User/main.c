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
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_ad5689.h"
#include "bsp_SysTick.h"
#include "bsp_ad5689_test.h"





/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	
	SysTick_Init();
	
	USART_Config();
	printf("欢迎使用野火 F103 AD5689单通道输出正弦波 例程\r\n");
	KEY_GPIO_Config();
	
	AD5689_Init();
	
	
	while (1)
	{	
		AD5689_Sine();
	}
}


/*********************************************END OF FILE**********************/
