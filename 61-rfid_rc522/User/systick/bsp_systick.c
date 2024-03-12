#include "bsp_systick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay = 0;


/* 启动系统滴答定时器 SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if(SysTick_Config(SystemCoreClock /1000000))
	{
		while(1);
	}
	/* 关闭滴答定时器 */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* us延时 1us为一个单位 */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	/* 使能滴答定时器 */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);
	
	/* 关闭滴答定时器 */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* 获取节拍程序 */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}



