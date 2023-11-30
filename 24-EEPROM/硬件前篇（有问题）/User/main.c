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

uint8_t readData [10] = {0};

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	USART_Config();
	
	printf("这是一个IIC通信实验\n");
	
	//初始化I2C
  I2C_EE_Init();
	
	//写入一个字节
	I2C_EE_ByteWrite(0x55, 11);
	
	EEPROM_WaitForWriteEnd();
	
	//读取数据
	I2C_EE_SeqRead(readData,11,1);
	
	printf("\r\n接收到的数据为0x%x\r\n",readData[0]);
	
	
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
