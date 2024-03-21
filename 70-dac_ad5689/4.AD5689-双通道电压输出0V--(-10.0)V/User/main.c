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

	float temp1,temp2;
	uint16_t vol = 0;
	
	SysTick_Init();
	
	USART_Config();
	printf("��ӭʹ��Ұ�� F103 AD5689��ѹ���-��ͨ�� ����\r\n");
  printf("����KEY1���������ѹ������KEY2��С�����ѹ\r\n"); 
	KEY_GPIO_Config();
	
	AD5689_Init();
	
	AD5689_WriteUpdata_DACREG(DAC_A,vol);
	AD5689_WriteUpdata_DACREG(DAC_B,0xFF-vol);
	while (1)
	{	
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			vol += 0x200;
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			AD5689_WriteUpdata_DACREG(DAC_B,0xFFFF-vol);
			temp1 = (float)vol*1000/0xFFFF;
			temp2 = (float)(0xFFFF-vol)*1000/0xFFFF;
			printf("Voltage Channel1 = %0.2fV\n",temp1/100);
			printf("Voltage Channel2 = %0.2fV\n",temp2/100);
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			vol -= 0x200;
			AD5689_WriteUpdata_DACREG(DAC_A,vol);
			AD5689_WriteUpdata_DACREG(DAC_B,0xFFFF-vol);
			temp1 = (float)vol*1000/0xFFFF;
			temp2 = (float)(0xFFFF-vol)*1000/0xFFFF;
			printf("Voltage Channel1 = %0.2fV\n",temp1/100);
			printf("Voltage Channel2 = %0.2fV\n",temp2/100);
		}
		SysTick_Delay_Ms(20);
	}
}


/*********************************************END OF FILE**********************/
