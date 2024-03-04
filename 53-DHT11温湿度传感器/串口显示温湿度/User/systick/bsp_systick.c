#include "bsp_systick.h"

static __IO uint32_t TimingDelay;


/* ����ϵͳ�δ�ʱ��SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000   1ms�ж�һ��  SystemFrequency / 100000   10us�ж�һ��  SystemFrequency / 1000000   1us�ж�һ�� */
	//if(SysTick_Config( SystemFrequency / 100000 )) ST3.0��汾
	if(SysTick_Config(SystemCoreClock / 1000000))// ST3.5.0��汾SystemCoreClock/10���ܳ���16777216
	{
		/* ������� */
		while(1);
	}
	
	/* �رյδ�ʱ�� */
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	//ʹ�ܵδ�ʱ���� 1Ms�ж�һ��
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* us��ʱ����,10usΪһ����λ  nTime: Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimingDelay != 0);
}

/* ��ȡ���ĳ��� �� SysTick �жϺ��� SysTick_Handler()���� */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}
