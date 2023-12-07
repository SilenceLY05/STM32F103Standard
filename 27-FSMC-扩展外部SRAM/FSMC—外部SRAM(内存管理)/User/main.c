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
#include "bsp_sram.h"
#include "bsp_malloc.h"



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	uint8_t *p=0;
	uint8_t sramx = 0;  //默认为内部sram

	Debug_USART_Config();

	FSMC_SRAM_Init();
	
	my_mem_init(SRAMIN);  //初始化内部内存池
	my_mem_init(SRAMEX);  //初始化外部内存池
	
	printf("\r\n 这是一个内存管理例程 \r\n");
	printf("\r\n 分配内存 \r\n");
	
	p = mymalloc (sramx,1024*2);  //申请2K字节
	
	sprintf((char*)p,"Memory Malloc"); // 试用申请空间存储内容
	
	printf("SRAMIN USED:%d%%\r\n",my_mem_perused(SRAMIN));  //显示内部内存使用率
	printf("SRAMEX USED:%d%%\r\n",my_mem_perused(SRAMEX));  //显示外部内存使用率
	
	myfree(sramx,p); // 释放内存
	p = 0; //指向空地址
	
	printf("\r\n 释放内存 \r\n");
	printf("SRAMIN USED:%d%%\r\n",my_mem_perused(SRAMIN));  //显示内部内存使用率
	printf("SRAMEX USED:%d%%\r\n",my_mem_perused(SRAMEX));  //显示外部内存使用率
	
	while(1);
	
}


/*********************************************END OF FILE**********************/
