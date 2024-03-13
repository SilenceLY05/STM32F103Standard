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
#include "bsp_usart.h"
#include "bsp_usart_blt.h"
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_hc05.h"
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "core_delay.h"


extern ReceiveData  BLT_USART_ReceiveData;
extern ReceiveData DEBUG_USART_ReceiveData;
extern int hc05_inquery_connect;

void TransData_CtrlLED_Test(void);
	


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	char hc05_name[30] = "HC05_SLAVE";
	char hc05_nameCMD[40];
	
	/* ��ʱ������ʼ�� */
	CPU_TS_TmrInit();

	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	#endif
	
	Debug_USART_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();

	/* HC05����ģ���ʼ��GPIO��USART2����Ϊ38400 8-N-1�����ж� */
	if(HC05_Init())
	{
		HC05_INFO("HC05ģ��������");
		
		#ifdef ENABLE_LCD_DISPLAY
		ILI9341_DispString_EN(40,40,"HC05 module detected!");
		#endif
	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪����֮������ӣ�Ȼ��λ�����²��ԣ�");
		#ifdef ENABLE_LCD_DISPLAY
		ILI9341_DispString_EN(20,40,"No HC05 module detected!");
		ILI9341_DispString_EN(5,60,"Please check the hardware connection and reset the system.");
		#endif
		
		while(1);
	}


	HC05_INFO("HC05ģ��ʵ��");
	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_DispString_EN(40,20,"HC05 BLUETOOTH demo");
	#endif
	
	/* ��λ �ָ�Ĭ��״̬ */
	HC05_Send_CMD("AT+RESET\r\n",1);  //��λָ������֮����Ҫһ��ʱ��HC05�Ż������һ��ָ��
	HC05_Send_CMD("AT+ORGL\r\n",1);
	
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/
	HC05_Send_CMD("AT+VERSON?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+START?\r\n",1);
	
	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	//��ʼ��SPP�淶
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	//����ģ������
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);
	
	HC05_INFO("��ģ������Ϊ:%s,ģ����׼��������",hc05_name);
	#ifdef ENABLE_LCD_DISPLAY
	ILI9341_DispStringLine_EN(LINE(4),"ReceiveData USART1");
	ILI9341_DispStringLine_EN(LINE(9),"ReceiveData HC-05");
	#endif
	
	while(1)
	{
		TransData_CtrlLED_Test();  //��������
	}
	
	
}

void TransData_CtrlLED_Test(void)
{
	//������Դ��ڽ��յ��Ĵ�����������
	if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
	{
		DEBUG_USART_ReceiveData.usart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
		
		//����������Լ�������Ҫ���յ��ַ���Ȼ����
    //������յ����ڵ������ַ����� ��RED_LED���ͻ�Ѱ�������ĺ��ȡ��һ��
		if(strstr((char *)DEBUG_USART_ReceiveData.usart_buff,"RED_LED"))
		{
			LED1_TOGGLE;
		}
		
		//����ֻ�����������ģ�飬���͵��ֻ�����ʾ����
		if(IS_HC05_CONNECTED )
		{
			BLT_KEY_LOW;
			USART_SendStr_Length(BLT_USARTx,DEBUG_USART_ReceiveData.usart_buff,DEBUG_USART_ReceiveData.datanum);
		}
		//�����������AT��ͷ�ģ��Ͱ�KEY�øߣ���������ģ��
		else if(strstr((char *)DEBUG_USART_ReceiveData.usart_buff,"AT") == (char *)DEBUG_USART_ReceiveData.usart_buff)
		{
			BLT_KEY_HIGHT;
			
			USART_SendStr_Length(BLT_USARTx,DEBUG_USART_ReceiveData.usart_buff,DEBUG_USART_ReceiveData.datanum);
			USART_SendStr_Length(BLT_USARTx,"\r\n",2);
			
			BLT_KEY_LOW;
		}
		
		//����������ʾ���յ�������
		USART_SendString(DEBUG_USARTx,"\r\n receive USART1 data: \r\n");
		USART_SendString(DEBUG_USARTx,DEBUG_USART_ReceiveData.usart_buff);
		USART_SendString(DEBUG_USARTx,"\r\n");
		
		//LCD��ʾ���յ�������
		#ifdef  ENABLE_LCD_DISPLAY
		LCD_ClearLine(LINE(5));
		LCD_ClearLine(LINE(6));
		LCD_ClearLine(LINE(7));
		LCD_ClearLine(LINE(8));
		
		ILI9341_DispStringLine_EN(LINE(5),(char*)DEBUG_USART_ReceiveData.usart_buff);
		
		#endif
		
		//������Դ������ݻ���
		DEBUG_USART_ReceiveData.receive_data_flag = 0;
		DEBUG_USART_ReceiveData.datanum = 0;
		
	}
	
	//������Դ��ڽ��յ��Ĵ�����������
	if(BLT_USART_ReceiveData.receive_data_flag == 1)
	{
		BLT_USART_ReceiveData.usart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
		
		//����������Լ�������Ҫ���յ��ַ���Ȼ����
    //������յ��ֻ����������� ��RED_LED���ͻ�Ѱ�������ĺ��ȡ��һ��
    if(strstr((char *)BLT_USART_ReceiveData.usart_buff,"RED_LED"))
    {
      LED1_TOGGLE;
    }
		
		//����������ʾ���յ�������
		USART_SendString(DEBUG_USARTx,"\r\n receive HC05 data: \r\n");
		USART_SendString(DEBUG_USARTx,BLT_USART_ReceiveData.usart_buff);
		USART_SendString(DEBUG_USARTx,"\r\n");
		
		//LCD��ʾ���յ�������
		#ifdef  ENABLE_LCD_DISPLAY
		LCD_ClearLine(LINE(10));
		LCD_ClearLine(LINE(11));
		LCD_ClearLine(LINE(12));
		LCD_ClearLine(LINE(13));
		
		ILI9341_DispStringLine_EN(LINE(10),(char*)BLT_USART_ReceiveData.usart_buff);
		
		#endif
		
		//������Դ������ݻ���
		BLT_USART_ReceiveData.receive_data_flag = 0;
		BLT_USART_ReceiveData.datanum = 0;
		
	}
}





/*********************************************END OF FILE**********************/
