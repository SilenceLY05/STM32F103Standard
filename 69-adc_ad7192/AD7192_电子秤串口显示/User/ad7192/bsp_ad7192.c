#include "bsp_ad7192.h"
#include <string.h>
#include <stdio.h>

uint32_t AD7192Registers[8] = {0,0,0,0,0,0,0,0};//Ҫ����дAD7192Registers[8]

//AD7192Registers����Ϊʲô������޷���32λ�����أ���Ϊʲô����8��Ԫ���أ�
//��Ϊ���ݼĴ�����λ�����(���ݼĴ���)���Դﵽ32λ����32λΪ׼������ͨѶ�Ĵ������ɲ����ļĴ���Ϊ8�������Զ���8��Ԫ�أ�һ���Ĵ�����ӦAD7192Registers�����һ��Ԫ�أ��������š�
//8���Ĵ�����ReadFromAD7192ViaSPI()

uint32_t AD7192Data = 0;
volatile unsigned char Read_flag = 0;


static uint16_t SPI_TIMEOUT_UserCallBac(uint8_t errorCode);


/* AD7192��ʼ�� */
void AD7192_Config(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	SPI_InitTypeDef         SPI_InitStructure;
	
	AD7192_SPI_APBxClockCmd(AD7192_SPI_CLK,ENABLE);
	
	AD7192_SPI_GPIO_APBxClockCmd(AD7192_SPI_SCK_CLK | AD7192_SPI_MISO_CLK | AD7192_SPI_MOSI_CLK | AD7192_SPI_CS_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = AD7192_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AD7192_SPI_CS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD7192_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(AD7192_SPI_SCK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD7192_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(AD7192_SPI_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD7192_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(AD7192_SPI_MOSI_PORT,&GPIO_InitStructure);
	
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(AD7192_SPIx,&SPI_InitStructure);
	
	/* ʹ��SPI */
	SPI_Cmd(AD7192_SPIx,ENABLE);
	
	/* Ƭѡʹ�� */
	AD7192_CS_ENABLE;
	
	
}


/* SPI���շ������� */
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte,uint32_t SPITimeOut)
{
	uint32_t timeout = SPITimeOut;
	unsigned char i;
	
	for(i = 0 ;i< NumberOfByte;i++)
	{
		/* �ȴ����ͻ�����Ϊ�գ�TXEʱ�� */
		while(SPI_I2S_GetFlagStatus(AD7192_SPIx,SPI_I2S_FLAG_TXE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBac(0);
				return;
			}
		}
		
		/* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
		SPI_I2S_SendData(AD7192_SPIx,WriteBuff[i]);
		
		SPITimeOut = timeout;
		
		/* �ȴ����ջ������ǿ� RXNE�¼� */
		while(SPI_I2S_GetFlagStatus(AD7192_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBac(1);
				return;
			}
		}
		
		/* ��ȡ���ݼĴ�������ȡ���ջ��������� */
		ReadBuff[i] = SPI_I2S_ReceiveData(AD7192_SPIx);
	}
}

/* AD7192��д���� WriteBuffer ����Ҫд�����ݵĻ����� ReadBuffer �� �������ݵĻ����� NumberOfByte �� ���ݴ�С(�ֽ�) */
void AD7192ReadData(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte)
{
	SPI_TransmitReceive(WriteBuff,ReadBuff,NumberOfByte,1000);
}



/* ͨ��SPI��AD7192ִ��һ�ζ������������� RegisterStartAddressҪ���Ĵ�������ʼ��ַ NumberOfRegistersToReadҪ���ļĴ�������
 * DataBuff�����ֵ OffsetInBuffer������ƫ�� */
unsigned char ReadFromAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToRead,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	)
{
	//�βΰ���Ҫ���Ĵ�������ʼ��ַRegisterStartAddress(ȡֵ��Χ�Ǵ�0x00����0x07)��Ҫ��ȡ�Ĵ����ĸ�����ָ�򽫶�ȡAD7192�Ĵ������ݴ���������ָ��(DataBuffer����Ҫ�����ֵ�������м����)��
//һ��ָ��AD7192Registers[8]��
//const unsigned char OffsetInBuffer��������˼�ǻ�����ƫ�ƣ���ָAD7192Registers[8]�����ڲ�ƫ�ƣ�ע��������Ŷ��֮ǰ����˵��AD7192Registers[8]֮���Զ���8��Ԫ�أ�
//һ���Ĵ�����ӦAD7192Registers[8]�����һ��Ԫ�أ��������š�
	unsigned char WriteBuff[4] = {0,0,0,0}; //������4��Ԫ�ص�д�������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char ReadBuff[4] = {0,0,0,0};  //������4��Ԫ�صĶ��������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char i;
	
	for(i = 0;i<NumberOfRegistersToRead;i++)
	{
		//д��ͨ�żĴ���;8λ����;��һ�������Ƕ�ָ���Ĵ���ִ�ж�������CREAD_DIS��ʾ��ʹ��������
		WriteBuff[0] = WRITE_EN | RW_W | ((RegisterStartAddress + i )<<3) | CREAD_DIS;
		
		//����ͨ��д��ͨ�żĴ�����ѡ����һ��Ҫ��ȡ�ļĴ���
		AD7192ReadData(WriteBuff,ReadBuff,1);
		
		//Ȼ���ٽ�WriteBuf���
		WriteBuff[0] = NOP;
		WriteBuff[1] = NOP;
		WriteBuff[2] = NOP;
		WriteBuff[3] = NOP;
		
		switch(RegisterStartAddress + i)
		{
			//��3������Ƕ�ȡһ���ֽ�
			case REG_ID:
			case REG_COM_STA:
			case REG_GPOCON:
				AD7192ReadData(WriteBuff,ReadBuff,1);
				DataBuff[OffsetInBuffer + i] = ReadBuff[0];
			break;
			
			//��4������Ƕ�ȡ3���ֽ�
			case REG_MODE:
			case REG_CONF:
			case REG_OFFSET:
			case REG_FS:
				AD7192ReadData(WriteBuff,ReadBuff,3);
				DataBuff[OffsetInBuffer + i] = ReadBuff[0];
				DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[1];
				DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[2];
			break;
			
			//���ĸ��ֽ�
			case REG_DATA:   //���ݼĴ���(0x03��24λ��32λ) 
				if(AD7192Registers[REG_MODE]&DAT_STA_DIS)//��ͨ��ʹ�ܣ���״̬�Ĵ��������ݸ��ӵ����ݼĴ���24λ�������ϣ�������32λ����
				{
					AD7192ReadData(WriteBuff,ReadBuff,4);
					DataBuff[OffsetInBuffer + i] = ReadBuff[0];
					DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[1];
					DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[2];
					DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[3];
					break;
				}
				else
				{
					AD7192ReadData(WriteBuff,ReadBuff,3);
					DataBuff[OffsetInBuffer + i] = ReadBuff[0];
					DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[1];
					DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[2];
					break;
				}
			default:
				break;
		}
	
	}
	
	return 0;
	
}




/* ͨ��SPI��AD7192ִ��д����ƼĴ����Ĳ������� RegisterStartAddressд��Ĵ�������ʼ��ַ NumberOfRegistersToWriteҪд��ļĴ�������
 * DataBuffд����ֵ OffsetInBufferû���õ���Ҫ����ͨ��SPI��AD7192�Ȳ��������Գ�*/
unsigned char WriteToAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToWrite,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	)
{
	unsigned char WriteBuff[4] = {0,0,0,0}; //������4��Ԫ�ص�д�������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char ReadBuff[4] = {0,0,0,0};  //������4��Ԫ�صĶ��������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char i;
	
	for(i=0;i<NumberOfRegistersToWrite;i++)
	{
		WriteBuff[0] = WRITE_EN | RW_W | ((RegisterStartAddress + i )<<3) | CREAD_DIS;
		WriteBuff[1] = DataBuff[RegisterStartAddress + i] >>16;  //����16λ��16-23λ��ֵ��WriteBuf[1]
		WriteBuff[2] = DataBuff[RegisterStartAddress + i] >>8;
		WriteBuff[3] = DataBuff[RegisterStartAddress + i];
		AD7192ReadData(WriteBuff,ReadBuff,4);
	}
	return 0;
}


/* AD7192ģʽ�����üĴ������� mode����ģʽ�Ĵ����������ѡ��ģʽ��cfg�������üĴ����������ѡ������ */
void AD7192_Mode_Conf_Registers(uint32_t mode,uint32_t cfg)
{
	AD7192Registers[REG_MODE] = mode;
	AD7192Registers[REG_CONF] = cfg;
	
	/* д������ ����д��ģʽ�Ĵ��������üĴ��� */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}



/* AD7192�����λ���� ͨ��ִ��һ��ռ������40��ʱ�����ڵ�д����д��1��ʹDIN���ڸߵ�ƽ */
void AD7192_SoftWareReset(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	unsigned char i;
	
	WriteBuff[0] = 0xFF;
	
	for(i=0;i<10;i++)
	{
		//��AD7192ͨ�żĴ���д40��1  0xFF��8��1 ѭ��10��
		AD7192ReadData(WriteBuff,ReadBuff,1);
	}
	AD7192_SCLK_H;
}



/* �ڲ����ƽУ׼ */
void AD7192InternalZeroScaleCalibration(void)
{
	/* ��ʼ��AD7192�Ĵ�����ֵ */
	AD7192Registers[REG_MODE] = 0;
	
	/* ���Ĵ���ģʽ */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ���ģʽλ */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* �����ڲ����ƽУ׼ */
	AD7192Registers[REG_MODE] |= MODE_INZCL;
	
	/* д��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ��ȡ״̬�Ĵ���ֱ��У׼��� */
	do
	{
		ReadFromAD7192ViaSPI(REG_COM_STA,1,AD7192Registers,REG_COM_STA);
		
	}while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);
	
}


/* �ڲ�������У׼ */
void AD7192InternalFullScaleCalibration(void)
{
	/* ��ʼ��AD7192�Ĵ�����ֵ */
	AD7192Registers[REG_MODE] = 0;
	
	/* ���Ĵ���ģʽ */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ���ģʽλ */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* �����ڲ����ƽУ׼ */
	AD7192Registers[REG_MODE] |= MODE_INFCL;
	
	/* д��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ��ȡ״̬�Ĵ���ֱ��У׼��� */
	do
	{
		ReadFromAD7192ViaSPI(REG_COM_STA,1,AD7192Registers,REG_COM_STA);
		
	}while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);
	
}



/* ϵͳ���ƽУ׼ */
void AD7192ExternalZeroScaleCalibration(void)
{
	/* ��ʼ��ģʽ�Ĵ��������üĴ��� */
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	
	/* �������üĴ�����ֵ */
	AD7192Registers[REG_CONF] = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	/* ����ֵд�����üĴ��� */
	WriteToAD7192ViaSPI(REG_CONF,1,AD7192Registers,REG_CONF);
	
	/* �û�Ӧ��ϵͳ�����������ӵ����üĴ�����CH7��CH0λ��ѡ���ͨ���������� */
	AD7192Registers[REG_MODE] = 0;
	
	/* ����ģʽ�Ĵ�������ֵ */
	AD7192Registers[REG_MODE] = MODE_SYSZCL | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_EN | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x080;
	
	/* ����ֵд��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
}



/* ϵͳ����ƽУ׼ */
void AD7192ExternalFullScaleCalibration(void)
{
	/* ��ʼ��ģʽ�Ĵ��������üĴ��� */
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	
	/* �������üĴ�����ֵ */
	AD7192Registers[REG_CONF] = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	/* ����ֵд�����üĴ��� */
	WriteToAD7192ViaSPI(REG_CONF,1,AD7192Registers,REG_CONF);
	
	/* �û�Ӧ��ϵͳ�����������ӵ����üĴ�����CH7��CH0λ��ѡ���ͨ���������� */
	AD7192Registers[REG_MODE] = 0;
	
	/* ����ģʽ�Ĵ�������ֵ */
	AD7192Registers[REG_MODE] = MODE_SYSFCL | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_EN | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x080;
	
	/* ����ֵд��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
}


/* AD7192����������ȡ���� ������ȡ��Ҫ���� һ�ζ�ȡ����Ҫ */
void AD7192StartContinuousRead(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	
	// a|b��λ�� ֻҪ������Ӧ�Ķ�����λ��ֻҪһ����1�������Ӧλ����1
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_EN;
	
	AD7192ReadData(WriteBuff,ReadBuff,1);
}



/* AD7192��ȡת��������� DataBuffΪת�������ֵ */
uint32_t AD7192ReadConvertData(void)
{
	unsigned char WriteBuff[4];
	unsigned char ReadBuff[4];
	//�޷���32λ���������ݻ��������
	unsigned long int DataBuff;
	
	/* ����ͨ�żĴ���Ϊ�����ݼĴ��� */
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_DIS;
	
	/* д��ͨ�żĴ�����ȷ����һ�������Ƕ�ȡ���ݼĴ��� */
	AD7192ReadData(WriteBuff,ReadBuff,1);
	
	WriteBuff[0] = NOP;
	WriteBuff[1] = NOP;
	WriteBuff[2] = NOP;
	WriteBuff[3] = NOP;
	
	/* �ȴ�RDYλ��Ϊ�͵�ƽ ��DOUT��Ϊ�͵�ƽ */
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	/* ģʽ�Ĵ���������ͨ��ģʽ */
	if((AD7192Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)
	{
		AD7192ReadData(WriteBuff,ReadBuff,4);
		DataBuff = ReadBuff[0];
		DataBuff = (DataBuff << 8) + ReadBuff[1];
		DataBuff = (DataBuff << 8) + ReadBuff[2];
		DataBuff = (DataBuff << 8) + ReadBuff[3];
	}
	else
	{
		AD7192ReadData(WriteBuff,ReadBuff,3);
		DataBuff = ReadBuff[0];
		DataBuff = (DataBuff << 8) + ReadBuff[1];
		DataBuff = (DataBuff << 8) + ReadBuff[2];
	}
	return DataBuff;
}


/* AD7192���������� DataBuffΪ��ȡ����ֵ */
uint32_t AD7192ContinuousRead(void)
{
	unsigned char WriteBuff[4];
	unsigned char ReadBuff[4];
	//�޷���32λ���������ݻ��������
	unsigned long int DataBuff;
	
	WriteBuff[0] = NOP;
	WriteBuff[1] = NOP;
	WriteBuff[2] = NOP;
	WriteBuff[3] = NOP;
	
	/* �ȴ�RDYλ��Ϊ�͵�ƽ ��DOUT��Ϊ�͵�ƽ */
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	/* ģʽ�Ĵ���������ͨ��ģʽ */
	if((AD7192Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)
	{
		AD7192ReadData(WriteBuff,ReadBuff,4);
		DataBuff = ReadBuff[0];
		DataBuff = (DataBuff << 8) + ReadBuff[1];
		DataBuff = (DataBuff << 8) + ReadBuff[2];
		DataBuff = (DataBuff << 8) + ReadBuff[3];
	}
	else
	{
		AD7192ReadData(WriteBuff,ReadBuff,3);
		DataBuff = 0x00;
		DataBuff = (DataBuff << 8) + ReadBuff[0];
		DataBuff = (DataBuff << 8) + ReadBuff[1];
		DataBuff = (DataBuff << 8) + ReadBuff[2];
	}
	return DataBuff;
}



/* AD7192�˳�������ȡ���� (�����������������������������������ʹ��) */
void AD7192ExitContinousRead(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_DIS;
	
	AD7192ReadData(WriteBuff,ReadBuff,1);
}


/* AD7192��ȡ�¶� ����ֵtemp_dataΪδת�����¶�����*/
uint32_t AD7192ReadTemperature(void)
{
	uint32_t temp_data;
	
	/* ��ģʽ��״̬�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ���ģʽλ */
	AD7192Registers[REG_MODE] &= ~0xE00000;
	
	/* ����Ϊ����ת��ģʽ */
	AD7192Registers[REG_MODE] |= MODE_SING;
	
	/* ����Ϊ�¶�ת��ͨ�� */
	AD7192Registers[REG_CONF] |= TEMP;
	
	/* д��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* ��ȡת������ */
	temp_data = AD7192ReadConvertData();
	
	return temp_data;
}


/* ��ȡ���¶�ֵ���㺯�� */
float RealTemperature(uint32_t TemperatureData)
{
	float temp =0;
	
	temp = (TemperatureData-0x8000000) / 2815.0 - 273;
	
	return temp;
}


/* AD7192��������ת�� ChannelҪ������ͨ�� */
void AD7192StartSingleConvertion(uint32_t Channel)
{
	/* ��ģʽ��״̬�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
	
	/* ���ģʽλ */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* ����Ϊ����ת��ģʽ */
	AD7192Registers[REG_MODE] |= MODE_SING;
	
	/* ���ͨ��ѡ��λ */
	AD7192Registers[REG_CONF] &= ~0xFF00;	
	
	/* ����ΪҪת����ͨ�� */
	AD7192Registers[REG_CONF] |= Channel;
	
	/* д��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}


/* AD7192��������ת�� ChannelsҪ������ͨ��(û��ѡ���ͨ���ᱻ�ر�) */
void AD7192StartContinuousConvertion(uint32_t Channels)
{
	/* ��ģʽ��״̬�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
	
	/* ���ģʽλ */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* ����Ϊ����ת��ģʽ */
	AD7192Registers[REG_MODE] |= MODE_CONT;
	
	/* ���ͨ��ѡ��λ */
	AD7192Registers[REG_CONF] &= ~0xFF00;	
	
	/* ����ΪҪת����ͨ�� */
	AD7192Registers[REG_CONF] |= Channels;
	
	/* д��ģʽ�Ĵ��� */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}



/* �ȴ���ʱ�ص����� */
static uint16_t SPI_TIMEOUT_UserCallBac(uint8_t errorCode)
{
	//�βΰ���Ҫд��Ĵ�������ʼ��ַRegisterStartAddress(ȡֵ��Χ�Ǵ�0x00����0x07)��д��Ĵ����ĸ�����ָ��Ҫд��AD7192�Ĵ����������ָ��(DataBuffer����Ҫд����ֵ�������м����)��
//const unsigned char OffsetInBuffer��Ȼ���嵫û���õ�����Ҫ����ͨ��SPI��AD7192�����������Գ�
/*�ֱ���һ���������д���棬�ٰ�д���渳ֵ��Ȼ��ͨ��
	����void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)��ִ�У���д�������ݴ�����AD7192*/
	printf("SPI�ȴ���ʱ��errorCode = %d",errorCode);
	return 0 ;
}


