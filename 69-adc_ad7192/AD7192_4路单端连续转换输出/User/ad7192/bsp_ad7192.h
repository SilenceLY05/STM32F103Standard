#ifndef __BSP_AD7192_H
#define __BSP_AD7192_H


#include "stm32f10x.h"

extern uint32_t AD7192Registers[8]; //AD7192Registers[8]������Ԫ��Ϊ�޷��ų����ͣ�ÿ��Ԫ��ռ4���ֽ�(��32λ)
extern uint32_t AD7192Data;         //AD7192DataΪ�޷��ų����ͣ�ռ4���ֽ�

/* ����SPI�ӿ� */
#define AD7192_SPIx                         SPI1
#define AD7192_SPI_APBxClockCmd             RCC_APB2PeriphClockCmd
#define AD7192_SPI_CLK                      RCC_APB2Periph_SPI1

/* CSƬѡ���� */
#define AD7192_SPI_GPIO_APBxClockCmd        RCC_APB2PeriphClockCmd
#define AD7192_SPI_CS_CLK                   RCC_APB2Periph_GPIOA
#define AD7192_SPI_CS_PORT                  GPIOA
#define AD7192_SPI_CS_PIN                   GPIO_Pin_4

/* SCK���� */
#define AD7192_SPI_SCK_CLK                  RCC_APB2Periph_GPIOA
#define AD7192_SPI_SCK_PORT                 GPIOA
#define AD7192_SPI_SCK_PIN                  GPIO_Pin_5

/* MISO�� */
#define AD7192_SPI_MISO_CLK                 RCC_APB2Periph_GPIOA
#define AD7192_SPI_MISO_PORT                GPIOA
#define AD7192_SPI_MISO_PIN                 GPIO_Pin_6

/* MOSI�� */
#define AD7192_SPI_MOSI_CLK                 RCC_APB2Periph_GPIOA
#define AD7192_SPI_MOSI_PORT                GPIOA
#define AD7192_SPI_MOSI_PIN                 GPIO_Pin_7

#define AD7192_SCLK_H                       GPIO_SetBits(AD7192_SPI_SCK_PORT,AD7192_SPI_SCK_PIN)
#define AD7192_SCLK_L                       GPIO_ResetBits(AD7192_SPI_SCK_PORT,AD7192_SPI_SCK_PIN)

#define AD7192_CS_ENABLE                    GPIO_SetBits(AD7192_SPI_CS_PORT,AD7192_SPI_CS_PIN)
#define AD7192_CS_DISABLE                   GPIO_ResetBits(AD7192_SPI_CS_PORT,AD7192_SPI_CS_PIN)

#define AD7192_RDY_STATUS                   GPIO_ReadInputDataBit(AD7192_SPI_MISO_PORT,AD7192_SPI_MISO_PIN)



/********************************** ͨ�żĴ��� **************************************/

#define WRITE_EN             0x00           //дʹ��   CR7д��ʹ��λ Ϊ0����д��Ϊ1����д��ֱ����λд��0
#define WRITE_DIS            0x80           //д����
#define RW_W                 0x00           //д����   CR6λΪ0��һ��������д������Ϊ1��һ��������ָ���Ĵ���ִ�ж�����
#define RW_R                 0x40           //������

//��AD7192�ڲ��Ĵ�����ַ���к궨��   λ��ͨ�żĴ���CR5-CR3
#define REG_COM_STA          0x00           //ѡ��״̬�Ĵ������в�����д�����ڼ�Ϊͨ�żĴ����Ĵ��� �������ڼ�Ϊ״̬�Ĵ�����С8λ
#define REG_MODE             0x01           //ѡ��ģʽ�Ĵ������в������Ĵ�����С24λ
#define REG_CONF             0x02           //ѡ�����üĴ������в������Ĵ�����С24λ
#define REG_DATA             0x03           //ѡ�����ݼĴ���/���ݼĴ�����״̬��Ϣ���в������Ĵ�����СΪ24/32λ 
#define REG_ID               0x04           //ѡ��ID�Ĵ������в������Ĵ�����С8λ
#define REG_GPOCON           0x05           //ѡ��ͨ����������Ĵ������в������Ĵ�����С8λ
#define REG_OFFSET           0x06           //ѡ��ʧ���Ĵ������в������Ĵ�����С24λ
#define REG_FS               0x07           //ѡ�������̼Ĵ������в������Ĵ�����С24λ

//CR2λ��λΪ1ʱ���нӿڿ���������ȡ���ݼĴ��� Ϊ0ʱ����������ȡģʽ
#define CREAD_EN             0x04           //������ȡʹ��
#define CREAD_DIS            0x00           //������ȡ��ֹ


/********************************** ״̬�Ĵ��� **************************************/
#define RDY_H                0x80           //ADC����λ������д�����0
#define RDY_L                0x00           //ADC����λSR7λ��0
#define ERR_H                0x40           //ADC����λ����RDYλͬʱд�� Ϊ1��ʾ���ݼĴ����������
#define ERR_L                0x00           //ADC����λSR6��0
#define NOREF_H              0x20           //���ⲿ��׼��ѹλ����1˵��ѡ���Ļ�׼��ѹ���ŵ��ڹ涨����ֵ
#define NOREF_L              0x00           //���ⲿ��׼��ѹλ��0��˵���Ի�׼��ѹ�����Լ�����Ч��׼��ѹ
#define PARITY_H             0x01           //���ݼĴ�������żУ��λ,�����ݼĴ�������������1���λ��1
#define PARITY_L             0x00           //�����ݼĴ�������ż����1���λ��0��ʹ�ô�λʱ��ģʽ�Ĵ����е�DAT_STA��1
                                            
//SR2��SR0λ ָʾ��һͨ����Ӧ���ݼĴ���������
#define CHDST_AIN1_AIN2      0x00
#define CHDST_AIN3_AIN4      0x01
#define CHDST_TEMP           0x02
#define CHDST_AIN2_AIN2      0x03
#define CHDST_AIN1_AINCOM    0x04
#define CHDST_AIN2_AINCOM    0x05
#define CHDST_AIN3_AINCOM    0x06
#define CHDST_AIN4_AINCOM    0x07



/********************************** ģʽ�Ĵ��� **************************************/
//ֻҪ��ģʽ�Ĵ���ִ��д�������Ὣ״̬�Ĵ�����RDYλ��1
//MR23��MR21λ ģʽѡ��λ
#define MODE_CONT            0x000000        //����ת��ģʽ(Ĭ��)��ת������ DOUT���ź�״̬�Ĵ���RDYλ���
#define MODE_SING            0x200000        //����ת��ģʽ��ת������������ݼĴ���RDY��ͣ����عض�ģʽ
#define MODE_IDLE            0x400000        //����ģʽ
#define MODE_PD              0x600000        //�ض�ģʽ
#define MODE_INZCL           0x800000        //�ڲ����ƽУ׼��У׼����ʱRDYΪ����ɱ�ͣ�У׼��ɺ��ڿ���ģʽ
#define MODE_INFCL           0xA00000        //�ڲ�������У׼��У׼����ʱRDYΪ����ɱ�ͣ�У׼��ɺ��ڿ���ģʽ�����ݱ����������̼Ĵ���,��AVDDС��4.75 CLK_DIV��1
#define MODE_SYSZCL          0xC00000        //ϵͳ���ƽУ׼��У׼����ʱRDYΪ����ɱ�ͣ�У׼��ɺ��ڿ���ģʽ�����ݱ����������̼Ĵ���
#define MODE_SYSFCL          0xE00000        //ϵͳ����ƽУ׼��У׼����ʱRDYΪ����ɱ�ͣ�У׼��ɺ��ڿ���ģʽ�����ݱ����������̼Ĵ���
#define DAT_STA_EN           0x100000        //ʹ��״̬����
#define DAT_STA_DIS          0x000000        //����״̬����
#define EXT_XTAL             0x000000        //�ⲿ����
#define EXT_CLK              0x040000        //�ⲿʱ��
#define INCLK_MCLK2TRI       0x080000        //�ڲ�ʱ�ӣ�MCLK2����Ϊ��̬
#define INCLK_MCLK2EN        0x0C0000        //�ڲ�ʱ�Ӵ�MCLK2���
#define SINC3                0x008000        //ʹ��SINC3�˲���
#define SINC4                0x000000        //ʹ��SINC4�˲���(Ĭ��ֵ)
#define ENPAR_EN             0x002000        //ʹ����żУ��λ��ʹ��ʱDAT_STAӦ��1
#define ENPAR_DIS            0x000000        //��ֹ��żУ��λ
#define CLK_DIV_2            0x001000        //��1ʱ����ʱ��2��Ƶ����AVDD����4.75��λ������1
#define CLK_DIV_DIS          0x000000        //��ʱ�Ӳ���Ƶ������ת��ʱ����λӦ��0
#define SINGLE_EN            0x000800        //������ת��ʹ��λ����1ʱ��AD7192��һ��ת�������ڽ���
#define SINGLE_DIS           0x000000        //���õ�����ת������ʹ�ܶ��ģ������ͨ�����򵥴�ת��ģʽ
#define	REJ60_EN				     0x000400        //ʹ��60Hz�ݲ�Ƶ��,ʵ��50Hz/60Hzͬʱ����
#define	REJ60_DIS				     0x000000        //ʧ��60Hz�ݲ�Ƶ��,ֻ����50Hz





/********************************** ���üĴ��� **************************************/
#define CHOP_EN              0x800000        //ն��ʹ��λ
#define CHOP_DIS             0x000000        //ն������
#define REFSEL_IN1           0x000000        //��׼��ѹѡ��λ��REFIN1֮��ʩ���ⲿ��׼��ѹ
#define REFSEL_IN2           0x100000        //��׼��ѹѡ��λ��REFIN2֮��ʩ���ⲿ��׼��ѹ
#define AIN1_AIN2    			   0x000100        //ʹ��ͨ��(AIN+)AIN1 (AIN-)AIN2
#define AIN3_AIN4   			   0x000200        //ʹ��ͨ��(AIN+)AIN3 (AIN-)AIN4
#define TEMP       			     0x000400        //ʹ��ͨ��  �¶ȴ�����
#define AIN2_AIN2    			   0x000800        //ʹ��ͨ��(AIN+)AIN2 (AIN-)AIN2
#define AIN1_AINCOM 		 	   0x001000        //ʹ��ͨ��(AIN+)AIN1 (AIN-)AINCOM 
#define AIN2_AINCOM  			   0x002000        //ʹ��ͨ��(AIN+)AIN2 (AIN-)AINCOM  
#define AIN3_AINCOM  			   0x004000        //ʹ��ͨ��(AIN+)AIN3 (AIN-)AINCOM 
#define AIN4_AINCOM  				 0x008000        //ʹ��ͨ��(AIN+)AIN4 (AIN-)AINCOM
#define BURN_EN              0x000080        //ʹ�ܼ�������,��������������Ч״̬��ն������
#define BURN_DIS             0x000000        //���ü�������
#define REFDET_EN            0x000040        //ʹ�ܻ�׼��ѹ��⹦��
#define REFDET_DIS           0x000000        //���û�׼��ѹ��⹦��
#define BUF_EN               0x000010        //ʹ��ģ������˵Ļ�����
#define BUF_DIS              0x000000        //ģ�����벻���л���
#define UB_UNI               0x000008        //����ѡ��λ�������Թ���ģʽ
#define UB_BI                0x000000        //����ѡ��λ��˫���Թ���ģʽ
#define	GAIN_1				    	 0x000000        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��5V
#define	GAIN_8               0x000003        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��625mV
#define	GAIN_16              0x000004        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��312.5mV
#define	GAIN_32              0x000005        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��156.2mV
#define	GAIN_64              0x000006        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��78.125mV
#define	GAIN_128             0x000007        //����ѡ��λ��ADC��׼��ѹ���뷶Χ��39.06mV



/********************************** ���üĴ��� **************************************/
#define BPDSW_EN             0x40            //���ŹضϿ���λ����1�պϵ��ŹضϿ���BPDSW
#define BPDSW_DIS            0x00            //���ŹضϿ���λ����0�պϵ��ŹضϿ���BPDSW
#define GP32EN_EN            0x20            //�������P3���������P2ʹ�ܣ���1ʱ���������P3��P2Ϊ��Ч״̬
#define GP32EN_DIS           0x00            //�������P3���������P2ʹ�ܣ���0ʱ��P3��P2����Ϊ��̬
#define GP10EN_EN            0x10            //�������P1���������P0ʹ�ܣ���1ʱ���������P1��P0Ϊ��Ч״̬
#define GP10EN_DIS           0x00            //�������P1���������P0ʹ�ܣ���0ʱ��P1��P0����Ϊ��̬
#define P3DAT_EN             0x08            //�������P3λ��GP32EN��1ʱ��P3DAT����P3����ֵ
#define P3DAT_DIS            0x00            //�������P3λ��GP32EN��1ʱ��P3DAT����P3����ֵ
#define P2DAT_EN             0x04            //�������P2λ��GP32EN��1ʱ��P2DAT����P2����ֵ
#define P2DAT_DIS            0x00            //�������P2λ��GP32EN��1ʱ��P2DAT����P2����ֵ
#define P1DAT_EN             0x02            //�������P1λ��GP10EN��1ʱ��P1DAT����P1����ֵ
#define P1DAT_DIS            0x00            //�������P1λ��GP10EN��1ʱ��P1DAT����P1����ֵ
#define P0DAT_EN             0x01            //�������P0λ��GP10EN��1ʱ��P0DAT����P0����ֵ
#define P0DAT_DIS            0x00            //�������P0λ��GP10EN��1ʱ��P0DAT����P0����ֵ

#define NOP                  0x00            //�ղ���



void AD7192_Config(void);
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte,uint32_t SPITimeOut);
void AD7192ReadData(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte);
unsigned char ReadFromAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToRead,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	);
unsigned char WriteToAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToWrite,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	);
void AD7192_Mode_Conf_Registers(uint32_t mode,uint32_t cfg);
void AD7192_SoftWareReset(void);
void AD7192InternalZeroScaleCalibration(void);
void AD7192InternalFullScaleCalibration(void);
void AD7192ExternalZeroScaleCalibration(void);
void AD7192ExternalFullScaleCalibration(void);
void AD7192StartContinuousRead(void);
uint32_t AD7192ReadConvertData(void);
uint32_t AD7192ContinuousRead(void);
void AD7192ExitContinousRead(void);
uint32_t AD7192ReadTemperature(void);
float RealTemperature(uint32_t TemperatureData);
void AD7192StartSingleConvertion(uint32_t Channel);
void AD7192StartContinuousConvertion(uint32_t Channels);



#endif /* __BSP_AD7192_H */

