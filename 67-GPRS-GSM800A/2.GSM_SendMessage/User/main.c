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

/* 请修改接收短信的电话号码 */
const char num[] = "10086";

char namenum[20*4] = {0}, str[512] = {0},gbkstr[1024] = {0},namegbk[512] = {0};


	
/* 测试发送短信功能，实际测试时把电话号码修改成要接收短信的电话号码 */
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint8_t IsRead = 0;
	uint8_t testCard = 0;
	
	/* 串口初始化 */
	USART_Config();
	
	SysTick_Init();
	
	GSM_USART_Config();
	
	printf("\r\n野火BH-GSM模块发送短信例程\r\n");
	  printf("\r\n测试前请确认GSM模块已接入电话卡\r\n");
    printf("\r\n测试前请在程序中修改接收短信的目标手机号码\r\n");
	
	//检测模块是否响应正常
	while(gsm_cmd("AT\r","OK",1000) != GSM_TRUE)
	{
		printf("\r\n 模块响应测试不正常！！ \r\n");
		printf("\r\n 若模块响应测试一直不正常，请检查模块的连接或者是否已开启电源开关 \r\n");
	}
	
	printf("\r\n 通过了模块响应测试 \r\n");
	printf("\r\n 正在等待GSM模块初始化... \r\n");
	
	//设置短信提示方式
	if(gsm_cmd("AT+CNMI=2,1\r","OK",100) != GSM_TRUE)
	{
		printf("AT+CNMI\r\n");
	}
	GSM_DELAY(1000);
	
	//如果卡内存储短信满了后不能继续接收短信，尝试使用此函数删除，注意这里是全部删除
//		if(gsm_cmd("AT+CMGDA=\"DEL ALL\"\r","OK", 500) != GSM_TRUE)
//		{
//						printf("AT+CMGDA\r\n");
//		}
//		GSM_DELAY(1000); 
	
	//先执行次设置文本模式
	if(gsm_cmd("AT+CMGF=1\r","OK",100) != GSM_TRUE)
	{
		printf("\r\n 设置文本模式错误 \r\n");
	}
	GSM_DELAY(1000);
	
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
	
	//发送英文短信
	if(gsm_sms((char*)num,"hello world") == GSM_TRUE)
		printf("\r\n 英文短信已发送至：%s,为方便测试，请在程序中修改接收短信的手机号码 \r\n",num);
	else
	{
		printf("\r\n 短信发送失败，请确认目标号码有效 \r\n");
	}
	GSM_DELAY(2000);
	
	//中英文短信，实际测试时请把电话号码修改成要接收短信的手机号
	if(gsm_sms((char*)num,"野火WF-GSM模块短信修改测试") == GSM_TRUE)
		printf("\r\n 中英文短信已发送至:%s，为方便测试，请在程序中修改接收短信的手机号码 \r\n",num);
	else
	{
		printf("\r\n 短息发送失败，请确认目标号码有效 \r\n");
		while(1);
	}
	
	printf("\r\n 接收短信测试：通过串口1打印新短信信息 \r\n");
	
	/*
	提示：经过测试发现接收短信时可能会被拆成很多小段存储，以下在获取了一轮新短信提示的存储编号后分别打印，
	如果当短信被拆成多段时，演示打印各个发送者名和消息，用户可以根据自己需求编写比如合并同一个发送者名的消息。	
	*/
	
	while(1)
	{
		GSM_DELAY(1000);
		
		if(IsReceiveMS_Test() ==1)
		{
			for(int i = 0; i< currentAddIndex;i++)
			{
				IsRead = readmessage(address[i],namenum,str);
				if(IsRead)
				{
					hexuni2gbk(namenum,namegbk);
					hexuni2gbk(str,gbkstr);
					printf("%s\n",namegbk);
					printf("%s\n",gbkstr);
					
					memset(str,0,sizeof(str));
					memset(str,0,sizeof(namegbk));
					memset(str,0,sizeof(gbkstr));
				}
			}
		}
	}
	
}


/*********************************************END OF FILE**********************/
