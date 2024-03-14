#ifndef __BSP_VS1053_H
#define __BSP_VS1053_H


#include "stm32f10x.h"

/****************** 定义VS1053 SPI使用的引脚 *****************/
#define VS_SPIx                               SPI2
#define VS_SPI_APBxPeriph_FUN                 RCC_APB1PeriphClockCmd
#define VS_SPI_CLK                            RCC_APB1Periph_SPI2

#define VS_GPIO_APBxPeriph_FUN                RCC_APB2PeriphClockCmd

/*定义VS1053的片选输入管脚（低电平有效）*/
#define VS_GPIO_XCS_CLK                       RCC_APB2Periph_GPIOB
#define VS_GPIO_XCS_PORT                      GPIOB
#define VS_GPIO_XCS_PIN                       GPIO_Pin_12

/*定义VS1053的时钟管脚*/
#define VS_GPIO_SCLK_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_SCLK_PORT                     GPIOB
#define VS_GPIO_SCLK_PIN                      GPIO_Pin_13

/*定义VS1053的MISO管脚*/
#define VS_GPIO_MISO_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_MISO_PORT                     GPIOB
#define VS_GPIO_MISO_PIN                      GPIO_Pin_14

/*定义VS1053的MOSI管脚*/
#define VS_GPIO_MOSI_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_MOSI_PORT                     GPIOB
#define VS_GPIO_MOSI_PIN                      GPIO_Pin_15

/*定义VS1053的DREQ数据请求，输入总线管脚*/
#define VS_GPIO_DREQ_CLK                      RCC_APB2Periph_GPIOC
#define VS_GPIO_DREQ_PORT                     GPIOC
#define VS_GPIO_DREQ_PIN                      GPIO_Pin_6

/*定义VS1053的RST管脚*/
#define VS_GPIO_RST_CLK                       RCC_APB2Periph_GPIOC
#define VS_GPIO_RST_PORT                      GPIOC
#define VS_GPIO_RST_PIN                       GPIO_Pin_6

/*定义VS1053的数据片选/字节同步管脚*/
#define VS_GPIO_XDCS_CLK                      RCC_APB2Periph_GPIOC
#define VS_GPIO_XDCS_PORT                     GPIOC
#define VS_GPIO_XDCS_PIN                      GPIO_Pin_6

/* 若DREQ引脚为高电平则可以接收最少32字节的数据 若为低电平则停止向VS1053发送数据和命令 */
#define VS_DREQ_IN                            GPIO_ReadInputDataBit(VS_GPIO_DREQ_PORT,VS_GPIO_DREQ_PIN)

#define VS_XDCS_SET                           GPIO_SetBits(VS_GPIO_XDCS_PORT,VS_GPIO_XDCS_PIN)
#define VS_XDCS_CLR                           GPIO_ResetBits(VS_GPIO_XDCS_PORT,VS_GPIO_XDCS_PIN)

#define VS_XCS_SET                            GPIO_SetBits(VS_GPIO_XCS_PORT,VS_GPIO_XCS_PIN)
#define VS_XCS_CLR                            GPIO_ResetBits(VS_GPIO_XCS_PORT,VS_GPIO_XCS_PIN)

#define VS_RST_SET                            GPIO_SetBits(VS_GPIO_RST_PORT,VS_GPIO_RST_PIN)
#define VS_RST_CLR                            GPIO_ResetBits(VS_GPIO_RST_PORT,VS_GPIO_RST_PIN)


__packed typedef struct 
{							  
	u8 mvol;		//主音量,范围:0~254
	u8 bflimit;		//低音频率限定,范围:2~15(单位:10Hz)
	u8 bass;		//低音,范围:0~15.0表示关闭.(单位:1dB)
	u8 tflimit;		//高音频率限定,范围:1~15(单位:Khz)
	u8 treble;		//高音,范围:0~15(单位:1.5dB)(原本范围是:-8~7,通过函数修改了);
	u8 effect;		//空间效果设置.0,关闭;1,最小;2,中等;3,最大.
	u8 speed; 	//播放速度：0，1:正常播放，2:2倍速度，3:3倍速度...  
}_vs1053_obj;

__packed typedef struct
{
	uint8_t input;                      //输入通道选择.0：MICP，1：LINE1
	uint8_t samplerate;                 //采样速率选择（x*8K）：1：8K，2：16K，3：24K...
	uint8_t channel;                    //声道：1：双声道，2：左声道，3：右声道
	uint8_t age;                        //增益：1~64
}_recoder_obj;


extern _vs1053_obj vsset;		//VS1053设置

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1053寄存器定义
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f 

#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 		 

#define I2S_CONFIG			0XC040
#define GPIO_DDR			0XC017
#define GPIO_IDATA			0XC018
#define GPIO_ODATA			0XC019

#define BUFFSIZE        1024*4

uint8_t VS_SPI_ReadWrite(uint8_t data);
void VS_SPI_SpeedLow(void);
void VS_SPI_SpeedHigh(void);
void VS1053_Init(void);
void VS1053_Stop(void);
void VS_WriteReg(uint8_t address,uint16_t data);
void VS_WR_Data(uint8_t data);
uint16_t VS_ReadReg(uint8_t address);
uint16_t VS_WRAM_Read(uint16_t address);
void VS_WRAM_Write(uint16_t address,uint16_t value);
uint16_t VS_GetHeadInfo(void);
uint32_t VS_GetByteRate(void);
uint32_t VS_GetEndiFillByte(void);
void VS_ResetDecodeTime(void);
uint16_t VS_GetDecodeTime(void);
void VS_SoftReset(void);
uint8_t VS_HardReset(void);
void VS_SineTest(void);
uint16_t VS_RamTest(void);
uint8_t VS_SendMusicData(uint8_t *buff);
void VS_SetSpeed(uint8_t speed);
void VS_RestartPlay(void);
void VS_LoadPatch(uint16_t *patch,uint16_t len);
void VS_SetVol(uint8_t volx);
void VS_SetBass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble);
void VS_SetEffect(uint8_t eft);
void VS_SetAll(void);
void Recoder_EnterRecMode(_recoder_obj *recset);

//RIFF块
typedef __packed struct
{
	uint32_t ChunkID;         //chunk id;这里固定为"RIFF",即0X46464952
	uint32_t ChunkSize;       //集合大小;文件总大小减8
	uint32_t Format;          //格式;WAVE,即0X45564157
}ChunkRIFF;

//FMT块
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;这里固定为"fmt ",即0X20746D66
	uint32_t SubChunkSize;    //子集合大小(不包括ID和Size);这里为:20.
	uint16_t AudioFormat;     //音频格式;0X10,表示线性PCM;0X11表示IMA ADPCM
	uint16_t NumOfChannels;   //通道数量;1,表示单声道;2,表示双声道;
	uint32_t SampleRate;      //采样率;0X1F40,表示8Khz
	uint32_t ByteRate;        //字节速率;
	uint16_t BlockAlign;      //块对齐(字节); 
	uint16_t BitsPerSample;   //单个采样数据大小;4位ADPCM,设置为4
	uint16_t ByteExtraData;   //附加的数据字节;2个; 线性PCM,没有这个参数
	uint16_t ExtraData;       //附加的数据,单个采样数据块大小;0X1F9:505字节  线性PCM,没有这个参数
}ChunkFMT;

//FACT块
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;这里固定为"fact",即0X74636166;
	uint32_t SubChunkSize;    //子集合大小(不包括ID和Size);这里为:4.
	uint32_t NumOfSamples;    //采样的数量; 
}ChunkFACT;

//data块
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;这里固定为"data",即0X61746164
	uint32_t SubChunkSize;    //子集合大小(不包括ID和Size);文件大小-60.
}ChunkDATA;

//WAVE头
typedef __packed struct
{
	ChunkRIFF riff;           //riff块
	ChunkFMT  fmt;            //fmt块
	ChunkFACT fact;           //fact块 线性PCM,没有这个结构体	 
	ChunkDATA data;          	//data块	
}__WaveHeader;




#endif /* __BSP_VS1053_H */

