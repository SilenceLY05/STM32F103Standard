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
#include "stm32f10x_it.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_mpu6050.h"

//设置是否使用LCD进行显示，不需要的话把这个宏注释掉即可
#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
 #include "./lcd/bsp_ili9341_lcd.h"
#endif


/* MPU6050数据 */
short Acel[3];
short Gyro[3];
float Temp;


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
#ifdef USE_LCD_DISPLAY
	char cStr[70];
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	
	ILI9341_Clear(0,0,240,320);
	ILI9341_DispStringLine_EN(LINE(1),"This is a MPU6050 demo");
#endif
	LED_GPIO_Config();
	
	USART_Config();
	
	i2c_GPIO_Config();
	
	MPU6050_Init();
	
	/* 检测到MPU6050 */
	if(MPU6050ReadID() == 0)
	{
		printf("\r\n 没有检测到MPU6050传感器！ \r\n");
		LED_RED;
		#ifdef USE_LCD_DISPLAY
		/* 设置字体颜色及字体的背景颜色 */
			LCD_SetColors(BLUE,BLACK);
			ILI9341_DispStringLine_EN(LINE(4),"No MPU6050 detected");
			ILI9341_DispStringLine_EN(LINE(5),"Please check the hardware connection!");
		#endif
		while(1);   // 检测不到会亮红灯然后卡死
	}
	
	printf("\r\n 欢迎使用野火  STM32 开发板。\r\n");		 
	printf("\r\n 这是一个MPU6050测试例程 \r\n");
	
	/* 配置Systick定时器和中断 */
	SysTick_Init();  //配置Systick为1ms中断一次，在中断里读取传感器数据
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //启动定时器

	
	while (1)
	{	
		if(task_readdata_finish)  //task_readdata_finish = 1 表示读取MPU6050数据完成
		{
			printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
			printf("    陀螺仪：%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
			printf("    温度%8.2f\r\n",Temp);
			
			#ifdef USE_LCD_DISPLAY
				ILI9341_DispStringLine_EN(LINE(7),"Acceleration");
				sprintf(cStr,"%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
				ILI9341_DispStringLine_EN(LINE(8),cStr);
			
				ILI9341_DispStringLine_EN(LINE(11),"Gyro       :");
				sprintf(cStr,"%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
				ILI9341_DispStringLine_EN(LINE(12),cStr);
			
				sprintf(cStr,"Temperture  :%8.2f",Temp);
				ILI9341_DispStringLine_EN(LINE(15),cStr);
			#endif
			
			task_readdata_finish = 0; //清除标志位
		}
		
	}
}


/*********************************************END OF FILE**********************/
