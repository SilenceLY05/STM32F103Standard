#include "bsp_GeneralTim.h"

//��ʱ�����벶���û��Զ�������ṹ�嶨��
TIM_ICUserValueTypeDef  TIM_ICUserValueStructure = {0,0,0,0};


//�ж����ȼ�����
static void GENERAL_TIM_NVIC_Config(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
		 // �����ж���Ϊ0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQ;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	// ���������ȼ�Ϊ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	// ������ռ���ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
	NVIC_Init(&NVIC_InitStructure);
}

//ͨ�ö�ʱ��3 ����Ƚ�4ͨ��GPIO��ʼ��
static void GENERAL_TIM_GPIO_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	//����Ƚ�ͨ��1 GPIO��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GENERAL_TIM_CH1_GPIO_PIN;
	GPIO_Init(GENERAL_TIM_CH1_GPIO_PORT,&GPIO_InitStructure);
	
}

///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
static void GENERAL_TIM_Mode_Config(void)
{
	/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	// �������ڣ���������Ϊ100K
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);
	
	//ʱ�ӷ�Ƶ���� ��������ʱ��û��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	
	//����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//�Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_PERIOD;
	
	//ʱ��Ԥ��Ƶ��Ϊ
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_PSC;
	
	//�ظ���������ֵ��������ʱ��û��
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	//��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIM,&TIM_TimeBaseStructure);
	
	/*--------------------����ȽϽṹ���ʼ��-------------------*/	
	
	TIM_ICInitTypeDef    TIM_ICInitStructure;
	
	//�������벶���ͨ������Ҫ���ݾ����GPIO������
	TIM_ICInitStructure.TIM_Channel = GENERAL_TIM_CH1_CHANNNEL_x;
	
	//�������Ҫ��������źŵ��˲�ϵ��
	TIM_ICInitStructure.TIM_ICFilter = 0;
	
	//���벶���źŵļ�������
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_TIM_START_ICPolarity;
	
	//�������Ҫ��������źŵķ�Ƶϵ��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	
	//����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	
	// ��ʱ�����벶���ʼ��
	TIM_ICInit(GENERAL_TIM,&TIM_ICInitStructure);
	
	//������ºͲ����жϱ�־λ
	TIM_ClearFlag(GENERAL_TIM,TIM_FLAG_Update | GENERAL_TIM_IT_CCx);
	
	//�������ºͲ����ж�
	TIM_ITConfig(GENERAL_TIM,TIM_IT_Update | GENERAL_TIM_IT_CCx,ENABLE);
	
	//ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIM,ENABLE);
	
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config(); 	
}
