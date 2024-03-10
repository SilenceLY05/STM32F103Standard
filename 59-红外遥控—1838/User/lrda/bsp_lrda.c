#include "bsp_lrda.h"
#include "bsp_systick.h"
#include "bsp_led.h"


uint32_t frame_data = 0;             /* 一帧数据缓存 */
uint8_t frame_cnt = 0;          
uint8_t frame_flag=0;      /* 一帧数据接收完标志 */

/* 配置嵌套中断NVIC */
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

/* 配置红外接收头用到的IO引脚 */
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
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿中断
	EXTI_Init(&EXTI_InitStructure);
	
}

/* 获取高电平时间 */
uint8_t Get_Pulse_Time(void)
{
	uint8_t time = 0;
	
	while(IRDA_DATA_IN())
	{
		time++;        // 延时 20us
		
		Delay_us(2);
		
		if(time == 250)
			return time;  // 超时溢出
	}
	return time;
}

/* 数据帧有4个字节，第一个字节是遥控的ID，第二个字节是第一个字节的反码
 * 第三个数据是遥控的真正的键值，第四个字节是第三个字节的反码 */
uint8_t Irda_Process(void)
{
	uint8_t first_data,second_data,third_data,fouth_data;
	
	first_data = (frame_data>>24)&0xFF;
	second_data = (frame_data>>16)&0xFF;
	third_data = (frame_data>>8)&0xFF;
	fouth_data = frame_data&0xFF;
	
	/* 清除标志位 */
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

