/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_mpu6050.h"

//�����Ƿ�ʹ��LCD������ʾ������Ҫ�Ļ��������ע�͵�����
#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
 #include "./lcd/bsp_ili9341_lcd.h"
#endif


/* MPU6050���� */
short Acel[3];
short Gyro[3];
float Temp;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
#ifdef USE_LCD_DISPLAY
	char cStr[70];
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	
	ILI9341_Clear(0,0,240,320);
	ILI9341_DispStringLine_EN(LINE(1),"This is a MPU6050 demo");
#endif
	LED_GPIO_Config();
	
	USART_Config();
	
	i2c_GPIO_Config();
	
	MPU6050_Init();
	
	/* ��⵽MPU6050 */
	if(MPU6050ReadID() == 0)
	{
		printf("\r\n û�м�⵽MPU6050�������� \r\n");
		LED_RED;
		#ifdef USE_LCD_DISPLAY
		/* ����������ɫ������ı�����ɫ */
			LCD_SetColors(BLUE,BLACK);
			ILI9341_DispStringLine_EN(LINE(4),"No MPU6050 detected");
			ILI9341_DispStringLine_EN(LINE(5),"Please check the hardware connection!");
		#endif
		while(1);   // ��ⲻ���������Ȼ����
	}
	
	printf("\r\n ��ӭʹ��Ұ��  STM32 �����塣\r\n");		 
	printf("\r\n ����һ��MPU6050�������� \r\n");
	
	/* ����Systick��ʱ�����ж� */
	SysTick_Init();  //����SystickΪ1ms�ж�һ�Σ����ж����ȡ����������
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //������ʱ��

	
	while (1)
	{	
		if(task_readdata_finish)  //task_readdata_finish = 1 ��ʾ��ȡMPU6050�������
		{
			printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
			printf("    �����ǣ�%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
			printf("    �¶�%8.2f\r\n",Temp);
			
			#ifdef USE_LCD_DISPLAY
				ILI9341_DispStringLine_EN(LINE(7),"Acceleration");
				sprintf(cStr,"%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
				ILI9341_DispStringLine_EN(LINE(8),cStr);
			
				ILI9341_DispStringLine_EN(LINE(11),"Gyro       :");
				sprintf(cStr,"%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
				ILI9341_DispStringLine_EN(LINE(12),cStr);
			
				sprintf(cStr,"Temperture  :%8.2f",Temp);
				ILI9341_DispStringLine_EN(LINE(15),cStr);
			#endif
			
			task_readdata_finish = 0; //�����־λ
		}
		
	}
}


/*********************************************END OF FILE**********************/
