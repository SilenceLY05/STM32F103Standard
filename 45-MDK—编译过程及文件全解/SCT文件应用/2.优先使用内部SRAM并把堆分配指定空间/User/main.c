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
#include "bsp_led.h"
#include "bsp_usart.h"
#include <stdlib.h>
#include "bsp_sram.h"


void Delay(__IO uint32_t nCount); 

//�ؼ��� __attribute__((at(��ַ)))   __attribute__((section("������")))
//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM __attribute__((section("EXRAM")))


//�����������ָ���Ĵ洢�ռ䡱
uint32_t testValue __EXRAM = 7;
//��������ЧΪ��uint32_t testValue __attribute__((section("EXRAM"))) = 7;

////���������SRAM
uint32_t testValue2 = 7;

//�������鵽��ָ���Ĵ洢�ռ䡱
uint8_t testGroup[3] __EXRAM = {1,2,3};
////�������鵽SRAM
uint8_t testGroup2[3] = {1,2,3};



/* ��ʵ���е�sct���ã���ʹ���ⲿ�洢��ʱ�������������ܲ�������ʹ��malloc�޷��õ������ĵ�ַ�����Ƽ���ʵ�ʹ���ʹ�� */
/* ��һ���Ƽ��������ڲο��̳��е�˵�� */

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint32_t inertestValue = 10;
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	

	/* ��ʼ��USART */
	USART_Config();
	
	printf("\r\n SCT�ļ�Ӧ�á�������ʹ���ڲ�SRAM���Ѷѷ���ָ���ռ� ʵ�� \r\n");
	
	printf("\r\n ʹ�á�uint32_t inertestValue = 10;����䶨��ľֲ�����  \r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&inertestValue,inertestValue);
	
	printf("\r\n ʹ�á�uint32_t testValue __EXRAM= 7;����䶨��ľֲ�����  \r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue,testValue);
	
	printf("\r\n ʹ�á�uint32_t testValue2 = 7;����䶨��ľֲ�����  \r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue2,testValue2);
	
	printf("\r\n ʹ�á�uint32_t testGroup[3] __EXRAM = {1,2,3};����䶨��ľֲ�����  \r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d,%d,%d\r\n",(uint32_t)&testGroup,testGroup[0],testGroup[1],testGroup[2]);
	
	printf("\r\n ʹ�á�uint32_t testGroup2[3] = {0,1,2};����䶨��ľֲ�����  \r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d,%d,%d\r\n",(uint32_t)&testGroup2,testGroup2[0],testGroup2[1],testGroup2[2]);
	
	
	/* ʹ��malloc���ⲿSRAM�з���ռ� */
	uint32_t * pointer = (uint32_t *)malloc(sizeof(uint32_t)*3);
	if(pointer != NULL)
	{
		*(pointer) = 1;
		*(++pointer) = 2;
		*(++pointer) = 3;
		
		printf("\r\n ʹ�á�uint32_t * pointer = (uint32_t *)malloc(sizeof(uint32_t)*3);����̬����ı��� \r\n");
		printf("\r\n �����Ĳ���Ϊ�� \r\n*(pointer++) = 1;\r\n*(pointer++) = 2;\r\n*pointer = 3;\r\n");
		printf("��������������ĵ�ַΪ��0x%x���鿴����������\r\n",(uint32_t)pointer);
		printf("*(pointer--) = %d,\r\n",*(pointer--));
		printf("*(pointer--) = %d,\r\n",*(pointer--));
		printf("*(pointer) = %d,\r\n",*(pointer));
		
		free(pointer);
	}
	else
	{
		printf("\r\n ʹ��malloc��̬��������������� \r\n");
	}
	
	LED_BLUE;
	


	while (1)
	{
	
	}
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
