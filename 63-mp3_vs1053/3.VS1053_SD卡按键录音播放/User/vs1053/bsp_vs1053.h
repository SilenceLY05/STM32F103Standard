#ifndef __BSP_VS1053_H
#define __BSP_VS1053_H


#include "stm32f10x.h"

/****************** ����VS1053 SPIʹ�õ����� *****************/
#define VS_SPIx                               SPI2
#define VS_SPI_APBxPeriph_FUN                 RCC_APB1PeriphClockCmd
#define VS_SPI_CLK                            RCC_APB1Periph_SPI2

#define VS_GPIO_APBxPeriph_FUN                RCC_APB2PeriphClockCmd

/*����VS1053��Ƭѡ����ܽţ��͵�ƽ��Ч��*/
#define VS_GPIO_XCS_CLK                       RCC_APB2Periph_GPIOB
#define VS_GPIO_XCS_PORT                      GPIOB
#define VS_GPIO_XCS_PIN                       GPIO_Pin_12

/*����VS1053��ʱ�ӹܽ�*/
#define VS_GPIO_SCLK_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_SCLK_PORT                     GPIOB
#define VS_GPIO_SCLK_PIN                      GPIO_Pin_13

/*����VS1053��MISO�ܽ�*/
#define VS_GPIO_MISO_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_MISO_PORT                     GPIOB
#define VS_GPIO_MISO_PIN                      GPIO_Pin_14

/*����VS1053��MOSI�ܽ�*/
#define VS_GPIO_MOSI_CLK                      RCC_APB2Periph_GPIOB
#define VS_GPIO_MOSI_PORT                     GPIOB
#define VS_GPIO_MOSI_PIN                      GPIO_Pin_15

/*����VS1053��DREQ���������������߹ܽ�*/
#define VS_GPIO_DREQ_CLK                      RCC_APB2Periph_GPIOC
#define VS_GPIO_DREQ_PORT                     GPIOC
#define VS_GPIO_DREQ_PIN                      GPIO_Pin_6

/*����VS1053��RST�ܽ�*/
#define VS_GPIO_RST_CLK                       RCC_APB2Periph_GPIOC
#define VS_GPIO_RST_PORT                      GPIOC
#define VS_GPIO_RST_PIN                       GPIO_Pin_6

/*����VS1053������Ƭѡ/�ֽ�ͬ���ܽ�*/
#define VS_GPIO_XDCS_CLK                      RCC_APB2Periph_GPIOC
#define VS_GPIO_XDCS_PORT                     GPIOC
#define VS_GPIO_XDCS_PIN                      GPIO_Pin_6

/* ��DREQ����Ϊ�ߵ�ƽ����Խ�������32�ֽڵ����� ��Ϊ�͵�ƽ��ֹͣ��VS1053�������ݺ����� */
#define VS_DREQ_IN                            GPIO_ReadInputDataBit(VS_GPIO_DREQ_PORT,VS_GPIO_DREQ_PIN)

#define VS_XDCS_SET                           GPIO_SetBits(VS_GPIO_XDCS_PORT,VS_GPIO_XDCS_PIN)
#define VS_XDCS_CLR                           GPIO_ResetBits(VS_GPIO_XDCS_PORT,VS_GPIO_XDCS_PIN)

#define VS_XCS_SET                            GPIO_SetBits(VS_GPIO_XCS_PORT,VS_GPIO_XCS_PIN)
#define VS_XCS_CLR                            GPIO_ResetBits(VS_GPIO_XCS_PORT,VS_GPIO_XCS_PIN)

#define VS_RST_SET                            GPIO_SetBits(VS_GPIO_RST_PORT,VS_GPIO_RST_PIN)
#define VS_RST_CLR                            GPIO_ResetBits(VS_GPIO_RST_PORT,VS_GPIO_RST_PIN)


__packed typedef struct 
{							  
	u8 mvol;		//������,��Χ:0~254
	u8 bflimit;		//����Ƶ���޶�,��Χ:2~15(��λ:10Hz)
	u8 bass;		//����,��Χ:0~15.0��ʾ�ر�.(��λ:1dB)
	u8 tflimit;		//����Ƶ���޶�,��Χ:1~15(��λ:Khz)
	u8 treble;		//����,��Χ:0~15(��λ:1.5dB)(ԭ����Χ��:-8~7,ͨ�������޸���);
	u8 effect;		//�ռ�Ч������.0,�ر�;1,��С;2,�е�;3,���.
	u8 speed; 	//�����ٶȣ�0��1:�������ţ�2:2���ٶȣ�3:3���ٶ�...  
}_vs1053_obj;

__packed typedef struct
{
	uint8_t input;                      //����ͨ��ѡ��.0��MICP��1��LINE1
	uint8_t samplerate;                 //��������ѡ��x*8K����1��8K��2��16K��3��24K...
	uint8_t channel;                    //������1��˫������2����������3��������
	uint8_t age;                        //���棺1~64
}_recoder_obj;


extern _vs1053_obj vsset;		//VS1053����

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1053�Ĵ�������
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

//RIFF��
typedef __packed struct
{
	uint32_t ChunkID;         //chunk id;����̶�Ϊ"RIFF",��0X46464952
	uint32_t ChunkSize;       //���ϴ�С;�ļ��ܴ�С��8
	uint32_t Format;          //��ʽ;WAVE,��0X45564157
}ChunkRIFF;

//FMT��
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;����̶�Ϊ"fmt ",��0X20746D66
	uint32_t SubChunkSize;    //�Ӽ��ϴ�С(������ID��Size);����Ϊ:20.
	uint16_t AudioFormat;     //��Ƶ��ʽ;0X10,��ʾ����PCM;0X11��ʾIMA ADPCM
	uint16_t NumOfChannels;   //ͨ������;1,��ʾ������;2,��ʾ˫����;
	uint32_t SampleRate;      //������;0X1F40,��ʾ8Khz
	uint32_t ByteRate;        //�ֽ�����;
	uint16_t BlockAlign;      //�����(�ֽ�); 
	uint16_t BitsPerSample;   //�����������ݴ�С;4λADPCM,����Ϊ4
	uint16_t ByteExtraData;   //���ӵ������ֽ�;2��; ����PCM,û���������
	uint16_t ExtraData;       //���ӵ�����,�����������ݿ��С;0X1F9:505�ֽ�  ����PCM,û���������
}ChunkFMT;

//FACT��
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;����̶�Ϊ"fact",��0X74636166;
	uint32_t SubChunkSize;    //�Ӽ��ϴ�С(������ID��Size);����Ϊ:4.
	uint32_t NumOfSamples;    //����������; 
}ChunkFACT;

//data��
typedef __packed struct
{
	uint32_t SubChunkID;      //chunk id;����̶�Ϊ"data",��0X61746164
	uint32_t SubChunkSize;    //�Ӽ��ϴ�С(������ID��Size);�ļ���С-60.
}ChunkDATA;

//WAVEͷ
typedef __packed struct
{
	ChunkRIFF riff;           //riff��
	ChunkFMT  fmt;            //fmt��
	ChunkFACT fact;           //fact�� ����PCM,û������ṹ��	 
	ChunkDATA data;          	//data��	
}__WaveHeader;




#endif /* __BSP_VS1053_H */

