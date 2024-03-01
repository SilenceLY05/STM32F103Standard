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
#include "wildfire_image_assistant.h"
#include "bsp_protocol.h"




extern uint8_t Ov7725_vsync;

unsigned int Task_Delay[NumOfTask];

extern OV7725_MODE_PARAM cam_mode;




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	uint8_t retry = 0;
	
	protocol_init();  // 通讯协议初始化
	
	
	/* 注意 *//* 注意 *//* 注意 *//* 注意 *//* 注意 *//* 注意 *//* 注意 */
  /*注意上位机波特率请设置为：1500000（没有这个波特率选项，请手动修改）*/
	/* 液晶初始化 */
	
	
	USART_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();
	SysTick_Init();
	
	/* OV7725 GPIO初始化 */
	OV7725_GPIO_Config();
	LED_BLUE;
	
	/* OV7725寄存器初始化 */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			LED_RED;
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
		write_rgb_wincc(0,cam_mode.cam_width,cam_mode.cam_height);
		receiving_process();  // 接收数据处理
	}
}






/*********************************************END OF FILE**********************/
