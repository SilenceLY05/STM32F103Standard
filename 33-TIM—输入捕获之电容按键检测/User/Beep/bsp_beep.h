#ifndef  __BSP_BEEP_H
#define  __BSP_BEEP_H

#include "stm32f10x.h"

/* BEEP 引脚配置参数定义 */
#define BEEP_GPIO_APBxCLock_FUN   RCC_APB2PeriphClockCmd
#define BEEP_GPIO_CLK             RCC_APB2Periph_GPIOA
#define BEEP_GPIO_PORT            GPIOA
#define BEEP_GPIO_PIN             GPIO_Pin_8


/* BEEP 函数宏定义 */
#define   BEEP_ON()               GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define   BEEP_OFF()              GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void Beep_Init(void);


#endif   /* __BSP_BEEP_H */
