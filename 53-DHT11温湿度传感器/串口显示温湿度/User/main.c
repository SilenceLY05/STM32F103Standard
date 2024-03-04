#include "stm32f10x.h"
#include "bsp_dht11.h"
#include "bsp_usart.h"
#include "bsp_systick.h"





/* 主函数 */
int main(void)
{	

	DHT11_Data_Typedef DHT11_Data;
	
	/* 初始化系统定时器 */
	SysTick_Init();
	
	/* 初始化串口 */
	USART_Config();
	
	printf("\r\n DHT11温湿度传感器测试实验 \r\n");
	
	/* 初始化传感器引脚 */
	DHT11_Init();
	
	while(1)
	{
		/* 调用DHT11_Read_TempAndHumidity读取温湿度信息 */
		if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
		{
			printf("\r\n读取DHT11成功！\r\n\r\n湿度为%d.%d ％RH ，温度为:%d.%d ℃\r\n",DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}
		else
		{
			printf("\r\n Read DHT11 ERROR！ \r\n");
		}
		Delay_ms(2000);
	}



}


/*********************************************END OF FILE**********************/
