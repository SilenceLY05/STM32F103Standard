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
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "utility.h"
#include <stdio.h>
#include <string.h>
#include "dhcp.h"
#include "tcp_demo.h"




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	SysTick_Init(72);                        /*初始化Systick工作时钟*/
	USART_Config();                          /*初始化串口通信:115200@8-n-1*/
	i2c_CfgGpio();                           /*初始化eeprom*/
	
	printf("网络初始化 TCP_CLient实验\r\n");
	
	W5500_GPIO_Config();                     /*初始化MCU相关引脚*/
	
	W5500_Reset();                           /*硬复位W5500*/
	Set_W5500_MAC();                         /*配置MAC地址*/
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);          /*初始化8个Socket的发送接收缓存大小*/
	
	printf("电脑作为TCP服务器，让W5500作为TCP客户端去连接\r\n");
	printf("服务器IP:%d.%d.%d.%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
	printf("W5500监听端口为：%d\r\n",remote_port);
	printf("连接成功后，TCP Client 发送数据为W5500，W5500将返回对应的数据\r\n");
	printf("应用程序执行中......\r\n");
	
	while(1)
	{
		Do_TCPClient();
	}
	
	
	
}



/*********************************************END OF FILE**********************/
