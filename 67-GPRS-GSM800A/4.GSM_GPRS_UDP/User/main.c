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
#include "bsp_key.h"
#include "bsp_SysTick.h"
#include "bsp_gsm_usart.h"
#include "bsp_gsm_gprs.h"
#include <string.h>


#define LOCALPORT "2000"
#define SERVERIP  "163.142.57.125"

//#define SERVERPORT "5000"
#define SERVERPORT "4000"

const char *TESTBUFF1 = "\r\n1.Ұ��GSMģ��TCP�����ϴ����ܲ���";
const char *TESTBUFF2 = "\r\n2.Ұ��GSMģ��TCP�����ϴ����ܲ���";


/* ϵͳ�����λ */
void Soft_Reset(void)
{
	__set_FAULTMASK(1);    /* �ر������ж�*/  
	NVIC_SystemReset();    /* ϵͳ��λ */
}


	
/* ���Է��Ͷ��Ź��ܣ�ʵ�ʲ���ʱ�ѵ绰�����޸ĳ�Ҫ���ն��ŵĵ绰���� */
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	static uint8_t timecount =1,timestop=0;
	
	uint8_t testCard=0;
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	SysTick_Init();
	
	 printf("\r\nҰ��WF-GSMģ��UDP�շ�����\r\n");
	
	//���ģ���Ƿ���Ӧ����
	while(gsm_init() != GSM_TRUE)
	{
		printf("\r\n ģ����Ӧ���Բ��������� \r\n");
		printf("\r\n ��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ���� \r\n");
		GSM_DELAY(1000);
	}
	printf("\r\n ���ڵȴ�GSMģ���ʼ��... \r\n");
	
	while(IsInserCard()!= GSM_TRUE)
	{
		if(++testCard>20)
		{
			printf("\r\n ��ⲻ���绰������ϵ粢���½���绰�� \r\n");
		}
		GSM_DELAY(1000);
	}
	
	printf("\r\n ��ʼ����ɣ�5���ʼ�Ŷ��Ų���... \r\n");
	
	/* ��ʱ5���ڷ������ģ�� */
	GSM_DELAY(5000);
	
	//ȷ�Ϲر�֮ǰ������
	gsm_gprs_link_close();
	
	GSM_DELAY(100);
	
	//ȷ�Ϲر�֮ǰ�ĳ���
	gsm_gprs_shut_close();
	
	printf("\n��ʼ��GPRS��������...\n");
	
	//���³�ʼ��gprs
	if(gsm_gprs_init()!=GSM_TRUE)   //GPRS��ʼ������
	{
		printf("\r ��ʼ��GPRSʧ�ܣ������¸�ģ���ϵ粢��λ������ \n");
		while(1);
	}
	
	printf("\n ���Խ���UDP���ӣ������ĵȴ�... \n");
	
	if(gsm_gprs_udp_link(LOCALPORT,SERVERIP,SERVERPORT) != GSM_TRUE)
	{
		printf("\r\n TCP����ʧ�ܣ�������ȷ���ø���ģ��... \r\n");
		GSM_DELAY(1000);
		printf("\n IP���ӶϿ� \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n �رճ��� \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		printf("\r\n 5����Զ����� \r\n");
		GSM_DELAY(5000);
		Soft_Reset();
	}
	
	printf("\n ���ӳɹ�,���Է�������... \n");
	printf("\r\n ע�⣺�����ԣ�ʹ��UDP��������ʱ�ӵڶ������ݿ�ʼ���ܱ����յ� \r\n");
	if(gsm_gprs_send(TESTBUFF1) != GSM_TRUE)
	{
		printf("\r\n ��������ʧ�ܣ�������ȷ���ø���ģ��... \r\n");
		GSM_DELAY(1000);
		printf("\n IP���ӶϿ� \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n �رճ��� \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n ���Է��͵ڶ�������... \n");
	if(gsm_gprs_send(TESTBUFF2) != GSM_TRUE)
	{
		printf("\r\n ��������ʧ�ܣ�������ȷ���ø���ģ��... \r\n");
		GSM_DELAY(1000);
		printf("\n IP���ӶϿ� \n");
		GSM_DELAY(100);
		gsm_gprs_link_close();
		
		printf("\r\n �رճ��� \r\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n ���͵ڶ������ݳɹ� \n");
	
	printf("\n ��׼���ý���Զ�����ݣ���ʹ���������������ģ�鷢������ \n");
	
	GSM_CLEAN_RX();
	
	while(1)
	{
		if((timecount>=50) &&(timestop != 0xFF))
		{
			if(PostGPRS()!= GSM_TRUE)
				timestop ++;
			else
				timestop = 0;
			timecount = 0;
		}
		if(timestop == 120)
		{
			printf("\n ��ʱ����ͨѶ�������ر����� \n");
			
			printf("\n IP���ӶϿ� \n");
			GSM_DELAY(100);
			gsm_gprs_link_close();
			
			printf("\n �رճ��� \n");
			GSM_DELAY(100);
			gsm_gprs_shut_close();
			
			GSM_DELAY(100);
			timestop = 0xFF;
		}
		timecount++;
		GSM_DELAY(10);
	}
	
}


/*********************************************END OF FILE**********************/
