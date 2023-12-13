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
#include <stdio.h>

static void LCD_Test(void);
static void LCD_Direction_Show(void);
static void Delay(__IO uint32_t nCount);
void Printf_Charater(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{


	ILI9341_Init();
	
	Debug_USART_Config();
	printf("\r\n ����һ��Һ����ʾ����˵������ʵ�� \r\n");
	printf("\r\n ������֧�����ģ���ʾ���ĵĳ�����ѧϰ��һ�� \r\n"); 

	LED_BLUE;

	
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
	
	while(1)
	{
		LCD_Direction_Show();
	}
}

/* ����չʾLCD�İ��з���ģʽ */
void LCD_Direction_Show(void)
{
	uint8_t i = 0;
	
	char dispBuff[100];
	
	//����չʾ��������ģʽ
	for(i = 0 ; i < 8 ; i++)
	{
		LCD_SetFont(&Font16x24);
		LCD_SetColors(RED,BLACK);
		
		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); /* ��������ʾȫ�� */
		
		//����0/3/5/6ģʽ�ʺϴ���������ʾ����
		//���Ƽ�ʹ������ģʽ��ʾ���֣�����ģʽ��ʾ���ֻ��о���Ч��
		//����6ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
		ILI9341_GramScan(i);
		
		sprintf(dispBuff , "o%d.  X --->",i);
		ILI9341_DispStringLine_EN(LINE(0),dispBuff);//��X������ʾ����
		
		sprintf(dispBuff , "o%d.  Y | V",i);
		ILI9341_DispString_EN_YDir(0,0,dispBuff);//��Y������ʾ����
		
		Delay(0xFFFFFF);
		
		//��ʾ����
		// *	����������0��3��5��6 ģʽ�ʺϴ���������ʾ���֣����Ƽ�ʹ������ģʽ��ʾ����	
		//����ģʽ��ʾ���ֻ��о���Ч��
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
	ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
	ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x340 px");
	
	if(lcdid == LCDID_ILI9341)
	{
		ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD Driver");
	}
	else if(lcdid == LCDID_ST7789V)
	{
		ILI9341_DispStringLine_EN(LINE(2),"ST7789V LCD Driver");
	}
	ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad Driver");
	
	/* ��ʾ����ʾ�� */
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	
	/* ʹ��C��׼��ѱ���ת�����ַ��� */
	sprintf(dispBuff,"Count: %d",testCNT);
	LCD_ClearLine(LINE(4));//�����������
	
	/* Ȼ����ʾ���ַ������ɣ���������Ҳ����������� */
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);
	
	/* ��ʾͼ�� */
	LCD_SetFont(&Font24x32);
	
	/* ��ֱ�� */
	LCD_ClearLine(LINE(4));//�����������
	LCD_SetTextColor(BLUE);
	
	ILI9341_DispStringLine_EN(LINE(4),"DRAW LINE");
	
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
	LCD_ClearLine(LINE(4)); /* ����������� */
	LCD_SetTextColor(GREEN);
	
	ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");
	
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(YELLOW);
	ILI9341_DrawRectangle(160,200,20,40,1);
	
	LCD_SetTextColor(GREY);
	ILI9341_DrawRectangle(170,200,50,20,0);
	
	Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
	
	/* ��Բ */
  LCD_ClearLine(LINE(4));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

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



/*********************************************END OF FILE**********************/
