#include "bsp_pvd.h"


void PVD_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	EXTI_InitTypeDef    EXTI_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	/* 使能 PVD 中断 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 配置 EXTI16线(PVD 输出) 来产生上升下降沿中断*/
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	/* 配置PVD级别PWR_PVDLevel_2V6 (PVD检测电压的阈值为2.6V，VDD电压低于2.6V时产生PVD中断) */
	/*具体级别根据自己的实际应用要求配置*/
	PWR_PVDLevelConfig(PWR_PVDLevel_2V6);
	
	/* 使能PVD输出 */
	PWR_PVDCmd(ENABLE);
	
	
}

