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
#include "bsp_usart_dma.h"


extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO uint32_t nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint16_t i ;
	
	USART_Config();
	
	USARTx_DMA_Config();
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
	
	//printf("\r\n USART1 DMA TX测试 \r\n");
	
	for(i = 0; i<SENDBUFF_SIZE;i++)
	{
		SendBuff[i] = 'P';
	}
	
	/*为演示DMA持续运行而CPU还能处理其它事情，持续使用DMA发送数据，量非常大，
  *长时间运行可能会导致电脑端串口调试助手会卡死，鼠标乱飞的情况，
  *或把DMA配置中的循环模式改为单次模式*/		
	
	/* USART1向DMA发出TX请求 */
	USART_DMACmd(DEBUG_USARTx,USART_DMAReq_Tx,ENABLE);
	
	/* 此时CPU是空闲的，可以干其他事情 */

	while (1)
	{
		LED1_TOGGLE;
		Delay(0xFFFFF);
	}
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
