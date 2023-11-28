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
#include "bsp_dma_mtm.h"

extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern uint32_t aDST_Buffer[BUFFER_SIZE];
#define SOFT_DELAY Delaly(0x0FFFFF);
void Delay(__IO uint32_t nCount);


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint8_t status = 0;
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	LED_YELLOW;
	Delay(0xFFFFFF);
  MtM_DMA_Config();
	
	/*��⴫���Ƿ����*/
	while(DMA_GetFlagStatus(MTM_DMA_FLAG_TC) == RESET);
	
	status = Buffercmp(aSRC_Const_Buffer,aDST_Buffer,BUFFER_SIZE);
	
	if(status == 0)
	{
		LED_RED;
	}
	else
	{
		LED_GREEN;
	}


	while (1)
	{
			
	}
}

void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0;nCount--);
}


/*********************************************END OF FILE**********************/