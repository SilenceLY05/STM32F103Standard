#ifndef  __BSP_BEEP_H
#define  __BSP_BEEP_H

#include "stm32f10x.h"

/* BEEP 引脚配置参数定义 */
#define BEEP_GPIO_APBxCLock_FUN   RCC_APB2PeriphClockCmd
#define BEEP_GPIO_CLK             RCC_APB2Periph_GPIOA
#define BEEP_GPIO_PORT            GPIOA
#define BEEP_GPIO_PIN             GPIO_Pin_8


/* 带参宏，可以像内联函数一样使用 */
#define BEEP(a)	if (a)	\
					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);


#endif   /* __BSP_BEEP_H */
