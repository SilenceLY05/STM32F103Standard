#include "bsp_spi_nrf.h"
#include "bsp_usart.h"

uint8_t RX_BUF[RX_PLOAD_WIDTH];      //�������ݻ���
uint8_t TX_BUF[TX_PLOAD_WIDTH];      //�������ݻ���
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; // ����һ����̬���͵�ַ
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};


void Delay(__IO uint32_t nCount)
{
	for(;nCount!= 0;nCount--);
}

/* SPI ��I/O���� */
void SPI_NRF_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	SPI_InitTypeDef      SPI_InitStructure;
	
	/* ����GPIO��Ӧ��ʱ�� */
	NRF_GPIO_ClockCmd(NRF_SCK_GPIO_CLK | NRF_MOSI_GPIO_CLK | NRF_MISO_GPIO_CLK | NRF_CSN_GPIO_CLK | NRF_CE_GPIO_CLK | NRF_IRQ_GPIO_CLK ,ENABLE);
	
	/* ����SPI1ʱ�� */
	NRF_SPI_ClockCmd(NRF_SPI_CLK,ENABLE);
	
	/* SCK,MISO,MOSI���� */
	GPIO_InitStructure.GPIO_Pin = NRF_SCK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_SCK_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_MISO_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_MISO_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_MOSI_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_MOSI_GPIO_PORT,&GPIO_InitStructure);
	
	/* ����CSN CE IRQ ���� */
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CSN_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_CE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_CE_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(NRF_IRQ_GPIO_PORT,&GPIO_InitStructure);
	
	/* �����Զ���ĺ꣬��������csn���ţ�NRF�������״̬ */
  NRF_CSN_HIGH(); 
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;    //8���䣬9MHz
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                          //��һ��������Ч��������Ϊ����ʱ��
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                            //ʱ�Ӽ��ԣ�����ʱΪ��
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                     //���ݴ�С8λ
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //˫��ȫ˫��
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                    //��λ����
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                         //��ģʽ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                             //NSS�ź����������
	
	SPI_Init(NRF_SPIx,&SPI_InitStructure);
	
	SPI_Cmd(NRF_SPIx,ENABLE);
	
}

/* ������NRF��дһ���ֽ����� */
uint8_t SPI_NRF_ReadWrite(uint8_t data)
{
	/* ��SPI���ͻ������ǿ�ʱ�ȴ� */
	while(SPI_I2S_GetFlagStatus(NRF_SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
	/* ͨ��SPI1����һ���ֽڵ����� */
	SPI_I2S_SendData(NRF_SPIx,data);
	
	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
	while(SPI_I2S_GetFlagStatus(NRF_SPIx,SPI_I2S_FLAG_RXNE) == RESET);
	
	/* ���ش�SPI���߶��������� */
	return SPI_I2S_ReceiveData(NRF_SPIx);
}

/* ��Ĵ���regд��һ���ֽڣ�ͬʱ����״̬�ֽ� */
uint8_t SPI_NRF_WriteReg(uint8_t reg,uint8_t data)
{
	uint8_t status;
	
	NRF_CE_LOW();
	
	/* �õ�CSN��ʹ��SPI���� */
	NRF_CSN_LOW();
	
	/* ��������Ĵ����� */
	status = SPI_NRF_ReadWrite(reg);
	
	/* ��Ĵ���д������ */
	SPI_NRF_ReadWrite(data);
	
	/* CSN���ߣ���� */
	NRF_CSN_HIGH();
	
	/* ���ؼĴ���״ֵ̬ */
	return(status);
	
}

/* ��NRF�ض��ļĴ����������� reg:NRF����+�Ĵ�����ַ ���ؼĴ����е����� */
uint8_t SPI_NRF_ReadReg(uint8_t reg)
{
	uint8_t reg_val;
	
	NRF_CE_LOW();
	
	/* �õ�CSN��ʹ��SPI���� */
	NRF_CSN_LOW();
	
	/* ���ͼĴ����� */
	SPI_NRF_ReadWrite(reg);
	
	/* ��ȡ�Ĵ����е�ֵ */
	reg_val = SPI_NRF_ReadReg(NOP);
	
	/* CSN���ߣ���� */
	NRF_CSN_HIGH();
	
	/* ���ؼĴ���״ֵ̬ */
	return(reg_val);
}

/* ������NRF�Ĵ�����д��һ������ reg:NRF����+�Ĵ�����ַ pBuffer:���ڴ洢����д��ļĴ������ݵ����飬Num��pBuffer���ݳ��� ���ؼĴ���״̬ */
uint8_t SPI_NRF_WriteBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();
	
	/* �õ�CSN��ʹ��SPI���� */
	NRF_CSN_LOW();
	
	/* ���ͼĴ����� */
	status = SPI_NRF_ReadWrite(reg);
	
	/* �򻺳���д������ */
	for(byte_cnt = 0;byte_cnt<Num;byte_cnt++)
		SPI_NRF_ReadWrite(*pBuffer++);  //д���ݵ������� 	
	
	/* CSN���ߣ���� */
	NRF_CSN_HIGH();
	
	/* ���ؼĴ���״ֵ̬ */
	return(status);
}

/* ���ڴ�NRF�Ĵ����ж���һ������ reg:NRF����+�Ĵ�����ַ pBuffer:���ڴ洢���������ļĴ������ݵ����飬Num��pBuffer���ݳ��� ���ؼĴ���״̬ */
uint8_t SPI_NRF_ReadBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num)
{
	uint8_t status,byte_cnt;
	
	NRF_CE_LOW();
	
	/* �õ�CSN��ʹ��SPI���� */
	NRF_CSN_LOW();
	
	/* ���ͼĴ����� */
	status = SPI_NRF_ReadWrite(reg);
	
	/* �򻺳���д������ */
	for(byte_cnt = 0;byte_cnt<Num;byte_cnt++)
		pBuffer[byte_cnt] = SPI_NRF_ReadWrite(NOP);//��NRF24L01��ȡ����  
	
	/* CSN���ߣ���� */
	NRF_CSN_HIGH();
	
	/* ���ؼĴ���״ֵ̬ */
	return(status);
}

/* ���ò��������ģʽ */
void NRF_RX_Mode(void)
{
	NRF_CE_LOW();
	
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);   //дRX�ڵ��ַ
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);  //ʹ��ͨ��0���Զ�Ӧ��
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);   //ʹ��ͨ��0�Ľ��յ�ַ
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANNAL);    //����RFͨ��Ƶ��
	SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);  //ѡ��ͨ��0����Ч���ݿ��
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);     //����TX���������0db���棬2Mbps�����������濪��
	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0f);       //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	
	/*CE���ߣ��������ģʽ*/	
	NRF_CE_HIGH();
}

/* ���÷���ģʽ */
void NRF_TX_Mode(void)
{
	NRF_CE_LOW();
	
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);  //дTX�ڵ��ַ 
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);   //����TX�ڵ��ַ��ʹ��ACK
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);  //ʹ��ͨ��0���Զ�Ӧ��
	SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);   //ʹ��ͨ��0�Ľ��յ�ַ
	SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);  //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANNAL);    //����RFͨ��ΪCHANAL
	SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);     //����TX���������0db���棬2Mbps�����������濪��
	SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);       //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	
	/*CE���ߣ��������ģʽ*/	
	NRF_CE_HIGH();
	Delay(0xFFFF);    //CEҪ����һ��ʱ��Ž��뷢��ģʽ
}

/* ��Ҫ����NRF��MCU�Ƿ��������� */
uint8_t NRF_Check(void)
{
	uint8_t buf[5] = {0xC2,0xC2,0xC2,0xC2,0xC2};
	uint8_t buf2[5];
	
	uint8_t i;
	
	/* д��5���ֽڵĵ�ַ */
	SPI_NRF_WriteBuffer(NRF_WRITE_REG+TX_ADDR,buf,5);
	
	/* ����д��ĵ�ַ */
	SPI_NRF_ReadBuffer(TX_ADDR,buf2,5);
	
	for(i=0;i<5;i++)
	{
		if(buf2[i] != 0xC2)
			break;
	}
	if(i == 5)
		return SUCCESS;  //MCU��NRF�ɹ�����
	else
		return ERROR;    //MCU��NRF����������
}

/* ������NRF�ķ��ͻ�������д������ txBuffer:�洢�˽�Ҫ���͵����ݵ����飬�ⲿ����  */
uint8_t NRF_TX_Data(uint8_t *txBuffer)
{
	uint8_t status;
	
	NRF_CE_LOW();
	
	/* д�����ݵ�txBuffer���32���ֽ� */
	SPI_NRF_WriteBuffer(WR_TX_PLOAD,txBuffer,TX_PLOAD_WIDTH);
	
	/* CEΪ�ߣ�txBuffer�ǿգ��������ݰ� */
	NRF_CE_HIGH();
	
	/* �ȴ���������ж� */
	while(NRF_Read_IRQ() != 0);
	
	/* ��ȡ�Ĵ�����״ֵ̬ */
	status = SPI_NRF_ReadReg(STATUS);
	
	/* ���TX_DS��MAX_RT�жϱ�־ */
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,status);
	
	SPI_NRF_WriteReg(FLUSH_TX,NOP);   //���TX FIFO�Ĵ��� 
	
	/* �ж��ж����� */
	if(status & MAX_RT)           //�ﵽ����ط�����
		return MAX_RT;
	
	else if( status & TX_DS)      //�������
		return TX_DS;
	else
		return ERROR;               //����ԭ����ʧ��
}

/* ���ڴ�NRF�Ľ��ջ������ж������� rxBuffer:���ڽ��ո����ݵ����飬�ⲿ����  */
uint8_t NRF_RX_Data(uint8_t *rxBuffer)
{
	uint8_t status;
	
	NRF_CE_HIGH();
	
	/* ��ѯ��־ */
	while(NRF_Read_IRQ() == 0)
	{
		NRF_CE_LOW();    //�������ģʽ
		
		/* ��ȡ�Ĵ�����״ֵ̬ */
		status = SPI_NRF_ReadReg(STATUS);
		
		/* ����жϱ�־ */
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,status);
		
		/* �ж��ж����� */
		if(status & RX_DR)           //�ﵽ����ط�����
		{
			SPI_NRF_ReadBuffer(RD_RX_PLOAD,rxBuffer,RX_PLOAD_WIDTH);    //��ȡ����
			SPI_NRF_WriteReg(FLUSH_RX,NOP);            //���RX FIFO�Ĵ��� 
			return RX_DR;
		}
		else
			return ERROR;               //û�յ��κ�����
		
	}
	
	return ERROR;          //û�յ��κ�����
	
}






