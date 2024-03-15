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
#include "stm32f10x_it.h"
#include "bsp_usart.h"
#include "bsp_esp8266.h"
#include "core_delay.h" 
#include "bsp_dht11.h"
#include "bsp_esp8266_test.h"
#include "bsp_led.h"
#include "bsp_Systick.h"
#include "bsp_beep.h"


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
	DHT11_Init();
	Beep_Init();
  
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                 //打印测试例程提示信息

	#ifndef BUILTAP_TEST
  ESP8266_StaTcpServer_ConfigTest();                                             //对ESP8266进行配置 STA模式
  #else
  ESP8266_ApTcpServer_ConfigTest();                                              //对ESP8266进行配置 AP模式
  #endif
	
  
  while(1)
	{	
		ESP8266_CheckRecv_SendDataTest(); // ESP8266 处理并发送数据
  }	
}	
/*********************************************END OF FILE**********************/
