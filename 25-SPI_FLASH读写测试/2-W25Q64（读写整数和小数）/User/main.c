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
#include "bsp_spi_flash.h"

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;


//����ԭ������
void Delay(__IO uint32_t nCount);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint8_t cal_flag = 0;
	uint8_t k;
	
	/* �洢С�������������飬��7�� */
	long double double_buffer[7] = {0};
	int int_buffer[7] = {0};
	
	LED_GPIO_Config();
	LED_BLUE;
	
	/* ���ô���1Ϊ��115200 8-N-1 */
	USART_Config();
	
	printf("\r\n ����һ��FLASH��дС���ͳ�����ʵ�� \r\n");
	printf("\r\n ʹ��ָ���ߵװ�ʱ ���Ͻ�����λ�� ��Ҫ��PC0������ñ ��ֹӰ��PC0��SPIFLASHƬѡ�� \r\n");
	
	/* 16M����FLASH W25Q128��ʼ�� */
	SPI_FLASH_Init();
	
	/* ��ȡ Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	
	Delay( 200 );
	
	/* ��ȡSPI FLASH ID */
	FlashID = SPI_FLASH_ReadID();
	
	printf("\r\n Flash ID is 0x%X , manufacturer Device ID is 0x%X  \r\n",FlashID,DeviceID);
	
	/* ���SPI Flash ID */
	if(FlashID == sFLASH_ID)
	{
		LED_GREEN;
		printf("\r\n ��⵽SPI FLASH W25Q128 \r\n");
		
		/* ��ȡ���ݱ�־λ */
		SPI_FLASH_BufferRead(&cal_flag,SPI_FLASH_PageSize*0,1);
		
		if(cal_flag == 0xCD)/*����־����0xcd����ʾ֮ǰ����д������*/
		{
			printf("\r\n ��⵽���ݱ�־ \r\n");
			
			/* ��ȡС������ */
			SPI_FLASH_BufferRead((void*)double_buffer,SPI_FLASH_PageSize*1,sizeof(double_buffer));
			
			/* ��ȡ�������� */
			SPI_FLASH_BufferRead((void*)int_buffer,SPI_FLASH_PageSize*2,sizeof(int_buffer));
			
			printf("\r\n ��оƬ�������ݣ� \r\n");
			for(k = 0 ; k < 7 ; k++)
			{
				printf("С�� rx = %LF \r\n",double_buffer[k]);
				printf("���� rx = %d \r\n",int_buffer[k]);
			}
		}
		else
		{
			printf("\r\n û�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ�� \r\n");
			cal_flag = 0xCD;
			
			/* �������� */
			SPI_FLASH_SectorErase(0);
			
			/* д���־����0ҳ */
			SPI_FLASH_BufferWrite(&cal_flag,SPI_FLASH_PageSize*0,1);
			
			/* ����Ҫд������� */
			for( k=0 ; k <7 ; k++)
			{
				double_buffer[k] = k+0.1;
				int_buffer[k] = k*500+1;
			}
			/* д��С�����ݵ���һҳ */
			SPI_FLASH_BufferWrite((void*)double_buffer,SPI_FLASH_PageSize*1,sizeof(double_buffer));
			
			/* д���������ݵ��ڶ�ҳ */
			SPI_FLASH_BufferWrite((void*)int_buffer,SPI_FLASH_PageSize*2,sizeof(int_buffer));
			
			printf("\r\n ��оƬд������ \r\n");
			for(k = 0 ; k < 7 ; k++)
			{
				printf("С�� rx = %LF \r\n",double_buffer[k]);
				printf("���� rx = %d \r\n",int_buffer[k]);
			}
			printf("\r\n �븴λ�����壬�Զ�ȡ���ݽ��м�� \r\n");
		}
	}
	else
	{
		LED_RED;
		printf("\r\n ��ȡ����W25Q128 \r\n");
	}
	SPI_FLASH_PowerDown(); 
	while (1);
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}  
 


/*********************************************END OF FILE**********************/
