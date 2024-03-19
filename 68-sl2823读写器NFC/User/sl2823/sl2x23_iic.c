#include "sl2x23_iic.h"

volatile uint32_t SL2x23_Timeout;



/* ���I2C���� */
#ifndef HARDWARE_I2C
#define IIC_CODE 0


/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 10; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
#if IIC_CODE == 0
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	SL2x23_I2C_SDA_1();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_1();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_0();  //ǯסI2C���ߣ�׼�����ͻ�������� 

#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Stop
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
#if IIC_CODE ==0
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	SL2x23_I2C_SDA_0();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_1();
	
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SCL_0();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_1();
	SL2x23_I2C_SDA_1();
	i2c_Delay();
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

#if IIC_CODE == 0
	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			SL2x23_I2C_SDA_1();
		}
		else
		{
			SL2x23_I2C_SDA_0();
		}
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		i2c_Delay();	
		SL2x23_I2C_SCL_0();
		if (i == 7)
		{
			 SL2x23_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SCL_0();    //����ʱ�ӿ�ʼ���ݴ���
	for (i = 0; i < 8; i++)
	{		
		if ((_ucByte & 0x80)>>7)
		{
			SL2x23_I2C_SDA_1();
		}
		else
		{
			SL2x23_I2C_SDA_0();
		}
		_ucByte<<=1;
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		i2c_Delay();	
		SL2x23_I2C_SCL_0();
		i2c_Delay();
	}
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

#if IIC_CODE == 0
	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SL2x23_I2C_SCL_1();
		i2c_Delay();
		if (SL2x23_I2C_SDA_READ())
		{
			value++;
		}
		SL2x23_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
	
#else
	SL2x23_I2C_SDA_IN();
	for (i = 0; i < 8; i++)
	{
		SL2x23_I2C_SCL_1();
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		value<<=1;
		if (SL2x23_I2C_SDA_READ())
		{
			value++;
		}
		i2c_Delay();
	}
	return value;
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	
#if IIC_CODE ==0
	
	SL2x23_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	SL2x23_Timeout = SL2x23_LONG_TIMEOUT;
	while(SL2x23_I2C_SDA_READ())
	{
		if(--SL2x23_Timeout == 0)
		{
			i2c_Stop();
			return 1;
		}
	}
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	return 0;
	
#else
	uint8_t ucErrTime = 0;
	SL2x23_I2C_SDA_IN();
	SL2x23_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();

	while(SL2x23_I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			i2c_Stop();
			return 1;
		}
	}
	SL2x23_I2C_SCL_0();
	return 0;
	
	
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
#if IIC_CODE == 0
	SL2x23_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	SL2x23_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	
#else
	SL2x23_I2C_SCL_0();	/* CPU����SDA = 0 */
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SL2x23_I2C_SCL_0();

#endif
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
#if IIC_CODE == 0
	SL2x23_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();	
	
#else
	SL2x23_I2C_SCL_0();
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SL2x23_I2C_SCL_0();

#endif
}



/* ��ʼ��I2C��ص�GPIO */
void SL2x23_I2C_Init(void)
{
//Ӳ��I2C
#define HARDWART_I2C
	GPIO_InitTypeDef    GPIO_InitStructure;
	I2C_InitTypeDef     I2C_InitStructure;
	
	/* ʹ����I2C�йص�ʱ�� */
	RCC_APB1PeriphClockCmd(SL2x23_I2C_CLK,ENABLE);
	RCC_APB2PeriphClockCmd ( SL2x23_I2C_SCL_GPIO_CLK |SL2x23_I2C_SDA_GPIO_CLK , ENABLE );
	
	
	/* ����I2C SDA ��SCL���� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ��©���
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_I2C_SDA_GPIO_PORT,&GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 700000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0;
	
	I2C_Init(SL2x23_I2C,&I2C_InitStructure);
	I2C_Cmd(SL2x23_I2C,ENABLE);
	
//���ģ��I2C
#else
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(SL2x23_I2C_SCL_GPIO_CLK | SL2x23_I2C_SDA_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SCL_PIN | SL2x23_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
#if IIC_CODE == 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   /* ��©��� */
#else
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   /* ������� */
	
#endif 
	GPIO_Init(SL2x23_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	
	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop();
#endif
}


uint32_t SL2x23_TIMEOUT_UserCallBack(void)
{
	printf("I2C ERROR \r\n");
	while(1)
	{}
}



uint32_t i2c_write_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t reg_value)
{
#ifdef HARDWART_I2C
	
	/* �ȴ�æµ��־��� */
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_LONG_TIMEOUT);
	
	/* ��ʼ���� */
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* �ȴ���ʼλ���� */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ���ʹӵ�ַ��дʹ�ܲ��� */
	I2C_Send7bitAddress(SL2x23_I2C,(i2c_dev_addr),I2C_Direction_Transmitter);
	
	/* �ȴ���ַλ���� */
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ���ADDR�ж�λ-ͨ����ȡSR1��SR2��ʵ�� */
	CLEAR_ADDR_BIT
	
	/* �ȴ�����ʹ��λ���� */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ׼��Ҫ���͵ļĴ���ֵ */
	I2C_SendData(SL2x23_I2C,reg_addr);
	
	/* �ȴ�����ʹ��λ���� */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ׼��Ҫ���͵ļĴ���ֵ */
	I2C_SendData(SL2x23_I2C,reg_value);
	
	/* �ȴ�BTFλ */
	WAIT_FOR_FLAG(I2C_FLAG_BTF,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ����ͨ�� */
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	
	
#else
	/* ��һ�� ����I2C�����ź� */
	i2c_Start();
	
	/* �ڶ�������������ֽڣ���7Bit�ǵ�ַ��Bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(i2c_dev_addr|0);
	/* ������������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :�����ֽ���Ӧ��\r\n");
		return 0;
	}
	
	/* ���Ĳ��������ֽڵ�ַ */
	i2c_SendByte(reg_addr);
	/* ���岽���ȴ�ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR :�Ĵ�����ַ�ֽ���Ӧ��\r\n");
		return 0;
	}
	
	/* ����������ʼд���� */
	i2c_SendByte(value);
	
	/* ���߲�������ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR:�����ֽ���Ӧ��\r\n");
		return 0;
	}
	
	/* ����I2Cֹͣ�ź� */
	i2c_Stop();
	i2c_Delay();
#endif
	return 0;
}



uint32_t i2c_read_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t *reg_value)
{
#ifdef HARDWART_I2C
	
	/* �ȴ�æµ��־��� */
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_LONG_TIMEOUT);
	
	/* ��ʼ���� */
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* �ȴ���ʼλ���� */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ���ʹӵ�ַ��дʹ�ܲ��� */
	I2C_Send7bitAddress(SL2x23_I2C,(i2c_dev_addr),I2C_Direction_Transmitter);
	
	/* �ȴ���ַλ���� */
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ���ADDR�ж�λ-ͨ����ȡSR1��SR2��ʵ�� */
	CLEAR_ADDR_BIT
	
	/* �ȴ�����ʹ��λ���� */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* ׼��Ҫ���͵ļĴ���ֵ */
	I2C_SendData(SL2x23_I2C,reg_addr);
	
	/* �ȴ�����ʹ��λ���� */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* �ȴ�BTFλ */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_Send7bitAddress(SL2x23_I2C,i2c_dev_addr,I2C_Direction_Receiver);
	
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_AcknowledgeConfig(SL2x23_I2C,DISABLE);
	
	CLEAR_ADDR_BIT
	
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	
	WAIT_FOR_FLAG(I2C_FLAG_RXNE,SET,SL2x23_FLAG_TIMEOUT);
	
	*reg_value = I2C_ReceiveData(SL2x23_I2C);
	
	I2C_ClearFlag(SL2x23_I2C,I2C_FLAG_BTF);
	
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_FLAG_TIMEOUT);
	
	I2C_AcknowledgeConfig(SL2x23_I2C,ENABLE);
	
	SL2x23_I2C->CR1 &= ~I2C_CR1_POS;
	
	
#else
	/* ��һ�� ����I2C�����ź� */
	i2c_Start();
	
	/* �ڶ�������������ֽڣ���7Bit�ǵ�ַ��Bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(i2c_dev_addr|0);
	/* ������������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :�����ֽ���Ӧ��2\r\n");
		return 0;
	}
	
	/* ���Ĳ��������ֽڵ�ַ */
	i2c_SendByte(reg_addr);
	/* ���岽���ȴ�ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR :�Ĵ�����ַ�ֽ���Ӧ��2\r\n");
		return 0;
	}
	
	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����SL2x23���͵�ַ�����濪ʼ��ȡ���� */
	i2c_Start();
	
	/* ���߲�����������ֽڣ���7Bit�ǵ�ַ��Bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(i2c_dev_addr|1);
	/* �ڰ˲�������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :�����ֽ���Ӧ��3\r\n");
		return 0;
	}
	
	/* �ھŲ� ��ȡһ�ֽ����� */
	*reg_value = i2c_ReadByte();
	i2c_NAck();
	
	i2c_Stop();
#endif
	return 0;
}

/******************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸�������豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ���ڣ� ����1��ʾδ̽�⵽
******************************************************************************/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	
#ifdef HARDWART_I2C
	SL2x23_Timeout = SL2x23_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SL2x23_I2C,I2C_FLAG_BUSY))
	{
		if((SL2x23_Timeout--) == 0) return 1;
	}
	
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	SL2x23_Timeout = SL2x23_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SL2x23_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((SL2x23_Timeout--)==0)
		{
			I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
			return 1;
		}
	}
	
	/* д���豸��ַ */
	I2C_Send7bitAddress(SL2x23_I2C,_Address,I2C_Direction_Transmitter);
	SL2x23_Timeout = SL2x23_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SL2x23_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((SL2x23_Timeout--) == 0)
		{
			I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
			return 1;
		}
	}
	
	/* ����STOP */
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	return 0;
	
	
#else
	uint8_t ucAck;
	i2c_Start();
	i2c_SendByte(_Address | 0);
	ucAck = i2c_WaitAck();
	i2c_Stop();
	
	return ucAck;
	
#endif
}





