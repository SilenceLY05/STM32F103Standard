#ifndef __BSP_ILI9341_LCD_H
#define __BSP_ILI9341_LCD_H

#include "stm32f10x.h"


/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X60000000
RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

/******************************* ILI9341 显示屏的 FSMC 参数定义 ***************************/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define FSMC_Addr_ILI9341_CMD        ((uint32_t)0x60000000)

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址
#define FSMC_Addr_ILI9341_DATA       ((uint32_t)0x60020000)

//由片选引脚决定的NOR/SRAM块
#define FSMC_Bank1_NORSRAMx          FSMC_Bank1_NORSRAM1

/******************************* ILI9341 显示屏8080通讯引脚定义 ***************************/
/******控制信号线******/

/* 片选，选择NOR/SRAM块 */
#define ILI9341_CS_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_CS_PORT            GPIOD
#define ILI9341_CS_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_CS_PIN             GPIO_Pin_7

//DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
//PD11为FSMC_A16
#define ILI9341_DC_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_DC_PORT            GPIOD
#define ILI9341_DC_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_DC_PIN             GPIO_Pin_11

//写使能
#define ILI9341_WR_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_WR_PORT            GPIOD
#define ILI9341_WR_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_WR_PIN             GPIO_Pin_5

//读使能
#define ILI9341_RD_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_RD_PORT            GPIOD
#define ILI9341_RD_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_RD_PIN             GPIO_Pin_4

//复位引脚
#define ILI9341_RST_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_RST_PORT           GPIOE
#define ILI9341_RST_CLK            RCC_APB2Periph_GPIOE
#define ILI9341_RST_PIN            GPIO_Pin_1

//背光引脚
#define ILI9341_BK_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define ILI9341_BK_PORT            GPIOD
#define ILI9341_BK_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_BK_PIN             GPIO_Pin_12




/* D数据信号线 */
#define ILI9341_D0_GPIO_PORT            GPIOD
#define ILI9341_D0_GPIO_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_D0_GPIO_PIN             GPIO_Pin_14

#define ILI9341_D1_GPIO_PORT            GPIOD
#define ILI9341_D1_GPIO_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_D1_GPIO_PIN             GPIO_Pin_15

#define ILI9341_D2_GPIO_PORT            GPIOD
#define ILI9341_D2_GPIO_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_D2_GPIO_PIN             GPIO_Pin_0

#define ILI9341_D3_GPIO_PORT            GPIOD
#define ILI9341_D3_GPIO_CLK             RCC_APB2Periph_GPIOD
#define ILI9341_D3_GPIO_PIN             GPIO_Pin_1

#define ILI9341_D4_GPIO_PORT            GPIOE
#define ILI9341_D4_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D4_GPIO_PIN             GPIO_Pin_7

#define ILI9341_D5_GPIO_PORT            GPIOE
#define ILI9341_D5_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D5_GPIO_PIN             GPIO_Pin_8

#define ILI9341_D6_GPIO_PORT            GPIOE
#define ILI9341_D6_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D6_GPIO_PIN             GPIO_Pin_9

#define ILI9341_D7_GPIO_PORT            GPIOE
#define ILI9341_D7_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D7_GPIO_PIN             GPIO_Pin_10

#define ILI9341_D8_GPIO_PORT            GPIOE
#define ILI9341_D8_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D8_GPIO_PIN             GPIO_Pin_11

#define ILI9341_D9_GPIO_PORT            GPIOE
#define ILI9341_D9_GPIO_CLK             RCC_APB2Periph_GPIOE
#define ILI9341_D9_GPIO_PIN             GPIO_Pin_12

#define ILI9341_D10_GPIO_PORT           GPIOE
#define ILI9341_D10_GPIO_CLK            RCC_APB2Periph_GPIOE
#define ILI9341_D10_GPIO_PIN            GPIO_Pin_13

#define ILI9341_D11_GPIO_PORT           GPIOE
#define ILI9341_D11_GPIO_CLK            RCC_APB2Periph_GPIOE
#define ILI9341_D11_GPIO_PIN            GPIO_Pin_14

#define ILI9341_D12_GPIO_PORT           GPIOE
#define ILI9341_D12_GPIO_CLK            RCC_APB2Periph_GPIOE
#define ILI9341_D12_GPIO_PIN            GPIO_Pin_15

#define ILI9341_D13_GPIO_PORT           GPIOD
#define ILI9341_D13_GPIO_CLK            RCC_APB2Periph_GPIOD
#define ILI9341_D13_GPIO_PIN            GPIO_Pin_8

#define ILI9341_D14_GPIO_PORT           GPIOD
#define ILI9341_D14_GPIO_CLK            RCC_APB2Periph_GPIOD
#define ILI9341_D14_GPIO_PIN            GPIO_Pin_9

#define ILI9341_D15_GPIO_PORT           GPIOD
#define ILI9341_D15_GPIO_CLK            RCC_APB2Periph_GPIOD
#define ILI9341_D15_GPIO_PIN            GPIO_Pin_10

/* 调试预用 */
#define DEBUG_DELAY()

/* ILI9341显示区域的起始坐标和总行列熟 */
#define ILI9341_DispWindow_X_Star     0  //起始点的X坐标
#define ILI9341_DispWindow_Y_Star     0  //起始点的Y坐标

#define ILI9341_LESS_PIXEL        240//液晶屏较短方向的像素宽度
#define ILI9341_MORE_PIXEL        320//液晶屏较短方向的像素宽度

#define ILI9341_DispWindow_COLUMN     240  //总列数
#define ILI9341_DispWindow_PAGE       320  //总行数


/********************** 在 ILI9341 显示屏上显示的字符的大小 *********************/
#define WIDTH_EN_CHAR                               8          //英文字符宽度
#define HEIGHT_EN_CHAR                              16         //英文字符高度

#define WIDTH_CH_CHAR                               16         //中文字符宽度
#define HEIGHT_CH_CHAR                              16         //中文字符高度

#define GetGBKCode(ucBuffer,usChar)                 //定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符


//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH;

//液晶屏扫描模式
//参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/* 定义 ILI9341 显示屏常用颜色 */
#define BACKGROUND              BLACK  //默认背景颜色

#define WHITE                   0xFFFF//白色
#define BLACK                   0x0000//黑色
#define GREY                    0xF7DE//灰色
#define BLUE                    0x001F//蓝色
#define BLUE2                   0x051F//浅蓝色
#define RED                     0xF800//红色
#define MAGENTA                 0xF81F//红紫色
#define GREEN                   0x07E0//绿色
#define CYAN                    0x7FFF//黄绿色
#define YELLOW                  0xFFE0//黄色
#define BRED                    0xF81F//紫色
#define GRED                    0xFFE0//黄色
#define GBLUE                   0x07FF//青色


/* 定义ILI9341常用命令 */
#define CMD_SetCoordinateX      0x2A    //设置X坐标
#define CMD_SetCoordinateY      0x2B    //设置Y坐标
#define CMD_SetPixel            0x2C    //填充像素


//#define RGB888_2_RGB565(R,G,B)		 (uint16_t)(((R & 0x1F) << 11) | ((G & 0x3F) << 5) | (B & 0x1F) )

/* 定义 LCD 驱动芯片 ID */
#define     LCDID_UNKNOWN             0
#define     LCDID_ILI9341             0x9341
#define     LCDID_ST7789V             0x8552


/********************************** 声明 ILI934 函数 ***************************************/
void                     ILI9341_Init                    ( void );
uint16_t                 ILI9341_ReadID                  ( void );
void                     ILI9341_Rst                     ( void );
void                     ILI9341_BackLed_Control         ( FunctionalState enumState );
__inline void            ILI9341_Write_Cmd               ( uint16_t usCmd );
__inline void            ILI9341_Write_Data              ( uint16_t usData );
__inline uint16_t        ILI9341_Read_Data               ( void );
void                     ILI9341_GramScan                ( uint8_t ucOtion );
void                     ILI9341_OpenWindow              ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_Clear                   ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_SetPointPixel           ( uint16_t usX, uint16_t usY );
uint16_t                 ILI9341_GetPointPixel           ( uint16_t usX , uint16_t usY );
void                     ILI9341_DrawLine                ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void                     ILI9341_DrawRectangle           ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled );
void                     ILI9341_DrawCircle              ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled );
void                     ILI9341_DispChar_EN             ( uint16_t usX, uint16_t usY, const char cChar );
void                     ILI9341_DispStringLine_EN      ( uint16_t line, char * pStr );
void                     ILI9341_DispString_EN      			( uint16_t usX, uint16_t usY, char * pStr );
void                     ILI9341_DispChar_CH             ( uint16_t usX, uint16_t usY, uint16_t usChar );
void                     ILI9341_DispString_CH           ( uint16_t usX, uint16_t usY,  char * pStr );
void                     ILI9341_DispString_EN_CH        (	uint16_t usX, uint16_t usY,  char * pStr );
void 									 	 ILI9341_DispStringLine_EN_CH 	(uint8_t usX,uint8_t usY , char* pStr);
void 									   ILI9341_DispString_EN_YDir 		(   uint16_t usX,uint16_t usY ,  char * pStr );
void 										 ILI9341_DispString_EN_CH_YDir 	(   uint16_t usX,uint16_t usY , char * pStr );



void ILI9341_DisplayStringEx(uint16_t x,//字符显示位置x
														 uint16_t y,//字符显示位置y
														 uint16_t Font_Width,//要显示的字体宽度，英文字符在此基础上/2。注意为偶数
														 uint16_t Font_Height,//要显示的字体高度，注意为偶数
														 uint8_t *ptr,  //显示的字符内容
														 uint16_t DrawModel);//是否反色显示

void ILI9341_DisplayStringEx_YDir(uint16_t x,//字符显示位置x
														      uint16_t y,//字符显示位置y
																  uint16_t Font_Width,//要显示的字体宽度，英文字符在此基础上/2。注意为偶数
																	uint16_t Font_Height,//要显示的字体高度，注意为偶数
																	uint8_t *ptr,  //显示的字符内容
																	uint16_t DrawModel);//是否反色显示


																				

#endif /* __BSP_ILI9341_LCD_H */

