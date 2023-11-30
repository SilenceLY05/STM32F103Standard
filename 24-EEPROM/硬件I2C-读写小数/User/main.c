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
#include "bsp_led.h"
#include "bsp_i2c_ee.h"
#include <string.h>

uint8_t cal_flag = 0;
uint8_t k;

/* �洢С�������������飬��7�� */
long double double_buffer[7] = {0};
int int_buffer[7] = {0};
char str1_buffer[7] = {'A','B','C','D','E','F','G'};
char str2_buffer[7] = {0};

#define DOUBLE_ADDR    10
#define LONGINT_ADDR   70
#define STRING_ADDR    100


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	LED_GPIO_Config();
	
	LED_BLUE;
	
	USART_Config();
	
	printf("����һ��I2C����(AT24C02)��дС���ͳ�������������\r\n");
	
	//��ʼ��I2C
  I2C_EE_Init();
	
	I2C_EE_BufferRead(&cal_flag,0,1);
	
	if(cal_flag != 0xCD)
	{
		printf("\r\n û�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ�� \r\n");
		cal_flag = 0xCD;
		
		/* д���־��0��ַ */
		I2C_EE_BufferWrite(&cal_flag,0,1);
		
		/* ����Ҫд������� */
		for( k = 0; k<7;k++)
		{
			double_buffer[k] = k+0.1;
			int_buffer[k] = k*500+1;
			str2_buffer[k] = str1_buffer[k];
		}
			/* д��С�����ݵ���ַ10 */
		I2C_EE_BufferWrite((void*)double_buffer,DOUBLE_ADDR,sizeof(double_buffer));
		
		/* д��С�����ݵ���ַ60 */
		I2C_EE_BufferWrite((void*)int_buffer,LONGINT_ADDR,sizeof(int_buffer));
		
		/* д��С�����ݵ���ַ60 */
		I2C_EE_BufferWrite((void*)str2_buffer,LONGINT_ADDR,sizeof(str2_buffer));
		
		printf("��оƬд������");
			
		for(k = 0;k<7;k++)
		{
			printf("С��tx = %LF\r\n",double_buffer[k]);
      printf("����tx = %d\r\n",int_buffer[k]);
			printf("�ַ�tx = %c\r\n",str2_buffer[k]);
		}
		printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");   
	
	}
	else
    {      
      	 printf("\r\n��⵽���ݱ�־\r\n");

				/*��ȡС������*/
        I2C_EE_BufferRead((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
				
				/*��ȡ��������*/
        I2C_EE_BufferRead((void*)int_buffer, LONGINT_ADDR, sizeof(int_buffer));
			/*��ȡ��������*/
        I2C_EE_BufferRead((void*)str2_buffer, LONGINT_ADDR, sizeof(str2_buffer));
	
			
				printf("\r\n��оƬ�������ݣ�\r\n");			
        for( k=0; k<7; k++ )
				{
					printf("С�� rx = %LF \r\n",double_buffer[k]);
					printf("���� rx = %d \r\n",int_buffer[k]);	
					printf("�ַ�tx = %c\r\n",str2_buffer[k]);					
				}
      
    }   
	
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
