/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ�鷢�Ͷ��Ų��Գ���ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
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
#include "./usart/bsp_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h" 
#include "bsp_sdfs_app.h"
#include <string.h>


/* ���޸Ľ��ն��ŵĵ绰���� */
const char num[]="10086";

char namenum[20*4]={0},str[512]={0},gbkstr[1024]={0},namegbk[512]={0};

/*
 * ���Է��Ͷ��Ź��ܣ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
 * 
 */
int main(void)
{
	uint8_t IsRead=0;
	uint8_t testCard = 0;

//	static uint8_t timecount=0;
     /* ����USART */
    USART_Config();

    /* ����USART */
    GSM_USART_Config();
    
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��BH-GSMģ�鷢�Ͷ�������\r\n");
	  printf("\r\n����ǰ��ȷ��GSMģ���ѽ���绰��\r\n");
    printf("\r\n����ǰ���ڳ������޸Ľ��ն��ŵ�Ŀ���ֻ�����\r\n");

    //���ģ����Ӧ�Ƿ�����
    while(gsm_cmd("AT\r","OK",1000) != GSM_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
    }
    
    printf("\r\nͨ����ģ����Ӧ����\r\n");
    
		printf("\r\n���ڵȴ�GSMģ���ʼ��...\r\n");
		
		//���ö�����ʾ��ʽ
		if(gsm_cmd("AT+CNMI=2,1\r","OK", 100) != GSM_TRUE)
		{
						printf("AT+CNMI\r\n");
		}
		GSM_DELAY(1000);  
		
		//������ڴ洢�������˺��ܼ������ն��ţ�����ʹ�ô˺���ɾ����ע��������ȫ��ɾ��
//		if(gsm_cmd("AT+CMGDA=\"DEL ALL\"\r","OK", 500) != GSM_TRUE)
//		{
//						printf("AT+CMGDA\r\n");
//		}
//		GSM_DELAY(1000);  			
		
		
		//��ִ�д������ı�ģʽ
		if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
		{
			printf("\r\n�����ı�ģʽ����\r\n");
		}
		GSM_DELAY(1000); 	
		
		
		while(IsInsertCard() != GSM_TRUE)
		{
			
			if(++testCard >20)
			{
				printf("\r\n��ⲻ���绰������ϵ粢���½���绰��\r\n");
			}
			GSM_DELAY(1000); 		
		}		
		
		printf("\r\n��ʼ����ɣ�5���ʼ���Ͷ��Ų���... \r\n");

    /* ��ʱ5���ٷ������ģ�� */
    GSM_DELAY(5000);    
    
    //����Ӣ�Ķ���
		
	if(gsm_sms((char *)num,"hellow world") == GSM_TRUE)
		printf("\r\nӢ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
	else
	{
		printf("\r\n���ŷ���ʧ�ܣ���ȷ��Ŀ�������Ч");
	}
    GSM_DELAY(2000);    

	//��Ӣ�Ķ��ţ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
	if(gsm_sms((char *)num,"Ұ��WF-GSMģ����Ų���") == GSM_TRUE)
		printf("\r\n��Ӣ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
	else
	{
		printf("\r\n���ŷ���ʧ�ܣ���ȷ��Ŀ�������Ч");
		while(1);
	}		

	printf("\r\n���ն��Ų��ԣ�ͨ������1��ӡ�¶�����Ϣ\r\n");
		
	/*
	��ʾ���������Է��ֽ��ն���ʱ���ܻᱻ��ɺܶ�С�δ洢�������ڻ�ȡ��һ���¶�����ʾ�Ĵ洢��ź�ֱ��ӡ��
	��������ű���ɶ��ʱ����ʾ��ӡ����������������Ϣ���û����Ը����Լ������д����ϲ�ͬһ��������������Ϣ��	
	*/
  while(1)
	{
		GSM_DELAY(1000);
			
		if(IsReceiveMS_Test()==1)
		{
			
			for(int i=0; i<currentAddIndex; i++)
			{			
				IsRead=readmessage(address[i],namenum,str);				
				//printf("newmessadd=%d,IsRead:%d\n",newmessadd,IsRead);
				if(IsRead)
				{			
					hexuni2gbk(namenum,namegbk);	
					hexuni2gbk(str,gbkstr);	
					printf("%s\n",namegbk);
					printf("%s\n",gbkstr);	

					memset(str,0,sizeof(str));
					memset(str,0,sizeof(namegbk));
					memset(str,0,sizeof(gbkstr));						
				}
				
			}			
		}
		
	}
}



