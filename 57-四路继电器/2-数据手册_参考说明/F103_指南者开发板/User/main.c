
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_relay.h"
#include "./key/bsp_key.h"

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	RELAY_GPIO_Config(); //�̵���IO�ڳ�ʼ��
  
  Key_GPIO_Config(); //������ʼ��

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
