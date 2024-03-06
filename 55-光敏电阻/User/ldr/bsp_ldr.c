#include "bsp_ldr.h"
#include "bsp_led.h"

/* ���ù������������ */
void LDR_GPIO_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	
	/* �����˿ڵ�ʱ�� */
	LDR_SCK_APBxClock_FUN(LDR_GPIO_CLK,ENABLE);
	
	/* ѡ����������� */
	GPIO_InitStructure.GPIO_Pin = LDR_GPIO_PIN;
	
	/* ���ù������������Ϊ�������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(LDR_GPIO_PORT,&GPIO_InitStructure);
}

/* ����������� �����������״̬ GPIOx��������ABCDE GPIO_Pin����ȡ�Ķ˿�λ */
uint8_t LDR_Test(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 1)
	{
		return LDR_OFF;  //�޹�
	}
	else
	{
		return LDR_ON;  //�й�
	}
}

