#ifndef __BSP_ADVANCETIM_H
#define __BSP_ADVANCETIM_H

#include "stm32f10x.h"


/************高级定时器TIM参数定义，只限TIM1和TIM8************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 这里我们使用高级控制定时器TIM1
#define     ADVANCE_TIM                       TIM1
#define     ADVANCE_TIM_APBxClock_FUN         RCC_APB2PeriphClockCmd
#define     ADVANCE_TIM_CLK                   RCC_APB2Periph_TIM1
// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}  小于1KHz捕获不了 一般在10K - 25K之间
#define     ADVANCE_TIM_PERIOD                (1000-1)
#define     ADVANCE_TIM_PSC                   (72-1)

#define     ADVANCE_TIM_IRQ                   TIM1_CC_IRQn
#define     ADVANCE_TIM_IRQHandler            TIM1_CC_IRQHandler

// TIM1 输出比较通道
#define     ADVANCE_TIM_CH1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define     ADVANCE_TIM_CH1_PORT              GPIOA
#define     ADVANCE_TIM_CH1_PIN               GPIO_Pin_8

#define     ADVANCE_TIM_IC1PWM_CHANNEL        TIM_Channel_1
#define     ADVANCE_TIM_IC2PWM_CHANNEL        TIM_Channel_2



void ADVANCE_TIM_Init(void);


#endif /* __BSP_ADVANCETIM_H */
