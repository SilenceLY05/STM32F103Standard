#ifndef __BSP_DAC_H
#define	__BSP_DAC_H


#include "stm32f10x.h"

/* DAC DHR12RD�Ĵ��� 12λ  �Ҷ��� ˫ͨ�� */
#define DAC_DHR12RD_ADDRESS              (DAC_BASE+0x20)

void DAC_Mode_Init(void);


#endif /* __BSP_DAC_H */
