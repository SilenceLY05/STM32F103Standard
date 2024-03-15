/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
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
#include "bsp_esp8266.h"
#include "core_delay.h" 
#include "bsp_dht11.h"
#include "bsp_esp8266_test.h"
#include "bsp_led.h"
#include "bsp_Systick.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  USART_Config();
	CPU_TS_TmrInit();
	LED_GPIO_Config();
	ESP8266_Init();
  
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                          //打印测试例程提示信息
	printf ( "\r\n在网络调试助手或者串口调试助手上发送以下命令可以控制板载RGB灯\r\n" );    //打印测试例程提示信息
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );  

	ESP8266_StaTcpClient_Unvarnish_ConfigTest();       //对ESP8266进行配置
	
	printf ( "\r\n在网络调试助手或者串口调试助手  发送以下命令控制板载RGB灯：\r\n" );    //打印测试例程提示信息
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );
  printf ( "\r\n观察RGB灯的状态变化\r\n" );
  
  while(1)
	{	
		
		ESP8266_CheckRecvDataTest();
  }	
}	
/*********************************************END OF FILE**********************/
