#include "bsp_TimeBase.h"

// �ж����ȼ�����
void BASIC_TIM_NCIV_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	//�����ж���Ϊ0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	//�����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ;
	
	//���������ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	//������ռ���ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
}

/*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * �ж�����Ϊ = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> �ж� ��TIMxCNT����Ϊ0���¼��� 
 */
void BASIC_TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK,ENABLE);
	
	//ʱ�ӷ�Ƶ���� ��������ʱ��û��
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	//����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
	//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//�Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period = BASIC_TIM_Period;
	
	//ʱ��Ԥ��Ƶ��Ϊ
	TIM_TimeBaseStructure.TIM_Prescaler = BASIC_TIM_Prescaler;
	
	//�ظ���������ֵ��������ʱ��û��
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//��ʼ����ʱ��
	TIM_TimeBaseInit(BASIC_TIM,&TIM_TimeBaseStructure);
	
	//����������жϱ�־λ
	TIM_ClearFlag(BASIC_TIM,TIM_FLAG_Update);
	
	//�����������ж�
	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
	
	//ʹ�ܼ�����
	TIM_Cmd(BASIC_TIM,ENABLE);
	
	//�ȹرյȴ�ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);
	
}

