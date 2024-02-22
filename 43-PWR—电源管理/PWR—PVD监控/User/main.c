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
#include "bsp_pvd.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	

	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	LED_GREEN;
	
	//配置PVD，当电压过低时，会进入中断服务函数，亮红灯
	PVD_Config();
	

	while (1)
	{
	
	}
}


/*********************************************END OF FILE**********************/
