#include "bsp_ad7192.h"
#include <string.h>
#include <stdio.h>

uint32_t AD7192Registers[8] = {0,0,0,0,0,0,0,0};//要读或写AD7192Registers[8]

//AD7192Registers数组为什么定义成无符号32位整型呢？又为什么定义8个元素呢？
//因为数据寄存器的位数最多(数据寄存器)可以达到32位，以32位为准。除了通讯寄存器，可操作的寄存器为8个，所以定义8个元素，一个寄存器对应AD7192Registers数组的一个元素，互不干扰。
//8个寄存器见ReadFromAD7192ViaSPI()

uint32_t AD7192Data = 0;
volatile unsigned char Read_flag = 0;


static uint16_t SPI_TIMEOUT_UserCallBac(uint8_t errorCode);


/* AD7192初始化 */
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
	
	/* 使能SPI */
	SPI_Cmd(AD7192_SPIx,ENABLE);
	
	/* 片选使能 */
	AD7192_CS_ENABLE;
	
	
}


/* SPI接收发送数据 */
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte,uint32_t SPITimeOut)
{
	uint32_t timeout = SPITimeOut;
	unsigned char i;
	
	for(i = 0 ;i< NumberOfByte;i++)
	{
		/* 等待发送缓冲区为空，TXE时间 */
		while(SPI_I2S_GetFlagStatus(AD7192_SPIx,SPI_I2S_FLAG_TXE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBac(0);
				return;
			}
		}
		
		/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
		SPI_I2S_SendData(AD7192_SPIx,WriteBuff[i]);
		
		SPITimeOut = timeout;
		
		/* 等待接收缓冲区非空 RXNE事件 */
		while(SPI_I2S_GetFlagStatus(AD7192_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBac(1);
				return;
			}
		}
		
		/* 读取数据寄存器，获取接收缓冲区数据 */
		ReadBuff[i] = SPI_I2S_ReceiveData(AD7192_SPIx);
	}
}

/* AD7192读写函数 WriteBuffer ：需要写入数据的缓冲区 ReadBuffer ： 读出数据的缓冲区 NumberOfByte ： 数据大小(字节) */
void AD7192ReadData(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte)
{
	SPI_TransmitReceive(WriteBuff,ReadBuff,NumberOfByte,1000);
}



/* 通过SPI向AD7192执行一次读操作操作函数 RegisterStartAddress要读寄存器的起始地址 NumberOfRegistersToRead要读的寄存器个数
 * DataBuff读入的值 OffsetInBuffer缓存内偏移 */
unsigned char ReadFromAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToRead,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	)
{
	//形参包括要读寄存器的起始地址RegisterStartAddress(取值范围是从0x00——0x07)，要读取寄存器的个数，指向将读取AD7192寄存器数据存入的数组的指针(DataBuffer才是要读入的值其他是中间变量)，
//一般指向AD7192Registers[8]，
//const unsigned char OffsetInBuffer，字面意思是缓存内偏移，是指AD7192Registers[8]数组内部偏移，注意是数组哦，之前我们说过AD7192Registers[8]之所以定义8个元素，
//一个寄存器对应AD7192Registers[8]数组的一个元素，互不干扰。
	unsigned char WriteBuff[4] = {0,0,0,0}; //定义有4个元素的写缓存数组，每个数组元素占1个字节。
	unsigned char ReadBuff[4] = {0,0,0,0};  //定义有4个元素的读缓存数组，每个数组元素占1个字节。
	unsigned char i;
	
	for(i = 0;i<NumberOfRegistersToRead;i++)
	{
		//写入通信寄存器;8位数据;下一个操作是对指定寄存器执行读操作。CREAD_DIS表示不使能连续读
		WriteBuff[0] = WRITE_EN | RW_W | ((RegisterStartAddress + i )<<3) | CREAD_DIS;
		
		//首先通过写入通信寄存器来选定下一步要读取的寄存器
		AD7192ReadData(WriteBuff,ReadBuff,1);
		
		//然后再将WriteBuf清空
		WriteBuff[0] = NOP;
		WriteBuff[1] = NOP;
		WriteBuff[2] = NOP;
		WriteBuff[3] = NOP;
		
		switch(RegisterStartAddress + i)
		{
			//此3种情况是读取一个字节
			case REG_ID:
			case REG_COM_STA:
			case REG_GPOCON:
				AD7192ReadData(WriteBuff,ReadBuff,1);
				DataBuff[OffsetInBuffer + i] = ReadBuff[0];
			break;
			
			//此4种情况是读取3个字节
			case REG_MODE:
			case REG_CONF:
			case REG_OFFSET:
			case REG_FS:
				AD7192ReadData(WriteBuff,ReadBuff,3);
				DataBuff[OffsetInBuffer + i] = ReadBuff[0];
				DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[1];
				DataBuff[OffsetInBuffer + i] = (DataBuff[OffsetInBuffer + i]<<8)+ReadBuff[2];
			break;
			
			//读四个字节
			case REG_DATA:   //数据寄存器(0x03，24位或32位) 
				if(AD7192Registers[REG_MODE]&DAT_STA_DIS)//多通道使能，将状态寄存器的内容附加到数据寄存器24位的数据上，所以是32位数据
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




/* 通过SPI向AD7192执行写入控制寄存器的操作函数 RegisterStartAddress写入寄存器的起始地址 NumberOfRegistersToWrite要写入的寄存器个数
 * DataBuff写出的值 OffsetInBuffer没有用到主要是与通过SPI向AD7192度操作函数对称*/
unsigned char WriteToAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToWrite,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	)
{
	unsigned char WriteBuff[4] = {0,0,0,0}; //定义有4个元素的写缓存数组，每个数组元素占1个字节。
	unsigned char ReadBuff[4] = {0,0,0,0};  //定义有4个元素的读缓存数组，每个数组元素占1个字节。
	unsigned char i;
	
	for(i=0;i<NumberOfRegistersToWrite;i++)
	{
		WriteBuff[0] = WRITE_EN | RW_W | ((RegisterStartAddress + i )<<3) | CREAD_DIS;
		WriteBuff[1] = DataBuff[RegisterStartAddress + i] >>16;  //右移16位将16-23位赋值给WriteBuf[1]
		WriteBuff[2] = DataBuff[RegisterStartAddress + i] >>8;
		WriteBuff[3] = DataBuff[RegisterStartAddress + i];
		AD7192ReadData(WriteBuff,ReadBuff,4);
	}
	return 0;
}


/* AD7192模式和配置寄存器配置 mode：对模式寄存器输入的数选择模式，cfg：对配置寄存器输入的数选择配置 */
void AD7192_Mode_Conf_Registers(uint32_t mode,uint32_t cfg)
{
	AD7192Registers[REG_MODE] = mode;
	AD7192Registers[REG_CONF] = cfg;
	
	/* 写入配置 连续写入模式寄存器和配置寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}



/* AD7192软件复位函数 通过执行一个占用至少40个时钟周期的写操作写入1并使DIN处于高电平 */
void AD7192_SoftWareReset(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	unsigned char i;
	
	WriteBuff[0] = 0xFF;
	
	for(i=0;i<10;i++)
	{
		//向AD7192通信寄存器写40个1  0xFF是8个1 循环10轮
		AD7192ReadData(WriteBuff,ReadBuff,1);
	}
	AD7192_SCLK_H;
}



/* 内部零电平校准 */
void AD7192InternalZeroScaleCalibration(void)
{
	/* 初始化AD7192寄存器数值 */
	AD7192Registers[REG_MODE] = 0;
	
	/* 读寄存器模式 */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 清除模式位 */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* 设置内部零电平校准 */
	AD7192Registers[REG_MODE] |= MODE_INZCL;
	
	/* 写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 读取状态寄存器直到校准完成 */
	do
	{
		ReadFromAD7192ViaSPI(REG_COM_STA,1,AD7192Registers,REG_COM_STA);
		
	}while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);
	
}


/* 内部满量程校准 */
void AD7192InternalFullScaleCalibration(void)
{
	/* 初始化AD7192寄存器数值 */
	AD7192Registers[REG_MODE] = 0;
	
	/* 读寄存器模式 */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 清除模式位 */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* 设置内部零电平校准 */
	AD7192Registers[REG_MODE] |= MODE_INFCL;
	
	/* 写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 读取状态寄存器直到校准完成 */
	do
	{
		ReadFromAD7192ViaSPI(REG_COM_STA,1,AD7192Registers,REG_COM_STA);
		
	}while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);
	
}



/* 系统零电平校准 */
void AD7192ExternalZeroScaleCalibration(void)
{
	/* 初始化模式寄存器和配置寄存器 */
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	
	/* 设置配置寄存器数值 */
	AD7192Registers[REG_CONF] = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	/* 将数值写入配置寄存器 */
	WriteToAD7192ViaSPI(REG_CONF,1,AD7192Registers,REG_CONF);
	
	/* 用户应将系统零标度输入连接到配置寄存器中CH7至CH0位所选择的通道输入引脚 */
	AD7192Registers[REG_MODE] = 0;
	
	/* 设置模式寄存器的数值 */
	AD7192Registers[REG_MODE] = MODE_SYSZCL | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_EN | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x080;
	
	/* 将数值写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
}



/* 系统满电平校准 */
void AD7192ExternalFullScaleCalibration(void)
{
	/* 初始化模式寄存器和配置寄存器 */
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	
	/* 设置配置寄存器数值 */
	AD7192Registers[REG_CONF] = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	/* 将数值写入配置寄存器 */
	WriteToAD7192ViaSPI(REG_CONF,1,AD7192Registers,REG_CONF);
	
	/* 用户应将系统零标度输入连接到配置寄存器中CH7至CH0位所选择的通道输入引脚 */
	AD7192Registers[REG_MODE] = 0;
	
	/* 设置模式寄存器的数值 */
	AD7192Registers[REG_MODE] = MODE_SYSFCL | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_EN | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x080;
	
	/* 将数值写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
}


/* AD7192启动连续读取函数 连续读取需要启动 一次读取不需要 */
void AD7192StartContinuousRead(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	
	// a|b按位或 只要两个对应的二进制位中只要一个是1，结果对应位就是1
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_EN;
	
	AD7192ReadData(WriteBuff,ReadBuff,1);
}



/* AD7192读取转换结果函数 DataBuff为转换结果的值 */
uint32_t AD7192ReadConvertData(void)
{
	unsigned char WriteBuff[4];
	unsigned char ReadBuff[4];
	//无符号32位长整型数据缓存变量。
	unsigned long int DataBuff;
	
	/* 设置通信寄存器为读数据寄存器 */
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_DIS;
	
	/* 写入通信寄存器，确认下一步操作是读取数据寄存器 */
	AD7192ReadData(WriteBuff,ReadBuff,1);
	
	WriteBuff[0] = NOP;
	WriteBuff[1] = NOP;
	WriteBuff[2] = NOP;
	WriteBuff[3] = NOP;
	
	/* 等待RDY位变为低电平 即DOUT变为低电平 */
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	/* 模式寄存器开启多通道模式 */
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


/* AD7192连续读函数 DataBuff为读取到的值 */
uint32_t AD7192ContinuousRead(void)
{
	unsigned char WriteBuff[4];
	unsigned char ReadBuff[4];
	//无符号32位长整型数据缓存变量。
	unsigned long int DataBuff;
	
	WriteBuff[0] = NOP;
	WriteBuff[1] = NOP;
	WriteBuff[2] = NOP;
	WriteBuff[3] = NOP;
	
	/* 等待RDY位变为低电平 即DOUT变为低电平 */
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	/* 模式寄存器开启多通道模式 */
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



/* AD7192退出连续读取函数 (和连续读启动函数、连续读操作函数配合使用) */
void AD7192ExitContinousRead(void)
{
	unsigned char WriteBuff[1];
	unsigned char ReadBuff[1];
	
	while(AD7192_RDY_STATUS == 0);
	while(AD7192_RDY_STATUS == 1);
	
	WriteBuff[0] = WRITE_EN | RW_R | (REG_DATA << 3) | CREAD_DIS;
	
	AD7192ReadData(WriteBuff,ReadBuff,1);
}


/* AD7192读取温度 返回值temp_data为未转换的温度数据*/
uint32_t AD7192ReadTemperature(void)
{
	uint32_t temp_data;
	
	/* 读模式和状态寄存器 */
	ReadFromAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 清除模式位 */
	AD7192Registers[REG_MODE] &= ~0xE00000;
	
	/* 设置为单次转换模式 */
	AD7192Registers[REG_MODE] |= MODE_SING;
	
	/* 设置为温度转换通道 */
	AD7192Registers[REG_CONF] |= TEMP;
	
	/* 写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,1,AD7192Registers,REG_MODE);
	
	/* 读取转换数据 */
	temp_data = AD7192ReadConvertData();
	
	return temp_data;
}


/* 读取的温度值计算函数 */
float RealTemperature(uint32_t TemperatureData)
{
	float temp =0;
	
	temp = (TemperatureData-0x8000000) / 2815.0 - 273;
	
	return temp;
}


/* AD7192启动单次转换 Channel要启动的通道 */
void AD7192StartSingleConvertion(uint32_t Channel)
{
	/* 读模式和状态寄存器 */
	ReadFromAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
	
	/* 清除模式位 */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* 设置为单次转换模式 */
	AD7192Registers[REG_MODE] |= MODE_SING;
	
	/* 清除通道选择位 */
	AD7192Registers[REG_CONF] &= ~0xFF00;	
	
	/* 设置为要转换的通道 */
	AD7192Registers[REG_CONF] |= Channel;
	
	/* 写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}


/* AD7192启动连续转换 Channels要启动的通道(没有选择的通道会被关闭) */
void AD7192StartContinuousConvertion(uint32_t Channels)
{
	/* 读模式和状态寄存器 */
	ReadFromAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
	
	/* 清除模式位 */
	AD7192Registers[REG_MODE] &= ~0xE00000; 
	
	/* 设置为单次转换模式 */
	AD7192Registers[REG_MODE] |= MODE_CONT;
	
	/* 清除通道选择位 */
	AD7192Registers[REG_CONF] &= ~0xFF00;	
	
	/* 设置为要转换的通道 */
	AD7192Registers[REG_CONF] |= Channels;
	
	/* 写入模式寄存器 */
	WriteToAD7192ViaSPI(REG_MODE,2,AD7192Registers,REG_MODE);
}



/* 等待超时回调函数 */
static uint16_t SPI_TIMEOUT_UserCallBac(uint8_t errorCode)
{
	//形参包括要写入寄存器的起始地址RegisterStartAddress(取值范围是从0x00——0x07)，写入寄存器的个数，指向要写入AD7192寄存器的数组的指针(DataBuffer才是要写出的值其他是中间变量)，
//const unsigned char OffsetInBuffer虽然定义但没有用到，主要是与通过SPI向AD7192读操作函数对称
/*分别定义一个读缓存和写缓存，再把写缓存赋值，然后通过
	调用void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)来执行，将写缓存数据传送至AD7192*/
	printf("SPI等待超时！errorCode = %d",errorCode);
	return 0 ;
}


