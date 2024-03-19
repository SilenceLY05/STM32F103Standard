#include "sl2x23_iic.h"

volatile uint32_t SL2x23_Timeout;



/* 软件I2C函数 */
#ifndef HARDWARE_I2C
#define IIC_CODE 0


/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 10; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
#if IIC_CODE == 0
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	SL2x23_I2C_SDA_1();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_1();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_0();  //钳住I2C总线，准备发送或接收数据 

#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
#if IIC_CODE ==0
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	SL2x23_I2C_SDA_0();
	SL2x23_I2C_SCL_1();
	i2c_Delay();
	SL2x23_I2C_SDA_1();
	
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SCL_0();
	SL2x23_I2C_SDA_0();
	i2c_Delay();
	SL2x23_I2C_SCL_1();
	SL2x23_I2C_SDA_1();
	i2c_Delay();
#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

#if IIC_CODE == 0
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			SL2x23_I2C_SDA_1();
		}
		else
		{
			SL2x23_I2C_SDA_0();
		}
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		i2c_Delay();	
		SL2x23_I2C_SCL_0();
		if (i == 7)
		{
			 SL2x23_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
#else
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SCL_0();    //拉低时钟开始数据传输
	for (i = 0; i < 8; i++)
	{		
		if ((_ucByte & 0x80)>>7)
		{
			SL2x23_I2C_SDA_1();
		}
		else
		{
			SL2x23_I2C_SDA_0();
		}
		_ucByte<<=1;
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		i2c_Delay();	
		SL2x23_I2C_SCL_0();
		i2c_Delay();
	}
#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

#if IIC_CODE == 0
	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SL2x23_I2C_SCL_1();
		i2c_Delay();
		if (SL2x23_I2C_SDA_READ())
		{
			value++;
		}
		SL2x23_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
	
#else
	SL2x23_I2C_SDA_IN();
	for (i = 0; i < 8; i++)
	{
		SL2x23_I2C_SCL_1();
		i2c_Delay();
		SL2x23_I2C_SCL_1();
		value<<=1;
		if (SL2x23_I2C_SDA_READ())
		{
			value++;
		}
		i2c_Delay();
	}
	return value;
#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	
#if IIC_CODE ==0
	
	SL2x23_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	SL2x23_Timeout = SL2x23_LONG_TIMEOUT;
	while(SL2x23_I2C_SDA_READ())
	{
		if(--SL2x23_Timeout == 0)
		{
			i2c_Stop();
			return 1;
		}
	}
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	return 0;
	
#else
	uint8_t ucErrTime = 0;
	SL2x23_I2C_SDA_IN();
	SL2x23_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();

	while(SL2x23_I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			i2c_Stop();
			return 1;
		}
	}
	SL2x23_I2C_SCL_0();
	return 0;
	
	
#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
#if IIC_CODE == 0
	SL2x23_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();
	SL2x23_I2C_SDA_1();	/* CPU释放SDA总线 */
	
#else
	SL2x23_I2C_SCL_0();	/* CPU驱动SDA = 0 */
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	SL2x23_I2C_SCL_0();

#endif
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
#if IIC_CODE == 0
	SL2x23_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	SL2x23_I2C_SCL_0();
	i2c_Delay();	
	
#else
	SL2x23_I2C_SCL_0();
	SL2x23_I2C_SDA_OUT();
	SL2x23_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	SL2x23_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	SL2x23_I2C_SCL_0();

#endif
}



/* 初始化I2C相关的GPIO */
void SL2x23_I2C_Init(void)
{
//硬件I2C
#define HARDWART_I2C
	GPIO_InitTypeDef    GPIO_InitStructure;
	I2C_InitTypeDef     I2C_InitStructure;
	
	/* 使能与I2C有关的时钟 */
	RCC_APB1PeriphClockCmd(SL2x23_I2C_CLK,ENABLE);
	RCC_APB2PeriphClockCmd ( SL2x23_I2C_SCL_GPIO_CLK |SL2x23_I2C_SDA_GPIO_CLK , ENABLE );
	
	
	/* 配置I2C SDA 和SCL引脚 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_I2C_SDA_GPIO_PORT,&GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 700000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0;
	
	I2C_Init(SL2x23_I2C,&I2C_InitStructure);
	I2C_Cmd(SL2x23_I2C,ENABLE);
	
//软件模拟I2C
#else
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(SL2x23_I2C_SCL_GPIO_CLK | SL2x23_I2C_SDA_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SL2x23_I2C_SCL_PIN | SL2x23_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
#if IIC_CODE == 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   /* 开漏输出 */
#else
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   /* 推挽输出 */
	
#endif 
	GPIO_Init(SL2x23_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
#endif
}


uint32_t SL2x23_TIMEOUT_UserCallBack(void)
{
	printf("I2C ERROR \r\n");
	while(1)
	{}
}



uint32_t i2c_write_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t reg_value)
{
#ifdef HARDWART_I2C
	
	/* 等待忙碌标志清除 */
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_LONG_TIMEOUT);
	
	/* 开始配置 */
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* 等待开始位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 发送从地址并写使能操作 */
	I2C_Send7bitAddress(SL2x23_I2C,(i2c_dev_addr),I2C_Direction_Transmitter);
	
	/* 等待地址位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 清除ADDR中断位-通过读取SR1和SR2来实现 */
	CLEAR_ADDR_BIT
	
	/* 等待发送使能位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 准备要发送的寄存器值 */
	I2C_SendData(SL2x23_I2C,reg_addr);
	
	/* 等待发送使能位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 准备要发送的寄存器值 */
	I2C_SendData(SL2x23_I2C,reg_value);
	
	/* 等待BTF位 */
	WAIT_FOR_FLAG(I2C_FLAG_BTF,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 结束通信 */
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	
	
#else
	/* 第一步 发起I2C启动信号 */
	i2c_Start();
	
	/* 第二步，发起控制字节，高7Bit是地址，Bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(i2c_dev_addr|0);
	/* 第三步，发送一个时钟，判断器件是否正确应答 */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :控制字节无应答\r\n");
		return 0;
	}
	
	/* 第四部，发送字节地址 */
	i2c_SendByte(reg_addr);
	/* 第五步，等待ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR :寄存器地址字节无应答\r\n");
		return 0;
	}
	
	/* 第六步，开始写数据 */
	i2c_SendByte(value);
	
	/* 第七步，发送ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR:数据字节无应答\r\n");
		return 0;
	}
	
	/* 发送I2C停止信号 */
	i2c_Stop();
	i2c_Delay();
#endif
	return 0;
}



uint32_t i2c_read_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t *reg_value)
{
#ifdef HARDWART_I2C
	
	/* 等待忙碌标志清除 */
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_LONG_TIMEOUT);
	
	/* 开始配置 */
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* 等待开始位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 发送从地址并写使能操作 */
	I2C_Send7bitAddress(SL2x23_I2C,(i2c_dev_addr),I2C_Direction_Transmitter);
	
	/* 等待地址位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 清除ADDR中断位-通过读取SR1和SR2来实现 */
	CLEAR_ADDR_BIT
	
	/* 等待发送使能位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	/* 准备要发送的寄存器值 */
	I2C_SendData(SL2x23_I2C,reg_addr);
	
	/* 等待发送使能位设置 */
	WAIT_FOR_FLAG(I2C_FLAG_TXE,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	
	/* 等待BTF位 */
	WAIT_FOR_FLAG(I2C_FLAG_SB,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_Send7bitAddress(SL2x23_I2C,i2c_dev_addr,I2C_Direction_Receiver);
	
	WAIT_FOR_FLAG(I2C_FLAG_ADDR,SET,SL2x23_FLAG_TIMEOUT);
	
	I2C_AcknowledgeConfig(SL2x23_I2C,DISABLE);
	
	CLEAR_ADDR_BIT
	
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	
	WAIT_FOR_FLAG(I2C_FLAG_RXNE,SET,SL2x23_FLAG_TIMEOUT);
	
	*reg_value = I2C_ReceiveData(SL2x23_I2C);
	
	I2C_ClearFlag(SL2x23_I2C,I2C_FLAG_BTF);
	
	WAIT_FOR_FLAG(I2C_FLAG_BUSY,RESET,SL2x23_FLAG_TIMEOUT);
	
	I2C_AcknowledgeConfig(SL2x23_I2C,ENABLE);
	
	SL2x23_I2C->CR1 &= ~I2C_CR1_POS;
	
	
#else
	/* 第一步 发起I2C启动信号 */
	i2c_Start();
	
	/* 第二步，发起控制字节，高7Bit是地址，Bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(i2c_dev_addr|0);
	/* 第三步，发送一个时钟，判断器件是否正确应答 */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :控制字节无应答2\r\n");
		return 0;
	}
	
	/* 第四部，发送字节地址 */
	i2c_SendByte(reg_addr);
	/* 第五步，等待ACK */
	if(i2c_WaitAck()!= 0)
	{
		i2c_Stop();
		printf("I2C ERROR :寄存器地址字节无应答2\r\n");
		return 0;
	}
	
	/* 第6步：重新启动I2C总线。前面的代码的目的向SL2x23传送地址，下面开始读取数据 */
	i2c_Start();
	
	/* 第七步，发起控制字节，高7Bit是地址，Bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(i2c_dev_addr|1);
	/* 第八步，发送一个时钟，判断器件是否正确应答 */
	if(i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("I2C ERROR :控制字节无应答3\r\n");
		return 0;
	}
	
	/* 第九步 读取一字节数据 */
	*reg_value = i2c_ReadByte();
	i2c_NAck();
	
	i2c_Stop();
#endif
	return 0;
}

/******************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示存在， 返回1表示未探测到
******************************************************************************/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	
#ifdef HARDWART_I2C
	SL2x23_Timeout = SL2x23_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(SL2x23_I2C,I2C_FLAG_BUSY))
	{
		if((SL2x23_Timeout--) == 0) return 1;
	}
	
	I2C_GenerateSTART(SL2x23_I2C,ENABLE);
	SL2x23_Timeout = SL2x23_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SL2x23_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((SL2x23_Timeout--)==0)
		{
			I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
			return 1;
		}
	}
	
	/* 写入设备地址 */
	I2C_Send7bitAddress(SL2x23_I2C,_Address,I2C_Direction_Transmitter);
	SL2x23_Timeout = SL2x23_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(SL2x23_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((SL2x23_Timeout--) == 0)
		{
			I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
			return 1;
		}
	}
	
	/* 发送STOP */
	I2C_GenerateSTOP(SL2x23_I2C,ENABLE);
	return 0;
	
	
#else
	uint8_t ucAck;
	i2c_Start();
	i2c_SendByte(_Address | 0);
	ucAck = i2c_WaitAck();
	i2c_Stop();
	
	return ucAck;
	
#endif
}





