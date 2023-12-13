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
#include <stdio.h>

static void LCD_Test(void);
static void Delay(__IO uint32_t nCount);
void Printf_Charater(void);
//void Display_char_test(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{


	ILI9341_Init();
	
	Debug_USART_Config();
	printf("\r\n ����һ��Һ��������ʾ����ʵ�飨�ֿ����ⲿFLASH�� \r\n");
	printf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ���� \r\n"); 

	LED_BLUE;
	
	//Display_char_test();

	//����0��3��5��6ģʽ�ʺϴ���������ʾ����
	//���Ƽ�ʹ������ģʽ��ʾ���֣�����ģʽ��ʾ���ֻ��о���Ч��
	//����6ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
	ILI9341_GramScan(6);
	
//	//��LCD��������
//	*CMD_ADDR = 0xABCD;
//	*(uint16_t*)(0x60000000) = 0xABCD;
//	
//	//��LCD��������
//	*DATA_ARRD = 0x1234;
//	*(uint16_t*)(0x60020000) = 0x1234;
//	
//	//��Һ������ȡ����
//	lcd_read_temp = *DATA_ARRD;
//	lcd_read_temp = *(uint16_t*)(0x60020000);
//	
//	
//	
//	temp =temp | (1<<(16+1));
//	printf("\r\nA16Ϊ�ߵ�ƽ����ַ=%x",temp);
//	
//	temp = 0x60000000;
//	
//	temp &=~(1<<(16+1));
//	printf("\r\nA16Ϊ�͵�ƽ����ַ=%x",temp);
	Printf_Charater();
	
	while(1)
	{
		LCD_Test();
	}
}

extern uint16_t lcdid;

//���ڲ��Ը���Һ���ĺ���
void LCD_Test(void)
{
	/* ��ʾ��ʾ���� */
	static uint8_t testCNT =0;
	char dispBuff[100];
	
	testCNT++;
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(CYAN,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);  //������ȫ����ʾ��ɫ
	
	/* ��ʾ�ַ���ʾ�� */
	ILI9341_DispStringLine_EN_CH(LINE(0),"Ұ��BH");
	//��ʾָ����С���ַ�
	ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"Ұ��BH",0);
	ILI9341_DisplayStringEx(2*48,0*48,48,48,(uint8_t *)"Ұ��BH",0);
	
	/* ��ʾ����ʾ�� */
	//LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	
	/* ʹ��C��׼��ѱ���ת�����ַ��� */
	sprintf(dispBuff,"��ʾ����: %d",testCNT);
	LCD_ClearLine(LINE(5));//�����������
	
	/* Ȼ����ʾ���ַ������ɣ���������Ҳ����������� */
	ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);
	
	/* ��ʾͼ�� */
	//LCD_SetFont(&Font24x32);
	
	/* ��ֱ�� */
	LCD_ClearLine(LINE(7));//�����������
	LCD_SetTextColor(BLUE);
	
	ILI9341_DispStringLine_EN_CH(LINE(7),"��ֱ�ߣ�");
	
	LCD_SetTextColor(RED);
	ILI9341_DrawLine(50,170,210,230);
	ILI9341_DrawLine(50,200,210,240);
	
	LCD_SetTextColor(YELLOW);
	ILI9341_DrawLine(100,170,200,230);
	ILI9341_DrawLine(200,200,220,240);
	
	LCD_SetTextColor(BRED);
	ILI9341_DrawLine(110,170,110,230);
	ILI9341_DrawLine(130,200,220,240);
	
	Delay(0xFFFFFF);
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);/* ��������ʾȫ�� */
	
	/* ������ */
	LCD_ClearLine(LINE(7)); /* ����������� */
	LCD_SetTextColor(GREEN);
	
	ILI9341_DispStringLine_EN_CH(LINE(7),"�����Σ�");
	
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(YELLOW);
	ILI9341_DrawRectangle(160,200,20,40,1);
	
	LCD_SetTextColor(GREY);
	ILI9341_DrawRectangle(170,200,50,20,0);
	
	Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
	
	/* ��Բ */
  LCD_ClearLine(LINE(7));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN_CH(LINE(7)," ��Բ��");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
	
	
}




/* �򵥵���ʱ���� nCountΪ��ʱ����ֵ */
static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0 ; nCount--);
}


//��ģ
unsigned char charater_matrix[] = 
{0x00,0x40,0x20,0x40,0x10,0xA0,0x11,0x10,0x82,0x08,0x44,0x06,0x41,
0xF0,0x10,0x40,0x10,0x40,0x23,0xF8,0xE0,0x40,0x22,0x48,
0x21,0x48,0x21,0x50,0x2F,0xFE,0x00,0x00};/*"��",0*/

/* ʹ�ô�������λ����ӡ��ģ */
void Printf_Charater(void)
{
	int i,j;
	unsigned char kk;
	
	/* ��i���м��� */
	for(i = 0 ; i < 16 ; i++)
	{
		/*j����һ�ֽ������ݵ���λ����*/
		/*һ�����صĵ�һ���ֽ�*/
		for( j = 0 ; j < 8 ; j++)
		{
			/*һ������λһ������λ�ش���*/
			kk = charater_matrix[2*i] << j; // ����Jλ
			if( kk & 0x80)
			{
				printf("��");
			}
			else
			{
				printf("��");
			}
		}
		/*һ�����صĵڶ����ֽ�*/
		for( j = 0 ; j < 8 ; j++)
		{
			/*һ������λһ������λ�ش���*/
			kk = charater_matrix[2*i+1] << j; // ����Jλ
			if( kk & 0x80)
			{
				printf("��");
			}
			else
			{
				printf("��");
			}
		}
		printf("\n");    //�����һ�����أ����� 
	}
	printf("\n\n");   //һ����������
}


//void Display_char_test(void)
//{
//	uint8_t Row_num,Byte_num,Pixel_num;
//	
//	for( Row_num = 0 ; Row_num < 16 ; Row_num ++)
//	{
//		printf("\n");
//		for(Byte_num = 0 ; Byte_num < 2 ; Byte_num ++)
//		{
//			for( Pixel_num = 0 ; Pixel_num < 8 ; Pixel_num ++)
//			{
//				if(test_module[Row_num * 2 + Byte_num]&( 0x80>>Pixel_num ))
//				{
//					printf("��");
//				}
//				else
//				{
//					printf("��");
//				}
//			}
//		}

//	}
//}


/* ------------------------------------------end of file---------------------------------------- */

