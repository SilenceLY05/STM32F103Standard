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


/* ��ʼ�� AD5689GPIO���ź�SPI */
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

/* ��ʱ���� */
static void delay(void)
{
	uint16_t i;
	for(i=0;i<50;i++)
	{
		__NOP();
	}
}



/* �ȴ���ʱ�ص����� */
static uint16_t SPI_TIMEOUT_UserCallBack(uint8_t errorCode)
{
	printf("SPI�ȴ���ʱ��errorCode = %d",errorCode);
	return 0 ;
}

/* SPI���շ������� */
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char NumberOfByte,uint32_t SPITimeOut)
{
	uint32_t timeout = SPITimeOut;
	unsigned char i;
	
	for(i = 0 ;i< NumberOfByte;i++)
	{
		/* �ȴ����ͻ�����Ϊ�գ�TXEʱ�� */
		while(SPI_I2S_GetFlagStatus(AD5689_SPIx,SPI_I2S_FLAG_TXE) == RESET)
		{
			if((timeout--) == 0)
			{
				SPI_TIMEOUT_UserCallBack(0);
				return;
			}
		}
		
		/* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
		SPI_I2S_SendData(AD5689_SPIx,WriteBuff[i]);
		
		SPITimeOut = timeout;
		
		/* �ȴ����ջ������ǿ� RXNE�¼� */
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



/* ����AD5689�Ĵ���ֵ commandָ�� channelͨ�� data���� */
void AD5689_SetRegisterValue(uint8_t command,uint16_t channel,uint16_t data)
{
	uint8_t reg[3] = {0};
	
	reg[0] = (command << 4) | channel;
	reg[1] = (data >> 8) & 0xFF;
	reg[2] = data & 0xFF;
	
	//SYNC����Ϊ�͵�ƽ����
	AD5689_SYNC_L;
	delay();
	SPI_TransmitReceive(reg,3,0xF);
	delay();
	AD5689_SYNC_H;
}

/* LDAC���μĴ������� LDAC����ѡ�� */
void AD5689_LDAC_MASK(uint16_t ldac1,uint16_t ldac2)
{
	uint8_t data = ldac1;
	data |= (ldac2<<3);
	AD5689_SetRegisterValue(CMD_LDAC_MASK,DAC_A_B,data);
}

/* �����λ */
void AD5689_SoftReset(void)
{
	AD5689_SetRegisterValue(CMD_RESET,DAC_A_B,0);
}

/* �������� channel1ͨ��1�ĵ���ģʽ channel2ͨ��2�ĵ���ģʽ */
void AD5689_PowerDown(uint16_t channel1,uint16_t channel2)
{
	uint8_t data = 0x3C;
	data |= (channel2<<6)| channel1;
	AD5689_SetRegisterValue(CMD_POWER_DOWN,DAC_A_B,data);
}

/* д��͸���DACͨ��n(��LDAC�޹�) channelͨ�� data ��ֵ */
void AD5689_WriteUpdata_DACREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITE_UPDATAn,channel,data);
}

/* д������Ĵ���n(ȡ����LDAC) channelͨ�� data ��ֵ */
void AD5689_Write_InputREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITEn,channel,data);
}

/* ������Ĵ���n�����ݸ���DAC�Ĵ���n channelͨ�� data ��ֵ */
void AD5689_InputREG_toUpdata_DACREG(uint16_t channel,uint16_t data)
{
	AD5689_SetRegisterValue(CMD_WRITEn_UPDATAn,channel,data);
}

/* LDAC���μĴ������� �ջ������� */
void AD5689_DaisyChan_Operation(uint16_t decn)
{
	AD5689_SetRegisterValue(CMD_DCEN_REG,DAC_A_B,decn);
}


/* AD5689��ʼ�� */
void AD5689_Init(void)
{
	/* �û�����ѡ��ʹ��RESET������մ���ֵ������һ��ʱ��͵�ƽ��ظ��Ժ�����㣬ִ�и�λ���ܿ��Խ�DAC��λ���ϴ��㸴λ���� */
	AD5689_Config();
	AD5689_RESET_L;
	SysTick_Delay_Ms(5);
	AD5689_RESET_H;
	SysTick_Delay_Ms(5);
	AD5689_SoftReset();
	AD5689_LDAC_MASK(LDAC_DIS,LDAC_DIS);
}


