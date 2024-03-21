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
#include "bsp_key.h"
#include "bsp_ad5689.h"
#include "bsp_SysTick.h"





/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint16_t vol = 0x7FFF;
	
	SysTick_Init();
	
	USART_Config();
	printf("欢迎使用野火 F103 AD5689双通道输出三角波、锯齿波 例程\r\n");
	KEY_GPIO_Config();
	
	AD5689_Init();
	
	AD5689_WriteUpdata_DACREG(DAC_A,vol);
	
	while (1)
	{	
		for(vol = 0;vol<0xFFFF;vol++)
		{
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			AD5689_WriteUpdata_DACREG(DAC_B,vol);
		}
		
		for(vol = 0xFFFF;vol>0;vol--)
		{
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			AD5689_WriteUpdata_DACREG(DAC_B,0xFFFF- vol);
		}
		SysTick_Delay_Ms(20);
	}
}


/*********************************************END OF FILE**********************/
