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
#include "bsp_i2c_ee.h"
#include <string.h>
#include "bsp_ap3216c.h"
#include "stm32f10x_it.h"


/* 读取原始数据 */
uint16_t ALS_RAW;
uint16_t PS_RAW;
uint16_t IR_RAW;



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	float ALSValue;
	
	
	LED_BLUE;
	
	USART_Config();
	
	printf("这是一个三合一光照传感器读写源数据测试例程\r\n");
	
	
	printf("\r\n 芯片初始化中。。。 \r\n");
	
	ap3216c_init();
	

	
	while (1)
	{	
		/* 每 1000 毫秒执行：翻转LED（在中断里执行） */
    
    /* 每 500 毫秒执行：读取数据（在中断里读取），然后在这里处理并打印数据 */
		if(task_readdata_finish == 1)
		{
			ALSValue = ALS_RAW *0.36;
			printf("环境光：%.2flux",ALSValue);
			printf("接近值：%d",PS_RAW);
			printf("红外光：%d",IR_RAW);
			
			task_readdata_finish = 0;
		}
	}
	
}



/*********************************************END OF FILE**********************/
