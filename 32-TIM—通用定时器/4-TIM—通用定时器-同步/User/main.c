/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_GeneralTim.h"
#include "bsp_usart.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 初始化通用定时器级联同步PWM输出 */
	/* 实现：TIM2发生事件更新时发送触发信号驱动TIM3计数 */
	/*       TIM3发生事件更新时发送触发信号驱动TIM4计数 */
	TIMx_Configuration();
	
  while(1)
  {
        
  }
}


/*********************************************END OF FILE**********************/
