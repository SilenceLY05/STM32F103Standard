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
#include "bsp_spi_flash.h"
#include <stdio.h>

static void LCD_Test(void);
static void Delay(__IO uint32_t nCount);
void Printf_Charater(void);
//void Display_char_test(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{


	ILI9341_Init();
	
	Debug_USART_Config();
	printf("\r\n 这是一个液晶中文显示测试实验（字库在外部FLASH） \r\n");
	printf("\r\n 若汉字显示不正常，请阅读工程中的readme.txt文件说明，根据要求给FLASH重刷字模数据 \r\n"); 

	LED_BLUE;
	
	//Display_char_test();

	//其中0、3、5、6模式适合从左至右显示文字
	//不推荐使用其他模式显示文字，其他模式显示文字会有镜像效果
	//其中6模式为大部分液晶例程的默认显示方向
	ILI9341_GramScan(6);
	
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
	Printf_Charater();
	
	while(1)
	{
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
	ILI9341_DispStringLine_EN_CH(LINE(0),"野火BH");
	//显示指定大小的字符
	ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"野火BH",0);
	ILI9341_DisplayStringEx(2*48,0*48,48,48,(uint8_t *)"野火BH",0);
	
	/* 显示变量示例 */
	//LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
	
	/* 使用C标准库把变量转化成字符串 */
	sprintf(dispBuff,"显示变量: %d",testCNT);
	LCD_ClearLine(LINE(5));//清楚单行文字
	
	/* 然后显示该字符串即可，其他变量也是这样处理的 */
	ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);
	
	/* 显示图形 */
	//LCD_SetFont(&Font24x32);
	
	/* 画直线 */
	LCD_ClearLine(LINE(7));//清楚单行文字
	LCD_SetTextColor(BLUE);
	
	ILI9341_DispStringLine_EN_CH(LINE(7),"画直线：");
	
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
	LCD_ClearLine(LINE(7)); /* 清除单行文字 */
	LCD_SetTextColor(GREEN);
	
	ILI9341_DispStringLine_EN_CH(LINE(7),"画矩形：");
	
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(YELLOW);
	ILI9341_DrawRectangle(160,200,20,40,1);
	
	LCD_SetTextColor(GREY);
	ILI9341_DrawRectangle(170,200,50,20,0);
	
	Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
	
	/* 画圆 */
  LCD_ClearLine(LINE(7));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN_CH(LINE(7)," 画圆：");

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


//字模
unsigned char charater_matrix[] = 
{0x00,0x40,0x20,0x40,0x10,0xA0,0x11,0x10,0x82,0x08,0x44,0x06,0x41,
0xF0,0x10,0x40,0x10,0x40,0x23,0xF8,0xE0,0x40,0x22,0x48,
0x21,0x48,0x21,0x50,0x2F,0xFE,0x00,0x00};/*"淦",0*/

/* 使用串口在上位机打印字模 */
void Printf_Charater(void)
{
	int i,j;
	unsigned char kk;
	
	/* 用i做行计数 */
	for(i = 0 ; i < 16 ; i++)
	{
		/*j用作一字节内数据的移位计数*/
		/*一行像素的第一个字节*/
		for( j = 0 ; j < 8 ; j++)
		{
			/*一个数据位一个数据位地处理*/
			kk = charater_matrix[2*i] << j; // 左移J位
			if( kk & 0x80)
			{
				printf("＊");
			}
			else
			{
				printf("　");
			}
		}
		/*一行像素的第二个字节*/
		for( j = 0 ; j < 8 ; j++)
		{
			/*一个数据位一个数据位地处理*/
			kk = charater_matrix[2*i+1] << j; // 左移J位
			if( kk & 0x80)
			{
				printf("＊");
			}
			else
			{
				printf("　");
			}
		}
		printf("\n");    //输出完一行像素，换行 
	}
	printf("\n\n");   //一个字输出完毕
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
//					printf("＊");
//				}
//				else
//				{
//					printf("　");
//				}
//			}
//		}

//	}
//}


/* ------------------------------------------end of file---------------------------------------- */

