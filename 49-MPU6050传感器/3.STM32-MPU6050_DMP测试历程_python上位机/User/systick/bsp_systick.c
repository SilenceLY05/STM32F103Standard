#include "bsp_systick.h"

volatile uint32_t g_ul_ms_ticks = 0;
static volatile uint32_t TimingDelay = 0;

/* 启动系统滴答定时器SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000   1ms中断一次  SystemFrequency / 100000   10us中断一次  SystemFrequency / 1000000   1us中断一次 */
	//if(SysTick_Config( SystemFrequency / 100000 )) ST3.0库版本
	if(SysTick_Config(SystemCoreClock / 1000))// ST3.5.0库版本SystemCoreClock/10不能超过16777216
	{
		/* 捕获错误 */
		while(1);
	}
	
	/* 关闭滴答定时器 */
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	//使能滴答定时器， 1Ms中断一次
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void mdelay(unsigned long nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);
}

int get_tick_count(unsigned long *count)
{
	count[0] = g_ul_ms_ticks;
	return 0 ;
}

void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
		TimingDelay--;
}

void TimeStamp_Increment(void)
{
	g_ul_ms_ticks++;
}
