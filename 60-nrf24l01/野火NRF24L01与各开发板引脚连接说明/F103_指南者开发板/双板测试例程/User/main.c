/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g����ģ��/nrf24l01+/slave ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO-V3 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"
#include "./key/bsp_key.h"  

u8 status = 0;		 //�����жϽ���/����״̬
u8 txbuf[32];	 //���ͻ���
u8 rxbuf[32];	 //���ջ���
u8 i; 

 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{    
  /* ����1��ʼ�� */
  USART1_Config(); 

  /*SPI�ӿڳ�ʼ��*/   
  SPI_NRF_Init(); 
  
  /* ������ʼ�� */
  
  Key_GPIO_Config();

  printf("\r\n ����һ�� NRF24L01 ���ߴ���ʵ�� \r\n");
  printf("\r\n �������ߴ��� �ӻ��� �ķ�����Ϣ\r\n");
  printf("\r\n   ���ڼ��NRF��MCU�Ƿ��������ӡ�����\r\n");

  /*���NRFģ����MCU������*/
  status = NRF_Check();   		
  if(status == SUCCESS)	   
    printf("\r\n      NRF��MCU���ӳɹ�\r\n");  
  else	  
    printf("\r\n   ����NRF��MCU�Ƿ��������ӡ�����\r\n");
  
  printf("\r\n NRF�������ģʽ���� K1 ��������\r\n"); 


  for(i=0;i<32;i++)
  {	
    txbuf[i] = i; 
  }
  
  NRF_RX_Mode();     // NRF �������ģʽ
  
  while(1)
  {  		 	
    /* �ȴ��������� */
    status = NRF_Rx_Dat(rxbuf);

    /* �жϽ���״̬ */
    if(status == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n ��������Ϊ��%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n�������ģʽ���� K1 ��������\r\n"); 
    }
     
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)    // �������£���ʼ������
    { 
      /* �������� */
      NRF_TX_Mode();
           
      status = NRF_Tx_Dat(txbuf);
      
      /* �������ݵ�״̬ */
       if(status == TX_DS)
      {
        printf("\r\n�������ݳɹ�\r\n");
      }
      else
      {
        printf("\r\n��������ʧ��  %d\r\n", status);
      }
      
      printf("\r\n �������ģʽ���� K1 ��������\r\n"); 

      NRF_RX_Mode();
    }
  } 
}
/*********************************************END OF FILE**********************/
