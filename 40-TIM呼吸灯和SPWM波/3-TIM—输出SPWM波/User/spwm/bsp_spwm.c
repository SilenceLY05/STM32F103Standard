#include "bsp_spwm.h"  

//控制输出波形的频率
__IO uint16_t period_class = 1;

/* LED 亮度登记PWM表，指数曲线，此表使用工程目录下的python脚本生成 */
const uint16_t indexWave[] = {
0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
	107, 116, 125, 133, 142, 151, 159, 168, 176,
	184, 193, 201, 209, 218, 226, 234, 242, 249,
	257, 265, 273, 280, 288, 295, 302, 310, 317, 
	324, 331, 337, 344, 351, 357, 364, 370, 376, 
	382, 388, 394, 399, 405, 410, 416, 421, 426, 
	431, 436, 440, 445, 449, 454, 458, 462, 465, 
	469, 473, 476, 479, 482, 485, 488, 491, 493, 
	496, 498, 500, 502, 503, 505, 506, 508, 509, 
	510, 510, 511, 512, 512, 512, 512, 512, 512,
	511, 510, 510, 509, 508, 506, 505, 503, 502,
	500, 498, 496, 493, 491, 488, 485, 482, 479,
	476, 473, 469, 465, 462, 458, 454, 449, 445, 
	440, 436, 431, 426, 421, 416, 410, 405, 399, 
	394, 388, 382, 376, 370, 364, 357, 351, 344, 
	337, 331, 324, 	317, 310, 302, 295, 288, 280, 
	273, 265, 257, 249, 242, 234, 226, 218, 209, 
	201, 193, 184, 176, 168, 159, 151, 142, 133, 
125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
27, 18, 9, 0
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
