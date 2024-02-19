#include "bsp_color_led.h"  

/* 配置 COLOR_TIMx 复用输出PWM时用到I/O */
static void COLOR_TIMx_GPIO_Config(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(COLOR_TIM_GPIO_CLK,ENABLE);
	
	COLOR_GPIO_REMAP_FUN();
	
	/* 配置红灯 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COLOR_RED_TIM_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COLOR_RED_TIM_LED_PORT,&GPIO_InitStructure);
	
	/* 配置绿灯 */
	GPIO_InitStructure.GPIO_Pin = COLOR_GREEN_TIM_LED_PIN;
	GPIO_Init(COLOR_GREEN_TIM_LED_PORT,&GPIO_InitStructure);
	
	/* 配置蓝灯 */
	GPIO_InitStructure.GPIO_Pin = COLOR_BLUE_TIM_LED_PIN;
	GPIO_Init(COLOR_BLUE_TIM_LED_PORT,&GPIO_InitStructure);
}

/* 配置COLOR_TIMx输出的PWM信号的模式，极性周期等 */
static void COLOR_TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef           TIM_OCInitStructure;
	
	/* 设置TIM CLK时钟 */
	COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK,ENABLE);                   //使能COLOR_TIMx时钟
	
	/* 基本定时器配置 */
	TIM_TimeBaseInitStructure.TIM_Period = 255;                      //当定时器从0计数到255，即为256次，为一个定时周期
	TIM_TimeBaseInitStructure.TIM_Prescaler = 4000;                  //设置预分频
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(COLOR_TIMx,&TIM_TimeBaseInitStructure);
	
	/* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;                               //设置初始PWM脉冲宽度为0	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //当定时器计数值小于CCR_Val时为低电平
	
	/* 使能通道和预装载 */
	COLOR_RED_TIM_OCxInit(COLOR_TIMx,&TIM_OCInitStructure);
	COLOR_RED_TIM_OCxPreloadConfig(COLOR_TIMx,TIM_OCPreload_Enable);
	
	/* 使能通道和预装载 */
	COLOR_GREEN_TIM_OCxInit(COLOR_TIMx,&TIM_OCInitStructure);
	COLOR_GREEN_TIM_OCxPreloadConfig(COLOR_TIMx,TIM_OCPreload_Enable);
	
	/* 使能通道和预装载 */
	COLOR_BLUE_TIM_OCxInit(COLOR_TIMx,&TIM_OCInitStructure);
	COLOR_BLUE_TIM_OCxPreloadConfig(COLOR_TIMx,TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(COLOR_TIMx,ENABLE);                          //使能COLOR_TIMx重载寄存器ARR
	
	TIM_Cmd(COLOR_TIMx,ENABLE);                                       //使能定时器		
	
}

/* COLOR_TIMx呼吸灯初始化 配置PWM模式和GPIO */
void COLOR_TIMx_LED_Init(void)
{
	COLOR_TIMx_GPIO_Config();
	COLOR_TIMx_Mode_Config();	
}

/* 停止PWM输出 */
void COLOR_TIMx_LED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_Cmd(COLOR_TIMx,DISABLE);                     //失能定时器	
	COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK,DISABLE);  //失能定时器时钟
	
	//关闭LED灯
	GPIO_SetBits(COLOR_RED_TIM_LED_PORT,COLOR_RED_TIM_LED_PIN);
	GPIO_SetBits(COLOR_GREEN_TIM_LED_PORT,COLOR_GREEN_TIM_LED_PIN);
	GPIO_SetBits(COLOR_BLUE_TIM_LED_PORT,COLOR_BLUE_TIM_LED_PIN);
	
	
}

/* 设置RGB LED的颜色 rgb:要设置LED显示的颜色格式RGB888 */
void SetRGBColor(uint32_t rgb)
{
	//根据颜色值修改定时器的比较寄存器值
	COLOR_TIMx->COLOR_RED_CCRx = (uint8_t)(rgb>>16);
	COLOR_TIMx->COLOR_GREEN_CCRx = (uint8_t)(rgb>>8);
	COLOR_TIMx->COLOR_BLUE_CCRx = (uint8_t)rgb;
}


/* 设置RGB LED的颜色 r\g\b:要设置LED显示的颜色值 */
void SetColorValue(uint8_t r,uint8_t g, uint8_t b)
{
	//根据颜色值修改定时器的比较寄存器值
	COLOR_TIMx->COLOR_RED_CCRx = r;
	COLOR_TIMx->COLOR_GREEN_CCRx = g;
	COLOR_TIMx->COLOR_BLUE_CCRx = b;
}


 
/*********************************************END OF FILE**********************/
