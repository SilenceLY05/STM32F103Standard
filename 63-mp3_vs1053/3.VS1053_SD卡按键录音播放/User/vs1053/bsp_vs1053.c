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


/* VS1053默认设置参数 */
_vs1053_obj vsset = 
{
	220,             //音量：220
	6,               //低音上限 60Hz
	15,              //低音提升 15dB
	10,              //高音下限 10KHzHz
	15,              //高音提升 10.5dB
	0,               //空间效果
	1,		           //正常速度
};

/* SPI读写 通过SPI接口发送一个字节，并返回从SPI总线接收到的字节。 */
unsigned char SPI_ReadWrite(unsigned char writedata)
{
	
	/* 等待发送数据缓冲区为空，TXE时间 */
	while(SPI_I2S_GetFlagStatus(VS_SPIx,SPI_I2S_FLAG_TXE) == RESET)
	
	SPI_I2S_SendData(VS_SPIx,writedata);
	
	/* 等待接收缓冲区非空，RXNE事件 */
	while (SPI_I2S_GetFlagStatus(VS_SPIx,SPI_I2S_FLAG_RXNE) == RESET)

	/* 读取数据寄存器，获取接收缓冲区数据 */
	return SPI_I2S_ReceiveData(VS_SPIx);
}



////////////////////////////////////////////////////////////////////////////////
//移植时候的接口
//data:要写入的数据
//返回值:读到的数据

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

/* 设置到低速模式 */
void VS_SPI_SpeedLow(void)
{
	SPI_SetSpeed(SPI_BaudRatePrescaler_32);
}

/* 设置到高速模式 */
void VS_SPI_SpeedHigh(void)
{
	SPI_SetSpeed(SPI_BaudRatePrescaler_8);
}


/* 初始化VS1053 GPIO */
void VS1053_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	SPI_InitTypeDef         SPI_InitStructure;
	
	VS_GPIO_APBxPeriph_FUN(VS_GPIO_XCS_CLK | VS_GPIO_SCLK_CLK 
											 | VS_GPIO_MISO_CLK | VS_GPIO_MOSI_CLK 
											 | VS_GPIO_DREQ_CLK | VS_GPIO_RST_CLK 
											 | VS_GPIO_XDCS_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XCS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_SCLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_DREQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_DREQ_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_RST_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XDCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
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
	
	/* 使能SPI2 */
	SPI_Cmd(VS_SPIx,ENABLE);
	
	//启动传输
	SPI_ReadWrite(0xFF);
}

/* 初始化VS1053 GPIO */
void VS1053_Stop(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	
	VS_GPIO_APBxPeriph_FUN(VS_GPIO_XCS_CLK | VS_GPIO_SCLK_CLK 
											 | VS_GPIO_MISO_CLK | VS_GPIO_MOSI_CLK 
											 | VS_GPIO_DREQ_CLK | VS_GPIO_RST_CLK 
											 | VS_GPIO_XDCS_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XCS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_SCLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_DREQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_DREQ_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_RST_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = VS_GPIO_XDCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VS_GPIO_XDCS_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(VS_GPIO_DREQ_PORT,VS_GPIO_DREQ_PIN);
	
	VS_RST_SET;
	VS_XDCS_SET;   //取消数据传输
	VS_XCS_SET;    //取消数据传输
}

/* 向VS1053写入命令 address：命令地址 data：命令数据 */
void VS_WriteReg(uint8_t address,uint16_t data)
{
	while(VS_DREQ_IN == 0);               //每次读写操作开始都需要将DREQ设置为低电平
	VS_SPI_SpeedLow();                    //设置为低速
	VS_XDCS_SET;
	VS_XCS_CLR;                           //整个数据传输期间XCS必须保持为低电平
	VS_SPI_ReadWrite(VS_WRITE_COMMAND);   //发送写指令
	VS_SPI_ReadWrite(address);            //发送写地址
	VS_SPI_ReadWrite(data>>8);            //发送高八位数据
	VS_SPI_ReadWrite(data);               //发送低八位数据
	VS_XCS_SET;                           //XCS在数据移动送出之后驱动到高电平
	VS_SPI_SpeedHigh();                   //高速
}

/* 向VS0153写数据 data：要写入的数据 */
void VS_WR_Data(uint8_t data)
{
	VS_SPI_SpeedHigh();//高速,对VS1003B,最大值不能超过36.864/4Mhz，这里设置为9M 
	VS_XDCS_CLR;
	VS_SPI_ReadWrite(data);
	VS_XDCS_SET;
}

/* 读VS1053的寄存器 address 寄存器地址 返回读取的数据 */
uint16_t VS_ReadReg(uint8_t address)
{
	uint16_t temp = 0;
	while(VS_DREQ_IN == 0);               //每次读写操作开始都需要将DREQ设置为低电平
	VS_SPI_SpeedLow();                    //设置为低速
	VS_XDCS_SET;
	VS_XCS_CLR;                           //整个数据传输期间XCS必须保持为低电平
	VS_SPI_ReadWrite(VS_READ_COMMAND);    //发送读指令
	VS_SPI_ReadWrite(address);            //发送写地址
	temp = VS_SPI_ReadWrite(0xff);        //读取高字节
	temp = temp<<8;
	temp += VS_SPI_ReadWrite(0xff);       //读取低字节并将高8位与低8位结合
	VS_XCS_SET;                           //XCS在数据移动送出之后驱动到高电平
	VS_SPI_SpeedHigh();                   //高速
	return temp;
}

/* 读取VS1053的RAM addressRAM地址 */
uint16_t VS_WRAM_Read(uint16_t address)
{
	uint16_t temp=0;
	VS_WriteReg(SPI_WRAMADDR,address);
	temp = VS_ReadReg(SPI_WRAM);
	return temp;
}

/* 写VS1053的RAM addressRAM地址 value 写入的数据 */
void VS_WRAM_Write(uint16_t address,uint16_t value)
{
	VS_WriteReg(SPI_WRAMADDR,address);    //写RAM地址
	while(VS_DREQ_IN == 0);               //等待空闲	
	VS_WriteReg(SPI_WRAM,value);          //写RAM值
}

/* 返回Kbps的大小 返回值：码率 */
uint16_t VS_GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;
	HEAD0 = VS_ReadReg(SPI_HDAT0);
	HEAD1 = VS_ReadReg(SPI_HDAT1);
	
	switch(HEAD1)
	{
		case 0x7665:      //WAV格式
		case 0x4145:      //AAC_ADTS
		case 0x574D:      //WMA格式
		case 0x4D54:      //MIDI格式
		case 0x4F67:      //OGG
		case 0x4144:      //AAC_ADIF
		case 0x4D34:      //AAC_MP4/M4A
		case 0x666C:      //FLAC格式
		{
			HEAD1 = HEAD0 * 2 /25;     //相当于*8/100
			if((HEAD1%10)>5)
				return HEAD1/10 +1;      //对于小数点第一位四舍五入
			else 
				return HEAD1/10;
		}
		default://MP3格式,仅做了阶层III的表
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


/* 得到平均字节数 返回值：平均字节数速度 */
uint32_t VS_GetByteRate(void)
{
	return VS_WRAM_Read(0x1E05);      //平均位速
}


/* 得到需要填充的数字 返回值：需要填充的数字 */
uint32_t VS_GetEndiFillByte(void)
{
	return VS_WRAM_Read(0x1E06);      //平均位速
}


/* 重设解码时间 */
void VS_ResetDecodeTime(void)
{
	VS_WriteReg(SPI_DECODE_TIME,0x0000);
	VS_WriteReg(SPI_DECODE_TIME,0x0000);   //需要操作两次，保证不会被固件覆盖掉
}

/* 得到MP3播放时间 返回值：解码时长 */
uint16_t VS_GetDecodeTime(void)
{
	uint16_t temp = 0;
	temp = VS_ReadReg(SPI_DECODE_TIME);
	return temp;
}


/* 软件复位VS */
void VS_SoftReset(void)
{
	uint8_t retry = 0;
	
	while(VS_DREQ_IN == 0);        //等待软件复位结束
	
	VS_SPI_ReadWrite(0xFF);        //启动传输
	retry = 0;
	
	while(VS_ReadReg(SPI_MODE) != 0x0800)    //软件复位 新模式
	{
		VS_WriteReg(SPI_MODE,0x0804);          //开启软件复位
		Delay_ms(2);                           //等待至少1.35ms 
		if(retry++>100) break;
		
	}
	while(VS_DREQ_IN == 0);                  //等待复位结束
	retry = 0;
	while(VS_ReadReg(SPI_CLOCKF) != 0x9800)  //设置VS1053的时钟,3倍频 ,1.5xADD 
	{
		VS_WriteReg(SPI_CLOCKF,0x9800);
		if(retry++>100) break;
	}
	Delay_ms(20);
}


/* 硬件复位 返回0成功 其他值失败 */
uint8_t VS_HardReset(void)
{
	uint8_t retry = 0;
	VS_RST_CLR;
	Delay_ms(20);
	VS_XDCS_SET;//取消数据传输
	VS_XCS_SET;//取消数据传输
	VS_RST_SET;
	
	while(VS_DREQ_IN == 0 && retry <200)   //等待DREQ为高
	{
		retry++;
		Delay_us(50);
	}
	Delay_ms(20);
	if(retry>=200) return 1;
	else
		return 0;
}


/* 正弦测试 */
void VS_SineTest(void)
{
	VS_HardReset();
	VS_WriteReg(SPI_VOL,0x2020);
	VS_WriteReg(SPI_MODE,0x0820);    //进入VS测试模式
	while(VS_DREQ_IN == 0);          //等待DREQ为高
	//向VS1053发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定VS1053所产生的正弦波的频率值，具体计算方法见VS1053的datasheet
	VS_SPI_SpeedLow();               //低速
	VS_XDCS_CLR;                     //选中数据传输
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
	
	//退出正弦测试
	VS_XDCS_CLR;                     //选中数据传输
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

/* 寄存器测试 */
uint16_t VS_RamTest(void)
{
	VS_HardReset();
	VS_WriteReg(SPI_VOL,0x2020);
	VS_WriteReg(SPI_MODE,0x0820);    //进入VS测试模式
	while(VS_DREQ_IN == 0);          //等待DREQ为高
	//向VS1053发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定VS1053所产生的正弦波的频率值，具体计算方法见VS1053的datasheet
	VS_SPI_SpeedLow();               //低速
	VS_XDCS_CLR;                     //选中数据传输
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
	
	//若返回值为0x83FF则表示全部都是好的
	return VS_ReadReg(SPI_HDAT0);
}




/* 发送一次音频数据 固定为32字节 返回值为0时成功其他值失败 */
uint8_t VS_SendMusicData(uint8_t *buff)
{
	uint8_t i;
	
	if(VS_DREQ_IN!=0)  //发送数据为VS
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

/* 设置播放速度 播放速度speed：0,1=正常速度，2=2倍速，3=3倍速以此类推 */
void VS_SetSpeed(uint8_t speed)
{
	VS_WRAM_Write(0x1E04,speed);    //向playspeed寄存器写入播放速度
}

/* 切歌 通过此方法不会出现切换噪声 */
void VS_RestartPlay(void)
{
	uint16_t temp,i;
	uint8_t n;
	uint8_t vsbuff[32];
	
	for(n=0;n<32;n++)
	{
		vsbuff[n] = 0;   //将数组清0
	}
	temp = VS_ReadReg(SPI_MODE);        //读取SPI_MODE寄存器中的数值
	temp |= 1<<3;                       //设置SM_CANCEL位
	temp |= 1<<2;                       //设置SM_LAYER 12位，允许播放MP1，MP2
	VS_WriteReg(SPI_MODE,temp);         //设置取消当前解码指令
	for(i=0;i<2048;)                    //发送2048个0,期间读取SM_CANCEL位.如果为0,则表示已经取消了当前解码
	{
		if(VS_SendMusicData(vsbuff) == 0) //每发送32个字节后检测一次
		{
			i+= 32;                         //发送了32个字节
			temp = VS_ReadReg(SPI_MODE);    //读取SPI_MODE的内容
			if((temp &(1<<3)) == 0) break;  //成功取消了
		}
	}
	if(i<2048)                          //SM_CANCEL正常
	{
		temp = VS_GetEndiFillByte() & 0xFF; //读取填充字节
		for(n=0;n<32;n++) vsbuff[n] = temp; //填充字节放入数组
		for(i=0;i<2052;)
		{
			if(VS_SendMusicData(vsbuff) == 0) i+= 32;   //填充
		}
	}
	else
	{
		VS_SoftReset();//SM_CANCEL不成功,坏情况,需要软复位 
	}
	
	temp = VS_ReadReg(SPI_HDAT0);
	temp += VS_ReadReg(SPI_HDAT1);
	
	if(temp)           //软复位,还是没有成功取消,放杀手锏,硬复位
	{
		VS_HardReset();  //硬复位
		VS_SoftReset();  //软复位 
	}
}


/* VS装载patch patch:patch首地址 len:patch长度 */
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


/* 设定VS播放的音量和高低音 */
void VS_SetVol(uint8_t volx)
{
	uint16_t volt = 0;           //暂存音量值
	volt = 254 - volx;           //取反一下,得到最大值,表示最大的表示 
	volt <<= 8;
	volt += 254 - volx;          //得到音量设置后大小
	VS_WriteReg(SPI_VOL,volt);   //设音量 
}


/* 设定高低音控制  bfreq:低频上限频率 2~15(单位:10Hz)  bass:低频增益 0~15(单位:1dB)
 * tfreq:高频下限频率 	1~15(单位:Khz)  treble:高频增益  0~15(单位:1.5dB,小于9的时候为负数) */
void VS_SetBass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble)
{
	uint16_t bass_set = 0;                //暂存音调寄存器值
	signed char temp = 0;                 //变换
	
	if(treble == 0)  temp = 0;
	else if(treble>8) temp = treble - 8;
	else temp = treble -9;
	
	bass_set = temp&0x0F;            //高音设定
	bass_set<<=4;
	bass_set += tfreq&0x0F;          //高音下限频率
	bass_set<<=4;
	bass_set += bass&0x0F;           //低音设定
	bass_set <<=4;
	bass_set += bfreq&0x0F;          //低音上限
	VS_WriteReg(SPI_BASS,bass_set);  //BASS
}

/* 设定音效 eft:0,关闭;1,最小;2,中等;3,最大 */
void VS_SetEffect(uint8_t eft)
{
	uint16_t temp;
	temp = VS_ReadReg(SPI_MODE);   //读取SPI_MODE的内容
	
	if(eft&0x01)  temp |= 1<<4;    //设定LO
	else temp &= ~(1<<5);          //取消LO
	if(eft&0x02)  temp |= 1<<7;    //设定HO
	else temp &= ~(1<<7);          //取消HO	
	VS_WriteReg(SPI_MODE,temp);    //设定模式  
		
}

/* 设置音量、音效、高低音 */
void VS_SetAll(void)
{
	VS_SetVol(vsset.mvol);
	VS_SetBass(vsset.bflimit,vsset.bass,vsset.tflimit,vsset.treble);
	VS_SetEffect(vsset.effect);
	VS_SetSpeed(vsset.speed);
}



/*----------------- 录音功能函数 --------------------------*/
//VS1053的WAV录音有bug,这个plugin可以修正这个问题 	
const uint16_t wav_plugin[40] = 
{
	0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e, 
};


/* 激活PCM录音模式 agc:0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍 */
void Recoder_EnterRecMode(_recoder_obj *recset)
{
	//如果是IMA ADPCM,采样率计算公式如下:
 	//采样率=CLKI/256*d;	
	//假设d=0,并2倍频,外部晶振为12.288M.那么Fc=(2*12288000)/256*6=16Khz
	//如果是线性PCM,采样率直接就写采样值
	VS_WriteReg(SPI_BASS,0x0000);                               
	VS_WriteReg(SPI_AICTRL0,recset->samplerate*8000);  //设置采样率     
	VS_WriteReg(SPI_AICTRL1,recset->age*1024/2);       //设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍	      
	VS_WriteReg(SPI_AICTRL2,0);                        //设置增益最大值,0,代表最大值65536=64X
	VS_WriteReg(SPI_AICTRL3,6+recset->channel);        //4：线性PCM模式 + 2： 左通道 3： 右通道
	VS_WriteReg(SPI_CLOCKF,0x2000);                    //设置VS10XX的时钟,MULT:2倍频;ADD:不允许;CLK:12.288Mhz
	VS_WriteReg(SPI_MODE,0x1804 | recset->input<<4);   //MIC,录音激活    
	Delay_ms(5);                                       //等待至少1.35ms 
	VS_LoadPatch((uint16_t*)wav_plugin,40);            //VS1053的WAV录音需要patch
}

