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
#include "bsp_exti.h"




/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;                             /* FatFs�ļ�ϵͳ���� */

char song_pt =0;
uint8_t song_list[][20] = {"0:TestFile_1.mp3", 	//�������������Զ�����������û�ֻ��Ҫ�ڴ���Ӹ���������
													 "0:TestFile_2.mp3"};

char song_number_max = 0;
													 
/* ����������� */
void song_list_init(void)
{
	song_number_max = sizeof(song_list) / sizeof(song_list[0]);
}

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
	
	EXTI_Key_Config();
	
	VS1053_Init();
	printf("vs1053:%4X\n",VS_RamTest());
	
	Delay_ms(100);
	VS_SineTest();
	VS_HardReset();
	VS_SoftReset();
	
  song_list_init();

	/*  ��ɲ��� ͣ��*/
	while (1)
	{
		// ���Ը��������ⲿFLASH W25Q64��
		VS1053_PlayerSong(song_list[song_pt]);
		printf("MusicPlay End\n");
	}
}


/*********************************************END OF FILE**********************/
