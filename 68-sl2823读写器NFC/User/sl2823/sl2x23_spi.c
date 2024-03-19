#include "sl2x23_spi.h"
#include "sl2x23.h"
#include "bsp_SysTick.h"

static volatile uint8_t Irq1_Delay_Cnt = 0;
static volatile uint8_t Irq2_Delay_Cnt = 0;

static volatile uint8_t DEV_Wkup_Pwm_Level_H1;
static volatile uint8_t DEV_Wkup_Pwm_Level_Hh;
static volatile uint16_t DEV_Wkup_Pwm_cnt;


//���SPIʱ����ʱ
#define SL2x23_DELAY()



/* Ӳ��SPI��дһ���ֽ� */
uint8_t SPI_ReadWriteByte(uint8_t byte)
{
#ifdef HARDWARE_SPI
	/* �ȴ��������ݻ�����Ϊ�գ�TXEʱ�� */
	while(SPI_I2S_GetFlagStatus(SL2x23_SPI,SPI_I2S_FLAG_TXE) == RESET);
	
	/* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
	SPI_I2S_SendData(SL2x23_SPI,byte);
	
	/* �ȴ����ջ������ǿգ�RXNE�¼� */
	while (SPI_I2S_GetFlagStatus(SL2x23_SPI,SPI_I2S_FLAG_RXNE) == RESET);
	/* ��ȡ���ݼĴ�������ȡ���ջ��������� */
	return SPI_I2S_ReceiveData(SL2x23_SPI);
	
#else
	uint8_t i;
	for(i = 0 ;i<8 ;i++)
	{
		if(byte & 0x80)
			SL2x23_MOSI_1();
		else
			SL2x23_MOSI_0();
		
		byte <<=1;
		SL2x23_DELAY();
		
		SL2x23_SCK_1();      //����ʱ��
		if(SL2x23_MISO_GET() == 1)
			byte |= 0x01;
		SL2x23_DELAY();
		SL2x23_SCK_0();
	}
	return byte;
	
#endif
}



/* SPI���Ĵ������� */
uint8_t DEV_ReadSR(uint8_t sr)
{
	uint8_t byte = 0;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(sr);        //���Ͷ�ȡ״̬�Ĵ�������
	byte = SPI_ReadWriteByte(0xFF);  //��ȡһ���ֽ�
	
	SL2x23_SPI_CS_HIGH;                 //ȡ��Ƭѡ
	
	return byte;
}



/* SPI д����ֽڵ��豸��ַ */
void DEV_SPI_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint32_t NumByteToRead)
{
	uint16_t i;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(WriteAddr);       //���λ��1��ʾд 0��ʾ��
	
	for(i=0;i<NumByteToRead;i++)
	{
		SPI_ReadWriteByte(pBuffer[i]);    //ѭ��д��
	}
	SL2x23_SPI_CS_HIGH;
}


/* SPI ������ֽڵ��豸��ַ */
void DEV_SPI_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint32_t NumByteToRead)
{
	uint16_t i;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(ReadAddr);       //���λ��1��ʾд 0��ʾ��
	
	for(i=0;i<NumByteToRead;i++)
	{
		pBuffer[i] = SPI_ReadWriteByte(0xFF);    //ѭ��д��
	}
	SL2x23_SPI_CS_HIGH;
}




/* SPI��ʼ�� */
void SL2x23_SPI_Config(void)
{
#ifdef HARDWARE_SPI
	SPI_InitTypeDef     SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ʹ��SPIʱ�� */
	RCC_APB1PeriphClockCmd (SL2x23_SPI_CLK,ENABLE);
	
	/* ʹ��SPI������ص�ʱ�� */
	RCC_APB2PeriphClockCmd (SL2x23_GPIO_CS_CLK | SL2x23_GPIO_SCK_CLK |
                        SL2x23_GPIO_MOSI_CLK | SL2x23_GPIO_MISO_CLK |
                        SL2x23_GPIO_RST_CLK | SL2x23_GPIO_IRQ_CLK,ENABLE);
	

	
	/* ����SPI��SCK���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MISO���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MISO_PIN;
	GPIO_Init(SL2x23_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MOSI���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MOSI_PIN;
	GPIO_Init(SL2x23_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	
	/* SPIģʽ���� */
	//FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPIʹ��ģʽ3   SCK����ʱΪ�ߵ�ƽ ����ʱ��Ϊż������
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(FLASH_SPIx, &SPI_InitStructure);
	
	/* ʹ��SPI */
	SPI_Cmd(FLASH_SPIx,ENABLE);
	
	/* ���ģ��SPI */
#else

	GPIO_InitTypeDef    GPIO_InitStructure;
	/* ʹ��SPI������ص�ʱ�� */
	RCC_APB2PeriphClockCmd (SL2x23_GPIO_CS_CLK | SL2x23_GPIO_SCK_CLK |
                        SL2x23_GPIO_MOSI_CLK | SL2x23_GPIO_MISO_CLK |
                        SL2x23_GPIO_RST_CLK | SL2x23_GPIO_IRQ_CLK,ENABLE);
												
	/* ����SPI��SCK���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_SCK_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MISO���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_MISO_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	/* ����SPI��MOSI���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MOSI_PIN;
		GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_MOSI_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_MOSI_PORT,&GPIO_InitStructure);

#endif
	
	
	
	/* ����SPI��CS���ţ���ͨIO���� */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_CS_Mode;
	GPIO_Init(SL2x23_GPIO_CS_PORT, &GPIO_InitStructure);
	
}



