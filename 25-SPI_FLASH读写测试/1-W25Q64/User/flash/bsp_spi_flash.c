#include "bsp_spi_flash.h"

static __IO uint32_t SPITimeout = SPIT_LONG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/* SPI��ʼ�� */
void SPI_FLASH_Init(void)
{
	SPI_InitTypeDef     SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ʹ��SPIʱ�� */
	FLASH_SPI_APBxClock_FUN (FLASH_SPI_CLK,ENABLE);
	
	/* ʹ��SPI������ص�ʱ�� */
	FLASH_SPI_CS_APBxClock_FUN (FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MISO_PIN|FLASH_SPI_MOSI_PIN,ENABLE);
	
	/* ����SPI��CS���ţ���ͨIO���� */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);
	
	/* ����SPI��SCK���� */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(FLASH_SPI_SCK_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MISO���� */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_Init(FLASH_SPI_MISO_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MOSI���� */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_Init(FLASH_SPI_MOSI_PORT,&GPIO_InitStructure);
	
	/* ֹͣ�ź�FLASH��CS���Ÿߵ�ƽ */
	SPI_FLASH_CS_HIGH();
	
	/* SPIģʽ���� */
	//FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//SPIʹ��ģʽ3   SCK����ʱΪ�ߵ�ƽ ����ʱ��Ϊż������
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(FLASH_SPIx, &SPI_InitStructure);
	
	/* ʹ��SPI */
	SPI_Cmd(FLASH_SPIx,ENABLE);
	
}

/* ����FLASH���� */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
	/* ����FLASHдʹ������ */
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	
	/* ѡ��FLASH��CS���ŵ͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ������������ָ�� */
	SPI_FLASH_SendByte(W25X_SectorErase);
	
	/* ���Ͳ���������ַ�ĸ�λ */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	
	/* ���Ͳ���������ַ����λ */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	
	/* ֹͣ�ź�FLASH��CS�ߵ�ƽ */
	SPI_FLASH_CS_HIGH();
	
	/* �ȴ�������� */
	SPI_FLASH_WaitForWriteEnd();
}

/* ����FLASH��������Ƭ���� */
void SPI_FLASH_BulkErase(void)
{
	/* ����FLASHдʹ������ */
	SPI_FLASH_WriteEnable();
	
	/* ѡ��FLASH��CS���ŵ͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ������������ָ�� */
	SPI_FLASH_SendByte(W25X_ChipErase);
	
	/* ֹͣ�ź�FLASH��CS�ߵ�ƽ */
	SPI_FLASH_CS_HIGH();
	
	/* �ȴ�������� */
	SPI_FLASH_WaitForWriteEnd();
	
}

/* ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ������� */
void SPI_FLASH_PageWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	SPI_FLASH_WriteEnable();
	/* ѡ��FLASH��CS���ŵ͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ������������ָ�� */
	SPI_FLASH_SendByte(W25X_PageProgram);
	
	/* ���Ͳ���������ַ�ĸ�λ */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	
	/* ���Ͳ���������ַ����λ */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);
	
	if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
	{
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
		FLASH_ERROR("SPI_FLASH_PageWrite too large!");
	}
	
	/* д������ */
	while(NumByteToWrite--)
	{
		/* ���͵�ǰҪд����ֽ�����*/
		SPI_FLASH_SendByte(*pBuffer);
		/* ָ����һ���ֽڻ����� */
		pBuffer++;
	}
	
	/* ֹͣ�ź�FLASH��CS�ߵ�ƽ */
	SPI_FLASH_CS_HIGH();
	
	SPI_FLASH_WaitForWriteEnd();

}

/* ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ������� */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0,Addr = 0, count = 0, temp = 0;
	
	/* mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0 */
	Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/* ��count������ֵ���պÿ��Զ��뵽ҳ��ַ */
	count = SPI_FLASH_PageSize - Addr;
	/* �����Ҫд����ҳ */
	NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
	/* mod�������࣬�����ʣ�಻��һҳ���ֽ��� */
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	
	/* Addr = 0,��WriteAddr�պð�ҳ����aligned */
	if(Addr == 0)
	{
		/* NumByteToWrite < SPI_FLASH_PageSize */
		if(NumOfPage == 0)
		{
			SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
		}
		else/* NumByteToWrite > SPI_FLASH_PageSize  */
		{
			while(NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,SPI_FLASH_PageSize);
				WriteAddr += SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			/* ���ж���Ĳ���һҳ�����ݣ�����д�� */
			SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumOfSingle);
		}
	}
	/* ����ַ��SPI_FLASH_PageSize������ */
	else
	{
		/* NumByteToWrite < SPI_FLASH_PageSize  */
		if(NumOfPage == 0)
		{
			/* ��ǰҳʣ���count��λ�ñ�NumOfSingleС��һҳд���� */
			if(NumOfPage > count)
			{
				temp = NumOfSingle - count;
				/*��д����ǰҳ*/
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
				
				WriteAddr += count;
				pBuffer += count;
				/*��дʣ�������*/
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,temp);
			}
			else/*��ǰҳʣ���count��λ����д��NumOfSingle������*/
			{
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
			}
		}
		else/* NumByteToWrite > SPI_FLASH_PageSize */
		{
			/* ��ַ�����룬�����count�ֿ������������������ */
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle= NumByteToWrite % SPI_FLASH_PageSize;
			
			/* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
			SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
			
			WriteAddr += count;
			pBuffer += count;
			
			while(NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,SPI_FLASH_PageSize);
				WriteAddr += SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
		}
	}
}

/* ��ȡFLASH���� */
void SPI_FLASH_BufferRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
	/* ѡ��FLASH��CS���ŵ͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ������������ָ�� */
	SPI_FLASH_SendByte(W25X_ReadData);
	
	/* ���Ͳ���������ַ�ĸ�λ */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	
	/* ���Ͳ���������ַ����λ */
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	/* ��ȡ���� */
	while(NumByteToRead--)
	{
		/* ��ȡһ���ֽ�*/
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
		/* ָ����һ���ֽڻ����� */
		pBuffer++;
	}
	
	/* ֹͣ�ź�FLASH��CS�ߵ�ƽ */
	SPI_FLASH_CS_HIGH();

}

/* ��ȡFLASH_ID */
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t Temp = 0 , Temp0 = 0 , Temp1 = 0 , Temp2 = 0;
	
	/* ��ʼͨѶ��CS�͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ����JEDECָ���ȡID */
	SPI_FLASH_SendByte(W25X_JedecDeviceID);
	
	/* ��ȡһ���ֽ����� */
	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* ��ȡһ���ֽ����� */
	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* ��ȡһ���ֽ����� */
	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* ֹͣͨѶ��CS�ߵ�ƽ */
	SPI_FLASH_CS_HIGH();
	
	/* �����������������Ϊ��������ֵ */
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	
	return Temp;
	
}

/* ��ȡFLASH���豸ID */
uint32_t SPI_FLASH_ReadDeviceID(void)
{
	uint32_t Temp = 0;
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte(W25X_DeviceID);
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);
	
	Temp = SPI_FLASH_SendByte(Dummy_Byte);
	
	SPI_FLASH_CS_HIGH();
	return Temp;
	
}	

void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
	/* ѡ��FLASH��CS���ŵ͵�ƽ */
	SPI_FLASH_CS_LOW();
	
	/* ������������ָ�� */
	SPI_FLASH_SendByte(W25X_ReadData);
	
	/* ���Ͳ���������ַ�ĸ�λ */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	
	/* ���Ͳ���������ַ����λ */
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
}

/* ʹ��SPI��ȡһ���ֽڵ����� */
uint8_t SPI_FLASH_ReadByte(void)
{
	return (SPI_FLASH_SendByte(Dummy_Byte));
}

/* ʹ��SPI����һ���ֽڵ����ݣ����ؽ��յ������� */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* �ȴ��������ݻ�����Ϊ�գ�TXEʱ�� */
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	/* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
	SPI_I2S_SendData(FLASH_SPIx,byte);
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* �ȴ����ջ������ǿգ�RXNE�¼� */
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	
	/* ��ȡ���ݼĴ�������ȡ���ջ��������� */
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord)
{
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* �ȴ��������ݻ�����Ϊ�գ�TXEʱ�� */
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	/* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
	SPI_I2S_SendData(FLASH_SPIx,HalfWord);
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* �ȴ����ջ������ǿգ�RXNE�¼� */
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	
	/* ��ȡ���ݼĴ�������ȡ���ջ��������� */
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

void SPI_FLASH_WriteEnable(void)
{
	/* ͨѶ��ʼ��CS�� */
	SPI_FLASH_CS_LOW();
	
	/* ����дʹ������ */
	SPI_FLASH_SendByte(W25X_WriteEnable);
	
	/* ͨѶ������CS�� */
	SPI_FLASH_CS_HIGH();
}

/* �ȴ�WIP��BUSY����־����0�����ȴ���FLASH�ڲ�����д����� */
void SPI_FLASH_WaitForWriteEnd(void)
{
	u8 FLASH_Status = 0;
	
	/* ѡ��FLASH��CS�� */
	SPI_FLASH_CS_LOW();
	
	/* ���Ͷ�״̬�Ĵ������� */
	SPI_FLASH_SendByte(W25X_ReadStatusReg);
	
	/* ��FLASHæµ����ȴ� */
	do
	{
		/* ��ȡFLASHоƬ��״̬�Ĵ��� */
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
	}
	while ((FLASH_Status & WIP_Flag) == SET);  //����д���־
	
	/* ֹͣ�ź� FLASH��CS�� */
	SPI_FLASH_CS_HIGH();
}

/* ����ģʽ */
void SPI_FLASH_PowerDown(void)
{
	SPI_FLASH_CS_LOW();
	
	/* ���͵������� */
	SPI_FLASH_SendByte(W25X_PowerDown);
	
	SPI_FLASH_CS_HIGH();
}

/* ����ģʽ */
void SPI_FLASH_WAKEUP(void)
{
	SPI_FLASH_CS_LOW();
	
	/* ���͵������� */
	SPI_FLASH_SendByte(W25X_ReleasePowerDown);
	
	SPI_FLASH_CS_HIGH();
}

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
	/* �ȴ���ʱ��Ĵ������������Ϣ */
	FLASH_ERROR("SPI�ȴ���ʱ��errorCode = %d",errorCode);
	return 0;
}

