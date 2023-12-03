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
#include "bsp_spi_flash.h"

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;


//函数原型声明
void Delay(__IO uint32_t nCount);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint8_t cal_flag = 0;
	uint8_t k;
	
	/* 存储小数和整数的数组，各7个 */
	long double double_buffer[7] = {0};
	int int_buffer[7] = {0};
	
	LED_GPIO_Config();
	LED_BLUE;
	
	/* 配置串口1为：115200 8-N-1 */
	USART_Config();
	
	printf("\r\n 这是一个FLASH读写小数和长整数实验 \r\n");
	printf("\r\n 使用指南者底板时 左上角排针位置 不要将PC0盖有跳帽 防止影响PC0做SPIFLASH片选脚 \r\n");
	
	/* 16M串行FLASH W25Q128初始化 */
	SPI_FLASH_Init();
	
	/* 获取 Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	
	Delay( 200 );
	
	/* 获取SPI FLASH ID */
	FlashID = SPI_FLASH_ReadID();
	
	printf("\r\n Flash ID is 0x%X , manufacturer Device ID is 0x%X  \r\n",FlashID,DeviceID);
	
	/* 检测SPI Flash ID */
	if(FlashID == sFLASH_ID)
	{
		LED_GREEN;
		printf("\r\n 检测到SPI FLASH W25Q128 \r\n");
		
		/* 读取数据标志位 */
		SPI_FLASH_BufferRead(&cal_flag,SPI_FLASH_PageSize*0,1);
		
		if(cal_flag == 0xCD)/*若标志等于0xcd，表示之前已有写入数据*/
		{
			printf("\r\n 检测到数据标志 \r\n");
			
			/* 读取小数数据 */
			SPI_FLASH_BufferRead((void*)double_buffer,SPI_FLASH_PageSize*1,sizeof(double_buffer));
			
			/* 读取整数数据 */
			SPI_FLASH_BufferRead((void*)int_buffer,SPI_FLASH_PageSize*2,sizeof(int_buffer));
			
			printf("\r\n 从芯片读到数据： \r\n");
			for(k = 0 ; k < 7 ; k++)
			{
				printf("小数 rx = %LF \r\n",double_buffer[k]);
				printf("整数 rx = %d \r\n",int_buffer[k]);
			}
		}
		else
		{
			printf("\r\n 没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验 \r\n");
			cal_flag = 0xCD;
			
			/* 擦除扇区 */
			SPI_FLASH_SectorErase(0);
			
			/* 写入标志到第0页 */
			SPI_FLASH_BufferWrite(&cal_flag,SPI_FLASH_PageSize*0,1);
			
			/* 生成要写入的数据 */
			for( k=0 ; k <7 ; k++)
			{
				double_buffer[k] = k+0.1;
				int_buffer[k] = k*500+1;
			}
			/* 写入小数数据到第一页 */
			SPI_FLASH_BufferWrite((void*)double_buffer,SPI_FLASH_PageSize*1,sizeof(double_buffer));
			
			/* 写入整数数据到第二页 */
			SPI_FLASH_BufferWrite((void*)int_buffer,SPI_FLASH_PageSize*2,sizeof(int_buffer));
			
			printf("\r\n 向芯片写入数据 \r\n");
			for(k = 0 ; k < 7 ; k++)
			{
				printf("小数 rx = %LF \r\n",double_buffer[k]);
				printf("整数 rx = %d \r\n",int_buffer[k]);
			}
			printf("\r\n 请复位开发板，以读取数据进行检测 \r\n");
		}
	}
	else
	{
		LED_RED;
		printf("\r\n 获取不到W25Q128 \r\n");
	}
	SPI_FLASH_PowerDown(); 
	while (1);
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}  
 


/*********************************************END OF FILE**********************/
