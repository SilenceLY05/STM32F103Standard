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
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_ad7192.h"
#include "bsp_ad7192_test.h"
#include "bsp_SysTick.h"




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	

	USART_Config();
	
	AD7192_Config();
	
	/* �����λAD7192 */
	AD7192_SoftWareReset();
	
	SysTick_Delay_Ms(10);
	
	ReadFromAD7192ViaSPI(REG_ID,1,AD7192Registers,REG_ID);
	
	if((AD7192Registers[REG_ID]&0x0F) != 0)
	{
		printf("\r\n AD7192��ʼ��ʧ�� �������ӣ� \r\n");
		while(1);
	}
	printf("\r\n AD7192 ID :0x%x \r\n",AD7192Registers[REG_ID]);
	
	electronic_scale_test();

	while (1)
	{

	}
}


/*********************************************END OF FILE**********************/
