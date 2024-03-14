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
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_vs1053.h"
#include "ff.h"
#include "bsp_spi_flash.h"
#include "bsp_systick.h"
//#include "bsp_exti.h"
#include "bsp_key.h"
#include "recorder.h"




/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;                             /* FatFs�ļ�ϵͳ���� */


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	SysTick_Init();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	//���ⲿSPI FLASH�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	f_mount(&fs,"0:",1);
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	VS1053_Init();
	printf("vs1053:%4X\n",VS_RamTest());
	
	Delay_ms(100);
	VS_SineTest();
	VS_HardReset();
	VS_SoftReset();
	

	/*  ��ɲ��� ͣ��*/
	while (1)
	{
		Recorder_Run();
	}
}


/*********************************************END OF FILE**********************/
