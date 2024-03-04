#include "stm32f10x.h"
#include "bsp_dht11.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "bsp_ili9341_lcd.h"





/* ������ */
int main(void)
{	
	char dispBuff[100];

	DHT11_Data_Typedef DHT11_Data;
	
	/* ��ʼ��ϵͳ��ʱ�� */
	SysTick_Init();
	
	ILI9341_Init();
	
	ILI9341_GramScan(6);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	/* ��ʼ������ */
	USART_Config();
	
	ILI9341_DispStringLine_EN(LINE(0),"DHT11 HumiAndTemp Test ");
	
	/* ��ʼ������������ */
	DHT11_Init();
	
	while(1)
	{
		/* ����DHT11_Read_TempAndHumidity��ȡ��ʪ����Ϣ */
		if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
		{
			ILI9341_DispStringLine_EN(LINE(1),"DHT11 demo");
			
			/* ��ʾʪ�� */
			sprintf(dispBuff,"Humidity: %d.%d",DHT11_Data.humi_int,DHT11_Data.humi_deci);
			LCD_ClearLine(LINE(2));
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			
			/* ��ʾ�¶� */
			sprintf(dispBuff,"Temperature: %d.%d",DHT11_Data.temp_int,DHT11_Data.temp_deci);
			LCD_ClearLine(LINE(3));
			ILI9341_DispStringLine_EN(LINE(3),dispBuff);
		}
		else
		{
			LCD_ClearLine(LINE(2));
			LCD_ClearLine(LINE(3));
			ILI9341_DispStringLine_EN(LINE(2),"Read DHT11 Humidity ERROR");
			ILI9341_DispStringLine_EN(LINE(3),"Read DHT11 Temperatur ERROR");
		}
		Delay_ms(2000);
	}



}


/*********************************************END OF FILE**********************/
