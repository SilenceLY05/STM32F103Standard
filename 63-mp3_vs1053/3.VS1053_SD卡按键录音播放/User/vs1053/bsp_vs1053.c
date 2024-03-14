#include "bsp_vs1053.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "string.h"
#include "flac.h"



const uint16_t bitrate[2][16] = 
{
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
	{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};


/* VS1053Ĭ�����ò��� */
_vs1053_obj vsset = 
{
	220,             //������220
	6,               //�������� 60Hz
	15,              //�������� 15dB
	10,              //�������� 10KHzHz
	15,              //�������� 10.5dB
	0,               //�ռ�Ч��
	1,		           //�����ٶ�
};

/* SPI��д ͨ��SPI�ӿڷ���һ���ֽڣ������ش�SPI���߽��յ����ֽڡ� */
unsigned char SPI_ReadWrite(unsigned char writedata)
{
	
	/* �ȴ��������ݻ�����Ϊ�գ�TXEʱ�� */
	while(SPI_I2S_GetFlagStatus(VS_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	
	SPI_I2S_SendData(VS_SPIx,writedata);
	
	/* �ȴ����ջ������ǿգ�RXNE�¼� */
	while (SPI_I2S_GetFlagStatus(VS_SPIx,SPI_I2S_FLAG_RXNE) == RESET)

	/* ��ȡ���ݼĴ�������ȡ���ջ��������� */
	return SPI_I2S_ReceiveData(VS_SPIx);
}



////////////////////////////////////////////////////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������

uint8_t VS_SPI_ReadWrite(uint8_t data)
{
	return SPI_ReadWrite(data);
}

static void SPI_SetSpeed(uint8_t SpeedSet)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0xFFC7;
	SPI2->CR1 |= SpeedSet;
	SPI_Cmd(SPI2,ENABLE);
}

/* ���õ�����ģʽ */
void VS_SPI_SpeedLow(void)
{
	SPI_SetSpeed(SPI_BaudRatePrescaler_32);
}

/* ���õ�����ģʽ */
void VS_SPI_SpeedHigh(void)
{
	SPI_SetSpeed(SPI_BaudRatePrescaler_8);
}


/* ��ʼ��VS1053 GPIO */
void VS1053_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	SPI_InitTypeDef         SPI_InitStructure;
	
	VS_GPIO_APBxPeriph_FUN(VS_GPIO_XCS_CLK | VS_GPIO_SCLK_CLK 
											 | VS_GPIO_MISO_CLK | VS_GPIO_MOSI_CLK 
											 | VS_GPIO_DREQ_CLK | VS_GPIO_RST_CLK 
											 | VS_GPIO_XDCS_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XCS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_SCLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_DREQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_DREQ_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_RST_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XDCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XDCS_PORT,&GPIO_InitStructure);
	
	VS_SPI_APBxPeriph_FUN(VS_SPI_CLK,ENABLE);
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(VS_SPIx,&SPI_InitStructure);
	
	/* ʹ��SPI2 */
	SPI_Cmd(VS_SPIx,ENABLE);
	
	//��������
	SPI_ReadWrite(0xFF);
}

/* ��ʼ��VS1053 GPIO */
void VS1053_Stop(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	VS_GPIO_APBxPeriph_FUN(VS_GPIO_XCS_CLK | VS_GPIO_SCLK_CLK 
											 | VS_GPIO_MISO_CLK | VS_GPIO_MOSI_CLK 
											 | VS_GPIO_DREQ_CLK | VS_GPIO_RST_CLK 
											 | VS_GPIO_XDCS_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XCS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_SCLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_DREQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_DREQ_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_RST_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XDCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XDCS_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(VS_GPIO_DREQ_PORT,VS_GPIO_DREQ_PIN);
	
	VS_RST_SET;
	VS_XDCS_SET;   //ȡ�����ݴ���
	VS_XCS_SET;    //ȡ�����ݴ���
}

/* ��VS1053д������ address�������ַ data���������� */
void VS_WriteReg(uint8_t address,uint16_t data)
{
	while(VS_DREQ_IN == 0);               //ÿ�ζ�д������ʼ����Ҫ��DREQ����Ϊ�͵�ƽ
	VS_SPI_SpeedLow();                    //����Ϊ����
	VS_XDCS_SET;
	VS_XCS_CLR;                           //�������ݴ����ڼ�XCS���뱣��Ϊ�͵�ƽ
	VS_SPI_ReadWrite(VS_WRITE_COMMAND);   //����дָ��
	VS_SPI_ReadWrite(address);            //����д��ַ
	VS_SPI_ReadWrite(data>>8);            //���͸߰�λ����
	VS_SPI_ReadWrite(data);               //���͵Ͱ�λ����
	VS_XCS_SET;                           //XCS�������ƶ��ͳ�֮���������ߵ�ƽ
	VS_SPI_SpeedHigh();                   //����
}

/* ��VS0153д���� data��Ҫд������� */
void VS_WR_Data(uint8_t data)
{
	VS_SPI_SpeedHigh();//����,��VS1003B,���ֵ���ܳ���36.864/4Mhz����������Ϊ9M 
	VS_XDCS_CLR;
	VS_SPI_ReadWrite(data);
	VS_XDCS_SET;
}

/* ��VS1053�ļĴ��� address �Ĵ�����ַ ���ض�ȡ������ */
uint16_t VS_ReadReg(uint8_t address)
{
	uint16_t temp = 0;
	while(VS_DREQ_IN == 0);               //ÿ�ζ�д������ʼ����Ҫ��DREQ����Ϊ�͵�ƽ
	VS_SPI_SpeedLow();                    //����Ϊ����
	VS_XDCS_SET;
	VS_XCS_CLR;                           //�������ݴ����ڼ�XCS���뱣��Ϊ�͵�ƽ
	VS_SPI_ReadWrite(VS_READ_COMMAND);    //���Ͷ�ָ��
	VS_SPI_ReadWrite(address);            //����д��ַ
	temp = VS_SPI_ReadWrite(0xff);        //��ȡ���ֽ�
	temp = temp<<8;
	temp += VS_SPI_ReadWrite(0xff);       //��ȡ���ֽڲ�����8λ���8λ���
	VS_XCS_SET;                           //XCS�������ƶ��ͳ�֮���������ߵ�ƽ
	VS_SPI_SpeedHigh();                   //����
	return temp;
}

/* ��ȡVS1053��RAM addressRAM��ַ */
uint16_t VS_WRAM_Read(uint16_t address)
{
	uint16_t temp=0;
	VS_WriteReg(SPI_WRAMADDR,address);
	temp = VS_ReadReg(SPI_WRAM);
	return temp;
}

/* дVS1053��RAM addressRAM��ַ value д������� */
void VS_WRAM_Write(uint16_t address,uint16_t value)
{
	VS_WriteReg(SPI_WRAMADDR,address);    //дRAM��ַ
	while(VS_DREQ_IN == 0);               //�ȴ�����	
	VS_WriteReg(SPI_WRAM,value);          //дRAMֵ
}

/* ����Kbps�Ĵ�С ����ֵ������ */
uint16_t VS_GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;
	HEAD0 = VS_ReadReg(SPI_HDAT0);
	HEAD1 = VS_ReadReg(SPI_HDAT1);
	
	switch(HEAD1)
	{
		case 0x7665:      //WAV��ʽ
		case 0x4145:      //AAC_ADTS
		case 0x574D:      //WMA��ʽ
		case 0x4D54:      //MIDI��ʽ
		case 0x4F67:      //OGG
		case 0x4144:      //AAC_ADIF
		case 0x4D34:      //AAC_MP4/M4A
		case 0x666C:      //FLAC��ʽ
		{
			HEAD1 = HEAD0 * 2 /25;     //�൱��*8/100
			if((HEAD1%10)>5)
				return HEAD1/10 +1;      //����С�����һλ��������
			else 
				return HEAD1/10;
		}
		default://MP3��ʽ,�����˽ײ�III�ı�
		{
			HEAD1>>=3;
			HEAD1 = HEAD1 & 0x03;
			if(HEAD1 == 3)
				HEAD1 = 1;
			else
				HEAD1 = 0;
			return bitrate[HEAD1][HEAD0>>12];
		}
	}
}


/* �õ�ƽ���ֽ��� ����ֵ��ƽ���ֽ����ٶ� */
uint32_t VS_GetByteRate(void)
{
	return VS_WRAM_Read(0x1E05);      //ƽ��λ��
}


/* �õ���Ҫ�������� ����ֵ����Ҫ�������� */
uint32_t VS_GetEndiFillByte(void)
{
	return VS_WRAM_Read(0x1E06);      //ƽ��λ��
}


/* �������ʱ�� */
void VS_ResetDecodeTime(void)
{
	VS_WriteReg(SPI_DECODE_TIME,0x0000);
	VS_WriteReg(SPI_DECODE_TIME,0x0000);   //��Ҫ�������Σ���֤���ᱻ�̼����ǵ�
}

/* �õ�MP3����ʱ�� ����ֵ������ʱ�� */
uint16_t VS_GetDecodeTime(void)
{
	uint16_t temp = 0;
	temp = VS_ReadReg(SPI_DECODE_TIME);
	return temp;
}


/* �����λVS */
void VS_SoftReset(void)
{
	uint8_t retry = 0;
	
	while(VS_DREQ_IN == 0);        //�ȴ������λ����
	
	VS_SPI_ReadWrite(0xFF);        //��������
	retry = 0;
	
	while(VS_ReadReg(SPI_MODE) != 0x0800)    //�����λ ��ģʽ
	{
		VS_WriteReg(SPI_MODE,0x0804);          //���������λ
		Delay_ms(2);                           //�ȴ�����1.35ms 
		if(retry++>100) break;
		
	}
	while(VS_DREQ_IN == 0);                  //�ȴ���λ����
	retry = 0;
	while(VS_ReadReg(SPI_CLOCKF) != 0x9800)  //����VS1053��ʱ��,3��Ƶ ,1.5xADD 
	{
		VS_WriteReg(SPI_CLOCKF,0x9800);
		if(retry++>100) break;
	}
	Delay_ms(20);
}


/* Ӳ����λ ����0�ɹ� ����ֵʧ�� */
uint8_t VS_HardReset(void)
{
	uint8_t retry = 0;
	VS_RST_CLR;
	Delay_ms(20);
	VS_XDCS_SET;//ȡ�����ݴ���
	VS_XCS_SET;//ȡ�����ݴ���
	VS_RST_SET;
	
	while(VS_DREQ_IN == 0 && retry <200)   //�ȴ�DREQΪ��
	{
		retry++;
		Delay_us(50);
	}
	Delay_ms(20);
	if(retry>=200) return 1;
	else
		return 0;
}


/* ���Ҳ��� */
void VS_SineTest(void)
{
	VS_HardReset();
	VS_WriteReg(SPI_VOL,0x2020);
	VS_WriteReg(SPI_MODE,0x0820);    //����VS����ģʽ
	while(VS_DREQ_IN == 0);          //�ȴ�DREQΪ��
	//��VS1053�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨VS1053�����������Ҳ���Ƶ��ֵ��������㷽����VS1053��datasheet
	VS_SPI_SpeedLow();               //����
	VS_XDCS_CLR;                     //ѡ�����ݴ���
	VS_SPI_ReadWrite(0x53);
	VS_SPI_ReadWrite(0xEF);
	VS_SPI_ReadWrite(0x6E);
	VS_SPI_ReadWrite(0x24);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	Delay_ms(100);
	VS_XDCS_SET;
	
	//�˳����Ҳ���
	VS_XDCS_CLR;                     //ѡ�����ݴ���
	VS_SPI_ReadWrite(0x45);
	VS_SPI_ReadWrite(0x78);
	VS_SPI_ReadWrite(0x69);
	VS_SPI_ReadWrite(0x74);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	Delay_ms(100);
	VS_XDCS_SET;
	
}

/* �Ĵ������� */
uint16_t VS_RamTest(void)
{
	VS_HardReset();
	VS_WriteReg(SPI_VOL,0x2020);
	VS_WriteReg(SPI_MODE,0x0820);    //����VS����ģʽ
	while(VS_DREQ_IN == 0);          //�ȴ�DREQΪ��
	//��VS1053�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨VS1053�����������Ҳ���Ƶ��ֵ��������㷽����VS1053��datasheet
	VS_SPI_SpeedLow();               //����
	VS_XDCS_CLR;                     //ѡ�����ݴ���
	VS_SPI_ReadWrite(0x4D);
	VS_SPI_ReadWrite(0xEA);
	VS_SPI_ReadWrite(0x6D);
	VS_SPI_ReadWrite(0x54);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	VS_SPI_ReadWrite(0x00);
	Delay_ms(150);
	VS_XDCS_SET;
	
	//������ֵΪ0x83FF���ʾȫ�����Ǻõ�
	return VS_ReadReg(SPI_HDAT0);
}




/* ����һ����Ƶ���� �̶�Ϊ32�ֽ� ����ֵΪ0ʱ�ɹ�����ֵʧ�� */
uint8_t VS_SendMusicData(uint8_t *buff)
{
	uint8_t i;
	
	if(VS_DREQ_IN!=0)  //��������ΪVS
	{
		VS_XDCS_CLR;
		for(i = 0;i<32;i++)
		{
			VS_SPI_ReadWrite(buff[i]);
		}
		VS_XDCS_SET;
	}
	else
		return 1;
	
	return 0;
}

/* ���ò����ٶ� �����ٶ�speed��0,1=�����ٶȣ�2=2���٣�3=3�����Դ����� */
void VS_SetSpeed(uint8_t speed)
{
	VS_WRAM_Write(0x1E04,speed);    //��playspeed�Ĵ���д�벥���ٶ�
}

/* �и� ͨ���˷�����������л����� */
void VS_RestartPlay(void)
{
	uint16_t temp,i;
	uint8_t n;
	uint8_t vsbuff[32];
	
	for(n=0;n<32;n++)
	{
		vsbuff[n] = 0;   //��������0
	}
	temp = VS_ReadReg(SPI_MODE);        //��ȡSPI_MODE�Ĵ����е���ֵ
	temp |= 1<<3;                       //����SM_CANCELλ
	temp |= 1<<2;                       //����SM_LAYER 12λ��������MP1��MP2
	VS_WriteReg(SPI_MODE,temp);         //����ȡ����ǰ����ָ��
	for(i=0;i<2048;)                    //����2048��0,�ڼ��ȡSM_CANCELλ.���Ϊ0,���ʾ�Ѿ�ȡ���˵�ǰ����
	{
		if(VS_SendMusicData(vsbuff) == 0) //ÿ����32���ֽں���һ��
		{
			i+= 32;                         //������32���ֽ�
			temp = VS_ReadReg(SPI_MODE);    //��ȡSPI_MODE������
			if((temp &(1<<3)) == 0) break;  //�ɹ�ȡ����
		}
	}
	if(i<2048)                          //SM_CANCEL����
	{
		temp = VS_GetEndiFillByte() & 0xFF; //��ȡ����ֽ�
		for(n=0;n<32;n++) vsbuff[n] = temp; //����ֽڷ�������
		for(i=0;i<2052;)
		{
			if(VS_SendMusicData(vsbuff) == 0) i+= 32;   //���
		}
	}
	else
	{
		VS_SoftReset();//SM_CANCEL���ɹ�,�����,��Ҫ��λ 
	}
	
	temp = VS_ReadReg(SPI_HDAT0);
	temp += VS_ReadReg(SPI_HDAT1);
	
	if(temp)           //��λ,����û�гɹ�ȡ��,��ɱ���,Ӳ��λ
	{
		VS_HardReset();  //Ӳ��λ
		VS_SoftReset();  //��λ 
	}
}


/* VSװ��patch patch:patch�׵�ַ len:patch���� */
void VS_LoadPatch(uint16_t *patch,uint16_t len)
{
	uint16_t i,address,n,value;
	
	for(i = 0; i<len;)
	{
		address = patch[i++];
		n = patch[i++];
		
		if(n&0x8000U)
		{
			n &= 0x7FFF;
			value = patch[i++];
			while(n--)
			{
				VS_WriteReg(address,value);
			}
		}
		else
		{
			while(n--)
			{
				value = patch[i++];
				VS_WriteReg(address,value);
			}
		}
	}
}


/* �趨VS���ŵ������͸ߵ��� */
void VS_SetVol(uint8_t volx)
{
	uint16_t volt = 0;           //�ݴ�����ֵ
	volt = 254 - volx;           //ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ 
	volt <<= 8;
	volt += 254 - volx;          //�õ��������ú��С
	VS_WriteReg(SPI_VOL,volt);   //������ 
}


/* �趨�ߵ�������  bfreq:��Ƶ����Ƶ�� 2~15(��λ:10Hz)  bass:��Ƶ���� 0~15(��λ:1dB)
 * tfreq:��Ƶ����Ƶ�� 	1~15(��λ:Khz)  treble:��Ƶ����  0~15(��λ:1.5dB,С��9��ʱ��Ϊ����) */
void VS_SetBass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble)
{
	uint16_t bass_set = 0;                //�ݴ������Ĵ���ֵ
	signed char temp = 0;                 //�任
	
	if(treble == 0)  temp = 0;
	else if(treble>8) temp = treble - 8;
	else temp = treble -9;
	
	bass_set = temp&0x0F;            //�����趨
	bass_set<<=4;
	bass_set += tfreq&0x0F;          //��������Ƶ��
	bass_set<<=4;
	bass_set += bass&0x0F;           //�����趨
	bass_set <<=4;
	bass_set += bfreq&0x0F;          //��������
	VS_WriteReg(SPI_BASS,bass_set);  //BASS
}

/* �趨��Ч eft:0,�ر�;1,��С;2,�е�;3,��� */
void VS_SetEffect(uint8_t eft)
{
	uint16_t temp;
	temp = VS_ReadReg(SPI_MODE);   //��ȡSPI_MODE������
	
	if(eft&0x01)  temp |= 1<<4;    //�趨LO
	else temp &= ~(1<<5);          //ȡ��LO
	if(eft&0x02)  temp |= 1<<7;    //�趨HO
	else temp &= ~(1<<7);          //ȡ��HO	
	VS_WriteReg(SPI_MODE,temp);    //�趨ģʽ  
		
}

/* ������������Ч���ߵ��� */
void VS_SetAll(void)
{
	VS_SetVol(vsset.mvol);
	VS_SetBass(vsset.bflimit,vsset.bass,vsset.tflimit,vsset.treble);
	VS_SetEffect(vsset.effect);
	VS_SetSpeed(vsset.speed);
}



/*----------------- ¼�����ܺ��� --------------------------*/
//VS1053��WAV¼����bug,���plugin��������������� 	
const uint16_t wav_plugin[40] = 
{
	0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e, 
};


/* ����PCM¼��ģʽ agc:0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64�� */
void Recoder_EnterRecMode(_recoder_obj *recset)
{
	//�����IMA ADPCM,�����ʼ��㹫ʽ����:
 	//������=CLKI/256*d;	
	//����d=0,��2��Ƶ,�ⲿ����Ϊ12.288M.��ôFc=(2*12288000)/256*6=16Khz
	//���������PCM,������ֱ�Ӿ�д����ֵ
	VS_WriteReg(SPI_BASS,0x0000);                               
	VS_WriteReg(SPI_AICTRL0,recset->samplerate*8000);  //���ò�����     
	VS_WriteReg(SPI_AICTRL1,recset->age*1024/2);       //��������,0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64��	      
	VS_WriteReg(SPI_AICTRL2,0);                        //�����������ֵ,0,�������ֵ65536=64X
	VS_WriteReg(SPI_AICTRL3,6+recset->channel);        //4������PCMģʽ + 2�� ��ͨ�� 3�� ��ͨ��
	VS_WriteReg(SPI_CLOCKF,0x2000);                    //����VS10XX��ʱ��,MULT:2��Ƶ;ADD:������;CLK:12.288Mhz
	VS_WriteReg(SPI_MODE,0x1804 | recset->input<<4);   //MIC,¼������    
	Delay_ms(5);                                       //�ȴ�����1.35ms 
	VS_LoadPatch((uint16_t*)wav_plugin,40);            //VS1053��WAV¼����Ҫpatch
}

