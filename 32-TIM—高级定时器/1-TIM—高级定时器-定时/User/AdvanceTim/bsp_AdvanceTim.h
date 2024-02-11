#ifndef __BSP_ADVANCETIM_H
#define __BSP_ADVANCETIM_H

#include "stm32f10x.h"

#define     ADVANCE_TIM1 // 如果使用TIM7，注释掉这个宏即可

#ifndef     ADVANCE_TIM   // 使用基本定时器TIM6
#define     ADVANCE_TIM                       TIM1
#define     ADVANCE_TIM_APBxClock_FUN         RCC_APB2PeriphClockCmd
#define     ADVANCE_TIM_CLK                   RCC_APB2Periph_TIM1
#define     ADVANCE_TIM_Period                (1000-1)
#define     ADVANCE_TIM_Prescaler             71
#define     ADVANCE_TIM_IRQ                   TIM1_UP_IRQn
#define     ADVANCE_TIM_IRQHandler            TIM1_UP_IRQHandler

#else        // 使用基本定时器TIM7
#define     ADVANCE_TIM                       TIM8
#define     ADVANCE_TIM_APBxClock_FUN         RCC_APB2PeriphClockCmd
#define     ADVANCE_TIM_CLK                   RCC_APB2Periph_TIM8
#define     ADVANCE_TIM_Period                (1000-1)
#define     ADVANCE_TIM_Prescaler             71
#define     ADVANCE_TIM_IRQ                   TIM8_UP_IRQn
#define     ADVANCE_TIM_IRQHandler            TIM8_UP_IRQHandler

#endif

void ADVANCE_TIM_Init(void);


#endif /* __BSP_ADVANCETIM_H */
