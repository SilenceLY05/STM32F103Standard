#include "bsp_GeneralTim.h"

//通用定时器3 输出比较4通道GPIO初始化
static void GENERAL_TIM_GPIO_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	//输出比较通道1 GPIO初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GENERAL_TIM_CH1_GPIO_PORT,&GPIO_InitStructure);
	
	//输出比较通道2 GPIO初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH2_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GENERAL_TIM_CH2_GPIO_PORT,&GPIO_InitStructure);
	
	//输出比较通道3 GPIO初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH3_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GENERAL_TIM_CH3_GPIO_PORT,&GPIO_InitStructure);
	
	//输出比较通道4GPIO初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH4_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GENERAL_TIM_CH4_GPIO_PORT,&GPIO_InitStructure);
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
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
	
	//时钟预分频数为
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_Prescaler;
	
	//重复计数器的值，基本定时器没有
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM,&TIM_TimeBaseStructure);
	
	/*--------------------输出比较结构体初始化-------------------*/	
	TIM_OCInitTypeDef    TIM_OCInitStructure;
	
	//配置为PWM1模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	
	//输出使能
	TIM_OCInitStructure.TIM_OutputState = ENABLE;
	
	//输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//输出比较通道1
	TIM_OCInitStructure.TIM_Pulse = GENERAL_TIM_CCR1;
	TIM_OC1Init(GENERAL_TIM,&TIM_OCInitStructure);
	TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPreload_Enable);
	
	//输出比较通道2
	TIM_OCInitStructure.TIM_Pulse = GENERAL_TIM_CCR2;
	TIM_OC1Init(GENERAL_TIM,&TIM_OCInitStructure);
	TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPreload_Enable);
	
	//输出比较通道3
	TIM_OCInitStructure.TIM_Pulse = GENERAL_TIM_CCR3;
	TIM_OC1Init(GENERAL_TIM,&TIM_OCInitStructure);
	TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPreload_Enable);
	
	//输出比较通道4
	TIM_OCInitStructure.TIM_Pulse = GENERAL_TIM_CCR4;
	TIM_OC1Init(GENERAL_TIM,&TIM_OCInitStructure);
	TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPreload_Enable);
	
	//使能计数器
	TIM_Cmd(GENERAL_TIM,ENABLE);
	
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_Mode_Config();
	GENERAL_TIM_GPIO_Config();
}
