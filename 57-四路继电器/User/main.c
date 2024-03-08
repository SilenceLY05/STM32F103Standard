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
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_relay.h"




void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	RELAY_GPIO_Config();
	
	KEY_GPIO_Config();

	for(;;)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			RELAY1_TOGGLE;
			RELAY2_TOGGLE;
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			RELAY3_TOGGLE;
			RELAY4_TOGGLE;
		}
	}
}


/*********************************************END OF FILE**********************/
