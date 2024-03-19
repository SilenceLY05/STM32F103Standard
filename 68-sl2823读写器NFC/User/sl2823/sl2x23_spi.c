#include "sl2x23_spi.h"
#include "sl2x23.h"
#include "bsp_SysTick.h"

static volatile uint8_t Irq1_Delay_Cnt = 0;
static volatile uint8_t Irq2_Delay_Cnt = 0;

static volatile uint8_t DEV_Wkup_Pwm_Level_H1;
static volatile uint8_t DEV_Wkup_Pwm_Level_Hh;
static volatile uint16_t DEV_Wkup_Pwm_cnt;


//软件SPI时序延时
#define SL2x23_DELAY()



/* 硬件SPI读写一个字节 */
uint8_t SPI_ReadWriteByte(uint8_t byte)
{
#ifdef HARDWARE_SPI
	/* 等待发送数据缓冲区为空，TXE时间 */
	while(SPI_I2S_GetFlagStatus(SL2x23_SPI,SPI_I2S_FLAG_TXE) == RESET);
	
	/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
	SPI_I2S_SendData(SL2x23_SPI,byte);
	
	/* 等待接收缓冲区非空，RXNE事件 */
	while (SPI_I2S_GetFlagStatus(SL2x23_SPI,SPI_I2S_FLAG_RXNE) == RESET);
	/* 读取数据寄存器，获取接收缓冲区数据 */
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
		
		SL2x23_SCK_1();      //拉高时钟
		if(SL2x23_MISO_GET() == 1)
			byte |= 0x01;
		SL2x23_DELAY();
		SL2x23_SCK_0();
	}
	return byte;
	
#endif
}



/* SPI读寄存器函数 */
uint8_t DEV_ReadSR(uint8_t sr)
{
	uint8_t byte = 0;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(sr);        //发送读取状态寄存器命令
	byte = SPI_ReadWriteByte(0xFF);  //读取一个字节
	
	SL2x23_SPI_CS_HIGH;                 //取消片选
	
	return byte;
}



/* SPI 写多个字节到设备地址 */
void DEV_SPI_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint32_t NumByteToRead)
{
	uint16_t i;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(WriteAddr);       //最高位：1表示写 0表示读
	
	for(i=0;i<NumByteToRead;i++)
	{
		SPI_ReadWriteByte(pBuffer[i]);    //循环写数
	}
	SL2x23_SPI_CS_HIGH;
}


/* SPI 读多个字节到设备地址 */
void DEV_SPI_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint32_t NumByteToRead)
{
	uint16_t i;
	SL2x23_SPI_CS_LOW;
	
	SPI_ReadWriteByte(ReadAddr);       //最高位：1表示写 0表示读
	
	for(i=0;i<NumByteToRead;i++)
	{
		pBuffer[i] = SPI_ReadWriteByte(0xFF);    //循环写数
	}
	SL2x23_SPI_CS_HIGH;
}




/* SPI初始化 */
void SL2x23_SPI_Config(void)
{
#ifdef HARDWARE_SPI
	SPI_InitTypeDef     SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能SPI时钟 */
	RCC_APB1PeriphClockCmd (SL2x23_SPI_CLK,ENABLE);
	
	/* 使能SPI引脚相关的时钟 */
	RCC_APB2PeriphClockCmd (SL2x23_GPIO_CS_CLK | SL2x23_GPIO_SCK_CLK |
                        SL2x23_GPIO_MOSI_CLK | SL2x23_GPIO_MISO_CLK |
                        SL2x23_GPIO_RST_CLK | SL2x23_GPIO_IRQ_CLK,ENABLE);
	

	
	/* 配置SPI的SCK引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MISO引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MISO_PIN;
	GPIO_Init(SL2x23_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MOSI_PIN;
	GPIO_Init(SL2x23_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	
	/* SPI模式配置 */
	//FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI使用模式3   SCK空闲时为高电平 采样时刻为偶数边沿
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(FLASH_SPIx, &SPI_InitStructure);
	
	/* 使能SPI */
	SPI_Cmd(FLASH_SPIx,ENABLE);
	
	/* 软件模拟SPI */
#else

	GPIO_InitTypeDef    GPIO_InitStructure;
	/* 使能SPI引脚相关的时钟 */
	RCC_APB2PeriphClockCmd (SL2x23_GPIO_CS_CLK | SL2x23_GPIO_SCK_CLK |
                        SL2x23_GPIO_MOSI_CLK | SL2x23_GPIO_MISO_CLK |
                        SL2x23_GPIO_RST_CLK | SL2x23_GPIO_IRQ_CLK,ENABLE);
												
	/* 配置SPI的SCK引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_SCK_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MISO引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_MISO_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	/* 配置SPI的MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_MOSI_PIN;
		GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_MOSI_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_MOSI_PORT,&GPIO_InitStructure);

#endif
	
	
	
	/* 配置SPI的CS引脚，普通IO即可 */
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_CS_Mode;
	GPIO_Init(SL2x23_GPIO_CS_PORT, &GPIO_InitStructure);
	
}



