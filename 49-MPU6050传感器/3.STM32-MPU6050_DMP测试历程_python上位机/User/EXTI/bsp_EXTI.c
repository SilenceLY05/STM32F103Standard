#include "bsp_EXTI.h"

static void EXTI_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_1);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init( &NVIC_InitStruct);
}

void EXTI_Pxy_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	EXTI_InitTypeDef  EXTI_InitStruct;
	
	RCC_APB2PeriphClockCmd(EXTI_GPIO_CLK, ENABLE);
	
	//配置中断优先级
	EXTI_NVIC_Config();
	//初始化GPIO

	GPIO_InitStruct.GPIO_Pin = EXTI_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(EXTI_GPIO_PORT, &GPIO_InitStruct);

	
	GPIO_EXTILineConfig(EXTI_SOURCE_PORT,  EXTI_SOURCE_PIN);	
	EXTI_InitStruct.EXTI_Line = EXTI_LINE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

}

void EnableInvInterrupt(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line = EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

void DisableInvInterrupt(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line = EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_LINE);
	
}

