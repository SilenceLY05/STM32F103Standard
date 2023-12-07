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

//1.使用指针对SRAM进行读写
//2.使用绝对地址的方式访问SRAM,这种方式必须定义成全局变量


uint8_t testValue __attribute__ ((at (Bank1_SRAM3_ADDR+0x40)));

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	
	LED_GPIO_Config();

	
	Debug_USART_Config();
	printf("\r\n 这是一个SRAM测试实验 \r\n");
	FSMC_SRAM_Init();
	
	LED_BLUE;
	
	/* 对SRAM进行读写测试，检测SRAM是否正常 */
	if(SRAM_Test() == 1)
	{
		//测试正常 绿灯亮
		LED_GREEN;
	}
	else
	{
		//测试失败 红灯亮
		LED_RED;
	}
	
	{
		/* 指针方式访问SRAM */
		uint32_t temp;
		printf("\r\n 指针方式访问SRAM \r\n");
		
		/* 向SRAM写入8位数据 */
		*(uint8_t*)(Bank1_SRAM3_ADDR) = (uint8_t)0xAA;
		printf("\r\n 指针访问SRAM，写入数据0xAA \r\n");
		
		/* 从SRAM中读取数据 */
		temp = *(uint8_t*)(Bank1_SRAM3_ADDR);
		printf("读取数据：0x%X \r\n",temp);
		
		/* 向SRAM写入16位数据 */
		*(uint16_t*)(Bank1_SRAM3_ADDR+10) = (uint16_t)0xBBBB;
		printf("\r\n 指针访问SRAM，写入数据0xAA \r\n");
		
		/* 从SRAM中读取数据 */
		temp = *(uint16_t*)(Bank1_SRAM3_ADDR+10);
		printf("读取数据：0x%X \r\n",temp);
		
		/* 向SRAM写入32位数据 */
		*(uint32_t*)(Bank1_SRAM3_ADDR+20) = (uint32_t)0xCCCCCCCC;
		printf("\r\n 指针访问SRAM，写入数据0xAA \r\n");
		
		/* 从SRAM中读取数据 */
		temp = *(uint32_t*)(Bank1_SRAM3_ADDR+20);
		printf("读取数据：0x%X \r\n",temp);
	}
	
	
	/* 绝对定位方式访问SRAM，这种方式必须定义成全局变量 */
	{
		testValue = 0xDD;
		printf("\r\n 绝对定位访问SRAM，写入数据0xDD,读出数据0x%X,变量地址为%X \r\n",testValue,(uint32_t )&testValue);
	}
}


/*********************************************END OF FILE**********************/
