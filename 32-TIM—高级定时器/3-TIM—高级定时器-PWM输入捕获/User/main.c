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
#include "bsp_AdvanceTim.h" 
#include "bsp_GeneralTim.h" 
#include "bsp_usart.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 串口初始化 */
	USART_Config();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/*通用定时器初始化*/
	GENERAL_TIM_Init();
	
	/*高级定时器初始化*/
	ADVANCE_TIM_Init();	

	while (1)
	{

	}
}


/*********************************************END OF FILE**********************/
