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
#include "http_Client.h"
#include "httputil.h"
#include "http_server.h"
#include "bsp_dht11.h"


uint8 reboot_flag = 0;


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
	DHT11_Init();
	
	printf("网络初始化 HTTP_Client实验\r\n");
	
	W5500_GPIO_Config();                     /*初始化MCU相关引脚*/
	
	W5500_Reset();                           /*硬复位W5500*/
	Set_W5500_MAC();                         /*配置MAC地址*/
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);          /*初始化8个Socket的发送接收缓存大小*/
	
	printf("Yeelink服务器IP为：%d.%d.%d.%d\r\n",yeelink_server_ip[0],yeelink_server_ip[1],yeelink_server_ip[2],yeelink_server_ip[3]);
	printf("Yeelink服务器端口默认为：%d\r\n",yeelink_port);
	printf("我的Yeelink ID 为：%s\r\n",yeelink_id);
	printf("我的Yeelink PassWord为: %s\r\n",yeelink_password);
	printf("请登录以上账号观察实时温度变化，串口也可以查看获取信息");
	printf("应用程序执行中......");
	
	while(1)
	{
		HTTP_ClientTest();
	}
	
	
	
}



/*********************************************END OF FILE**********************/
