#include "bsp_ad5689.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"



static void delay(void);
static uint16_t SPI_TIMEOUT_UserCallBack(uint8_t errorCode);

void AD5689_SetSPIDirection(uint16_t direction)
{
	SPI_InitTypeDef                 SPI_InitStructure;
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = direction;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(AD5689_SPIx,&SPI_InitStructure);
}


/* 初始化 AD5689GPIO引脚和SPI */
void AD5689_Config(void)
{
	GPIO_InitTypeDef                GPIO_InitStructure;
	
	
	AD5689_SPI_APBxClockCmd(AD5689_SPI_CLK,ENABLE);
	
	AD5689_GPIO_APBxClockCmd(AD5689_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = AD5689_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(AD5689_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD5689_GPIO_MISO_PIN;
	GPIO_Init(AD5689_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD5689_GPIO_MOSI_PIN;
	GPIO_Init(AD5689_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD5689_GPIO_SYNC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(AD5689_GPIO_SYNC_PORT,&GPIO_InitStructure);
	
	AD5689_SetSPIDirection(SPI_Direction_1Line_Tx);
	
	SPI_Cmd(AD5689_SPIx,ENABLE);
	
	
}

/* 延时函数 */
static void delay(void)
{
	uint16_t i;
	for(i=0;i<50;i++)
	{
		__NOP();
	}
}



/* 等待超时回调函数 */
static uint16_t SPI_TIMEOUT_UserCallBack(uint8_t errorCode)
{
	printf("SPI等待超时！errorCode = %d",errorCode);
	return 0 ;
}

/* SPI接收发送数据 */
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char NumberOfByte,uint32_t SPITimeOut)
{
	uint32_t timeout = SPITimeOut;
	unsigned char i;
	
	for(i = 0 ;i< NumberOfByte;i++)
	{
		/* 等待发送缓冲区为空，TXE时间 */
		while(SPI_I2S_GetFlagStatus(AD5689_SPIx,SPI_I2S_FLAG_TXE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBack(0);
				return;
			}
		}
		
		/* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
		SPI_I2S_SendData(AD5689_SPIx,WriteBuff[i]);
		
		SPITimeOut = timeout;
		
		/* 等待接收缓冲区非空 RXNE事件 */
		while(SPI_I2S_GetFlagStatus(AD5689_SPIx,SPI_I2S_FLAG_RXNE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBack(1);
				return;
			}
		}
		
	}
}



/* 设置AD5689寄存器值 command指令 channel通道 data数据 */
void AD5689_SetRegisterValue(uint8_t command,uint16_t channel,uint16_t data)
{
	uint8_t reg[3] = {0};
	
	reg[0] = (command << 4) | channel;
	reg[1] = (data >> 8) & 0xFF;
	reg[2] = data & 0xFF;
	
	//SYNC引脚为低电平启动
	AD5689_SYNC_L;
	delay();
	SPI_TransmitReceive(reg,3,0xF);
	delay();
	AD5689_SYNC_H;
}

/* LDAC屏蔽寄存器设置 LDAC功能选择 */
void AD5689_LDAC_MASK(uint16_t ldac1,uint16_t ldac2)
{
	uint8_t data = ldac1;
	data |= (ldac2<<3);
	AD5689_SetRegisterValue(CMD_LDAC_MASK,DAC_A_B,data);
}

/* 软件复位 */
void AD5689_SoftReset(void)
{
	AD5689_SetRegisterValue(CMD_RESET,DAC_A_B,0);
}

/* 掉电设置 channel1通道1的掉电模式 channel2通道2的掉电模式 */
void AD5689_PowerDown(uint16_t channel1,uint16_t channel2)
{
	uint8_t data = 0x3C;
	data |= (channel2<<6)| channel1;
	AD5689_SetRegisterValue(CMD_POWER_DOWN,DAC_A_B,data);
}

/* 写入和更新DAC通道n(与LDAC无关) channel通道 data 数值 */
void AD5689_WriteUpdata_DACREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITE_UPDATAn,channel,data);
}

/* 写入输入寄存器n(取决于LDAC) channel通道 data 数值 */
void AD5689_Write_InputREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITEn,channel,data);
}

/* 以输入寄存器n的内容更新DAC寄存器n channel通道 data 数值 */
void AD5689_InputREG_toUpdata_DACREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITEn_UPDATAn,channel,data);
}

/* LDAC屏蔽寄存器设置 菊花链操作 */
void AD5689_DaisyChan_Operation(uint16_t decn)
{
	AD5689_SetRegisterValue(CMD_DCEN_REG,DAC_A_B,decn);
}


/* AD5689初始化 */
void AD5689_Init(void)
{
	/* 用户可以选择使用RESET引脚清空代码值，保持一段时间低电平变回高以后会清零，执行复位功能可以将DAC复位至上带你复位代码 */
	AD5689_Config();
	AD5689_RESET_L;
	SysTick_Delay_Ms(5);
	AD5689_RESET_H;
	SysTick_Delay_Ms(5);
	AD5689_SoftReset();
	AD5689_LDAC_MASK(LDAC_DIS,LDAC_DIS);
}


