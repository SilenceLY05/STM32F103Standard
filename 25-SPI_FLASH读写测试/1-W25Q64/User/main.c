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

typedef enum { FAILED = 0,PASSED = !FAILED} TestStatus;

/* 获取缓冲区的长度 */
#define TxBufferSize1    (countof(TxBuffer1) - 1)
#define RxBufferSize1    (countof(TxBuffer1) - 1)
#define countof(a)       (sizeof(a) / sizeof(*(a)))
#define BufferSize       (countof(Tx_Buffer) - 1)

#define FLASH_WriteAddress     0x00000
#define FLASH_ReadAddress      FLASH_WriteAddress
#define FLASH_SectorToErase    FLASH_WriteAddress

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "感谢选用野火STM32开发板\r\n";
uint8_t Rx_Buffer[BufferSize];

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

//函数原型声明
void Delay(__IO uint32_t nCount);
TestStatus Buffercmp(uint8_t* pBuffer1,uint8_t* pBuffer2,uint16_t BufferLength);




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
	
	printf("\r\n 这是一个8Mbyte串行flash(W25Q64)实验 \r\n");
	printf("\r\n 使用指南者底板时 左上角排针位置 不要将PC0盖有跳帽 防止影响PC0做SPIFLASH片选脚 \r\n");
	
	/* 8M串行FLASH W25Q64初始化 */
	SPI_FLASH_Init();
	
	/* 获取 Flash DeviceID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	Delay(200);
	
	/* 获取SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	printf("\r\n FlashID is 0x%X,\
	Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
	
	/* 检测SPI Flash ID */
	if(FlashID == sFLASH_ID)
	{
		printf("\r\n 检测到串行flash W25Q64 !\r\n");
		
		/* 擦除将要写入的SPI FLASH扇区，FLASH写入前要先擦除 */
		//这里擦除4K 即一个扇区，擦除的最小单位是扇区
		SPI_FLASH_SectorErase(FLASH_SectorToErase);
		
		/* 将发送缓冲区的数据写入FLASH中 */
		//这里写一页，一页的大小为256个字节
		SPI_FLASH_BufferWrite(Tx_Buffer,FLASH_WriteAddress,BufferSize);
		printf("\r\n 写入的数据为：%s \r\t", Tx_Buffer);
		
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(Rx_Buffer,FLASH_ReadAddress,BufferSize);
		printf("\r\n 读出的数据为：%s \r\n", Rx_Buffer);
		
		/* 检查写入的数据与读出的数据是否相等 */
		TransferStatus1 = Buffercmp(Tx_Buffer,Rx_Buffer,BufferSize);
		
		if( PASSED == TransferStatus1 )
		{ 
			LED_GREEN;
			printf("\r\n 8M串行flash(W25Q64)测试成功!\n\r");
		}
		else
		{        
			LED_RED;
			printf("\r\n 8M串行flash(W25Q64)测试失败!\n\r");
		}
	}
	
	else// if (FlashID == sFLASH_ID)
	{ 
		LED_RED;
		printf("\r\n 获取不到 W25Q64 ID!\n\r");
	}
	
	while (1)
	{	
	}
}

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}  
 


/*********************************************END OF FILE**********************/
