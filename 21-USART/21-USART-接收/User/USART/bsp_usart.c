#include "bsp_usart.h"


/* ����Ƕ�������жϿ�����NVIC */
//static void NVIC_Configuration(void)
//{
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	
//	/* Ƕ�������жϿ�������ѡ�� */
//	/* ��ʾNVIC_PriorityGroupConfig()����������ֻ��Ҫ����һ�����������ȼ����� */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	/* ����USARTΪ�ж�Դ */
//	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
//	/* �������ȼ� */
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	
//	/* �����ȼ� */
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	/* ʹ���ж� */
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	
//	/* ��ʼ������NVIC */
//	NVIC_Init(&NVIC_InitStructure);
//	
//}

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
	
//	//�����ж����ȼ�����
//	NVIC_Configuration();
//	
//	//ʹ�ܴ��ڽ����ж�
//	USART_ITConfig(DEBUG_USARTx,USART_IT_RXNE,ENABLE);
	
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

