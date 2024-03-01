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
#include "bsp_ov7725.h"
#include "bsp_key.h"
#include "bsp_systick.h"
#include "bsp_bmp.h"
#include "ff.h"


extern uint8_t Ov7725_vsync;

unsigned int Task_Delay[NumOfTask];

extern OV7725_MODE_PARAM cam_mode;

FATFS fs;              /* FatFs文件系统对象 */
FRESULT res_sd;        /* 文件操作结果 */  



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	float frame_count =0;
	uint8_t retry = 0;
	
	/* 液晶初始化 */
	ILI9341_Init();
	ILI9341_GramScan(3);
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	/* 显示字符串 */
	ILI9341_DispStringLine_EN(LINE(0),"BH OV7725 Test Demo");
	
	Debug_USART_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();
	SysTick_Init();
	
	/* 挂载sd文件系统 */
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd ==FR_OK)
	{
		printf("\r\n 请给开发板插入已格式化成fat格式的SD卡 \r\n");
	}
	printf("\r\n ** OV7725摄像头实时显示例程 ** \r\n");
	
	/* OV7725 GPIO初始化 */
	OV7725_GPIO_Config();
	LED_BLUE;
	
	/* OV7725寄存器初始化 */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			printf("\r\n 没有检测到OV7725摄像头 \r\n");
			ILI9341_DispStringLine_EN(LINE(2),"No OV7725 module detected!");
			while(1);
		}
	}
	
	/*根据摄像头参数组配置模式*/
	OV7725_Special_Effect(cam_mode.effect);
	/*光照度*/
	OV7725_Brightness(cam_mode.brightness);
	/*饱和度*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*光照模式*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*对比度*/
	OV7725_Contrast(cam_mode.contrast);
	/*特殊效果*/
	OV7725_Special_Effect(cam_mode.effect);
	/*设置图像采样及模式大小*/
	OV7725_Window_Set(cam_mode.cam_sx,cam_mode.cam_sy,cam_mode.cam_width,cam_mode.cam_height,cam_mode.QVGA_VGA);
	
	/* 液晶屏扫描模式 */
	ILI9341_GramScan(cam_mode.lcd_scan);
	
	ILI9341_DispStringLine_EN(LINE(2),"OV7725 initialize success!");
	printf("\r\n OV7725摄像头初始化完成 \r\n");
	
	Ov7725_vsync = 0;

	
	while(1)
	{
		/* 接收到新图像进行显示 */
		if(Ov7725_vsync == 2)
		{
			frame_count++;
			FIFO_PREPARE;    /* FIFO准备 */
			ImagDisp(cam_mode.cam_sx,cam_mode.cam_sy,cam_mode.cam_width,cam_mode.cam_height);
			
			Ov7725_vsync = 0;
//			LED1_TOGGLE;
		}
		
		/* 检测按钮 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			static uint8_t name_count = 0;
			char name[40];
			
			/* 用来设置截图名字，防止重复，实际可用系统时间命名 */
			name_count++;
			sprintf(name,"0:photo_%d.bmp",name_count);
			
			LED_BLUE;
			printf("\r\n正在截图。。。");
			
			/* 截图必须设置好液晶显示方向和截图窗口 */
			ILI9341_GramScan(cam_mode.lcd_scan);
			
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n截图成功!");
				LED_GREEN;
			}
			else
			{
				printf("\r\n截图失败");
				LED_RED;
			}
		}
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
//			LED3_TOGGLE;
			
			/*动态配置摄像头的模式，
			    有需要可以添加使用串口、用户界面下拉选择框等方式修改这些变量，
			    达到程序运行时更改摄像头模式的目的*/
			cam_mode.QVGA_VGA = 0,
			cam_mode.cam_sx = 0,
			cam_mode.cam_sy = 0,
			
			cam_mode.cam_width = 320,
			cam_mode.cam_height = 240,
			
			cam_mode.lcd_sx = 0,
			cam_mode.lcd_sy = 0,
			cam_mode.lcd_scan = 3,
			
			cam_mode.brightness = 0,
			cam_mode.contrast = 0,
			cam_mode.effect = 1,
			cam_mode.light_mode = 0,
			cam_mode.saturation = 0,
			
			OV7725_Special_Effect(cam_mode.effect);
			/*光照度*/
			OV7725_Brightness(cam_mode.brightness);
			/*饱和度*/
			OV7725_Color_Saturation(cam_mode.saturation);
			/*光照模式*/
			OV7725_Light_Mode(cam_mode.light_mode);
			/*对比度*/
			OV7725_Contrast(cam_mode.contrast);
			/*特殊效果*/
			OV7725_Special_Effect(cam_mode.effect);
			/*设置图像采样及模式大小*/
			OV7725_Window_Set(cam_mode.cam_sx,cam_mode.cam_sy,cam_mode.cam_width,cam_mode.cam_height,cam_mode.QVGA_VGA);
			/* 液晶屏扫描模式 */
			ILI9341_GramScan(cam_mode.lcd_scan);
		}
		/*每隔一段时间计算一次帧率*/
		if(Task_Delay[0]==0)
		{
			printf("\r\n frame_ate = %.2f fps \r\n",frame_count/10);
			frame_count = 0;
			Task_Delay[0] = 1000;
		}
	}
}






/*********************************************END OF FILE**********************/
