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
#include "bsp_ov7725.h"
#include "bsp_key.h"
#include "bsp_systick.h"
#include "wildfire_image_assistant.h"
#include "bsp_protocol.h"




extern uint8_t Ov7725_vsync;

unsigned int Task_Delay[NumOfTask];

extern OV7725_MODE_PARAM cam_mode;




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	uint8_t retry = 0;
	
	protocol_init();  // ͨѶЭ���ʼ��
	
	
	/* ע�� *//* ע�� *//* ע�� *//* ע�� *//* ע�� *//* ע�� *//* ע�� */
  /*ע����λ��������������Ϊ��1500000��û�����������ѡ����ֶ��޸ģ�*/
	/* Һ����ʼ�� */
	
	
	USART_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();
	SysTick_Init();
	
	/* OV7725 GPIO��ʼ�� */
	OV7725_GPIO_Config();
	LED_BLUE;
	
	/* OV7725�Ĵ�����ʼ�� */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			LED_RED;
			while(1);
		}
	}
	
	/*��������ͷ����������ģʽ*/
	OV7725_Special_Effect(cam_mode.effect);
	/*���ն�*/
	OV7725_Brightness(cam_mode.brightness);
	/*���Ͷ�*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*����ģʽ*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*�Աȶ�*/
	OV7725_Contrast(cam_mode.contrast);
	/*����Ч��*/
	OV7725_Special_Effect(cam_mode.effect);
	/*����ͼ�������ģʽ��С*/
	OV7725_Window_Set(cam_mode.cam_sx,cam_mode.cam_sy,cam_mode.cam_width,cam_mode.cam_height,cam_mode.QVGA_VGA);
	
	/* Һ����ɨ��ģʽ */
	ILI9341_GramScan(cam_mode.lcd_scan);
	
	ILI9341_DispStringLine_EN(LINE(2),"OV7725 initialize success!");
	printf("\r\n OV7725����ͷ��ʼ����� \r\n");
	
	Ov7725_vsync = 0;

	
	while(1)
	{
		write_rgb_wincc(0,cam_mode.cam_width,cam_mode.cam_height);
		receiving_process();  // �������ݴ���
	}
}






/*********************************************END OF FILE**********************/
