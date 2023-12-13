/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{


	ILI9341_Init();
	
	Debug_USART_Config();
	printf("\r\n 这是一个液晶显示方向说明测试实验 \r\n");
	printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n"); 

	LED_BLUE;

	
//	//向LCD发送命令
//	*CMD_ADDR = 0xABCD;
//	*(uint16_t*)(0x60000000) = 0xABCD;
//	
//	//向LCD发送数据
//	*DATA_ARRD = 0x1234;
//	*(uint16_t*)(0x60020000) = 0x1234;
//	
//	//从液晶屏读取数据
//	lcd_read_temp = *DATA_ARRD;
//	lcd_read_temp = *(uint16_t*)(0x60020000);
//	
//	
//	
//	temp =temp | (1<<(16+1));
//	printf("\r\nA16为高电平，地址=%x",temp);
//	
//	temp = 0x60000000;
//	
//	temp &=~(1<<(16+1));
//	printf("\r\nA16为低电平，地址=%x",temp);
	
	while(1)
	{
		LCD_Direction_Show();
	}
}

/* 用于展示LCD的八中方向模式 */
void LCD_Direction_Show(void)
{
	uint8_t i = 0;
	
	char dispBuff[100];
	
	//轮流展示各个方向模式
	for(i = 0 ; i < 8 ; i++)
	{
		LCD_SetFont(&Font16x24);
		LCD_SetColors(RED,BLACK);
		
		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); /* 清屏，显示全黑 */
		
		//其中0/3/5/6模式适合从左至右显示文字
		//不推荐使用其他模式显示文字，其他模式显示文字会有镜像效果
		//其中6模式为大部分液晶例程的默认显示方向
		ILI9341_GramScan(i);
		
		sprintf(dispBuff , "o%d.  X --->",i);
		ILI9341_DispStringLine_EN(LINE(0),dispBuff);//沿X方向显示文字
		
		sprintf(dispBuff , "o%d.  Y | V",i);
		ILI9341_DispString_EN_YDir(0,0,dispBuff);//沿Y方向显示文字
		
		Delay(0xFFFFFF);
		
		//显示测试
		// *	！！！其中0、3、5、6 模式适合从左至右显示文字，不推荐使用其它模式显示文字	
		//其它模式显示文字会有镜像效果
		LCD_Test();
	}
}

extern uint16_t lcdid;

//用于测试各种液晶的函数
void LCD_Test(void)
{
	/* 演示显示变量 */
	static uint8_t testCNT =0;
	char dispBuff[100];
	
	testCNT++;
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(CYAN,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);  //清屏，全屏显示黑色
	
	/* 显示字符串示例 */
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
	
	/* 显示变量示例 */
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	
	/* 使用C标准库把变量转化成字符串 */
	sprintf(dispBuff,"Count: %d",testCNT);
	LCD_ClearLine(LINE(4));//清楚单行文字
	
	/* 然后显示该字符串即可，其他变量也是这样处理的 */
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);
	
	/* 显示图形 */
	LCD_SetFont(&Font24x32);
	
	/* 画直线 */
	LCD_ClearLine(LINE(4));//清楚单行文字
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
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);/* 清屏，显示全黑 */
	
	/* 画矩形 */
	LCD_ClearLine(LINE(4)); /* 清除单行文字 */
	LCD_SetTextColor(GREEN);
	
	ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");
	
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(YELLOW);
	ILI9341_DrawRectangle(160,200,20,40,1);
	
	LCD_SetTextColor(GREY);
	ILI9341_DrawRectangle(170,200,50,20,0);
	
	Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
	
	/* 画圆 */
  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
	
	
}




/* 简单的延时函数 nCount为延时计数值 */
static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0 ; nCount--);
}



/*********************************************END OF FILE**********************/
