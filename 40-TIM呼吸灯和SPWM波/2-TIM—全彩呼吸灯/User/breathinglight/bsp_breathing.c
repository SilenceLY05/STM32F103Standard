#include "bsp_breathing.h"  

//控制输出波形的频率
__IO uint16_t period_class = 1;

/* LED 亮度登记PWM表，指数曲线，此表使用工程目录下的python脚本生成 */
const uint16_t indexWave[] = {
1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 
	2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
	11, 12, 12, 13, 14, 15, 17, 18,
	19, 20, 22, 23, 25, 27, 29, 31, 
	33, 36, 38, 41, 44, 47, 51, 54, 
	58, 63, 67, 72, 77, 83, 89, 95, 
	102, 110, 117, 126, 135, 145, 156, 
	167, 179, 192, 206, 221, 237, 254,
	272, 292, 313, 336, 361, 387, 415,
	445, 477, 512, 512, 477, 445, 415, 
	387, 361, 336, 313, 292, 272, 254, 
	237, 221, 206, 192, 179, 167, 156, 
	145, 135, 126, 117, 110, 102, 95, 
	89, 83, 77, 72, 67, 63, 58, 54, 51, 
	47, 44, 41, 38, 36, 33, 31, 29, 27,
	25, 23, 22, 20, 19, 18, 17, 15, 14,
	13, 12, 12, 11, 10, 9, 9, 8, 8, 7, 7,
	6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 
	3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

/* 计算PWM表有多少个元素 */
uint16_t POINT_NUM = sizeof(indexWave) / sizeof(indexWave[0]);

/* 配置 COLOR_TIMx 复用输出PWM时用到I/O */
static void TIMx_GPIO_Config(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(BRE_TIM_GPIO_CLK,ENABLE);
	
	BRE_GPIO_REMAP_FUN();
	
	/* 配置红灯 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BRE_RED_TIM_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BRE_RED_TIM_LED_PORT,&GPIO_InitStructure);
	
	/* 配置绿灯 */
	GPIO_InitStructure.GPIO_Pin = BRE_GREEN_TIM_LED_PIN;
	GPIO_Init(BRE_GREEN_TIM_LED_PORT,&GPIO_InitStructure);
	
	/* 配置蓝灯 */
	GPIO_InitStructure.GPIO_Pin = BRE_BLUE_TIM_LED_PIN;
	GPIO_Init(BRE_BLUE_TIM_LED_PORT,&GPIO_InitStructure);
	
}

/* 配置嵌套向量中断控制器NVIC */
static void NVIC_Config_PWM(void)
{
	NVIC_InitTypeDef     NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* 配置TIM3_IRQ中断为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = BRE_TIMx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init(&NVIC_InitStructure);
}

/* 配置TIMx输出的PWM信号的模式，极性周期等 */
static void TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef           TIM_OCInitStructure;
	
	/* 设置TIM CLK时钟 */
	BRE_TIM_APBxClock_FUN(BRE_TIM_CLK,ENABLE);                   //使能TIM3_Clock时钟
	
		/* 基本定时器配置 ,配合PWM表点数、中断服务函数中的period_cnt循环次数设置*/	
	
	/* 设置使得整个呼吸过程为3秒左右即可达到很好的效果 */	
	
	//要求：
	//TIM_Period：与PWM表中数值范围一致
	//TIM_Prescaler：越小越好，可减轻闪烁现象
	//PERIOD_CLASS：中断服务函数中控制单个点循环的次数，调整它可控制拟合曲线的周期
	//POINT_NUM：PWM表的元素，它是PWM拟合曲线的采样点数

	/*************本实验中的配置***************/	
	/***********************************************
	#python计算脚本	count.py
	#PWM点数
	POINT_NUM = 110

	#周期倍数
	PERIOD_CLASS = 10

	#定时器定时周期
	TIMER_TIM_Period = 2**10
	#定时器分频
	TIMER_TIM_Prescaler = 200

	#STM32系统时钟频率和周期
	f_pclk = 72000000
	t_pclk = 1/f_pclk

	#定时器update事件周期
	t_timer = t_pclk*TIMER_TIM_Prescaler*TIMER_TIM_Period

	#每个PWM点的时间
	T_Point = t_timer * PERIOD_CLASS

	#整个呼吸周期
	T_Up_Down_Cycle = T_Point * POINT_NUM

	print ("呼吸周期：",T_Up_Down_Cycle)
	
	#运行结果：
	
	呼吸周期：3.12888
	************************************************************/
	
	
	/* 基本定时器配置 */
	TIM_TimeBaseInitStructure.TIM_Period = (512-1);                 //当定时器从0计数到255，即为256次，为一个定时周期
	TIM_TimeBaseInitStructure.TIM_Prescaler = (10-1);               //设置预分频
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(BRE_TIMx,&TIM_TimeBaseInitStructure);
	
	/* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;                               //设置初始PWM脉冲宽度为0	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //当定时器计数值小于CCR_Val时为低电平
	
	/* 使能通道和预装载 */
	BRE_RED_TIM_OCxInit(BRE_TIMx,&TIM_OCInitStructure);
	BRE_RED_TIM_OCxPreloadConfig(BRE_TIMx,TIM_OCPreload_Enable);
	
	/* 使能通道和预装载 */
	BRE_GREEN_TIM_OCxInit(BRE_TIMx,&TIM_OCInitStructure);
	BRE_GREEN_TIM_OCxPreloadConfig(BRE_TIMx,TIM_OCPreload_Enable);
	
	/* 使能通道和预装载 */
	BRE_BLUE_TIM_OCxInit(BRE_TIMx,&TIM_OCInitStructure);
	BRE_BLUE_TIM_OCxPreloadConfig(BRE_TIMx,TIM_OCPreload_Enable);
	
	
	TIM_ARRPreloadConfig(BRE_TIMx,ENABLE);                           //使能COLOR_TIMx重载寄存器ARR
	
	TIM_Cmd(BRE_TIMx,ENABLE);                                        //使能定时器		
	
	TIM_ITConfig(BRE_TIMx,TIM_IT_Update,ENABLE);                     //使能update
	NVIC_Config_PWM();                                               //配置中断优先级		
	
}

/* TIMx呼吸灯初始化 配置PWM模式和GPIO */
void TIMx_Breathing_Init(void)
{
	TIMx_GPIO_Config();
	TIMx_Mode_Config();	
}


 
/*********************************************END OF FILE**********************/
