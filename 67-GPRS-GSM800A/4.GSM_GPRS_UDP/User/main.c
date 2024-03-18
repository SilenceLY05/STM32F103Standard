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
#include "bsp_SysTick.h"
#include "bsp_gsm_usart.h"
#include "bsp_gsm_gprs.h"
#include <string.h>


#define LOCALPORT "2000"
#define SERVERIP  "163.142.57.125"

//#define SERVERPORT "5000"
#define SERVERPORT "4000"

const char *TESTBUFF1 = "\r\n1.野火GSM模块TCP数据上传功能测试";
const char *TESTBUFF2 = "\r\n2.野火GSM模块TCP数据上传功能测试";


/* 系统软件复位 */
void Soft_Reset(void)
{
	__set_FAULTMASK(1);    /* 关闭所有中断*/  
	NVIC_SystemReset();    /* 系统复位 */
}


	
/* 测试发送短信功能，实际测试时把电话号码修改成要接收短信的电话号码 */
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	static uint8_t timecount =1,timestop=0;
	
	uint8_t testCard=0;
	
	/* 串口初始化 */
	USART_Config();
	
	SysTick_Init();
	
	 printf("\r\n野火WF-GSM模块UDP收发例程\r\n");
	
	//检测模块是否响应正常
	while(gsm_init() != GSM_TRUE)
	{
		printf("\r\n 模块响应测试不正常！！ \r\n");
		printf("\r\n 若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关 \r\n");
		GSM_DELAY(1000);
	}
	printf("\r\n 正在等待GSM模块初始化... \r\n");
	
	while(IsInserCard()!= GSM_TRUE)
	{
		if(++testCard>20)
		{
			printf("\r\n 检测不到电话卡，请断电并重新接入电话卡 \r\n");
		}
		GSM_DELAY(1000);
	}
	
	printf("\r\n 初始化完成，5秒后开始放短信测试... \r\n");
	
	/* 延时5秒在发送命令到模块 */
	GSM_DELAY(5000);
	
	//确认关闭之前的连接
	gsm_gprs_link_close();
	
	GSM_DELAY(100);
	
	//确认关闭之前的场景
	gsm_gprs_shut_close();
	
	printf("\n初始化GPRS工作环境...\n");
	
	//重新初始化gprs
	if(gsm_gprs_init()!=GSM_TRUE)   //GPRS初始化环境
	{
		printf("\r 初始化GPRS失败，请重新给模块上电并复位开发板 \n");
		while(1);
	}
	
	printf("\n 尝试建立UDP连接，请耐心等待... \n");
	
	if(gsm_gprs_udp_link(LOCALPORT,SERVERIP,SERVERPORT) != GSM_TRUE)
	{
		printf("\r\n TCP连接失败，请检测正确设置各个模块... \r\n");
		GSM_DELAY(1000);
		printf("\n IP连接断开 \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n 关闭场景 \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		printf("\r\n 5秒后自动重启 \r\n");
		GSM_DELAY(5000);
		Soft_Reset();
	}
	
	printf("\n 连接成功,尝试发送数据... \n");
	printf("\r\n 注意：经测试，使用UDP发送数据时从第二条数据开始才能被接收到 \r\n");
	if(gsm_gprs_send(TESTBUFF1) != GSM_TRUE)
	{
		printf("\r\n 发送数据失败，请检测正确设置各个模块... \r\n");
		GSM_DELAY(1000);
		printf("\n IP连接断开 \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n 关闭场景 \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n 尝试发送第二条数据... \n");
	if(gsm_gprs_send(TESTBUFF2) != GSM_TRUE)
	{
		printf("\r\n 发送数据失败，请检测正确设置各个模块... \r\n");
		GSM_DELAY(1000);
		printf("\n IP连接断开 \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n 关闭场景 \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n 发送第二条数据成功 \n");
	
	printf("\n 已准备好接收远端数据，可使用网络调试助手往模块发送数据 \n");
	
	GSM_CLEAN_RX();
	
	while(1)
	{
		if((timecount>=50) &&(timestop != 0xFF))
		{
			if(PostGPRS()!= GSM_TRUE)
				timestop ++;
			else
				timestop = 0;
			timecount = 0;
		}
		if(timestop == 120)
		{
			printf("\n 长时间无通讯，即将关闭网络 \n");
			
			printf("\n IP连接断开 \n");
			GSM_DELAY(100);
			gsm_gprs_link_close();
			
			printf("\n 关闭场景 \n");
			GSM_DELAY(100);
			gsm_gprs_shut_close();
			
			GSM_DELAY(100);
			timestop = 0xFF;
		}
		timecount++;
		GSM_DELAY(10);
	}
	
}


/*********************************************END OF FILE**********************/
