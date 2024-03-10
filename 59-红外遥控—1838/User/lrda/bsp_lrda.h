#ifndef __BSP_LRDA_H
#define __BSP_LRDA_H

#include "stm32f10x.h"

#define IRDA_ID    0

//红外遥控使用的GPIO及时钟
#define IRDA_GPIO_SCK_ClockCmd    RCC_APB2PeriphClockCmd
#define IRDA_GPIO_CLK             RCC_APB2Periph_GPIOE
#define IRDA_GPIO_PORT            GPIOE
#define IRDA_GPIO_PIN             GPIO_Pin_5
#define IRDA_GPIO_PORTSOURCE      GPIO_PortSourceGPIOE
#define IRDA_GPIO_PINSOURCE       GPIO_PinSource5

//中断相关
#define IRDA_EXTI_IRQ             EXTI9_5_IRQn
#define IRDA_EXTI_IRQHandler      EXTI9_5_IRQHandler
#define IRDA_EXTI_LINE            EXTI_Line5

//读取引脚电平
#define IRDA_DATA_IN()            GPIO_ReadInputDataBit(IRDA_GPIO_PORT,IRDA_GPIO_PIN)



void Irda_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t Irda_Process(void);


#endif /* __BSP_LRDA_H */

