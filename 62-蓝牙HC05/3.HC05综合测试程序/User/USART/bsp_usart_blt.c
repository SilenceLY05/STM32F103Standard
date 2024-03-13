#include "bsp_usart_blt.h"
#include <stdarg.h>


static void BLT_NVIC_Configuration(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = BLT_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/* 初始化USART GPIO配置，工作参数配置 */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	//打开串口GPIO的时钟
	BLT_USART_GPIO_APBxClockCmd(BLT_USART_GPIO_CLK,ENABLE);
	
	//打开串口外设的时钟
	BLT_USART_APBxClockCmd(BLT_USART_CLK,ENABLE);
	
	//将USART_TX配置为推挽复用模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BLT_SUART_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLT_USART_TX_PORT,&GPIO_InitStructure);
	
	//将USART_RX配置为浮空输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = BLT_SUART_RX_PIN;
	GPIO_Init(BLT_USART_RX_PORT,&GPIO_InitStructure);
	
	//配置串口的工作参数
	//配置波特率
	USART_InitStructure.USART_BaudRate = BLT_USART_BAUDRATE;
	//配置 帧数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//配置工作模式 收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;
	//完成串口的初始化
	USART_Init(BLT_USARTx, &USART_InitStructure);
	
	BLT_NVIC_Configuration();
	
	USART_ITConfig(BLT_USARTx,USART_IT_RXNE,ENABLE);
	
	USART_ITConfig(BLT_USARTx,USART_IT_IDLE,ENABLE);
	
	
	//使能串口
	USART_Cmd(BLT_USARTx,ENABLE);
	USART_ClearFlag(BLT_USARTx,USART_FLAG_TC);
	
	
}


//*  发送一个字节 */
void USART_SendByte(USART_TypeDef *pUSARTx,uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
	
	/* 等待发送数据寄存器为空 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	
}

/* 发送两个字节 */
void USART_SendHalfWord(USART_TypeDef *pUSARTx,uint16_t ch)
{
	uint8_t temp_h;
	uint8_t temp_l;
	temp_h = (ch&0xff00) >>8;
	temp_l = (ch&0xff);
	
	/* 发送两个字节数据到USART */
	USART_SendData(pUSARTx,temp_h);
	
	/* 等待发送数据寄存器为空 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	
	USART_SendData(pUSARTx,temp_l);
	
	/* 等待发送数据寄存器为空 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
}

/* 发送8位数据的数组 */
void USART_SendArray(USART_TypeDef *pUSARTx,uint8_t *array,uint16_t num)
{
	uint8_t i;
	for(i = 0 ; i < num ; i++)
	{
		USART_SendByte( pUSARTx,array[i]);
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
}


/* 发送字符串 */
void USART_SendString(USART_TypeDef *pUSARTx,uint8_t *str)
{
	unsigned int k = 0;
	do
	{
		USART_SendByte(pUSARTx,*(str+k));
		k++;
	}while(*(str+k) != '\0');
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
		
}

/* 发送指定长度字符串 */
void USART_SendStr_Length(USART_TypeDef *pUSARTx,uint8_t *str,uint32_t strlen)
{
		unsigned int k = 0;
	do
	{
		USART_SendByte(pUSARTx,*(str+k));
		k++;
		
	}while(k<strlen);
}

//中断缓存串口数据
volatile uint16_t usart_p = 0;
uint8_t usart_buff[USART_BUFF_SIZE];

//获取接收到的数据和长度
char *get_rebuff(uint16_t *len)
{
	*len = usart_p;
	return (char*)&usart_buff;
}

//清空缓存区
void clean_rebuff(void)
{
	uint16_t i = USART_BUFF_SIZE+1;
	usart_p = 0;
	while(i)
		usart_buff[--i] = 0;
}


