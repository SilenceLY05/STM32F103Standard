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
#include "bsp_led.h"
#include "bsp_adc.h"

extern  __IO uint32_t ADC_ConvertedValue[NOFCHANEL];


//局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal[NOFCHANEL*2];

void Delay(__IO uint32_t nCount)
{
	for(; nCount!=0;nCount--);
}

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint16_t temp0 = 0, temp1 = 0;
	USART_Config();
	
	ADCx_Init();
	

	
	printf("\r\n ----- 这是一个双ADC规则同步采集实验 ----- \r\n");
	
	while (1)
	{	
		//取出ADC1数据寄存器的高16位，这个是ADC2的转换数据
		temp0 = (ADC_ConvertedValue[0]&0xFFFF0000) >> 16;
		
		//取出ADC1数据寄存器的高16位，这个是ADC1的转换数据
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
