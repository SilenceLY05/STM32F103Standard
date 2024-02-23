#include "bsp_beep.h"

void Beep_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/*开启连接蜂鸣器的GPIO的外设时钟*/
	BEEP_GPIO_APBxCLock_FUN(BEEP_GPIO_CLK,ENABLE);
	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*选择要控制的GPIO引脚*/		
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(BEEP_GPIO_PORT,&GPIO_InitStructure);
	
	/* 默认关闭蜂鸣器*/
	GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);	
}

