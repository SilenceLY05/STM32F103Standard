#include "bsp_usart.h"




/* 配置嵌套向量中断控制器NVIC */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/* 嵌套向量中断控制器组选择 */
	/* 提示NVIC_PriorityGroupConfig()在整个工程只需要调用一次来配置优先级分组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
	/* 抢断优先级 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
	
}

/* 初始化USART GPIO配置，工作参数配置 */
void USART_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	//打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK,ENABLE);
	
	//打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK,ENABLE);
	
	//将USART_TX配置为推挽复用模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	//将USART_RX配置为浮空输入模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	//配置串口的工作参数
	//配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
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
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	//串口中断优先级配置
	NVIC_Configuration();
	
	//使能串口接收中断
	USART_ITConfig(DEBUG_USARTx,USART_IT_RXNE,ENABLE);
	
	//使能串口
	USART_Cmd(DEBUG_USARTx,ENABLE);
	
	
}

/*  发送一个字节 */
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
void USART_SendString(USART_TypeDef *pUSARTx,char *str)
{
	unsigned int k = 0;
	do
	{
		USART_SendByte(pUSARTx,*(str+k));
		k++;
	}while(*(str+k) != '\0');
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
		
}

//重定向C库函数printf到串口，重定向后可使用printf函数
int fputc(int ch ,FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(DEBUG_USARTx,(uint8_t) ch);
	
	/* 等待发送完毕 */
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
	
	return (ch);
}

/* 重定向C库函数scanf到串口，重定向后可使用scanf/getchar等函数 */
int fgetc(FILE*f)
{
	/* 等待串口输出数据 */
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_RXNE) == RESET);
	
	return (int)USART_ReceiveData(DEBUG_USARTx);
}

