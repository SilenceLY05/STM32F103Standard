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
#include "bsp_ili9341_lcd.h"
#include "bsp_spi_flash.h"
#include "bsp_xpt2046_lcd.h"
#include "palette.h"
#include <string.h>

static void Delay(__IO uint32_t nCount);



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	uint16_t value_test = 0;

	ILI9341_Init();
	XPT2046_Init();
	
	Debug_USART_Config();
	LED_GPIO_Config();
	printf("\r\n ********** 触摸画板程序 *********** \r\n"); 
	printf("\r\n 若汉字显示不正常，请阅读工程中的readme.txt文件说明，根据要求给FLASH重刷字模数据\r\n"); 

	LED_BLUE;
	

	//其中0、3、5、6 模式适合从左至右显示文字，
	//不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
	//其中 6 模式为大部分液晶例程的默认显示方向  
  ILI9341_GramScan ( 3 );	
	
	
	
	while(1)
	{
//		XPT2046_SendCMD(0x90);
//	Delay(0xFF);
//	value_test = XPT2046_ReceiveData();
//		
//	printf("\r\n value_test = 0x%x \r\n",value_test);
		if( touch_detect() == TOUCH_PRESSED)
		{
			printf("\r\n触摸被按下");
		}
		else
		{
			printf("\r\n触摸未被按下");
		}
		Delay(0xFFFFF);
	}
}


static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0 ; nCount--);
}



/* ------------------------------------------end of file---------------------------------------- */

