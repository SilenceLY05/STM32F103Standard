#include "bsp_AdvanceTim.h"

// �ж����ȼ�����
static void ADVANCE_TIM_NCIV_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	//�����ж���Ϊ0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	//�����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ;
	
	//���������ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	//������ռ���ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
}


static void ADVANCE_TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);
	
	//ʱ�ӷ�Ƶ���� ��������ʱ��û��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	//����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//�Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_Period;
	
	//ʱ��Ԥ��Ƶ��Ϊ
	TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_Prescaler;
	
	//�ظ���������ֵ��������ʱ��û��
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//��ʼ����ʱ��
	TIM_TimeBaseInit(ADVANCE_TIM,&TIM_TimeBaseStructure);
	
	//����������жϱ�־λ
	TIM_ClearFlag(ADVANCE_TIM,TIM_FLAG_Update);
	
	//�����������ж�
	TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);
	
	//ʹ�ܼ�����
	TIM_Cmd(ADVANCE_TIM,ENABLE);
	
}

void ADVANCE_TIM_Init(void)
{
	ADVANCE_TIM_Mode_Config();
	ADVANCE_TIM_NCIV_Config();
}
