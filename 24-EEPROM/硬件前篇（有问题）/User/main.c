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

uint8_t readData [10] = {0};

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	USART_Config();
	
	printf("����һ��IICͨ��ʵ��\n");
	
	//��ʼ��I2C
  I2C_EE_Init();
	
	//д��һ���ֽ�
	I2C_EE_ByteWrite(0x55, 11);
	
	EEPROM_WaitForWriteEnd();
	
	//��ȡ����
	I2C_EE_SeqRead(readData,11,1);
	
	printf("\r\n���յ�������Ϊ0x%x\r\n",readData[0]);
	
	
	
	while (1)
	{	
	}
}


/*********************************************END OF FILE**********************/
