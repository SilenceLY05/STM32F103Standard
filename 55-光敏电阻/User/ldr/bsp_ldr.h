#ifndef __BSP_LDR_H
#define __BSP_LDR_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"


/* LDR引脚定义 */
#define LDR_SCK_APBxClock_FUN    RCC_APB2PeriphClockCmd
#define LDR_GPIO_CLK             RCC_APB2Periph_GPIOC
#define LDR_GPIO_PORT            GPIOC
#define LDR_GPIO_PIN             GPIO_Pin_13


/* 按键按下宏定义 按键按下为高电平，设置KEY_ON = 1,KEY_OFF=0  若按键按下为低电平，设置为KEY_ON = 0,KEY_OFF=1 */
#define LDR_ON     1  //有光
#define LDR_OFF    0  //无光

void LDR_GPIO_Config(void);
uint8_t LDR_Test(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_LDR_H */
