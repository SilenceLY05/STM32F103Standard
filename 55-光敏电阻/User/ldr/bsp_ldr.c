#include "bsp_ldr.h"
#include "bsp_led.h"

/* 配置光敏电阻的引脚 */
void LDR_GPIO_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	
	/* 开启端口的时钟 */
	LDR_SCK_APBxClock_FUN(LDR_GPIO_CLK,ENABLE);
	
	/* 选择输入的引脚 */
	GPIO_InitStructure.GPIO_Pin = LDR_GPIO_PIN;
	
	/* 设置光敏输入的引脚为下拉输入 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(LDR_GPIO_PORT,&GPIO_InitStructure);
}

/* 光敏电阻测试 检测光敏电阻的状态 GPIOx：可以是ABCDE GPIO_Pin待读取的端口位 */
uint8_t LDR_Test(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 1)
	{
		return LDR_OFF;  //无光
	}
	else
	{
		return LDR_ON;  //有光
	}
}

