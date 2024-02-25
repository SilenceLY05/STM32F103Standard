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
#include "bsp_led.h"
#include "bsp_usart.h"
#include <stdlib.h>
#include "bsp_sram.h"


void Delay(__IO uint32_t nCount); 

//关键字 __attribute__((at(地址)))   __attribute__((section("区域名")))
//设置变量定义到“EXRAM”节区的宏
#define __EXRAM __attribute__((section("EXRAM")))


//定义变量到“指定的存储空间”
uint32_t testValue __EXRAM = 7;
//上述语句等效为：uint32_t testValue __attribute__((section("EXRAM"))) = 7;

////定义变量到SRAM
uint32_t testValue2 = 7;

//定义数组到“指定的存储空间”
uint8_t testGroup[3] __EXRAM = {1,2,3};
////定义数组到SRAM
uint8_t testGroup2[3] = {1,2,3};



/* 本实验中的sct配置，若使用外部存储器时，堆区工作可能不正常，使用malloc无法得到正常的地址，不推荐在实际工程使用 */
/* 另一种推荐的配置在参考教程中的说明 */

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint32_t inertestValue = 10;
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	

	/* 初始化USART */
	USART_Config();
	
	printf("\r\n SCT文件应用——优先使用内部SRAM并把堆分配指定空间 实验 \r\n");
	
	printf("\r\n 使用“uint32_t inertestValue = 10;”语句定义的局部变量  \r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&inertestValue,inertestValue);
	
	printf("\r\n 使用“uint32_t testValue __EXRAM= 7;”语句定义的局部变量  \r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue,testValue);
	
	printf("\r\n 使用“uint32_t testValue2 = 7;”语句定义的局部变量  \r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue2,testValue2);
	
	printf("\r\n 使用“uint32_t testGroup[3] __EXRAM = {1,2,3};”语句定义的局部变量  \r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d,%d,%d\r\n",(uint32_t)&testGroup,testGroup[0],testGroup[1],testGroup[2]);
	
	printf("\r\n 使用“uint32_t testGroup2[3] = {0,1,2};”语句定义的局部变量  \r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d,%d,%d\r\n",(uint32_t)&testGroup2,testGroup2[0],testGroup2[1],testGroup2[2]);
	
	
	/* 使用malloc从外部SRAM中分配空间 */
	uint32_t * pointer = (uint32_t *)malloc(sizeof(uint32_t)*3);
	if(pointer != NULL)
	{
		*(pointer) = 1;
		*(++pointer) = 2;
		*(++pointer) = 3;
		
		printf("\r\n 使用“uint32_t * pointer = (uint32_t *)malloc(sizeof(uint32_t)*3);”动态分配的变量 \r\n");
		printf("\r\n 定义后的操作为： \r\n*(pointer++) = 1;\r\n*(pointer++) = 2;\r\n*pointer = 3;\r\n");
		printf("结果：操作后它的地址为：0x%x，查看操作变量：\r\n",(uint32_t)pointer);
		printf("*(pointer--) = %d,\r\n",*(pointer--));
		printf("*(pointer--) = %d,\r\n",*(pointer--));
		printf("*(pointer) = %d,\r\n",*(pointer));
		
		free(pointer);
	}
	else
	{
		printf("\r\n 使用malloc动态分配变量出错！！！ \r\n");
	}
	
	LED_BLUE;
	


	while (1)
	{
	
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
