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
#include "bsp_key.h"
#include "ff.h"
#include "bsp_sdio_sdcard.h"


/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;                             /* FatFs�ļ�ϵͳ���� */
FIL fnew;                             /* �ļ����� */
FRESULT res_sd;                       /* �ļ�������� */
UINT fnum;                            /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024] = {0};          /* �������� */
BYTE WriteBuffer[] = "��ӭʹ��STM32�����壬�½��ļ�ϵͳ�����ļ�\r\n";        /* д������*/
extern SD_CardInfo  SDCardInfo;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	printf("\r\n********** ����һ��SD�� �ļ�ϵͳʵ�� **********\r\n");
	
	//���ⲿSPI FLASH�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_sd = f_mount(&fs,"0:",1);
	
  //	printf("����=%lld",SDCardInfo.CardCapacity/1024/1024);
/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("�� SD����û���ļ�ϵͳ���������и�ʽ��������\r\n");
		/* ��ʽ�� */
		res_sd = f_mkfs("0:",0,0);
		
		if(res_sd == FR_OK)
		{
		
			printf(" �� SD���ѳɹ���ʽ���ļ�\r\n");
			/* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);
			/* ���¹��� */
			res_sd = f_mount (&fs,"0:",1);
		}
		else
		{
			LED_RED;
			printf(" ����ʽ��ʧ�ܡ� \r\n");
			while(1);
			
		}
	}
	else if(res_sd != FR_OK)
	{
		printf("\r\n ����SD�������ļ�ϵͳʧ�ܡ�(%d) \r\n",res_sd);
		printf(" ��������ԭ��SD����ʼ�����ɹ� \r\n");
		while(1);
	}
	else
	{
		printf("���ļ�ϵͳ���سɹ������Խ��ж�д���� \r\n");
	}
	
	/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	printf("\r\n ********** ���������ļ�д����� ********** \r\n");
	res_sd = f_open(&fnew,"0:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE);
	if(res_sd == FR_OK)
	{
		printf("����/����FatFs��д�����ļ�.txt�ɹ������ļ�д������\r\n");
		/* ��ָ���洢������д�뵽�ļ��� */
		res_sd = f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
		if(res_sd == FR_OK)
		{
			printf("�����ļ�д��ɹ���д���ֽ�����:%d\n",fnum);
			printf("\r\n �����ļ�д�������Ϊ��\r\n%s \r\n",WriteBuffer);
		}
		else
		{
			printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_sd);
		}
		/* ���ٶ�д���ر��ļ� */
		f_close(&fnew);
	}
	else
	{
		LED_RED;
		printf("������/�����ļ�ʧ��\r\n");
	}
	/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	printf("\r\n ********** ���������ļ���ȡ����... ********** \r\n");
	res_sd = f_open(&fnew,"0:FatFs��ȡ�����ļ�",FA_OPEN_EXISTING | FA_READ);
	if(res_sd == FR_OK)
	{
		LED_GREEN;
		printf("�����ļ��ɹ�\r\n");
		res_sd = f_read(&fnew,ReadBuffer,sizeof(ReadBuffer),&fnum);
		if(res_sd == FR_OK)
		{
			printf("���ļ���ȡ�ɹ��������ֽ����ݣ�%d\r\n",fnum);
			printf("����ȡ�����ļ�����Ϊ��\r\n%s\r\n",ReadBuffer);
		}
		else
		{
			printf("�����ļ���ȡʧ�ܣ�(%d)\r\n",res_sd);
		}
	}
	else
	{
		LED_RED;
		printf("�������ļ�ʧ��\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);
	
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"0:",1);
	

	/*  ��ɲ��� ͣ��*/
	while (1)
	{
	}
}


/*********************************************END OF FILE**********************/
