#include "bsp_i2c_ee.h"
#include "bsp_usart.h"


uint16_t EEPROM_ADDRESS;

/* 初始化I2C相关的GPIO */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能与I2C有关的时钟 */
	EEPROM_I2C_APBxClock_FUN(EEPROM_I2C_CLK,ENABLE);
	EEPROM_I2C_GPIO_APBxClock_FUN ( EEPROM_I2C_GPIO_CLK, ENABLE );
	
	
	/* 配置I2C SDA 和SCL引脚 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C_SDA_PORT,&GPIO_InitStructure);
	
}

/* 配置I2C工作模式 */
static void I2C_Mode_Config(void)
{
	I2C_InitTypeDef     I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7;
	
	I2C_Init(EEPROM_I2Cx,&I2C_InitStructure);
	I2C_Cmd(EEPROM_I2Cx,ENABLE);
}

void I2C_EE_Init(void)
{

  I2C_GPIO_Config(); 
 
  I2C_Mode_Config();

/* 根据头文件i2c_ee.h中的定义来选择EEPROM的设备地址 */
#ifdef EEPROM_Block0_ADDRESS
  /* 选择 EEPROM Block0 来写入 */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif

#ifdef EEPROM_Block1_ADDRESS  
	/* 选择 EEPROM Block1 来写入 */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif

#ifdef EEPROM_Block2_ADDRESS  
	/* 选择 EEPROM Block2 来写入 */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif

#ifdef EEPROM_Block3_ADDRESS  
	/* 选择 EEPROM Block3 来写入 */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/* 向EEPROM写入一个字节 */
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer,uint8_t WriteAddr)
{
	/* 发送起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* 检测EV5事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* 检测EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* 检测EV8事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTING) == ERROR);
	
	/* 发送数据 */
	I2C_SendData(EEPROM_I2Cx,*pBuffer);
	
	/* 数据发送完，检测EV8_2 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	/* 数据传输完成，发送结束信号 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	
}

/* 在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数不能超过EEPROM页的大小，AT24C02每页有8个字节 */
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToWrite)
{
	/* 发送起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* 检测EV5事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* 检测EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* 检测EV8事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_SLAVE_BYTE_TRANSMITTING) == ERROR);
	
	while(NumByteToWrite)
	{
		/* 发送数据 */
		I2C_SendData(EEPROM_I2Cx,*pBuffer);
		
		/* 数据发送完，检测EV8_2 */
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
		
		pBuffer++;
		NumByteToWrite--;
		
	}
	
	/* 数据传输完成，发送结束信号 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
}


/* 从EEPROM中读取一个数据 */
uint32_t I2C_EE_SeqRead(uint8_t *pBuffer,uint8_t WriteAddr,uint8_t NumByteToRead)
{
	/* 发送起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* 检测EV5事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	
	/* 检测EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	/* EV6事件被检测到，发送要操作的存储单元地址 */
	I2C_SendData(EEPROM_I2Cx,WriteAddr);
	
	/* 检测EV8事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	/* 第二次起始信号 */
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
	
	/* 检测EV5事件 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	/* EV5事件被检测到，发送地址 */
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Receiver);
	
	/* 检测EV6 */
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
		
	while(NumByteToRead)
	{
		if(NumByteToRead == 1)
		{
			//若接收到的是最后一个字节
			I2C_AcknowledgeConfig(EEPROM_I2Cx,DISABLE);
			
		}
		
		/* 检测EV7事件 即寄存器有新的有效数据 */
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		
		*pBuffer = I2C_ReceiveData(EEPROM_I2Cx);	
		
		pBuffer++;
		NumByteToRead--;

	}
	
	/* 数据传输完成，发送结束信号 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	//重新配置ACK使能，以便下次通讯
	I2C_AcknowledgeConfig(EEPROM_I2Cx,ENABLE);
	
}

//等待EEPROM内部时序完成
void EEPROM_WaitForWriteEnd(void)
{
	do{
		/* 发送起始信号 */
		I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);
		
		/* 检测EV5事件 */
		while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_SB) == RESET);
	
		/* EV5事件被检测到，发送地址 */
		I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);
	}
	
	/* 检测EV6 */
	while(I2C_GetFlagStatus(EEPROM_I2Cx,I2C_FLAG_ADDR) == RESET);
	

	/* EEPROM内部时序完成 传输完成 */
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
	
	
	
}

