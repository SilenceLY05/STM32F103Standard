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
	uint16_t vol = 0x7FFF;
	
	SysTick_Init();
	
	USART_Config();
	printf("��ӭʹ��Ұ�� F103 AD5689˫ͨ��������ǲ�����ݲ� ����\r\n");
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
