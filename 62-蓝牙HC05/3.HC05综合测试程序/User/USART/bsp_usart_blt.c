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

/* ��ʼ��USART GPIO���ã������������� */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	//�򿪴���GPIO��ʱ��
	BLT_USART_GPIO_APBxClockCmd(BLT_USART_GPIO_CLK,ENABLE);
	
	//�򿪴��������ʱ��
	BLT_USART_APBxClockCmd(BLT_USART_CLK,ENABLE);
	
	//��USART_TX����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BLT_SUART_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLT_USART_TX_PORT,&GPIO_InitStructure);
	
	//��USART_RX����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = BLT_SUART_RX_PIN;
	GPIO_Init(BLT_USART_RX_PORT,&GPIO_InitStructure);
	
	//���ô��ڵĹ�������
	//���ò�����
	USART_InitStructure.USART_BaudRate = BLT_USART_BAUDRATE;
	//���� ֡�����ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//���ù���ģʽ �շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;
	//��ɴ��ڵĳ�ʼ��
	USART_Init(BLT_USARTx, &USART_InitStructure);
	
	BLT_NVIC_Configuration();
	
	USART_ITConfig(BLT_USARTx,USART_IT_RXNE,ENABLE);
	
	USART_ITConfig(BLT_USARTx,USART_IT_IDLE,ENABLE);
	
	
	//ʹ�ܴ���
	USART_Cmd(BLT_USARTx,ENABLE);
	USART_ClearFlag(BLT_USARTx,USART_FLAG_TC);
	
	
}


//*  ����һ���ֽ� */
void USART_SendByte(USART_TypeDef *pUSARTx,uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
	
	/* �ȴ��������ݼĴ���Ϊ�� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	
}

/* ���������ֽ� */
void USART_SendHalfWord(USART_TypeDef *pUSARTx,uint16_t ch)
{
	uint8_t temp_h;
	uint8_t temp_l;
	temp_h = (ch&0xff00) >>8;
	temp_l = (ch&0xff);
	
	/* ���������ֽ����ݵ�USART */
	USART_SendData(pUSARTx,temp_h);
	
	/* �ȴ��������ݼĴ���Ϊ�� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	
	USART_SendData(pUSARTx,temp_l);
	
	/* �ȴ��������ݼĴ���Ϊ�� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
}

/* ����8λ���ݵ����� */
void USART_SendArray(USART_TypeDef *pUSARTx,uint8_t *array,uint16_t num)
{
	uint8_t i;
	for(i = 0 ; i < num ; i++)
	{
		USART_SendByte( pUSARTx,array[i]);
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
}


/* �����ַ��� */
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

/* ����ָ�������ַ��� */
void USART_SendStr_Length(USART_TypeDef *pUSARTx,uint8_t *str,uint32_t strlen)
{
		unsigned int k = 0;
	do
	{
		USART_SendByte(pUSARTx,*(str+k));
		k++;
		
	}while(k<strlen);
}

//�жϻ��洮������
volatile uint16_t usart_p = 0;
uint8_t usart_buff[USART_BUFF_SIZE];

//��ȡ���յ������ݺͳ���
char *get_rebuff(uint16_t *len)
{
	*len = usart_p;
	return (char*)&usart_buff;
}

//��ջ�����
void clean_rebuff(void)
{
	uint16_t i = USART_BUFF_SIZE+1;
	usart_p = 0;
	while(i)
		usart_buff[--i] = 0;
}


