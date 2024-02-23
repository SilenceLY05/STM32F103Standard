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
#include "bsp_ili9341_lcd.h"
#include "bsp_rtc.h"
#include "bsp_led.h"
#include "bsp_beep.h"


// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/* 时钟结构体 闹钟时间 2000-01-01 00:00:08 */
struct rtc_time clocktime = 
{
8,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	
	uint32_t clock_timestamp;
	uint32_t current_timestamp;

//可使用该宏设置是否使用液晶显示
#ifdef  USE_LCD_DISPLAY
	
	ILI9341_Init();
	LCD_SetFont(&Font8x16);
	LCD_SetColors(BLUE,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	ILI9341_DispStringLine_EN(LINE(0),"         BH RTC demo");
#endif
	
	Debug_USART_Config();
	
	Key_GPIO_Config();
	
	/* 配置RTC中断优先级 */
	RTC_NVIC_Config();
	RTC_CheckAndConfig(&systmtime);
	
	/* 设置闹钟寄存器 */
	clock_timestamp = mktimev(&clocktime) - TIME_ZOOM;
	RTC_SetAlarm(clock_timestamp);
	
	while(1)
	{
		/* 每过一秒更新一次时间 */
		if(TimeDisplay == 1)
		{
			/* 当前时间 */
			current_timestamp = RTC_GetCounter();
			Time_Display(current_timestamp,&systmtime);
			TimeDisplay = 0;
		}
		
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			struct rtc_time set_time;
			
			/* 使用串口接收设置的时间，输入数字时注意加回车 */
			Time_Regulate_Get(&set_time);
			/* 用接受到的时间设置RTC */
			Time_Adjust(&set_time);
			
			//向备份寄存器写入标志
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
		}
		
		//响铃
		if(TimeAlarm == 1)
		{
			BEEP(ON);
		}
		
		//按下按键，关闭蜂鸣器
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			BEEP(OFF);
			TimeAlarm = 0;
		}
	}
	
	

	
}


/* ------------------------------------------end of file---------------------------------------- */

