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
	
	USART_Config();
	
	printf("\r\nҰ�� GPSģ���������\r\n"); 
  
	printf("\r\n�������SD���ڵ�gpslog.txt�ļ���λ��־���ݽ��н���\r\n"); 
	printf("\r\n����Ҫ��GPSģ�鴮�ڴ��ص����ݽ��룬");
	printf("��ע�͵�gps_config.h�ļ��ĺ�__GPS_LOG_FILE \r\n"); 
	
	/* GPS������� */
  nmea_decode_test();
  
  while(1);

}


/*********************************************END OF FILE**********************/
