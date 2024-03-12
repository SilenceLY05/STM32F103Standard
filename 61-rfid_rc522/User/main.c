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
#include "bsp_systick.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_key.h"
#include <stdbool.h>
#include "rc522_config.h"
#include "rc522_function.h"


uint8_t KeyValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/* IC���Ժ��� */
void IC_Test(void)
{
	uint32_t writeValue = 100;
	uint32_t readValue;
	char cStr[30];
	uint8_t ucArray_ID[4];
	uint8_t ucStatusReturn;
	
	while(1)
	{
		if((ucStatusReturn = PcdRequest(PICC_REQALL,ucArray_ID)) != MI_OK)
		{
			/*��ʧ���ٴ�Ѱ��*/
			printf("Ѱ��ʧ��\r\n");
			ucStatusReturn = PcdRequest(PICC_REQALL,ucArray_ID);
		}
		if(ucStatusReturn == MI_OK)
		{
			printf("Ѱ���ɹ�\r\n");
			
			/*����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
			if(PcdAnticoll(ucArray_ID) == MI_OK)
			{
				PcdSelect(ucArray_ID);
				
				PcdAuthState(PICC_AUTHENT1A,0x11,KeyValue,ucArray_ID);   //У������ 
				WriteAmount(0x11,writeValue);  //д����
				
				if(ReadAmount(0x11,&readValue) == MI_OK)//��ȡ���
				{
					sprintf(cStr,"The Card ID is: %02X%02X%02X%02X",ucArray_ID[0],ucArray_ID[1],ucArray_ID[2],ucArray_ID[3]);
					printf("%s\r\n",cStr);
					ILI9341_DispString_EN(0,0,cStr);
					
					printf("���Ϊ��%d\r\n",readValue);
					sprintf(cStr,"TThe residual amount: %d",readValue);
					ILI9341_DispString_EN(0,16,cStr);
					PcdHalt();
					
				}
				
				if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
				{
					printf("��Ǯ + 100 \r\n");
					writeValue += 100;
				}
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
				{
					printf("��Ǯ -100 \r\n");
					writeValue -= 100;
				}
			}
		}
	}
}

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	/*�δ�ʱ�ӳ�ʼ��*/
	SysTick_Init (); 
	
	Debug_USART_Config();
	
	/* Key ��ʼ�� */
  KEY_GPIO_Config();
	
	/*RC522ģ����������ĳ�ʼ������*/
	RC522_Init ();     
	
   /*LCD ��ʼ��*/
	ILI9341_Init(); 

	printf("WF-RC522 Test \r\n");

	ILI9341_Clear ( 0,   0, 240, 320);	
	ILI9341_Clear ( 0, 272, 240,   2);
	ILI9341_DispString_EN ( 0, 280, 
      "Please put the IC card on WF-RC522 antenna area ..." );
	PcdReset ();
  
  /*���ù�����ʽ*/
	M500PcdConfigISOType ( 'A' );
	
	
	while(1)
	{
		/*IC�����*/
    IC_Test (); 
	}
}


/* ------------------------------------------end of file---------------------------------------- */

