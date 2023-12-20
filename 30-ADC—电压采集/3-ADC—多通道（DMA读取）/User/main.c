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

extern  __IO uint16_t ADC_ConvertedValue[NOFCHANEL];


//局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal[NOFCHANEL];

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
	USART_Config();
	
	ADCx_Init();
	

	
	printf("\r\n ----- 这是一个ADC单通道中断读取实验 ----- \r\n");
	
	while (1)
	{	
		ADC_ConvertedValueLocal[0] = (float) ADC_ConvertedValue[0]/4096*3.3;
		ADC_ConvertedValueLocal[1] = (float) ADC_ConvertedValue[1]/4096*3.3;
		ADC_ConvertedValueLocal[2] = (float) ADC_ConvertedValue[2]/4096*3.3;
		ADC_ConvertedValueLocal[3] = (float) ADC_ConvertedValue[3]/4096*3.3;
		ADC_ConvertedValueLocal[4] = (float) ADC_ConvertedValue[4]/4096*3.3;
		ADC_ConvertedValueLocal[5] = (float) ADC_ConvertedValue[5]/4096*3.3;
		
		printf("\r\n CH0 value AD value = 0x%04X \r\n",ADC_ConvertedValue[0]);
		printf("\r\n CH0 value AD value = %f V \r\n", ADC_ConvertedValueLocal[0]);
		
		printf("\r\n CH1 value AD value = 0x%04X \r\n",ADC_ConvertedValue[1]);
		printf("\r\n CH1 value AD value = %f V \r\n", ADC_ConvertedValueLocal[1]);
		
		printf("\r\n CH2 value AD value = 0x%04X \r\n",ADC_ConvertedValue[2]);
		printf("\r\n CH2 value AD value = %f V \r\n", ADC_ConvertedValueLocal[2]);
		
		printf("\r\n CH3 value AD value = 0x%04X \r\n",ADC_ConvertedValue[3]);
		printf("\r\n CH3 value AD value = %f V \r\n", ADC_ConvertedValueLocal[3]);
		
		printf("\r\n CH4 value AD value = 0x%04X \r\n",ADC_ConvertedValue[4]);
		printf("\r\n CH4 value AD value = %f V \r\n", ADC_ConvertedValueLocal[4]);
		
		printf("\r\n CH5 value AD value = 0x%04X \r\n",ADC_ConvertedValue[5]);
		printf("\r\n CH5 value AD value = %f V \r\n", ADC_ConvertedValueLocal[5]);
		
		printf("\r\n\r\n");
		
		Delay(0xFFFFEE);
	}
}




/*********************************************END OF FILE**********************/
