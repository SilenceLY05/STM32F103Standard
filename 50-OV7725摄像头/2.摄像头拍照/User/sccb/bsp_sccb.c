#include "bsp_sccb.h"

#define DEV_ADR    ADDR_OV7725 


/*  SCCB_Configuration   SCCB管脚配置  */
void SCCB_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	OV7725_SIO_C_SCK_ABPxClock_FUN(OV7725_SIO_C_GPIO_CLK,ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = OV7725_SIO_C_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(OV7725_SIO_C_GPIO_PORT, &GPIO_InitStructure);
	
	OV7725_SIO_D_SCK_ABPxClock_FUN(OV7725_SIO_D_GPIO_CLK,ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = OV7725_SIO_D_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(OV7725_SIO_D_GPIO_PORT, &GPIO_InitStructure);

}


/* SCCB_Delay 延迟时间 */
static void SCCB_Delay(void)
{
	uint16_t i=400;

	while(i)
	{
		i--;
	}
}

/*  SCCB_Start SCCB起始信号 */
static int SCCB_Start(void)
{
	SDA_H;
	SCL_H;
	SCCB_Delay();
	if(!SDA_read)
		return DISABLE; /* SDA线为低电平则总线忙，退出 */
	SDA_L;
	SCCB_Delay();
	if(SDA_read)
		return DISABLE;
	SDA_L;
	SCCB_Delay();
	return ENABLE;
}

/* SCCB_Stop SCCB停止信号 */
static void SCCB_Stop(void)
{
	SCL_L;
	SCCB_Delay();
	SDA_L;
	SCCB_Delay();
	SCL_H;
	SCCB_Delay();
	SDA_H;
	SCCB_Delay();
}

/* SCCB_Ack SCCB应答方式*/
static void SCCB_Ack(void)
{
	SCL_L;
	SCCB_Delay();
	SDA_L;
	SCCB_Delay();
	SCL_H;
	SCCB_Delay();
	SCL_L;
	SCCB_Delay();
}

/* SCCB_NAck SCCB无应答方式 */
static void SCCB_NAck(void)
{
	SCL_L;
	SCCB_Delay();
	SDA_H;
	SCCB_Delay();
	SCL_H;
	SCCB_Delay();
	SCL_L;
	SCCB_Delay();	
}

/* SCCB_WaitAck SCCB等待应答 返回为：=1有ACK =0无ACK*/
static int SCCB_WaitAck(void)
{
	SCL_L;
	SCCB_Delay();
	SDA_H;
	SCCB_Delay();
	SCL_H;
	SCCB_Delay();
	if(SDA_read)
	{
		SCL_L;
		return DISABLE;
	}
	SCL_L;
	return ENABLE;
}



/* SCCB_SendByte 数据从高位到低位 SendByte:发送的数据*/
static void SCCB_SendByte(uint8_t SendByte)
{
	uint8_t i;

	while(i--)
	{
		SCL_L;
		SCCB_Delay();
		if(SendByte&0x80)
			SDA_H;
		else
			SDA_L;
		SendByte<<=1;
		SCCB_Delay();
		SCL_H;
		SCCB_Delay();
	}
	SCL_L;
}

/*  SCCB_ReceiveByte 数据从高位到低位 SCCB总线返回的数据*/
static int SCCB_ReceiveByte(void)
{
	uint8_t i;
	uint8_t ReceiveByte=0;

	SDA_H;
	while(i--)
	{
		ReceiveByte<<=1;
		SCL_L;
		SCCB_Delay();
		SCL_H;
		SCCB_Delay();
		if(SDA_read)
		{
			ReceiveByte |= 0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
}


/* SCCB_WriteByte 写入一字节数据 WriteAddress 待写入地址 SendByte待写入数据 DeviceAddress器件类型 */
int SCCB_WriteByte(uint16_t WriteAddress,uint8_t SendByte)
{
	if(!SCCB_Start())
	{
		return DISABLE;
	}
	SCCB_SendByte(DEV_ADR);
	if(!SCCB_WaitAck())
	{
		SCCB_Stop();
		return DISABLE;
	}
	SCCB_SendByte((uint8_t)(WriteAddress & 0x00FF));    /* 设置低起始地址 */
	SCCB_WaitAck();
	SCCB_SendByte(SendByte);
	SCCB_WaitAck();
	SCCB_Stop();
	return ENABLE;
}

/* SCCB_ReadByte 读取一串数据 length 待读出长度 ReadAddress 待读出地址  DeviceAddress器件类型 */
int SCCB_ReadByte(uint8_t* pBuffer,uint16_t length,uint8_t ReadAddress)
{
	if(!SCCB_Start())
	{
		return DISABLE;
	}
	SCCB_SendByte(DEV_ADR);
	if(!SCCB_WaitAck())
	{
		SCCB_Stop();
		return DISABLE;
	}
	SCCB_SendByte(ReadAddress);
	SCCB_WaitAck();
	SCCB_Stop();
	if(!SCCB_Start())
	{
		return DISABLE;
	}
	SCCB_SendByte(DEV_ADR+1);
	if(!SCCB_WaitAck())
	{
		SCCB_Stop();
		return DISABLE;
	}
	while(length)
	{
		*pBuffer = SCCB_ReceiveByte();
		if(length == 1)
		{
			SCCB_NAck();
		}
		else
		{
			SCCB_Ack();
		}
		pBuffer++;
		length++;
	}
	SCCB_Stop();
	return ENABLE;
}

