#include "bsp_485.h"


static void Delay(__IO uint32_t nCount);

static void NVIC_Config(void)
{
	NVIC_InitTypeDef     NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = RS485_INT_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}


/* RS485初始化函数 */
void RS485_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RS485_USAER_RX_GPIO_CLK| RS485_USAER_TX_GPIO_CLK|RS485_RE_GPIO_CLK ,ENABLE);
	
	RCC_APB1PeriphClockCmd(RS485_USAER_CLK,ENABLE);
	
	/* 将USART TX的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = RS485_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	/* 将USART RX的GPIO配置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Pin = RS485_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	/* 485收发控制管脚 */
	GPIO_InitStructure.GPIO_Pin = RS485_RE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_RE_GPIO_PORT,&GPIO_InitStructure);
	
	/* USART 模式配置*/
	USART_InitStructure.USART_BaudRate = RS485_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(RS485_USART,&USART_InitStructure);
	
	/*使能USART*/
	USART_Cmd(RS485_USART,ENABLE);
	
	/*配置中断优先级*/
	NVIC_Config();
	
	/* 使能串口接收中断 */
	USART_ITConfig(RS485_USART,USART_IT_RXNE,ENABLE);
	
	/*控制485芯片进入接收模式*/
	GPIO_SetBits(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN);
	
}


/***************** 发送一个字符  **********************/
//使用单字节数据发送前要使能发送引脚，发送后要使能接收引脚。
void RS485_SendByte(uint8_t ch)
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(RS485_USART,ch);
	
	/* 等待发送完毕 */
	while(USART_GetFlagStatus(RS485_USART,USART_IT_TXE) == RESET);
}

/* 发送指定数据长度 */
void RS485_SendString_Length(uint8_t *str,uint32_t strlen)
{
	unsigned int k = 0;
	//使能发送数据
	RS485_TX_EN();
		do{
			RS485_SendByte(*(str+k));
			k++;
		}while(k<strlen);
		
	/* 加入时延，使RS485传输完成 */
	Delay(0xFFFF);
		
	RS485_RX_EN();
}

/* 发送字符串 */
void RS485_SendString(uint8_t *str)
{
	unsigned int k = 0;
	//使能发送数据
	RS485_TX_EN();
		do{
			RS485_SendByte(*(str+k));
			k++;
		}while(*(str+k) != '\0');
		
	/* 加入时延，使RS485传输完成 */
	Delay(0xFFFF);
		
	RS485_RX_EN();
}



//中断缓存串口数据
#define USART_BUFF_SIZE    1024
volatile uint16_t usart_p = 0;
uint8_t usart_buff[USART_BUFF_SIZE];
void bsp_RS485_IRQHandler(void)
{
	if(usart_p < USART_BUFF_SIZE)
	{
		if(USART_GetFlagStatus(RS485_USART,USART_IT_RXNE) != RESET)
		{
			usart_buff[usart_p] = USART_ReceiveData(RS485_USART);
			usart_p++;
			
			USART_ClearITPendingBit(RS485_USART,USART_IT_RXNE);
		}
	}
	else
	{
		USART_ClearITPendingBit(RS485_USART,USART_IT_RXNE);
	}
}


//获取接收到的数据和长度
char *get_rebuff(uint16_t *len)
{
	*len = usart_p;
	return (char*)&usart_buff;
}

//清空缓存
void clean_rebuff(void)
{
	uint16_t i = USART_BUFF_SIZE +1;
	usart_p = 0;
	while(i)
		usart_buff[--i] = 0; 
}


static void Delay(__IO uint32_t nCount)
{
	for(;nCount!=0;nCount--);
}
