/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_rtc.h"
#include "bsp_led.h"


// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{

//��ʹ�øú������Ƿ�ʹ��Һ����ʾ
#ifdef  USE_LCD_DISPLAY
	
	ILI9341_Init();
	LCD_SetFont(&Font8x16);
	LCD_SetColors(BLUE,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	ILI9341_DispStringLine_EN(LINE(0),"         BH RTC demo");
#endif
	
	Debug_USART_Config();
	
	Key_GPIO_Config();
	
	/* ����RTC�ж����ȼ� */
	RTC_NVIC_Config();
	RTC_CheckAndConfig(&systmtime);
	
	while(1)
	{
		/* ÿ��һ�����һ��ʱ�� */
		if(TimeDisplay == 1)
		{
			/* ��ǰʱ�� */
			Time_Display(RTC_GetCounter(),&systmtime);
			TimeDisplay = 0;
		}
		
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			struct rtc_time set_time;
			
			/* ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ӻس� */
			Time_Regulate_Get(&set_time);
			/* �ý��ܵ���ʱ������RTC */
			Time_Adjust(&set_time);
			
			//�򱸷ݼĴ���д���־
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
		}
	}
	
	

	
}


/* ------------------------------------------end of file---------------------------------------- */

