#include "bsp_beep.h"

void Beep_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/*�������ӷ�������GPIO������ʱ��*/
	BEEP_GPIO_APBxCLock_FUN(BEEP_GPIO_CLK,ENABLE);
	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*ѡ��Ҫ���Ƶ�GPIO����*/		
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(BEEP_GPIO_PORT,&GPIO_InitStructure);
	
	/* Ĭ�Ϲرշ�����*/
	BEEP_OFF();
}

void Beep_Init(void)
{
	Beep_GPIO_Config();
	
	/* Ĭ�Ϲرշ�����*/
	BEEP_OFF();
}
