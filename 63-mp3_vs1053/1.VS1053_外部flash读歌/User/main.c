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
#include "bsp_vs1053.h"
#include "ff.h"
#include "bsp_spi_flash.h"
#include "bsp_systick.h"



/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;                             /* FatFs文件系统对象 */



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	SysTick_Init();
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	LED_BLUE;
	
	/* 串口初始化 */
	USART_Config();
	
	//在外部SPI FLASH挂在文件系统，文件系统挂载时会对SPI设备初始化
	f_mount(&fs,"1:",1);
	
	VS1053_Init();
	printf("vs1053:%4X\n",VS_RamTest());
	
	Delay_ms(100);
	VS_SineTest();
	VS_HardReset();
	VS_SoftReset();
	
  

	/*  完成操作 停机*/
	while (1)
	{
		// 测试歌曲放在外部FLASH W25Q64中
		VS1053_PlayerSong("1:TestFile.mp3");
		printf("MusicPlay End\n");
	}
}


/*********************************************END OF FILE**********************/
