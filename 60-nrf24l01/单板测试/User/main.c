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
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_spi_nrf.h"

uint8_t status;
uint8_t status2;
uint8_t txBuffer[32];
uint8_t rxBuffer[32];

int i = 0;

void Self_Test(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ��ʼ��NRF1 */
	SPI_NRF_Config();
	/* ��ʼ��NRF2 */
	SPI_NRF2_Config();
	/* ���ڳ�ʼ�� */
	USART_Config();
	/* ������ʼ�� */
	KEY_GPIO_Config();
	
	printf("\r\n ����һ�� NRF24L01 ���ߴ���ʵ�� \r\n");
  printf("\r\n �������ߴ��� ������ �ķ�����Ϣ\r\n");
  printf("\r\n   ���ڼ��NRF��MCU�Ƿ��������ӡ�����\r\n");

	Self_Test();
	
}

/* NRFģ����Ժ�����NRF1��NRF2֮��ѭ���������� */
void Self_Test(void)
{
	/* ���NRF1ģ���MCU���� */
	status = NRF_Check();
	
	/* �ж�����״̬ */
	if(status == SUCCESS)
		printf("\r\n NRF1��MCU���ӳɹ��� ��K1�������� \r\n");
	else
		printf("\r\n NRF1��MCU����ʧ�ܣ������¼������ \r\n");
	
	/* ���NRF2ģ���MCU���� */
	status = NRF2_Check();
	
	/* �ж�����״̬ */
	if(status == SUCCESS)
		printf("\r\n NRF2��MCU���ӳɹ��� ��K2�������� \r\n");
	else
		printf("\r\n NRF2��MCU����ʧ�ܣ������¼������ \r\n");
	
	NRF_RX_Mode();       //NRF1�������ģʽ
	NRF2_RX_Mode();      //NRF2�������ģʽ
	
	for(i=0;i<32;i++)
	{
		txBuffer[i] = i;
	}
	
	while(1)
	{
		/* �ȵ�NRF1�������� */
		status = NRF_RX_Data(rxBuffer);
		
		/* �жϽ���״̬ */
		if(status == RX_DR)
		{
			for(i=0;i<32;i++)
			{
				printf("\r\n NRF1���յ�����Ϊ��%d \r\n",rxBuffer[i]);
			}
			
			printf("\r\n ��K1��ʹ��NRF1�������� \r\n");
			printf("\r\n ��K2��ʹ��NRF2�������� \r\n");
		}
		
		/* ʹ��NRF1�������� */
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)  // �������£���ʼ������
		{
			/* �������� */
			NRF_TX_Mode();
			
			status = NRF_TX_Data(txBuffer);
			
			/* ��������״̬ */
			if(status == TX_DS)
			{
				printf("\r\n �������ݳɹ� \r\n");
			}
			else
			{
				printf("\r\n ��������ʧ��   %d \r\n",status);
			}
			
			printf("\r\n �������ģʽ \r\n");
			NRF_RX_Mode();
		}
		
		/* �ȴ�NRF2�������� */
		status2 = NRF2_RX_Data(rxBuffer);
		
		/* �жϽ���״̬ */
		if(status2 == RX_DR)
		{
			for(i=0;i<32;i++)
			{
				printf("\r\n NRF2���յ�����Ϊ��%d \r\n",rxBuffer[i]);
			}
			
			printf("\r\n ��K1��ʹ��NRF1�������� \r\n");
			printf("\r\n ��K2��ʹ��NRF2�������� \r\n");
		}
		
		/* NRF2�������� */  
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)   // ���� 2 ���£���ʼ������
		{
			/* �������� */
			NRF2_TX_Mode();
			
			status2 = NRF2_TX_Data(txBuffer);
			
			/* ��������״̬ */
			if(status2 == TX_DS)
			{
				printf("\r\n NRF2�������ݳɹ� \r\n");
			}
			else
			{
				printf("\r\n NRF2��������ʧ��   %d \r\n",status2);
			}
			
			printf("\r\n NRF2 �������ģʽ \r\n");
			NRF2_RX_Mode();
		}
			
		
	}
}


/*********************************************END OF FILE**********************/
