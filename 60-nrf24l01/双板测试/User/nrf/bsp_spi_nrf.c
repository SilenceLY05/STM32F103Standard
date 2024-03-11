#include "bsp_spi_nrf.h"
#include "bsp_usart.h"

uint8_t RX_BUF[RX_PLOAD_WIDTH];      //接收数据缓存
uint8_t TX_BUF[TX_PLOAD_WIDTH];      //发射数据缓存
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; // 定义一个静态发送地址
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};


void Delay(__IO uint32_t nCount)
{
	for(;nCount!= 0;nCount--);
}

/* SPI 的I/O配置 */
void SPI_NRF_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	SPI_InitTypeDef      SPI_InitStructure;
	
	/* 开启GPIO对应的时钟 */
	NRF_GPIO_ClockCmd(NRF_SCK_GPIO_CLK | NRF_MOSI_GPIO_CLK | NRF_MISO_GPIO_CLK | NRF_CSN_GPIO_CLK | NRF_CE_GPIO_CLK | NRF_IRQ_GPIO_CLK ,ENABLE);
	
	/* 开启SPI1时钟 */
	NRF_SPI_ClockCmd(NRF_SPI_CLK,ENABLE);
	
	/* SCK,MISO,MOSI引脚 */
	GPIO_InitStructure.GPIO_Pin = NRF_SCK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_SCK_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_MISO_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_MISO_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_MOSI_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_MOSI_GPIO_PORT,&GPIO_InitStructure);
	
	/* 配置CSN CE IRQ 引脚 */
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CSN_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_CE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CE_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_IRQ_GPIO_PORT,&GPIO_InitStructure);
	
	/* 这是自定义的宏，用于拉高csn引脚，NRF进入空闲状态 */
  NRF_CSN_HIGH(); 
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;    //8分配，9MHz
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                          //第一个边沿有效，上升沿为采样时刻
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                            //时钟极性，空闲时为低
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                     //数据大小8位
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //双线全双工
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                    //高位先行
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                         //主模式
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                             //NSS信号由软件产生
	
	SPI_Init(NRF_SPIx,&SPI_InitStructure);
	
	SPI_Cmd(NRF_SPIx,ENABLE);
	
}

/* 用于向NRF读写一个字节数据 */
uint8_t SPI_NRF_ReadWrite(uint8_t data)
{
	/* 当SPI发送缓冲器非空时等待 */
	while(SPI_I2S_GetFlagStatus(NRF_SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
	/* 通过SPI1发送一个字节的数据 */
	SPI_I2S_SendData(NRF_SPIx,data);
	
	/* 当SPI接收缓冲器为空时等待 */
	while(SPI_I2S_GetFlagStatus(NRF_SPIx,SPI_I2S_FLAG_RXNE) == RESET);
	
	/* 返回从SPI总线独到的数据 */
	return SPI_I2S_ReceiveData(NRF_SPIx);
}

/* 向寄存器reg写入一个字节，同时返回状态字节 */
uint8_t SPI_NRF_WriteReg(uint8_t reg,uint8_t data)
{
	uint8_t status;
	
	NRF_CE_LOW();
	
	/* 置低CSN，使能SPI传输 */
	NRF_CSN_LOW();
	
	/* 发送命令及寄存器号 */
	status = SPI_NRF_ReadWrite(reg);
	
	/* 向寄存器写入数据 */
	SPI_NRF_ReadWrite(data);
	
	/* CSN拉高，完成 */
	NRF_CSN_HIGH();
	
	/* 返回寄存器状态值 */
	return(status);
	
}

/* 从NRF特定的寄存器读出数据 reg:NRF命令+寄存器地址 返回寄存器中的数据 */
uint8_t SPI_NRF_ReadReg(uint8_t reg)
{
	uint8_t reg_val;
	
	NRF_CE_LOW();
	
	/* 置低CSN，使能SPI传输 */
	NRF_CSN_LOW();
	
	/* 发送寄存器号 */
	SPI_NRF_ReadWrite(reg);
	
	/* 读取寄存器中的值 */
	reg_val = SPI_NRF_ReadReg(NOP);
	
	/* CSN拉高，完成 */
	NRF_CSN_HIGH();
	
	/* 返回寄存器状态值 */
	return(reg_val);
}

/* 用于向NRF寄存器中写入一串数据 reg:NRF命令+寄存器地址 pBuffer:用于存储将被写入的寄存器数据的数组，Num：pBuffer数据长度 返回寄存器状态 */
uint8_t SPI_NRF_WriteBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();
	
	/* 置低CSN，使能SPI传输 */
	NRF_CSN_LOW();
	
	/* 发送寄存器号 */
	status = SPI_NRF_ReadWrite(reg);
	
	/* 向缓冲区写入数据 */
	for(byte_cnt = 0;byte_cnt<Num;byte_cnt++)
		SPI_NRF_ReadWrite(*pBuffer++);  //写数据到缓冲区 	
	
	/* CSN拉高，完成 */
	NRF_CSN_HIGH();
	
	/* 返回寄存器状态值 */
	return(status);
}

/* 用于从NRF寄存器中读出一串数据 reg:NRF命令+寄存器地址 pBuffer:用于存储将被读出的寄存器数据的数组，Num：pBuffer数据长度 返回寄存器状态 */
uint8_t SPI_NRF_ReadBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();
	
	/* 置低CSN，使能SPI传输 */
	NRF_CSN_LOW();
	
	/* 发送寄存器号 */
	status = SPI_NRF_ReadWrite(reg);
	
	/* 向缓冲区写入数据 */
	for(byte_cnt = 0;byte_cnt<Num;byte_cnt++)
		pBuffer[byte_cnt] = SPI_NRF_ReadWrite(NOP);//从NRF24L01读取数据  
	
	/* CSN拉高，完成 */
	NRF_CSN_HIGH();
	
	/* 返回寄存器状态值 */
	return(status);
}

/* 配置并进入接收模式 */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();
	
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);   //写RX节点地址
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);  //使能通道0的自动应答
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);   //使能通道0的接收地址
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANNAL);    //设置RF通信频率
	SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);  //选择通道0的有效数据宽度
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);     //设置TX发射参数，0db增益，2Mbps，低噪声增益开启
	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0f);       //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	
	/*CE拉高，进入接收模式*/	
	NRF_CE_HIGH();
}

/* 配置发送模式 */
void NRF_TX_Mode(void)
{
	NRF_CE_LOW();
	
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);  //写TX节点地址 
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);   //设置TX节点地址，使能ACK
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);  //使能通道0的自动应答
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);   //使能通道0的接收地址
	SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);  //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANNAL);    //设置RF通道为CHANAL
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);     //设置TX发射参数，0db增益，2Mbps，低噪声增益开启
	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);       //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	
	/*CE拉高，进入接收模式*/	
	NRF_CE_HIGH();
	Delay(0xFFFF);    //CE要拉高一段时间才进入发射模式
}

/* 主要用于NRF与MCU是否正常连接 */
uint8_t NRF_Check(void)
{
	uint8_t buf[5] = {0xC2,0xC2,0xC2,0xC2,0xC2};
	uint8_t buf2[5];
	
	uint8_t i;
	
	/* 写入5个字节的地址 */
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+TX_ADDR,buf,5);
	
	/* 读出写入的地址 */
	SPI_NRF_ReadBuffer(TX_ADDR,buf2,5);
	
	for(i=0;i<5;i++)
	{
		if(buf2[i] != 0xC2)
			break;
	}
	if(i == 5)
		return SUCCESS;  //MCU与NRF成功连接
	else
		return ERROR;    //MCU与NRF不正常连接
}

/* 用于向NRF的发送缓冲区中写入数据 txBuffer:存储了将要发送的数据的数组，外部定义  */
uint8_t NRF_TX_Data(uint8_t *txBuffer)
{
	uint8_t status;
	
	NRF_CE_LOW();
	
	/* 写入数据到txBuffer最大32个字节 */
	SPI_NRF_WriteBuffer(WR_TX_PLOAD,txBuffer,TX_PLOAD_WIDTH);
	
	/* CE为高，txBuffer非空，发送数据包 */
	NRF_CE_HIGH();
	
	/* 等待发送完成中断 */
	while(NRF_Read_IRQ() != 0);
	
	/* 读取寄存器的状态值 */
	status = SPI_NRF_ReadReg(STATUS);
	
	/* 清除TX_DS或MAX_RT中断标志 */
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,status);
	
	SPI_NRF_WriteReg(FLUSH_TX,NOP);   //清除TX FIFO寄存器 
	
	/* 判断中断类型 */
	if(status & MAX_RT)           //达到最大重发次数
		return MAX_RT;
	
	else if( status & TX_DS)      //发送完成
		return TX_DS;
	else
		return ERROR;               //其他原因发送失败
}

/* 用于从NRF的接收缓冲区中读出数据 rxBuffer:用于接收该数据的数组，外部定义  */
uint8_t NRF_RX_Data(uint8_t *rxBuffer)
{
	uint8_t status;
	
	NRF_CE_HIGH();
	
	/* 轮询标志 */
	while(NRF_Read_IRQ() == 0)
	{
		NRF_CE_LOW();    //进入待机模式
		
		/* 读取寄存器的状态值 */
		status = SPI_NRF_ReadReg(STATUS);
		
		/* 清除中断标志 */
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,status);
		
		/* 判断中断类型 */
		if(status & RX_DR)           //达到最大重发次数
		{
			SPI_NRF_ReadBuffer(RD_RX_PLOAD,rxBuffer,RX_PLOAD_WIDTH);    //读取数据
			SPI_NRF_WriteReg(FLUSH_RX,NOP);            //清除RX FIFO寄存器 
			return RX_DR;
		}
		else
			return ERROR;               //没收到任何数据
		
	}
	
	return ERROR;          //没收到任何数据
	
}






