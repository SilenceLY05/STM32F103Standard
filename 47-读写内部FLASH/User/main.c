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
#include "bsp_led.h"
#include "bsp_internal_flash.h"


static void Delay(__IO uint32_t nCount);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	LED_GPIO_Config();

	USART_Config();
	
	LED_BLUE;
	printf("\r\n 欢迎使用野火 STM32 开发板 \r\n");
	printf("\r\n 正在进行读写内部FLASH实验，请等待 \r\n");
	
	if(internalFlash_Test() !=FAILED )
	{
		LED_GREEN;
		printf("读写内部FLASH测试成功");
	}
	else
	{
		LED_RED;
		printf("读写内部FLASH测试失败");
	}
	

	

	
	while (1)
	{	
	}
}

static void Delay(__IO uint32_t nCount)
{
	for(;nCount != 0 ; nCount--);
}


/*********************************************END OF FILE**********************/
