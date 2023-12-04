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
#include "ff.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_spi_flash.h"

FATFS fs;         /* FatFs文件系统对象 */
FIL fnew;         /* 文件对象 */
FRESULT res_flash;   /* 文件操作结果 */
UINT fnum;           /* 文件成功读写测量 */
BYTE ReadBuffer[1024] = {0};  /* 读缓冲区 */
BYTE WriteBuffer[] = "欢迎使用野火STM32开发板 今天是个好日子，新建文件系统测试文件 \r\n";   /* 写缓冲区 */


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 初始化LED */
	LED_GPIO_Config();
	LED_BLUE;
	
	/* 初始化调试串口，一般为串口1 */
	USART_Config();
	printf(" 这是一个SPI FLASH文件系统实验\r\n ");
	printf(" \r\n 使用指南者底板时，左上角排针位置 不要将PC0盖有跳帽 防止影响PC0做SPI FLASH片选引脚 \r\n ");
	
	/* 在外部SPI_Flash挂载文件系统 文件系统挂载时会对SPI设备初始化 */
	/* 初始化函数使用流程如下 f_mount()-> find_volume()->disk_initialize->SPI_Flash_Init() */
	res_flash =f_mount(&fs,"1:",1);
	
	/* 格式化测试 */
	/* 如果没有系统文件就格式化创建文件系统 */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("》FLASH还没有文件系统，即将进行格式化… \r\n");
		/* 格式化 */
		res_flash = f_mkfs("1:",0,0);
		if(res_flash == FR_OK)
		{
			printf("》FLASH已成功格式化文件系统。\r\n");
			/* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"1:" , 1);
			/* 重新挂载 */
  		res_flash = f_mount(&fs,"1:" , 1);
		}
		else
		{
			LED_RED;
			printf("《格式化失败》\r\n");
			while(1);
		}
	}
	else if(res_flash != FR_OK)
	{
		printf("！！外部FLASH挂载文件系统是吧。(%d)\r\n",res_flash);
		printf("！！可能原因：SPI_Flash初始化不成功。\r\n");
		printf("请下载SPI-读写串行FLASH例程测试，如果正常，在该例程f_mount()语句下if语句前临时多添加一句res_flash = FR_NO_FILESYSTEM；让重新直接执行格式化流程\r\n");
		while(1);
	}
	else
	{
		printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}
	
	/* 文件系统测试：写测试 */
	/* 打开文件，每次都以新建的形式打开，属性为可写 */
	printf(" \r\n 即将进行文件写入测试 \r\n ");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if(res_flash == FR_OK)
	{
		printf(" \r\n 》打开/创建FatFs文件系统测试.txt成功，向文件写入数据 \r\n ");
		/* 将指定存储区内容写入到文件内 */
		res_flash = f_write(&fnew , WriteBuffer,sizeof(WriteBuffer) , &fnum);
		if(res_flash == FR_OK)
		{
			printf("》文件写入成功，写入字节数据：%d\n",fnum);
			printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
			
		}
		else
		{
			printf("！！文件写入数据失败：(%d)\n",res_flash);
		}
		/* 不再读写，关闭文件 */
		f_close(&fnew);
	}
	else
	{
		LED_RED;
		printf("！！打开/创建文件失败。\r\n");
	}
	
	/* 文件系统测试：读测试 */
	
	printf(" \r\n 即将进行文件读测试 \r\n ");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		printf(" \r\n 》打开文件成功\r\n ");
		res_flash = f_read(&fnew , ReadBuffer,sizeof(ReadBuffer) , &fnum);
		if(res_flash == FR_OK)
		{
			printf("》文件读取成功，读取字节数据：%d\n",fnum);
			printf("》读取到的文件数据为：\r\n%s\r\n",ReadBuffer);
			
		}
		else
		{
			printf("！！文件读取数据失败：(%d)\n",res_flash);
		}
		
	}
	else
	{
		LED_RED;
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"1:",1);
	
//	FRESULT res;
//	LED_GPIO_Config();
//	
//	LED_BLUE;
//	
//	USART_Config();
//	
//	printf("\r\n 这是一个FatFs移植实验 \r\n");
//	
//	//挂载文件系统
//	res = f_mount(&fs,"1:" , 1);
//	
//	printf("\r\n f_mount res = %d ",res);
//	
//	if(res == FR_NO_FILESYSTEM)
//	{
//		res = f_mkfs("1:",0,0);
//		printf("\r\n f_mkfs res = %d ",res);
//		
//		//格式化后要取消挂载，再重新挂载文件系统
//		res = f_mount(NULL,"1:" , 1);
//		res = f_mount(&fs,"1:" , 1);
//		
//		printf("\r\n second f_mount res = %d ",res);
//	}
//	
//	res = f_open(&fnew, "1:中文",FA_OPEN_ALWAYS | FA_READ|FA_WRITE );
//	printf("\r\n f_open res = %d ",res);
//	
//	if(res == FR_OK)
//	{
//		res = f_write(&fnew, "垃圾福带",sizeof(wData) , &bw);
//		printf("\r\n bw = %d",bw);
//		if(res == FR_OK)
//		{
//			f_lseek(&fnew,0);
//			res = f_read(&fnew, rData,f_size(&fnew) ,&br);
//			if(res == FR_OK)
//				printf("\r\n文件内容：%s br = %d",rData,br);
//			
//		}
//		f_close(&fnew);
//	}

	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
