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
#include "bsp_key.h"
#include "bsp_sdfs_app.h"
#include "bsp_SysTick.h"
#include "bsp_gsm_usart.h"
#include "bsp_gsm_gprs.h"
#include <string.h>

char testbuff[100];
uint8_t len;
char *rebuff;


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	static uint8_t timecount = 0;
	char num[20] = {0};
	
	/* 初始化独立按键 */
	KEY_GPIO_Config();
	
	/* 串口初始化 */
	USART_Config();
	
	SysTick_Init();
	
	printf("\r\n欢迎使用野火  STM32 开发板。\r\n");
	
	//检测模块是否响应正常
	while(gsm_init() != GSM_TRUE)
	{
		printf("\r\n 模块响应测试不正常！！ \r\n");
		printf("\r\n 若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关 \r\n");
		GSM_DELAY(500);
	}
	printf("\r\n 通过了模块响应测试，5秒后开始拨号测试.... \r\n");
	
	/* 延时5秒再发送命令到模块 */
	Delay_ms(5000);
	
	//拨打号码
	gsm_call("112");              //拨打112电话测试
	rebuff = gsm_waitask(0);      
	
	if(strstr(rebuff,"ATD")!= NULL)   //响应OK，表示模块正常接收到命令
	{
		printf("\r\n 正在呼叫 \r\n");
		GSM_CLEAN_RX();
		
		rebuff = gsm_waitask(0);       //重新等待消息
		
		if(strstr(rebuff,"NO CARRIER") != NULL)       //响应NO CARRIER,通话结束
		{
			printf("\r\n 通话结束 \r\n");
		}
		else if(strstr(rebuff,"NO ANSWER") != NULL)   //响应NO ANSWER，无人接听
		{
			printf("\r\n 您拨打的电话暂时无人接听，请稍后再拨 \r\n");
		}
	}
	
	GSM_CLEAN_RX();         //清除接收缓冲区
	
	
	while (1)
	{
		if(timecount >=100)
		{
			if(IsRing(num) == GSM_TRUE)
			{
				printf("Ringling:\nFrom:%s\n请按下KEY2接听或者按下KEY1挂断\r\n",num);
			}
			timecount =0;
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			GSM_HANGON();
		}
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			GSM_HANGOFF();
		}
		timecount++;
		GSM_DELAY(10);
	}
}


/*********************************************END OF FILE**********************/
