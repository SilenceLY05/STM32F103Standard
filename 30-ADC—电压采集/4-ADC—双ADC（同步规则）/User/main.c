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
#include "bsp_led.h"
#include "bsp_adc.h"

extern  __IO uint32_t ADC_ConvertedValue[NOFCHANEL];


//�ֲ����������ڱ���ת�������ĵ�ѹֵ
float ADC_ConvertedValueLocal[NOFCHANEL*2];

void Delay(__IO uint32_t nCount)
{
	for(; nCount!=0;nCount--);
}

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint16_t temp0 = 0, temp1 = 0;
	USART_Config();
	
	ADCx_Init();
	

	
	printf("\r\n ----- ����һ��˫ADC����ͬ���ɼ�ʵ�� ----- \r\n");
	
	while (1)
	{	
		//ȡ��ADC1���ݼĴ����ĸ�16λ�������ADC2��ת������
		temp0 = (ADC_ConvertedValue[0]&0xFFFF0000) >> 16;
		
		//ȡ��ADC1���ݼĴ����ĸ�16λ�������ADC1��ת������
		temp1 = (ADC_ConvertedValue[0]&0xFFFF);
		
		ADC_ConvertedValueLocal[0] = (float) temp0/4096*3.3;
		ADC_ConvertedValueLocal[1] = (float) temp1/4096*3.3;
		
		printf("\r\n ADCx_1 AD value = 0x%04X \r\n",ADC_ConvertedValue[0]);
		
		printf("\r\n ADCx_1 AD value = %f V \r\n", ADC_ConvertedValueLocal[0]);
		
		//printf("\r\n ADCx_2 AD value = 0x%04X \r\n",ADC_ConvertedValue[1]);
		
		printf("\r\n ADCx_2 AD value = %f V \r\n", ADC_ConvertedValueLocal[1]);
		printf("\r\n\r\n");
		
		Delay(0xFFFFEE);
	}
}




/*********************************************END OF FILE**********************/
