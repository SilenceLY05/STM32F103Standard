#ifndef __SYSTICK_H
#define __SYSTICK_H



#include "stm32f10x.h"


void SysTick_Init( void );
void Delay_us(__IO uint32_t nTime);
void Delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#define delay_1ms       Delay_ms
#define delay_10us(x)   Delay_us((x*10))


#endif /* __SYSTICK_H */
