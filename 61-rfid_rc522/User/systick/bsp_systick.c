#include "bsp_systick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay = 0;


/* ����ϵͳ�δ�ʱ�� SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if(SysTick_Config(SystemCoreClock /1000000))
	{
		while(1);
	}
	/* �رյδ�ʱ�� */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* us��ʱ 1usΪһ����λ */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	/* ʹ�ܵδ�ʱ�� */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);
	
	/* �رյδ�ʱ�� */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* ��ȡ���ĳ��� */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}



