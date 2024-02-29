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
	LED_GPIO_Config();
	
	USART_Config();
	
	I2C_Bus_Init();
	
	MPU6050_Init();
	
	/* 检测到MPU6050 */
	if(MPU6050ReadID() == 0)
	{
		printf("\r\n 没有检测到MPU6050传感器！ \r\n");
		LED_RED;
		while(1);   // 检测不到会亮红灯然后卡死
	}
	
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
			
			task_readdata_finish = 0; //清除标志位
		}
		
	}
}


/*********************************************END OF FILE**********************/
