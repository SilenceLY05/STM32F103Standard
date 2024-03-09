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
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "bsp_as608.h"
#include "as608_test.h"
#include "rx_data_queue.h"




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 初始化USART 配置模式为115200  8-N-1 中断接收 */
	USART_Config();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	

	/* 初始化环形缓冲区 */
	rx_queue_init();
	
	/* 初始化指纹模块配置 */
	AS608_Config();
	
	/* 测试STM32与指纹模块的通信 */
	Connect_Test();

	while (1)
	{
		FR_Task();
	}
}


/*********************************************END OF FILE**********************/
