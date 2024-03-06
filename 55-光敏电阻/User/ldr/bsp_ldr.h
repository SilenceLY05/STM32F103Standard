#ifndef __BSP_LDR_H
#define __BSP_LDR_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"


/* LDR���Ŷ��� */
#define LDR_SCK_APBxClock_FUN    RCC_APB2PeriphClockCmd
#define LDR_GPIO_CLK             RCC_APB2Periph_GPIOC
#define LDR_GPIO_PORT            GPIOC
#define LDR_GPIO_PIN             GPIO_Pin_13


/* �������º궨�� ��������Ϊ�ߵ�ƽ������KEY_ON = 1,KEY_OFF=0  ����������Ϊ�͵�ƽ������ΪKEY_ON = 0,KEY_OFF=1 */
#define LDR_ON     1  //�й�
#define LDR_OFF    0  //�޹�

void LDR_GPIO_Config(void);
uint8_t LDR_Test(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_LDR_H */
