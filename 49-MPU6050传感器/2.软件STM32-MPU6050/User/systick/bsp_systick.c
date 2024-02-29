#include "bsp_systick.h"


/* ����ϵͳ�δ�ʱ��SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000   1ms�ж�һ��  SystemFrequency / 100000   10us�ж�һ��  SystemFrequency / 1000000   1us�ж�һ�� */
	//if(SysTick_Config( SystemFrequency / 100000 )) ST3.0��汾
	if(SysTick_Config(SystemCoreClock / 1000))// ST3.5.0��汾SystemCoreClock/10���ܳ���16777216
	{
		/* ������� */
		while(1);
	}
	
	/* �رյδ�ʱ�� */
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	//ʹ�ܵδ�ʱ���� 1Ms�ж�һ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
