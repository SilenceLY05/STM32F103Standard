#include "bsp_spi_flash.h"

static __IO uint32_t SPITimeout = SPIT_LONG_TIMEOUT;
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/* SPI初始化 */
void SPI_FLASH_Init(void)
{
	SPI_InitTypeDef     SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能SPI时钟 */
	FLASH_SPI_APBxClock_FUN (FLASH_SPI_CLK,ENABLE);
	
	/* 使能SPI引脚相关的时钟 */
	FLASH_SPI_CS_APBxClock_FUN (FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MISO_PIN|FLASH_SPI_MOSI_PIN,ENABLE);
	
	/* 配置SPI的CS引脚，普通IO即可 */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);
	
	/* 配置SPI的SCK引脚 */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(FLASH_SPI_SCK_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MISO引脚 */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_Init(FLASH_SPI_MISO_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_Init(FLASH_SPI_MOSI_PORT,&GPIO_InitStructure);
	
	/* 停止信号FLASH：CS引脚高电平 */
	SPI_FLASH_CS_HIGH();
	
	/* SPI模式配置 */
	//FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//SPI使用模式3   SCK空闲时为高电平 采样时刻为偶数边沿
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(FLASH_SPIx, &SPI_InitStructure);
	
	/* 使能SPI */
	SPI_Cmd(FLASH_SPIx,ENABLE);
	
}

/* 擦除FLASH扇区 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
	/* 发送FLASH写使能命令 */
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	
	/* 选择FLASH：CS引脚低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送扇区擦除指令 */
	SPI_FLASH_SendByte(W25X_SectorErase);
	
	/* 发送擦除扇区地址的高位 */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	
	/* 发送擦除扇区地址的中位 */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	
	/* 发送擦除扇区地址的低位 */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	
	/* 停止信号FLASH：CS高电平 */
	SPI_FLASH_CS_HIGH();
	
	/* 等待擦除完毕 */
	SPI_FLASH_WaitForWriteEnd();
}

/* 擦除FLASH扇区，正片擦除 */
void SPI_FLASH_BulkErase(void)
{
	/* 发送FLASH写使能命令 */
	SPI_FLASH_WriteEnable();
	
	/* 选择FLASH：CS引脚低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送扇区擦除指令 */
	SPI_FLASH_SendByte(W25X_ChipErase);
	
	/* 停止信号FLASH：CS高电平 */
	SPI_FLASH_CS_HIGH();
	
	/* 等待擦除完毕 */
	SPI_FLASH_WaitForWriteEnd();
	
}

/* 对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区 */
void SPI_FLASH_PageWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	SPI_FLASH_WriteEnable();
	/* 选择FLASH：CS引脚低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送扇区擦除指令 */
	SPI_FLASH_SendByte(W25X_PageProgram);
	
	/* 发送擦除扇区地址的高位 */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	
	/* 发送擦除扇区地址的中位 */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	
	/* 发送擦除扇区地址的低位 */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);
	
	if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
	{
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
		FLASH_ERROR("SPI_FLASH_PageWrite too large!");
	}
	
	/* 写入数据 */
	while(NumByteToWrite--)
	{
		/* 发送当前要写入的字节数据*/
		SPI_FLASH_SendByte(*pBuffer);
		/* 指向下一个字节缓冲区 */
		pBuffer++;
	}
	
	/* 停止信号FLASH：CS高电平 */
	SPI_FLASH_CS_HIGH();
	
	SPI_FLASH_WaitForWriteEnd();

}

/* 对FLASH写入数据，调用本函数写入数据前需要先擦除扇区 */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0,Addr = 0, count = 0, temp = 0;
	
	/* mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0 */
	Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/* 差count个数据值，刚好可以对齐到页地址 */
	count = SPI_FLASH_PageSize - Addr;
	/* 计算出要写多少页 */
	NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
	/* mod运算求余，计算出剩余不满一页的字节数 */
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	
	/* Addr = 0,则WriteAddr刚好按页对齐aligned */
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
			/* 若有多余的不满一页的数据，把它写完 */
			SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumOfSingle);
		}
	}
	/* 若地址与SPI_FLASH_PageSize不对齐 */
	else
	{
		/* NumByteToWrite < SPI_FLASH_PageSize  */
		if(NumOfPage == 0)
		{
			/* 当前页剩余的count个位置比NumOfSingle小，一页写不完 */
			if(NumOfPage > count)
			{
				temp = NumOfSingle - count;
				/*先写满当前页*/
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
				
				WriteAddr += count;
				pBuffer += count;
				/*再写剩余的数据*/
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,temp);
			}
			else/*当前页剩余的count个位置能写完NumOfSingle个数据*/
			{
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,NumByteToWrite);
			}
		}
		else/* NumByteToWrite > SPI_FLASH_PageSize */
		{
			/* 地址不对齐，多出的count分开处理，不加入这个运算 */
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle= NumByteToWrite % SPI_FLASH_PageSize;
			
			/* 先写完count个数据，为的是让下一次要写的地址对齐 */
			SPI_FLASH_PageWrite(pBuffer,WriteAddr,count);
			
			WriteAddr += count;
			pBuffer += count;
			
			while(NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer,WriteAddr,SPI_FLASH_PageSize);
				WriteAddr += SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
		}
	}
}

/* 读取FLASH数据 */
void SPI_FLASH_BufferRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
	/* 选择FLASH：CS引脚低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送扇区擦除指令 */
	SPI_FLASH_SendByte(W25X_ReadData);
	
	/* 发送擦除扇区地址的高位 */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	
	/* 发送擦除扇区地址的中位 */
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	
	/* 发送擦除扇区地址的低位 */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	/* 读取数据 */
	while(NumByteToRead--)
	{
		/* 读取一个字节*/
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
		/* 指向下一个字节缓冲区 */
		pBuffer++;
	}
	
	/* 停止信号FLASH：CS高电平 */
	SPI_FLASH_CS_HIGH();

}

/* 读取FLASH_ID */
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t Temp = 0 , Temp0 = 0 , Temp1 = 0 , Temp2 = 0;
	
	/* 开始通讯：CS低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送JEDEC指令，读取ID */
	SPI_FLASH_SendByte(W25X_JedecDeviceID);
	
	/* 读取一个字节数据 */
	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* 读取一个字节数据 */
	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* 读取一个字节数据 */
	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* 停止通讯：CS高电平 */
	SPI_FLASH_CS_HIGH();
	
	/* 把数据组合起来，作为函数返回值 */
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	
	return Temp;
	
}

/* 读取FLASH的设备ID */
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
	/* 选择FLASH：CS引脚低电平 */
	SPI_FLASH_CS_LOW();
	
	/* 发送扇区擦除指令 */
	SPI_FLASH_SendByte(W25X_ReadData);
	
	/* 发送擦除扇区地址的高位 */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	
	/* 发送擦除扇区地址的中位 */
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	
	/* 发送擦除扇区地址的低位 */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
}

/* 使用SPI读取一个字节的数据 */
uint8_t SPI_FLASH_ReadByte(void)
{
	return (SPI_FLASH_SendByte(Dummy_Byte));
}

/* 使用SPI发送一个字节的数据，返回接收到的数据 */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* 等待发送数据缓冲区为空，TXE时间 */
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	SPI_I2S_SendData(FLASH_SPIx,byte);
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* 等待接收缓冲区非空，RXNE事件 */
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	
	/* 读取数据寄存器，获取接收缓冲区数据 */
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord)
{
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* 等待发送数据缓冲区为空，TXE时间 */
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	SPI_I2S_SendData(FLASH_SPIx,HalfWord);
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	/* 等待接收缓冲区非空，RXNE事件 */
	while (SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	
	/* 读取数据寄存器，获取接收缓冲区数据 */
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

void SPI_FLASH_WriteEnable(void)
{
	/* 通讯开始：CS低 */
	SPI_FLASH_CS_LOW();
	
	/* 发送写使能命令 */
	SPI_FLASH_SendByte(W25X_WriteEnable);
	
	/* 通讯结束：CS高 */
	SPI_FLASH_CS_HIGH();
}

/* 等待WIP（BUSY）标志被置0，即等待到FLASH内部数据写入完毕 */
void SPI_FLASH_WaitForWriteEnd(void)
{
	u8 FLASH_Status = 0;
	
	/* 选择FLASH：CS低 */
	SPI_FLASH_CS_LOW();
	
	/* 发送读状态寄存器命令 */
	SPI_FLASH_SendByte(W25X_ReadStatusReg);
	
	/* 若FLASH忙碌，则等待 */
	do
	{
		/* 读取FLASH芯片的状态寄存器 */
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
	}
	while ((FLASH_Status & WIP_Flag) == SET);  //正在写入标志
	
	/* 停止信号 FLASH：CS高 */
	SPI_FLASH_CS_HIGH();
}

/* 掉电模式 */
void SPI_FLASH_PowerDown(void)
{
	SPI_FLASH_CS_LOW();
	
	/* 发送掉电命令 */
	SPI_FLASH_SendByte(W25X_PowerDown);
	
	SPI_FLASH_CS_HIGH();
}

/* 唤醒模式 */
void SPI_FLASH_WAKEUP(void)
{
	SPI_FLASH_CS_LOW();
	
	/* 发送掉电命令 */
	SPI_FLASH_SendByte(W25X_ReleasePowerDown);
	
	SPI_FLASH_CS_HIGH();
}

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
	/* 等待超时后的处理，输出错误信息 */
	FLASH_ERROR("SPI等待超时！errorCode = %d",errorCode);
	return 0;
}

