#include "stm32f10x.h"
#include "bsp_dht11.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "bsp_ili9341_lcd.h"





/* 主函数 */
int main(void)
{	
	char dispBuff[100];

	DHT11_Data_Typedef DHT11_Data;
	
	/* 初始化系统定时器 */
	SysTick_Init();
	
	ILI9341_Init();
	
	ILI9341_GramScan(6);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	/* 初始化串口 */
	USART_Config();
	
	ILI9341_DispStringLine_EN(LINE(0),"DHT11 HumiAndTemp Test ");
	
	/* 初始化传感器引脚 */
	DHT11_Init();
	
	while(1)
	{
		/* 调用DHT11_Read_TempAndHumidity读取温湿度信息 */
		if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
		{
			ILI9341_DispStringLine_EN(LINE(1),"DHT11 demo");
			
			/* 显示湿度 */
			sprintf(dispBuff,"Humidity: %d.%d",DHT11_Data.humi_int,DHT11_Data.humi_deci);
			LCD_ClearLine(LINE(2));
			ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			
			/* 显示温度 */
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
