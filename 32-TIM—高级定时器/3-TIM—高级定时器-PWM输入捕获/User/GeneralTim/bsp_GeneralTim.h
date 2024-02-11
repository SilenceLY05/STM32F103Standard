#ifndef __BSP_GENERALTIM_H
#define __BSP_GENERALTIM_H

#include "stm32f10x.h"

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM3

#define     GENERAL_TIM                       TIM2
#define     GENERAL_TIM_APBxClock_FUN         RCC_APB1PeriphClockCmd
#define     GENERAL_TIM_CLK                   RCC_APB1Periph_TIM2
// 输出PWM的频率为 72M/{ (ARR+1)*(PSC+1) }
#define     GENERAL_TIM_Period                (10-1)
#define     GENERAL_TIM_Prescaler             (72-1)


#define            GENERAL_TIM_CCR1              5
#define            GENERAL_TIM_CCR2              4
#define            GENERAL_TIM_CCR3              3
#define            GENERAL_TIM_CCR4              2


//TIM3输出比较通道1
#define     GENERAL_TIM_CH1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define     GENERAL_TIM_CH1_GPIO_PORT         GPIOA
#define     GENERAL_TIM_CH1_GPIO_PIN          GPIO_Pin_6

//TIM3输出比较通道2
#define     GENERAL_TIM_CH2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define     GENERAL_TIM_CH2_GPIO_PORT         GPIOA
#define     GENERAL_TIM_CH2_GPIO_PIN          GPIO_Pin_7

//TIM3输出比较通道3
#define     GENERAL_TIM_CH3_GPIO_CLK          RCC_APB2Periph_GPIOB
#define     GENERAL_TIM_CH3_GPIO_PORT         GPIOB
#define     GENERAL_TIM_CH3_GPIO_PIN          GPIO_Pin_0

//TIM3输出比较通道4
#define     GENERAL_TIM_CH4_GPIO_CLK          RCC_APB2Periph_GPIOB
#define     GENERAL_TIM_CH4_GPIO_PORT         GPIOB
#define     GENERAL_TIM_CH4_GPIO_PIN          GPIO_Pin_1



void GENERAL_TIM_Init(void);


#endif /* __BSP_GENERALTIM_H */
