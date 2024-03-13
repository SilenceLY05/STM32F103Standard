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
#include "bsp_usart_blt.h"
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_hc05.h"
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "core_delay.h"


extern ReceiveData  BLT_USART_ReceiveData;
extern ReceiveData DEBUG_USART_ReceiveData;
extern int hc05_inquery_connect;

void TransData_CtrlLED_Test(void);
	


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	char hc05_name[30] = "HC05_SLAVE";
	char hc05_nameCMD[40];
	
	/* 延时函数初始化 */
	CPU_TS_TmrInit();

	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	#endif
	
	Debug_USART_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();

	/* HC05蓝牙模块初始化GPIO和USART2配置为38400 8-N-1接收中断 */
	if(HC05_Init())
	{
		HC05_INFO("HC05模块检测正常");
		
		#ifdef ENABLE_LCD_DISPLAY
		ILI9341_DispString_EN(40,40,"HC05 module detected!");
		#endif
	}
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检测模块与开发板之间的连接，然后复位后重新测试！");
		#ifdef ENABLE_LCD_DISPLAY
		ILI9341_DispString_EN(20,40,"No HC05 module detected!");
		ILI9341_DispString_EN(5,60,"Please check the hardware connection and reset the system.");
		#endif
		
		while(1);
	}


	HC05_INFO("HC05模块实验");
	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_DispString_EN(40,20,"HC05 BLUETOOTH demo");
	#endif
	
	/* 复位 恢复默认状态 */
	HC05_Send_CMD("AT+RESET\r\n",1);  //复位指令发送完成之后，需要一定时间HC05才会接受下一条指令
	HC05_Send_CMD("AT+ORGL\r\n",1);
	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/
	HC05_Send_CMD("AT+VERSON?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+START?\r\n",1);
	
	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	//初始化SPP规范
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	//设置模块名字
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);
	
	HC05_INFO("本模块名字为:%s,模块已准备就绪。",hc05_name);
	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_DispStringLine_EN(LINE(4),"ReceiveData USART1");
	ILI9341_DispStringLine_EN(LINE(9),"ReceiveData HC-05");
	#endif
	
	while(1)
	{
		TransData_CtrlLED_Test();  //处理数据
	}
	
	
}

void TransData_CtrlLED_Test(void)
{
	//处理调试串口接收到的串口助手数据
	if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
	{
		DEBUG_USART_ReceiveData.usart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
		
		//在这里可以自己定义想要接收的字符串然后处理
    //这里接收到串口调试助手发来的 “RED_LED”就会把板子上面的红灯取反一次
		if(strstr((char *)DEBUG_USART_ReceiveData.usart_buff,"RED_LED"))
		{
			LED1_TOGGLE;
		}
		
		//如果手机蓝牙连接了模块，则发送到手机端显示数据
		if(IS_HC05_CONNECTED )
		{
			BLT_KEY_LOW;
			USART_SendStr_Length(BLT_USARTx,DEBUG_USART_ReceiveData.usart_buff,DEBUG_USART_ReceiveData.datanum);
		}
		//如果数据是以AT开头的，就把KEY置高，设置蓝牙模块
		else if(strstr((char *)DEBUG_USART_ReceiveData.usart_buff,"AT") == (char *)DEBUG_USART_ReceiveData.usart_buff)
		{
			BLT_KEY_HIGHT;
			
			USART_SendStr_Length(BLT_USARTx,DEBUG_USART_ReceiveData.usart_buff,DEBUG_USART_ReceiveData.datanum);
			USART_SendStr_Length(BLT_USARTx,"\r\n",2);
			
			BLT_KEY_LOW;
		}
		
		//串口助手显示接收到的数据
		USART_SendString(DEBUG_USARTx,"\r\n receive USART1 data: \r\n");
		USART_SendString(DEBUG_USARTx,DEBUG_USART_ReceiveData.usart_buff);
		USART_SendString(DEBUG_USARTx,"\r\n");
		
		//LCD显示接收到的数据
		#ifdef  ENABLE_LCD_DISPLAY
		LCD_ClearLine(LINE(5));
		LCD_ClearLine(LINE(6));
		LCD_ClearLine(LINE(7));
		LCD_ClearLine(LINE(8));
		
		ILI9341_DispStringLine_EN(LINE(5),(char*)DEBUG_USART_ReceiveData.usart_buff);
		
		#endif
		
		//清零调试串口数据缓存
		DEBUG_USART_ReceiveData.receive_data_flag = 0;
		DEBUG_USART_ReceiveData.datanum = 0;
		
	}
	
	//处理调试串口接收到的串口助手数据
	if(BLT_USART_ReceiveData.receive_data_flag == 1)
	{
		BLT_USART_ReceiveData.usart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
		
		//在这里可以自己定义想要接收的字符串然后处理
    //这里接收到手机蓝牙发来的 “RED_LED”就会把板子上面的红灯取反一次
    if(strstr((char *)BLT_USART_ReceiveData.usart_buff,"RED_LED"))
    {
      LED1_TOGGLE;
    }
		
		//串口助手显示接收到的数据
		USART_SendString(DEBUG_USARTx,"\r\n receive HC05 data: \r\n");
		USART_SendString(DEBUG_USARTx,BLT_USART_ReceiveData.usart_buff);
		USART_SendString(DEBUG_USARTx,"\r\n");
		
		//LCD显示接收到的数据
		#ifdef  ENABLE_LCD_DISPLAY
		LCD_ClearLine(LINE(10));
		LCD_ClearLine(LINE(11));
		LCD_ClearLine(LINE(12));
		LCD_ClearLine(LINE(13));
		
		ILI9341_DispStringLine_EN(LINE(10),(char*)BLT_USART_ReceiveData.usart_buff);
		
		#endif
		
		//清零调试串口数据缓存
		BLT_USART_ReceiveData.receive_data_flag = 0;
		BLT_USART_ReceiveData.datanum = 0;
		
	}
}





/*********************************************END OF FILE**********************/
