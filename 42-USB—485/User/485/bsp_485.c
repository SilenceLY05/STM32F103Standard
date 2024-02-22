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


/* RS485��ʼ������ */
void RS485_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RS485_USAER_RX_GPIO_CLK| RS485_USAER_TX_GPIO_CLK|RS485_RE_GPIO_CLK ,ENABLE);
	
	RCC_APB1PeriphClockCmd(RS485_USAER_CLK,ENABLE);
	
	/* ��USART TX��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = RS485_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	/* ��USART RX��GPIO����Ϊ��������ģʽ */
	GPIO_InitStructure.GPIO_Pin = RS485_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	/* 485�շ����ƹܽ� */
	GPIO_InitStructure.GPIO_Pin = RS485_RE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_RE_GPIO_PORT,&GPIO_InitStructure);
	
	/* USART ģʽ����*/
	USART_InitStructure.USART_BaudRate = RS485_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(RS485_USART,&USART_InitStructure);
	
	/*ʹ��USART*/
	USART_Cmd(RS485_USART,ENABLE);
	
	/*�����ж����ȼ�*/
	NVIC_Config();
	
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(RS485_USART,USART_IT_RXNE,ENABLE);
	
	/*����485оƬ�������ģʽ*/
	GPIO_SetBits(RS485_RE_GPIO_PORT,RS485_RE_GPIO_PIN);
	
}


/***************** ����һ���ַ�  **********************/
//ʹ�õ��ֽ����ݷ���ǰҪʹ�ܷ������ţ����ͺ�Ҫʹ�ܽ������š�
void RS485_SendByte(uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(RS485_USART,ch);
	
	/* �ȴ�������� */
	while(USART_GetFlagStatus(RS485_USART,USART_IT_TXE) == RESET);
}

/* ����ָ�����ݳ��� */
void RS485_SendString_Length(uint8_t *str,uint32_t strlen)
{
	unsigned int k = 0;
	//ʹ�ܷ�������
	RS485_TX_EN();
		do{
			RS485_SendByte(*(str+k));
			k++;
		}while(k<strlen);
		
	/* ����ʱ�ӣ�ʹRS485������� */
	Delay(0xFFFF);
		
	RS485_RX_EN();
}

/* �����ַ��� */
void RS485_SendString(uint8_t *str)
{
	unsigned int k = 0;
	//ʹ�ܷ�������
	RS485_TX_EN();
		do{
			RS485_SendByte(*(str+k));
			k++;
		}while(*(str+k) != '\0');
		
	/* ����ʱ�ӣ�ʹRS485������� */
	Delay(0xFFFF);
		
	RS485_RX_EN();
}



//�жϻ��洮������
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


//��ȡ���յ������ݺͳ���
char *get_rebuff(uint16_t *len)
{
	*len = usart_p;
	return (char*)&usart_buff;
}

//��ջ���
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
