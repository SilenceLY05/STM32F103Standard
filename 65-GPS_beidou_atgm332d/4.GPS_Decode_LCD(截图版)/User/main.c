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
#include "bsp_gps.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_bmp.h"
#include "bsp_key.h"


extern void nmea_decode_test(void);


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	LED_GPIO_Config();
	LED_BLUE;
	
	ILI9341_Init();
	Key_GPIO_Config();
	
	USART_Config();
	
	GPS_Config();
	
	printf("\r\n野火 GPS模块测试例程\r\n"); 
  
	
	/* GPS解码测试 */
  nmea_decode_test();
  
  while(1);

}


/*********************************************END OF FILE**********************/
