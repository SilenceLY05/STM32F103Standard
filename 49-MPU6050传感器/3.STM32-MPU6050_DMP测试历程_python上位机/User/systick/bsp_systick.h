#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f10x.h"

int get_tick_count(unsigned long *count);
void mdelay(unsigned long nTime);
void SysTick_Init(void);

#endif /* __BSP_SYSTICK_H */

