#include "bsp_lrda.h"
#include "bsp_systick.h"
#include "bsp_led.h"


uint32_t frame_data = 0;             /* һ֡���ݻ��� */
uint8_t frame_cnt = 0;          
uint8_t frame_flag=0;      /* һ֡���ݽ������־ */

/* ����Ƕ���ж�NVIC */
static void NVIC_Config(void)
{
	NVIC_InitTypeDef     NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = IRDA_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/* ���ú������ͷ�õ���IO���� */
void Irda_Init(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	EXTI_InitTypeDef      EXTI_InitStructure;
	
	IRDA_GPIO_SCK_ClockCmd(IRDA_GPIO_CLK | RCC_APB2Periph_AFIO,ENABLE);
	
	NVIC_Config();
	
	GPIO_InitStructure.GPIO_Pin = IRDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IRDA_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(IRDA_GPIO_PORTSOURCE,IRDA_GPIO_PINSOURCE);
	EXTI_InitStructure.EXTI_Line = IRDA_EXTI_LINE;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�½����ж�
	EXTI_Init(&EXTI_InitStructure);
	
}

/* ��ȡ�ߵ�ƽʱ�� */
uint8_t Get_Pulse_Time(void)
{
	uint8_t time = 0;
	
	while(IRDA_DATA_IN())
	{
		time++;        // ��ʱ 20us
		
		Delay_us(2);
		
		if(time == 250)
			return time;  // ��ʱ���
	}
	return time;
}

/* ����֡��4���ֽڣ���һ���ֽ���ң�ص�ID���ڶ����ֽ��ǵ�һ���ֽڵķ���
 * ������������ң�ص������ļ�ֵ�����ĸ��ֽ��ǵ������ֽڵķ��� */
uint8_t Irda_Process(void)
{
	uint8_t first_data,second_data,third_data,fouth_data;
	
	first_data = (frame_data>>24)&0xFF;
	second_data = (frame_data>>16)&0xFF;
	third_data = (frame_data>>8)&0xFF;
	fouth_data = frame_data&0xFF;
	
	/* �����־λ */
	frame_flag = 0;
	
	if((first_data == (uint8_t)~second_data) && (first_data == IRDA_ID))
	{
		if(third_data == (uint8_t)~fouth_data)
		{
			return third_data;
		}
	}
	return 0;
	
}

