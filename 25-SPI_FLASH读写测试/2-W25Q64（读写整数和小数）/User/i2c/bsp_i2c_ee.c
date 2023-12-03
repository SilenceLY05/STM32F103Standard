#include "bsp_i2c_ee.h"
#include "bsp_usart.h"


uint16_t EEPROM_ADDRESS;

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   


static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);


/* 初始化I2C相关的GPIO */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能与I2C有关的时钟 */
	EEPROM_I2C_APBxClock_FUN(EEPROM_I2C_CLK,ENABLE);
	EEPROM_I2C_GPIO_APBxClock_FUN ( EEPROM_I2C_GPIO_CLK, ENABLE );
	
	
	/* 配置I2C SDA 和SCL引脚 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SDA_PORT,&GPIO_InitStructure);
	
}

/* 配置I2C工作模式 */
static void I2C_Mode_Config(void)
{
	I2C_InitTypeDef     I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7;
	
	I2C_Init(EEPROM_I2Cx,&I2C_InitStructure);
	I2C_Cmd(EEPROM_I2Cx,ENABLE);
}

void I2C_EE_Init(void)
{

  I2C_GPIO_Config(); 
 
  I2C_Mode_Config();

/* 根据头文件i2c_ee.h中的定义来选择EEPROM的设备地址 */
#ifdef EEPROM_Block0_ADDRESS
  /* 选择 EEPROM Block0 来写入 */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif

#ifdef EEPROM_Block1_ADDRESS  
	/* 选择 EEPROM Block1 来写入 */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif

#ifdef EEPROM_Block2_ADDRESS  
	/* 选择 EEPROM Block2 来写入 */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif

#ifdef EEPROM_Block3_ADDRESS  
	/* 选择 EEPROM Block3 来写入 */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/* 将缓冲区中的数据写到I2C EEPROM中 */
void I2C_EE_BufferWrite(uint8_t *pBuffer,uint8_t WriteAddr,uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0;
	
	Addr = WriteAddr % I2C_PageSize;
	count = I2C_PageSize - Addr;
	NumOfPage = NumByteToWrite / I2C_PageSize;
	NumOfSingle = NumByteToWrite % I2C_PageSize;
	
	if(Addr == 0)
	{
		if(NumOfPage == 0)
		{
			I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
			I2C_EE_WaitEepromStandbyState();
		}
		else
		{
			while(NumOfPage--)
			{
				I2C_EE_PageWrite(pBuffer,WriteAddr,I2C_PageSize);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr += I2C_PageSize;
				pBuffer += I2C_PageSize;
			}
			if(NumOfSingle != 0)
			{
				I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
				I2C_EE_WaitEepromStandbyState();
			}
		}
	}
	else
	{
		if(NumOfPage == 0)
		{
			I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
			I2C_EE_WaitEepromStandbyState();
		}
		else
		{
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / I2C_PageSize;
			NumOfSingle = NumByteToWrite % I2C_PageSize;
			
			if(count != 0)
			{
				I2C_EE_PageWrite(pBuffer,WriteAddr,count);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr += count;
				pBuffer += count;
			}
			
			while(NumOfPage--)
			{
				I2C_EE_PageWrite(pBuffer,WriteAddr,I2C_PageSize);
				I2C_EE_WaitEepromStandbyState();
				WriteAddr += I2C_PageSize;
				pBuffer += I2C_PageSize;
			}
			if(NumOfSingle != 0)
			{
				I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
				I2C_EE_WaitEepromStandbyState();
			}
		}
	}
	
}


/* 向EEPROM写入一个字节 */
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer,uint8_t WriteAddr)
{
	/* 发送起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV5事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
	}
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* 检测EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
	}
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV8事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
	}
	
	/* 发送数据 */
	I2C_SendData(EEPROM_I2Cx,*pBuffer);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 数据发送完，检测EV8_2 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
	}
	
	/* 数据传输完成，发送结束信号 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	return 1;
}

/* 在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数不能超过EEPROM页的大小，AT24C02每页有8个字节 */
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToWrite)
{
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV5事件 */
	while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
	}
	
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
	}
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
	}
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV8事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
	}	
	
	while(NumByteToWrite--)
	{
		/* 发送数据 */
		I2C_SendData(EEPROM_I2Cx,*pBuffer);
		
		pBuffer++;
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* 数据发送完，检测EV8_2 */
		while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
		  if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
	  }
		
	}
	
	/* 数据传输完成，发送结束信号 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	return 1;
	
}


/* 从EEPROM中读取一个数据 */
uint32_t I2C_EE_BufferRead(uint8_t *pBuffer,uint8_t ReadAddr,uint16_t NumByteToRead)
{
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	/* 发送起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV5事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
	}
	
	I2C_Cmd(EEPROM_I2Cx,ENABLE);
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,ReadAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV8事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
	}
	
	
	/* 第二次起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV5事件 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
	}
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Receiver);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* 检测EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
	}
		
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//若接收到的是最后一个字节
			I2C_AcknowledgeConfig(EEPROM_I2Cx,DISABLE);
			
			I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
			
		}
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		/* 检测EV7事件 即寄存器有新的有效数据 */
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		}
		{
			*pBuffer = I2C_ReceiveData(EEPROM_I2Cx);	
			
			pBuffer++;
			NumByteToRead--;
		}

	}
	
	//重新配置ACK使能，以便下次通讯
	I2C_AcknowledgeConfig(EEPROM_I2Cx,ENABLE);
	
	return 1;
	
}

//等待EEPROM内部时序完成
void I2C_EE_WaitEepromStandbyState(void)
{
	uint16_t SR1_Tmp = 0;
	  do
  {
    /* Send START condition */
    I2C_GenerateSTART(EEPROM_I2Cx, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(EEPROM_I2Cx, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(EEPROM_I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(EEPROM_I2Cx, I2C_Register_SR1) & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag(EEPROM_I2Cx, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(EEPROM_I2Cx, ENABLE);  

}

static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
	EEPROM_ERROR("I2C等待超时！errorCode = %d",errorCode);
	
	return 0;
}

