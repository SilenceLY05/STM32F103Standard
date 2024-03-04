#include "bsp_systick.h"

static __IO uint32_t TimingDelay;


/* 启动系统滴答定时器SysTick */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000   1ms中断一次  SystemFrequency / 100000   10us中断一次  SystemFrequency / 1000000   1us中断一次 */
	//if(SysTick_Config( SystemFrequency / 100000 )) ST3.0库版本
	if(SysTick_Config(SystemCoreClock / 1000000))// ST3.5.0库版本SystemCoreClock/10不能超过16777216
	{
		/* 捕获错误 */
		while(1);
	}
	
	/* 关闭滴答定时器 */
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
	//使能滴答定时器， 1Ms中断一次
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* us延时程序,10us为一个单位  nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimingDelay != 0);
}

/* 获取节拍程序 在 SysTick 中断函数 SysTick_Handler()调用 */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}
