#include "bsp_i2c_ee.h"
#include "bsp_usart.h"


uint16_t EEPROM_ADDRESS;

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   


static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);


/* ��ʼ��I2C��ص�GPIO */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ʹ����I2C�йص�ʱ�� */
	EEPROM_I2C_APBxClock_FUN(EEPROM_I2C_CLK,ENABLE);
	EEPROM_I2C_GPIO_APBxClock_FUN ( EEPROM_I2C_GPIO_CLK, ENABLE );
	
	
	/* ����I2C SDA ��SCL���� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ��©���
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SDA_PORT,&GPIO_InitStructure);
	
}

/* ����I2C����ģʽ */
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

/* ����ͷ�ļ�i2c_ee.h�еĶ�����ѡ��EEPROM���豸��ַ */
#ifdef EEPROM_Block0_ADDRESS
  /* ѡ�� EEPROM Block0 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif

#ifdef EEPROM_Block1_ADDRESS  
	/* ѡ�� EEPROM Block1 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif

#ifdef EEPROM_Block2_ADDRESS  
	/* ѡ�� EEPROM Block2 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif

#ifdef EEPROM_Block3_ADDRESS  
	/* ѡ�� EEPROM Block3 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/* ���������е�����д��I2C EEPROM�� */
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


/* ��EEPROMд��һ���ֽ� */
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer,uint8_t WriteAddr)
{
	/* ������ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV5�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
	}
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* ���EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
	}
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV8�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
	}
	
	/* �������� */
	I2C_SendData(EEPROM_I2Cx,*pBuffer);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���ݷ����꣬���EV8_2 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
	}
	
	/* ���ݴ�����ɣ����ͽ����ź� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	return 1;
}

/* ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ������ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ� */
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToWrite)
{
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV5�¼� */
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
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
	}
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV8�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
	}	
	
	while(NumByteToWrite--)
	{
		/* �������� */
		I2C_SendData(EEPROM_I2Cx,*pBuffer);
		
		pBuffer++;
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* ���ݷ����꣬���EV8_2 */
		while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
		  if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
	  }
		
	}
	
	/* ���ݴ�����ɣ����ͽ����ź� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	return 1;
	
}


/* ��EEPROM�ж�ȡһ������ */
uint32_t I2C_EE_BufferRead(uint8_t *pBuffer,uint8_t ReadAddr,uint16_t NumByteToRead)
{
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	/* ������ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV5�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
	}
	
	I2C_Cmd(EEPROM_I2Cx,ENABLE);
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,ReadAddr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV8�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
	}
	
	
	/* �ڶ�����ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV5�¼� */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
	}
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Receiver);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	/* ���EV6 */
	while(!I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) )
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
	}
		
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//�����յ��������һ���ֽ�
			I2C_AcknowledgeConfig(EEPROM_I2Cx,DISABLE);
			
			I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
			
		}
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		/* ���EV7�¼� ���Ĵ������µ���Ч���� */
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
	
	//��������ACKʹ�ܣ��Ա��´�ͨѶ
	I2C_AcknowledgeConfig(EEPROM_I2Cx,ENABLE);
	
	return 1;
	
}

//�ȴ�EEPROM�ڲ�ʱ�����
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
	EEPROM_ERROR("I2C�ȴ���ʱ��errorCode = %d",errorCode);
	
	return 0;
}

