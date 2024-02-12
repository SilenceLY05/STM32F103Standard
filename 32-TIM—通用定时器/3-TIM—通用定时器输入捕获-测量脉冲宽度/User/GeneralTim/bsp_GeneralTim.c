#include "bsp_GeneralTim.h"

//定时器输入捕获用户自定义变量结构体定义
TIM_ICUserValueTypeDef  TIM_ICUserValueStructure = {0,0,0,0};


//中断优先级配置
static void GENERAL_TIM_NVIC_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
		 // 设置中断组为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQ;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	// 设置主优先级为 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	// 设置抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_Init(&NVIC_InitStructure);
}

//通用定时器3 输出比较4通道GPIO初始化
static void GENERAL_TIM_GPIO_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	//输出比较通道1 GPIO初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH1_GPIO_PIN;
	GPIO_Init(GENERAL_TIM_CH1_GPIO_PORT,&GPIO_InitStructure);
	
}

///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
static void GENERAL_TIM_Mode_Config(void)
{
	/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为100K
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	
	// 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);
	
	//时钟分频因子 ，基本定时器没有
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	//计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_PERIOD;
	
	//时钟预分频数为
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_PSC;
	
	//重复计数器的值，基本定时器没有
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM,&TIM_TimeBaseStructure);
	
	/*--------------------输出比较结构体初始化-------------------*/	
	
	TIM_ICInitTypeDef    TIM_ICInitStructure;
	
	//配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = GENERAL_TIM_CH1_CHANNNEL_x;
	
	//输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	
	//输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_TIM_START_ICPolarity;
	
	//输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	
	//输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	
	// 定时器输入捕获初始化
	TIM_ICInit(GENERAL_TIM,&TIM_ICInitStructure);
	
	//清除更新和捕获中断标志位
	TIM_ClearFlag(GENERAL_TIM,TIM_FLAG_Update | GENERAL_TIM_IT_CCx);
	
	//开启更新和捕获中断
	TIM_ITConfig(GENERAL_TIM,TIM_IT_Update | GENERAL_TIM_IT_CCx,ENABLE);
	
	//使能计数器
	TIM_Cmd(GENERAL_TIM,ENABLE);
	
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config(); 	
}
