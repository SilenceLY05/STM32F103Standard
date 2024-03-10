#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f10x.h"
#include "core_cm3.h"

#define Delay_ms(x) Delay_us(100*x)	 //µ¥Î»ms

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);



#endif /* __BSP_SYSTICK_H */

