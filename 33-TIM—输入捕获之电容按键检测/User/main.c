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
#include "bsp_tpad.h"
#include "bsp_usart.h"
#include "bsp_beep.h"
#include "bsp_systick.h"



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/* 蜂鸣器初始化 */
	Beep_Init();
	
	/* 串口初始化 */
	USART_Config();
	
	printf ( "\r\n野火STM32 输入捕获电容按键检测实验\r\n" );
	printf ( "\r\n触摸电容按键，蜂鸣器则会响\r\n" );
	
	

	//TPAD_TIM_Init();	
	while(TPAD_Init());

	while (1)
	{
		if(TPAD_Scan() == TPAD_ON)
		{
			BEEP_ON();
			SysTick_Delay_Ms(25);
			BEEP_OFF();
		}
	}
}


/*********************************************END OF FILE**********************/
