#ifndef __BSP_GENERALTIM_H
#define __BSP_GENERALTIM_H

#include "stm32f10x.h"

/* 定时器 */
#define GENERAL_TIM_MASTER              TIM2
#define GENERAL_TIM_MASTER_CLK          RCC_APB1Periph_TIM2

#define GENERAL_TIM_SLAVE1              TIM3
#define GENERAL_TIM_SLAVE1_CLK          RCC_APB1Periph_TIM3

#define GENERAL_TIM_SLAVE2              TIM4
#define GENERAL_TIM_SLAVE2_CLK          RCC_APB1Periph_TIM4


/* TIM主模式PWM输出引脚 */
#define GENERAL_MASTER_CLK              RCC_APB2Periph_GPIOB
#define GENERAL_MASTER_PORT             GPIOB
#define GENERAL_MASTER_PIN              GPIO_Pin_10
#define GENERAL_MASTER_PINSOURCE        GPIO_PinSource5
#define GENERAL_MASTER_AF               GPIO_AF_TIM2

/* TIM从模式PWM输出引脚 */
#define GENERAL_SLAVE1_CLK              RCC_APB2Periph_GPIOC
#define GENERAL_SLAVE1_PORT             GPIOC
#define GENERAL_SLAVE1_PIN              GPIO_Pin_6
#define GENERAL_SLAVE1_PINSOURCE        GPIO_PinSource6
#define GENERAL_SLAVE1_AF               GPIO_AF_TIM3

/* TIM从模式PWM输出引脚 */
#define GENERAL_SLAVE2_CLK              RCC_APB2Periph_GPIOB
#define GENERAL_SLAVE2_PORT             GPIOB
#define GENERAL_SLAVE2_PIN              GPIO_Pin_6
#define GENERAL_SLAVE2_PINSOURCE        GPIO_PinSource6
#define GENERAL_SLAVE2_AF               GPIO_AF_TIM4



void TIMx_Configuration(void);


#endif /* __BSP_GENERALTIM_H */
