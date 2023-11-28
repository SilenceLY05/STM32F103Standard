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



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint8_t a[10] = {1,2,3,4,5,6,7,8,9,10};
	USART_Config();
	
	USART_SendByte(DEBUG_USARTx,'A');
	USART_SendHalfWord(DEBUG_USARTx,0xff56);
	USART_SendArray(DEBUG_USARTx,a,10);
	USART_SendString(DEBUG_USARTx,"垃圾福带\n");
	
	printf("垃圾AJ\n");
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
