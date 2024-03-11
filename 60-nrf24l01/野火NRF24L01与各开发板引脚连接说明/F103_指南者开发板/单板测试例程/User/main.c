/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g����ģ��/nrf24l01+/���� ����  ֻʹ��һ�鿪���壬
	*					 ��2��NRFģ������շ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 ָ���߿����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"
#include "./key/bsp_key.h"

u8 status;	               // �����жϽ���/����״̬
u8 status2;                // �����жϽ���/����״̬
u8 txbuf[32];	             // ���ͻ���
u8 rxbuf[32];			         // ���ջ���
int i=0;

void Self_Test(void);

 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)                  
{   
	/* ��ʼ��NRF1 */
  SPI_NRF_Init();
	
	/* ��ʼ��NRF2 */
	SPI_NRF2_Init();

  /* ����1��ʼ�� */
  USART1_Config();
  
  /* ������ʼ�� */
  Key_GPIO_Config();

  printf("\r\n ����һ�� NRF24L01 ���ߴ���ʵ�� \r\n");
  printf("\r\n �������ߴ��� ������ �ķ�����Ϣ\r\n");
  printf("\r\n   ���ڼ��NRF��MCU�Ƿ��������ӡ�����\r\n");

	Self_Test();	

}

 /**
  * @brief  NRFģ����Ժ�����NRF1��NRF2֮��ѭ����������
  * @param  ��
  * @retval ��
  */
void Self_Test(void)
{
  /*��� NRF ģ���� MCU ������*/
  status = NRF_Check(); 

  /*�ж�����״̬*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF1��MCU���ӳɹ��� �� K1 ��������\r\n");  
  else	  
    printf("\r\n  NRF1��MCU����ʧ�ܣ������¼����ߡ�\r\n");
	
	
	/*��� NRF2 ģ���� MCU ������*/
  status = NRF2_Check();
	
	 /*�ж�����״̬*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF2��MCU���ӳɹ��� �� K2 ��������\r\n");  
  else	  
    printf("\r\n  NRF2��MCU����ʧ�ܣ������¼����ߡ�\r\n");

  NRF_RX_Mode();     // NRF1 �������ģʽ
  NRF2_RX_Mode();    // NRF2 �������ģʽ
  
  for(i=0;i<32;i++)
  {	
    txbuf[i] = i; 
  }

  while(1)
  {
    /* �ȴ� NRF1 �������� */
    status = NRF_Rx_Dat(rxbuf);

    /* �жϽ���״̬ */
    if(status == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n NRF1 ��������Ϊ��%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n�� K1 ʹ��NRF1 ��������\n"); 
      printf("\r\n�� K2 ʹ��NRF2 ��������\r\n"); 
    }
    
    /* NRF1 �������� */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)    // �������£���ʼ������
    { 
      /* �������� */
      NRF_TX_Mode();
           
      status = NRF_Tx_Dat(txbuf);
      
      /* �������ݵ�״̬ */
       if(status == TX_DS)
      {
        printf("\r\nNRF1 �������ݳɹ�\r\n");
      }
      else
      {
        printf("\r\nNRF1 ��������ʧ��  %d\r\n", status);
      }
      
      printf("\r\nNRF1 �������ģʽ\r\n"); 

      NRF_RX_Mode();
    }
    
    /* �ȴ� NRF2 �������� */
    status2 = NRF2_Rx_Dat(rxbuf);

    /* �жϽ���״̬ */
    if(status2 == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n NRF2 ��������Ϊ��%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n�� K1 ʹ��NRF1 ��������\n"); 
      printf("\r\n�� K2 ʹ��NRF2 ��������\r\n"); 
    }
    
    /* NRF2 �������� */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)    // ���� 2 ���£���ʼ������
    { 
      /* �������� */
      NRF2_TX_Mode();
           
      status2 = NRF2_Tx_Dat(txbuf);
      
      /* �������ݵ�״̬ */
       if(status2 == TX_DS)
      {
        printf("\r\nNRF2 �������ݳɹ�\r\n");
      }
      else
      {
        printf("\r\nNRF2 ��������ʧ��  %d\r\n", status);
      }
      
      printf("\r\nNRF2 �������ģʽ\r\n"); 

      NRF2_RX_Mode();
    }
  }
}
  


/*********************************************END OF FILE**********************/
