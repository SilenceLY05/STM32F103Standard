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
#include "weibo.h"


#define weibo_id           "3196855541@qq.com"             //微博ID
#define weibo_pwd          "wildfire123"                   //微博密码



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
	
	printf("网络初始化 NTP实验\r\n");
	
	W5500_GPIO_Config();                     /*初始化MCU相关引脚*/
	
	W5500_Reset();                           /*硬复位W5500*/
	Set_W5500_MAC();                         /*配置MAC地址*/
	Set_W5500_IP();
	
	socket_buf_init(txsize,rxsize);          /*初始化8个Socket的发送接收缓存大小*/
	
	printf("我的微博ID为：%s\r\n",weibo_id);
	printf("我的微博密码为:%s\r\n",weibo_pwd);
	printf("看串口调试信息：rev255:OK 说明上传成功，登录以上微博账号就可以看到发送微博信息\r\n");
	
	socket_buf_init(txsize,rxsize);
	printf("应用程序执行中....\r\n");
	
	while(1)
	{
		WEIBO_Test();
	}

	
}



/*********************************************END OF FILE**********************/
