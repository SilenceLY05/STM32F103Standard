/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_ad5689.h"
#include "bsp_SysTick.h"





/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	

	float temp;
	uint16_t vol = 0x7FFF;
	
	SysTick_Init();
	
	USART_Config();
	
	KEY_GPIO_Config();
	
	AD5689_Init();
	
	AD5689_WriteUpdata_DACREG(DAC_A,vol);
	
	while (1)
	{	
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			vol += 0x200;
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			temp = (float)vol*2000/0xFFFF;
			printf("Voltage = %0.2fV\n",temp/100-10);
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			vol -= 0x200;
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			temp = (float)vol*2000/0xFFFF;
			printf("Voltage = %0.2fV\n",temp/100-10);
		}
		SysTick_Delay_Ms(20);
	}
}


/*********************************************END OF FILE**********************/
