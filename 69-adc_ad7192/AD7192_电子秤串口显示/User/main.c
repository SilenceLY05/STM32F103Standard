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
#include "bsp_ad7192.h"
#include "bsp_ad7192_test.h"
#include "bsp_SysTick.h"




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	

	USART_Config();
	
	AD7192_Config();
	
	/* 软件复位AD7192 */
	AD7192_SoftWareReset();
	
	SysTick_Delay_Ms(10);
	
	ReadFromAD7192ViaSPI(REG_ID,1,AD7192Registers,REG_ID);
	
	if((AD7192Registers[REG_ID]&0x0F) != 0)
	{
		printf("\r\n AD7192初始化失败 请检查连接！ \r\n");
		while(1);
	}
	printf("\r\n AD7192 ID :0x%x \r\n",AD7192Registers[REG_ID]);
	
	electronic_scale_test();

	while (1)
	{

	}
}


/*********************************************END OF FILE**********************/
