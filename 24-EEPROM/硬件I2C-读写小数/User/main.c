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

uint8_t cal_flag = 0;
uint8_t k;

/* 存储小数和整数的数组，各7个 */
long double double_buffer[7] = {0};
int int_buffer[7] = {0};
char str1_buffer[7] = {'A','B','C','D','E','F','G'};
char str2_buffer[7] = {0};

#define DOUBLE_ADDR    10
#define LONGINT_ADDR   70
#define STRING_ADDR    100


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	LED_GPIO_Config();
	
	LED_BLUE;
	
	USART_Config();
	
	printf("这是一个I2C外设(AT24C02)读写小数和长整数测试例程\r\n");
	
	//初始化I2C
  I2C_EE_Init();
	
	I2C_EE_BufferRead(&cal_flag,0,1);
	
	if(cal_flag != 0xCD)
	{
		printf("\r\n 没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验 \r\n");
		cal_flag = 0xCD;
		
		/* 写入标志到0地址 */
		I2C_EE_BufferWrite(&cal_flag,0,1);
		
		/* 生成要写入的数据 */
		for( k = 0; k<7;k++)
		{
			double_buffer[k] = k+0.1;
			int_buffer[k] = k*500+1;
			str2_buffer[k] = str1_buffer[k];
		}
			/* 写入小数数据到地址10 */
		I2C_EE_BufferWrite((void*)double_buffer,DOUBLE_ADDR,sizeof(double_buffer));
		
		/* 写入小数数据到地址60 */
		I2C_EE_BufferWrite((void*)int_buffer,LONGINT_ADDR,sizeof(int_buffer));
		
		/* 写入小数数据到地址60 */
		I2C_EE_BufferWrite((void*)str2_buffer,LONGINT_ADDR,sizeof(str2_buffer));
		
		printf("向芯片写入数据");
			
		for(k = 0;k<7;k++)
		{
			printf("小数tx = %LF\r\n",double_buffer[k]);
      printf("整数tx = %d\r\n",int_buffer[k]);
			printf("字符tx = %c\r\n",str2_buffer[k]);
		}
		printf("\r\n请复位开发板，以读取数据进行检验\r\n");   
	
	}
	else
    {      
      	 printf("\r\n检测到数据标志\r\n");

				/*读取小数数据*/
        I2C_EE_BufferRead((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
				
				/*读取整数数据*/
        I2C_EE_BufferRead((void*)int_buffer, LONGINT_ADDR, sizeof(int_buffer));
			/*读取整数数据*/
        I2C_EE_BufferRead((void*)str2_buffer, LONGINT_ADDR, sizeof(str2_buffer));
	
			
				printf("\r\n从芯片读到数据：\r\n");			
        for( k=0; k<7; k++ )
				{
					printf("小数 rx = %LF \r\n",double_buffer[k]);
					printf("整数 rx = %d \r\n",int_buffer[k]);	
					printf("字符tx = %c\r\n",str2_buffer[k]);					
				}
      
    }   
	
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
