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
#include "bsp_exti.h"




/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;                             /* FatFs文件系统对象 */

char song_pt =0;
uint8_t song_list[][20] = {"0:TestFile_1.mp3", 	//歌曲的数量会自动计算出来，用户只需要在此添加歌曲名即可
													 "0:TestFile_2.mp3"};

char song_number_max = 0;
													 
/* 计算歌曲数量 */
void song_list_init(void)
{
	song_number_max = sizeof(song_list) / sizeof(song_list[0]);
}

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
	
	/* 串口初始化 */
	USART_Config();
	
	//在外部SPI FLASH挂在文件系统，文件系统挂载时会对SPI设备初始化
	f_mount(&fs,"0:",1);
	
	EXTI_Key_Config();
	
	VS1053_Init();
	printf("vs1053:%4X\n",VS_RamTest());
	
	Delay_ms(100);
	VS_SineTest();
	VS_HardReset();
	VS_SoftReset();
	
  song_list_init();

	/*  完成操作 停机*/
	while (1)
	{
		// 测试歌曲放在外部FLASH W25Q64中
		VS1053_PlayerSong(song_list[song_pt]);
		printf("MusicPlay End\n");
	}
}


/*********************************************END OF FILE**********************/
