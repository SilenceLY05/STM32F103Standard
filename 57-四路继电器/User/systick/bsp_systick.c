#include "bsp_systick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay;


/* ����ϵͳ�δ�ʱ�� SysTick */
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock /100000))
	{
		while(1);
	}
}

/* us��ʱ 10usΪһ����λ */
void Delay_us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);
}

/* ��ȡ���ĳ��� */
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
	//�ж� tick ��ֵ�Ƿ���� 2^24��������ڣ��򲻷��Ϲ���
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            
  
  //��ʼ��reload�Ĵ�����ֵ 	
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
  
	//�����ж����ȼ� ����Ϊ15��Ĭ��Ϊ��͵����ȼ�
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M0 System Interrupts */
  
	//����counter��������ֵ
	SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  
	//����systick��ʱ��Ϊ72M   ʹ���ж�  ʹ��systick
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
  return (0);                                                  /* Function successful */
}
#endif


// couter ��1��ʱ�� ���� 1/systick_clk
// ��counter �� reload ��ֵ��С��0��ʱ��Ϊһ��ѭ��������������ж���ִ���жϷ������
// ͬʱ CTRL �� countflag λ����1
// ��һ��ѭ����ʱ��Ϊ reload * (1/systick_clk)
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

