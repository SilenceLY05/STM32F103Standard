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
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_spi_nrf.h"

uint8_t status;
uint8_t status2;
uint8_t txBuffer[32];
uint8_t rxBuffer[32];

int i = 0;

void Self_Test(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 初始化NRF1 */
	SPI_NRF_Config();
	/* 初始化NRF2 */
	SPI_NRF2_Config();
	/* 串口初始化 */
	USART_Config();
	/* 按键初始化 */
	KEY_GPIO_Config();
	
	printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 主机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

	Self_Test();
	
}

/* NRF模块测试函数，NRF1和NRF2之间循环发送数据 */
void Self_Test(void)
{
	/* 检测NRF1模块和MCU连接 */
	status = NRF_Check();
	
	/* 判断连接状态 */
	if(status == SUCCESS)
		printf("\r\n NRF1与MCU连接成功！ 按K1发送数据 \r\n");
	else
		printf("\r\n NRF1与MCU连接失败！请重新检查连线 \r\n");
	
	/* 检测NRF2模块和MCU连接 */
	status = NRF2_Check();
	
	/* 判断连接状态 */
	if(status == SUCCESS)
		printf("\r\n NRF2与MCU连接成功！ 按K2发送数据 \r\n");
	else
		printf("\r\n NRF2与MCU连接失败！请重新检查连线 \r\n");
	
	NRF_RX_Mode();       //NRF1进入接收模式
	NRF2_RX_Mode();      //NRF2进入接收模式
	
	for(i=0;i<32;i++)
	{
		txBuffer[i] = i;
	}
	
	while(1)
	{
		/* 等地NRF1接收数据 */
		status = NRF_RX_Data(rxBuffer);
		
		/* 判断接收状态 */
		if(status == RX_DR)
		{
			for(i=0;i<32;i++)
			{
				printf("\r\n NRF1接收的数据为：%d \r\n",rxBuffer[i]);
			}
			
			printf("\r\n 按K1键使用NRF1发送数据 \r\n");
			printf("\r\n 按K2键使用NRF2发送数据 \r\n");
		}
		
		/* 使用NRF1发送数据 */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)  // 按键按下，开始送数据
		{
			/* 发送数据 */
			NRF_TX_Mode();
			
			status = NRF_TX_Data(txBuffer);
			
			/* 发送数据状态 */
			if(status == TX_DS)
			{
				printf("\r\n 发送数据成功 \r\n");
			}
			else
			{
				printf("\r\n 发送数据失败   %d \r\n",status);
			}
			
			printf("\r\n 进入接收模式 \r\n");
			NRF_RX_Mode();
		}
		
		/* 等待NRF2接收数据 */
		status2 = NRF2_RX_Data(rxBuffer);
		
		/* 判断接收状态 */
		if(status2 == RX_DR)
		{
			for(i=0;i<32;i++)
			{
				printf("\r\n NRF2接收的数据为：%d \r\n",rxBuffer[i]);
			}
			
			printf("\r\n 按K1键使用NRF1发送数据 \r\n");
			printf("\r\n 按K2键使用NRF2发送数据 \r\n");
		}
		
		/* NRF2发送数据 */  
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)   // 按键 2 按下，开始送数据
		{
			/* 发送数据 */
			NRF2_TX_Mode();
			
			status2 = NRF2_TX_Data(txBuffer);
			
			/* 发送数据状态 */
			if(status2 == TX_DS)
			{
				printf("\r\n NRF2发送数据成功 \r\n");
			}
			else
			{
				printf("\r\n NRF2发送数据失败   %d \r\n",status2);
			}
			
			printf("\r\n NRF2 进入接收模式 \r\n");
			NRF2_RX_Mode();
		}
			
		
	}
}


/*********************************************END OF FILE**********************/
