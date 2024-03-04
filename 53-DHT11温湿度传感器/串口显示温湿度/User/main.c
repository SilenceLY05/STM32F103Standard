#include "stm32f10x.h"
#include "bsp_dht11.h"
#include "bsp_usart.h"
#include "bsp_systick.h"





/* ������ */
int main(void)
{	

	DHT11_Data_Typedef DHT11_Data;
	
	/* ��ʼ��ϵͳ��ʱ�� */
	SysTick_Init();
	
	/* ��ʼ������ */
	USART_Config();
	
	printf("\r\n DHT11��ʪ�ȴ���������ʵ�� \r\n");
	
	/* ��ʼ������������ */
	DHT11_Init();
	
	while(1)
	{
		/* ����DHT11_Read_TempAndHumidity��ȡ��ʪ����Ϣ */
		if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ���\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ:%d.%d ��\r\n",DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}
		else
		{
			printf("\r\n Read DHT11 ERROR�� \r\n");
		}
		Delay_ms(2000);
	}



}


/*********************************************END OF FILE**********************/
