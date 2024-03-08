
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_relay.h"
#include "./key/bsp_key.h"

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	RELAY_GPIO_Config(); //继电器IO口初始化
  
  Key_GPIO_Config(); //按键初始化

	for(;;)
	{
    
    if( Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON )
    {
      RELAY1_TOGGLE;
      RELAY2_TOGGLE;
    }
    
    if( Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON )
    {
      RELAY3_TOGGLE;
      RELAY4_TOGGLE;
    }
	} 
}






/*********************************************END OF FILE**********************/
