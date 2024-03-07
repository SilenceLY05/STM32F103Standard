#include "bsp_usart.h"


/* ��ʼ��USART GPIO���ã������������� */
void USART_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	USART_InitTypeDef    USART_InitStructure;
	
	//�򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK,ENABLE);
	
	//�򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK,ENABLE);
	
	//��USART_TX����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	//��USART_RX����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	//���ô��ڵĹ�������
	//���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
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
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	//ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx,ENABLE);
	
}

/*  ����һ���ֽ� */
void USART_SendByte(USART_TypeDef *pUSARTx,uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
	
	/* �ȴ��������ݼĴ���Ϊ�� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	
}

/* �����ַ��� */
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

//�ض���C�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch ,FILE *f)
{
	/* ����һ���ֽ����ݵ����� */
	USART_SendData(DEBUG_USARTx,(uint8_t) ch);
	
	/* �ȴ�������� */
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
	
	return (ch);
}

/* �ض���C�⺯��scanf�����ڣ��ض�����ʹ��scanf/getchar�Ⱥ��� */
int fgetc(FILE*f)
{
	/* �ȴ������������ */
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_RXNE) == RESET);
	
	return (int)USART_ReceiveData(DEBUG_USARTx);
}

