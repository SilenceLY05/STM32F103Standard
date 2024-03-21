/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
  *          常用的有 1us 10us 1ms 中断。     
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103-霸道 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
	
#include "bsp_SysTick.h"


static __IO uint32_t TimingDelay;

/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init( void )
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if ( SysTick_Config(SystemCoreClock / 100000) )	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while (1);
	}
  
  // 关闭滴答定时器  
//	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
  // 使能滴答定时器  10ms中断一次
	//SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
  
}

/* us延时程序 10us为一个单位 */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	//使能滴答定时器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimingDelay != 0);
}

///* ms延时程序 10ms为一个单位 */
//void Delay_ms(__IO uint32_t nTime)
//{
//	while(nTime--)
//	{
//		Delay_us(1000);
//	}
//}

/* 获取节拍器 */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

#if 0
//这个固件库函数 在core_cm3.h中
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{
	//reload寄存器为24bit，最大值为2^24
	if(ticks > SysTick_LOAD_RELOAD_Msk) return (1);
	
	//配置 reload 寄存器的初始值
	SysTick->LOAD = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
	
	//配置中断优先级为1<<4-1 = 15，优先级为最低
	NVIC_SetPriority(SysTick_IRQn,(1<<__NVIC_PRIO_BITS) - 1);
	
	//配置 counter 计数器的值
	SysTick->VAL = 0;
	
	//配置systick 的时钟为 72M
	//使能中断
	//使能systick
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	return (0);
	
}
#endif



//counter减1的时间 等于 1/Systick_Clk
//当counter从reload的值减小到0的时候，为一个循环，如果开启了中断则执行中断服务程序
//同时CTRL的countflag位会置1
//这一个循环的时间为reload * (1/systick_clk)
void SysTick_Delay_Us(__IO uint32_t us)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000000);
	
	for(i=0;i<us;i++)
	{
		//当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		while(!(SysTick->CTRL)&(1<<16));
	}
	//关闭Systick定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
}


void SysTick_Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000);
	
	for(i=0;i<ms;i++)
	{
		//当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		while(!(SysTick->CTRL)&(1<<16));
	}
	//关闭Systick定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
}






/*********************************************END OF FILE**********************/
