#include "bsp_AdvanceTim.h"

// 中断优先级配置
static void ADVANCE_TIM_NCIV_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	//设置中断组为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	//设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ;
	
	//设置主优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	//设置抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
}


static void ADVANCE_TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	
	// 开启定时器时钟,即内部时钟CK_INT=72M
	ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);
	
	//时钟分频因子 ，基本定时器没有
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	//计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_Period;
	
	//时钟预分频数为
	TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_Prescaler;
	
	//重复计数器的值，基本定时器没有
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//初始化定时器
	TIM_TimeBaseInit(ADVANCE_TIM,&TIM_TimeBaseStructure);
	
	//清除计数器中断标志位
	TIM_ClearFlag(ADVANCE_TIM,TIM_FLAG_Update);
	
	//开启计数器中断
	TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);
	
	//使能计数器
	TIM_Cmd(ADVANCE_TIM,ENABLE);
	
}

void ADVANCE_TIM_Init(void)
{
	ADVANCE_TIM_Mode_Config();
	ADVANCE_TIM_NCIV_Config();
}
