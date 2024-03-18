#ifndef __SYSTICK_H
#define __SYSTICK_H



#include "stm32f10x.h"


void SysTick_Init( void );
void Delay_us(__IO uint32_t nTime);
void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);


#endif /* __SYSTICK_H */
