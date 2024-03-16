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
#include "bsp_gps.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_bmp.h"
#include "bsp_key.h"


extern void nmea_decode_test(void);


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	LED_GPIO_Config();
	LED_BLUE;
	
	ILI9341_Init();
	Key_GPIO_Config();
	
	USART_Config();
	
	GPS_Config();
	
	printf("\r\nҰ�� GPSģ���������\r\n"); 
  
	
	/* GPS������� */
  nmea_decode_test();
  
  while(1);

}


/*********************************************END OF FILE**********************/
