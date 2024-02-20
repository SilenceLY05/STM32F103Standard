#ifndef __BSP_BREATHING_H
#define	__BSP_BREATHING_H


#include "stm32f10x.h"

/*PWM表中的点数*/
extern uint16_t  POINT_NUM	;
//控制输出波形的频率
extern __IO uint16_t period_class ;

#define  RED_LIGHT           1
#define  GREEN_LIGHT         2
#define  BLUE_LIGHT          3


/* 要使用什么颜色的灯，可选RED_LIGHT GREEN_LIGHT BLUE_LIGHT */
#define  LIGHT_COLOR         RED_LIGHT

/* 定时器通道 */
#if   LIGHT_COLOR == RED_LIGHT

/* 红灯 */
#define  BRE_TIMx                          TIM3
#define  BRE_TIM_APBxClock_FUN             RCC_APB1PeriphClockCmd
#define  BRE_TIM_CLK                       RCC_APB1Periph_TIM3
#define  BRE_TIM_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define  BRE_TIM_GPIO_CLK                  (RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO)

//红灯的引脚需要重映射
#define  BRE_GPIO_REMAP_FUN()              GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);


//红灯
#define  BRE_RED_TIM_LED_PORT              GPIOB
#define  BRE_RED_TIM_LED_PIN               GPIO_Pin_5

#define  BRE_RED_TIM_OCxInit               TIM_OC2Init             //通道初始化函数
#define  BRE_RED_TIM_OCxPreloadConfig      TIM_OC2PreloadConfig    //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_RED_CCRx ，可访问该通道的比较寄存器
#define  BRE_RED_CCRx                      CCR2

#define  BRE_TIMx_IRQn                     TIM3_IRQn
#define  BRE_TIMx_IRQHandler               TIM3_IRQHandler


#elif    LIGHT_COLOR == GREEN_LIGHT
/* 绿灯 */
#define  BRE_TIMx                          TIM3
#define  BRE_TIM_APBxClock_FUN             RCC_APB1PeriphClockCmd
#define  BRE_TIM_CLK                       RCC_APB1Periph_TIM3
#define  BRE_TIM_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define  BRE_TIM_GPIO_CLK                  (RCC_APB2Periph_GPIOB)

//绿灯的引脚需要重映射
#define  BRE_GPIO_REMAP_FUN()             


//绿灯
#define  BRE_GREEN_TIM_LED_PORT            GPIOB
#define  BRE_GREEN_TIM_LED_PIN             GPIO_Pin_0

#define  BRE_GREEN_TIM_OCxInit             TIM_OC3Init             //通道初始化函数
#define  BRE_GREEN_TIM_OCxPreloadConfig    TIM_OC3PreloadConfig    //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_RED_CCRx ，可访问该通道的比较寄存器
#define  BRE_GREEN_CCRx                    CCR3

#define  BRE_TIMx_IRQn                     TIM3_IRQn
#define  BRE_TIMx_IRQHandler               TIM3_IRQHandler


#elif    LIGHT_COLOR == BLUE_LIGHT
/* 蓝灯 */
#define  BRE_TIMx                          TIM3
#define  BRE_TIM_APBxClock_FUN             RCC_APB1PeriphClockCmd
#define  BRE_TIM_CLK                       RCC_APB1Periph_TIM3
#define  BRE_TIM_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define  BRE_TIM_GPIO_CLK                  (RCC_APB2Periph_GPIOB)

//蓝灯的引脚需要重映射
#define  BRE_GPIO_REMAP_FUN()            


//蓝灯
#define  BRE_BLUE_TIM_LED_PORT             GPIOB
#define  BRE_BLUE_TIM_LED_PIN              GPIO_Pin_1

#define  BRE_BLUE_TIM_OCxInit              TIM_OC4Init             //通道初始化函数
#define  BRE_BLUE_TIM_OCxPreloadConfig     TIM_OC4PreloadConfig    //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_RED_CCRx ，可访问该通道的比较寄存器
#define  BRE_BLUE_CCRx                     CCR4

#define  BRE_TIMx_IRQn                     TIM3_IRQn
#define  BRE_TIMx_IRQHandler               TIM3_IRQHandler




#endif
void      TIMx_Breathing_Init          (void);

#endif /* __BSP_BREATHING_H */
