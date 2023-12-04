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
#include "ff.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_spi_flash.h"

FATFS fs;         /* FatFs�ļ�ϵͳ���� */
FIL fnew;         /* �ļ����� */
FRESULT res_flash;   /* �ļ�������� */
UINT fnum;           /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024] = {0};  /* �������� */
BYTE WriteBuffer[] = "��ӭʹ��Ұ��STM32������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ� \r\n";   /* д������ */


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ��ʼ��LED */
	LED_GPIO_Config();
	LED_BLUE;
	
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	USART_Config();
	printf(" ����һ��SPI FLASH�ļ�ϵͳʵ��\r\n ");
	printf(" \r\n ʹ��ָ���ߵװ�ʱ�����Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPI FLASHƬѡ���� \r\n ");
	
	/* ���ⲿSPI_Flash�����ļ�ϵͳ �ļ�ϵͳ����ʱ���SPI�豸��ʼ�� */
	/* ��ʼ������ʹ���������� f_mount()-> find_volume()->disk_initialize->SPI_Flash_Init() */
	res_flash =f_mount(&fs,"1:",1);
	
	/* ��ʽ������ */
	/* ���û��ϵͳ�ļ��͸�ʽ�������ļ�ϵͳ */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("��FLASH��û���ļ�ϵͳ���������и�ʽ���� \r\n");
		/* ��ʽ�� */
		res_flash = f_mkfs("1:",0,0);
		if(res_flash == FR_OK)
		{
			printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
			/* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"1:" , 1);
			/* ���¹��� */
  		res_flash = f_mount(&fs,"1:" , 1);
		}
		else
		{
			LED_RED;
			printf("����ʽ��ʧ�ܡ�\r\n");
			while(1);
		}
	}
	else if(res_flash != FR_OK)
	{
		printf("�����ⲿFLASH�����ļ�ϵͳ�ǰɡ�(%d)\r\n",res_flash);
		printf("��������ԭ��SPI_Flash��ʼ�����ɹ���\r\n");
		printf("������SPI-��д����FLASH���̲��ԣ�����������ڸ�����f_mount()�����if���ǰ��ʱ�����һ��res_flash = FR_NO_FILESYSTEM��������ֱ��ִ�и�ʽ������\r\n");
		while(1);
	}
	else
	{
		printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
	}
	
	/* �ļ�ϵͳ���ԣ�д���� */
	/* ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
	printf(" \r\n ���������ļ�д����� \r\n ");
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if(res_flash == FR_OK)
	{
		printf(" \r\n ����/����FatFs�ļ�ϵͳ����.txt�ɹ������ļ�д������ \r\n ");
		/* ��ָ���洢������д�뵽�ļ��� */
		res_flash = f_write(&fnew , WriteBuffer,sizeof(WriteBuffer) , &fnum);
		if(res_flash == FR_OK)
		{
			printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
			printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
			
		}
		else
		{
			printf("�����ļ�д������ʧ�ܣ�(%d)\n",res_flash);
		}
		/* ���ٶ�д���ر��ļ� */
		f_close(&fnew);
	}
	else
	{
		LED_RED;
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
	/* �ļ�ϵͳ���ԣ������� */
	
	printf(" \r\n ���������ļ������� \r\n ");
	res_flash = f_open(&fnew, "1:FatFs��д�����ļ�.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		printf(" \r\n �����ļ��ɹ�\r\n ");
		res_flash = f_read(&fnew , ReadBuffer,sizeof(ReadBuffer) , &fnum);
		if(res_flash == FR_OK)
		{
			printf("���ļ���ȡ�ɹ�����ȡ�ֽ����ݣ�%d\n",fnum);
			printf("����ȡ�����ļ�����Ϊ��\r\n%s\r\n",ReadBuffer);
			
		}
		else
		{
			printf("�����ļ���ȡ����ʧ�ܣ�(%d)\n",res_flash);
		}
		
	}
	else
	{
		LED_RED;
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"1:",1);
	
//	FRESULT res;
//	LED_GPIO_Config();
//	
//	LED_BLUE;
//	
//	USART_Config();
//	
//	printf("\r\n ����һ��FatFs��ֲʵ�� \r\n");
//	
//	//�����ļ�ϵͳ
//	res = f_mount(&fs,"1:" , 1);
//	
//	printf("\r\n f_mount res = %d ",res);
//	
//	if(res == FR_NO_FILESYSTEM)
//	{
//		res = f_mkfs("1:",0,0);
//		printf("\r\n f_mkfs res = %d ",res);
//		
//		//��ʽ����Ҫȡ�����أ������¹����ļ�ϵͳ
//		res = f_mount(NULL,"1:" , 1);
//		res = f_mount(&fs,"1:" , 1);
//		
//		printf("\r\n second f_mount res = %d ",res);
//	}
//	
//	res = f_open(&fnew, "1:����",FA_OPEN_ALWAYS | FA_READ|FA_WRITE );
//	printf("\r\n f_open res = %d ",res);
//	
//	if(res == FR_OK)
//	{
//		res = f_write(&fnew, "��������",sizeof(wData) , &bw);
//		printf("\r\n bw = %d",bw);
//		if(res == FR_OK)
//		{
//			f_lseek(&fnew,0);
//			res = f_read(&fnew, rData,f_size(&fnew) ,&br);
//			if(res == FR_OK)
//				printf("\r\n�ļ����ݣ�%s br = %d",rData,br);
//			
//		}
//		f_close(&fnew);
//	}

	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
