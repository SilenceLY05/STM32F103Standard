#include "bsp_i2c_ee.h"
#include "bsp_usart.h"


uint16_t EEPROM_ADDRESS;

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

/* ��EEPROMд��һ���ֽ� */
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer,uint8_t WriteAddr)
{
	/* ������ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* ���EV5�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* ���EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* ���EV8�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTING) == ERROR);
	
	/* �������� */
	I2C_SendData(EEPROM_I2Cx,*pBuffer);
	
	/* ���ݷ����꣬���EV8_2 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	/* ���ݴ�����ɣ����ͽ����ź� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	
}

/* ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ������ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ� */
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToWrite)
{
	/* ������ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* ���EV5�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* ���EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* ���EV8�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTING) == ERROR);
	
	while(NumByteToWrite)
	{
		/* �������� */
		I2C_SendData(EEPROM_I2Cx,*pBuffer);
		
		/* ���ݷ����꣬���EV8_2 */
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
		
		pBuffer++;
		NumByteToWrite--;
		
	}
	
	/* ���ݴ�����ɣ����ͽ����ź� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
}


/* ��EEPROM�ж�ȡһ������ */
uint32_t I2C_EE_SeqRead(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToRead)
{
	/* ������ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* ���EV5�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* ���EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* ���EV8�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	/* �ڶ�����ʼ�ź� */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* ���EV5�¼� */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5�¼�����⵽�����͵�ַ */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Receiver);
	
	/* ���EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
		
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//�����յ��������һ���ֽ�
			I2C_AcknowledgeConfig(EEPROM_I2Cx,DISABLE);
			
		}
		
		/* ���EV7�¼� ���Ĵ������µ���Ч���� */
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		
		*pBuffer = I2C_ReceiveData(EEPROM_I2Cx);	
		
		pBuffer++;
		NumByteToRead--;

	}
	
	/* ���ݴ�����ɣ����ͽ����ź� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	//��������ACKʹ�ܣ��Ա��´�ͨѶ
	I2C_AcknowledgeConfig(EEPROM_I2Cx,ENABLE);
	
}

//�ȴ�EEPROM�ڲ�ʱ�����
void EEPROM_WaitForWriteEnd(void)
{
	do{
		/* ������ʼ�ź� */
		I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
		
		/* ���EV5�¼� */
		while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_SB) == RESET);
	
		/* EV5�¼�����⵽�����͵�ַ */
		I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	}
	
	/* ���EV6 */
	while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_ADDR) == RESET);
	

	/* EEPROM�ڲ�ʱ����� ������� */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	
	
}

