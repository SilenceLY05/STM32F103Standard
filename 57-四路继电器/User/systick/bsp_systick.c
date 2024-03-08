#include "bsp_systick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay;


/* 启动系统滴答定时器 SysTick */
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock /100000))
	{
		while(1);
	}
}

/* us延时 10us为一个单位 */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);
}

/* 获取节拍程序 */
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

#if 0
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{ 
	//判断 tick 的值是否大于 2^24，如果大于，则不符合规则
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            
  
  //初始化reload寄存器的值 	
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
  
	//配置中断优先级 配置为15，默认为最低的优先级
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M0 System Interrupts */
  
	//配置counter计数器的值
	SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  
	//配置systick的时钟为72M   使能中断  使能systick
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
  return (0);                                                  /* Function successful */
}
#endif


// couter 减1的时间 等于 1/systick_clk
// 当counter 从 reload 的值减小到0的时候，为一个循环，如果开启了中断则执行中断服务程序，
// 同时 CTRL 的 countflag 位会置1
// 这一个循环的时间为 reload * (1/systick_clk)
void SysTick_Delay_Us(__IO uint32_t us)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000000);
	
	for(i = 0;i<us;i++)
	{
		while( !((SysTick->CTRL) & (1<<16)) );
	}
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock/1000);
	
	for(i = 0;i<ms;i++)
	{
		while( !((SysTick->CTRL) & (1<<16)) );
	}
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

