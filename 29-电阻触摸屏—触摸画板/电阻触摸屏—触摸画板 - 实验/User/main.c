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
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_spi_flash.h"
#include "bsp_xpt2046_lcd.h"
#include "palette.h"
#include <string.h>

static void Delay(__IO uint32_t nCount);



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	uint16_t value_test = 0;

	ILI9341_Init();
	XPT2046_Init();
	
	Debug_USART_Config();
	LED_GPIO_Config();
	printf("\r\n ********** ����������� *********** \r\n"); 
	printf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ����\r\n"); 

	LED_BLUE;
	

	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
	//���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
	//���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9341_GramScan ( 3 );	
	
	
	
	while(1)
	{
//		XPT2046_SendCMD(0x90);
//	Delay(0xFF);
//	value_test = XPT2046_ReceiveData();
//		
//	printf("\r\n value_test = 0x%x \r\n",value_test);
		if( touch_detect() == TOUCH_PRESSED)
		{
			printf("\r\n����������");
		}
		else
		{
			printf("\r\n����δ������");
		}
		Delay(0xFFFFF);
	}
}


static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0 ; nCount--);
}



/* ------------------------------------------end of file---------------------------------------- */

