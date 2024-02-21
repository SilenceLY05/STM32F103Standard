#include "bsp_can.h"


static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = CAN_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_TX_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CAN_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_RX_GPIO_PORT,&GPIO_InitStructure);
	
}

static void NVIC_Config(void)
{
	NVIC_InitTypeDef     NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef     CAN_InitStructure;
	
	RCC_APB2PeriphClockCmd(CAN_CLK,ENABLE);
	
	/* CAN�Ĵ�����ʼ�� */
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN��Ԫ��ʼ�� */
	CAN_InitStructure.CAN_ABOM = ENABLE;                     //MCR-ABOM  �Զ����߹��� 
	CAN_InitStructure.CAN_AWUM = ENABLE;                     //MCR-AWUM  ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART = DISABLE;                    //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;            //��������ģʽ
	CAN_InitStructure.CAN_RFLM = DISABLE;                    //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б��� 
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;                 //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_TTCM = DISABLE;                    //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	CAN_InitStructure.CAN_TXFP = DISABLE;
	
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;                 //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;                 //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB1 = 36 MHz) */
	CAN_InitStructure.CAN_Prescaler = 4;                     //BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 36/(1+5+3)/4=1 Mbps
	CAN_Init(CANx,&CAN_InitStructure);
	
	
}

static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef      CAN_FilerInitStructure;
	
	/* CANɸѡ����ʼ�� */
	CAN_FilerInitStructure.CAN_FilterNumber = 0;                              //ɸѡ����0
	CAN_FilerInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;            //����������ģʽ
	CAN_FilerInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;           //ɸѡ��λ��Ϊ����32λ��
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */
	
	CAN_FilerInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA ) &0xFFFF0000 ) >> 16;  //Ҫɸѡ��ID��λ 
	CAN_FilerInitStructure.CAN_FilterIdLow = (((uint32_t)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF; //Ҫɸѡ��ID��λ
	CAN_FilerInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;   //ɸѡ����16λÿλ����ƥ��
	CAN_FilerInitStructure.CAN_FilterMaskIdLow = 0xFFFF;    //ɸѡ����16λÿλ����ƥ��
	CAN_FilerInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;          //ɸѡ����������FIFO0
	CAN_FilerInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilerInitStructure);
	
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);

}

void CAN_Config(void)
{
	CAN_GPIO_Config();
	NVIC_Config();
	CAN_Mode_Config();
	CAN_Filter_Config();
	
}

/* ��ʼ��RxMessage���ݽṹ�� */
void Init_RxMessage(CanRxMsg *RxMessage)
{
	uint8_t ubCounter = 0;
	
	/* �ѽ��սṹ������ */
	RxMessage->DLC = 0 ;
	RxMessage->ExtId = 0x00;
	RxMessage->FMI = 0;
	RxMessage->IDE = CAN_ID_STD;
	RxMessage->StdId = 0x00;
	for(ubCounter = 0 ; ubCounter <8 ; ubCounter ++)
	{
		RxMessage->Data[ubCounter] = 0x00;
	}
}

/* ��ʼ��TxMessage���ݽṹ�� */
void CAN_SetMsg(CanTxMsg *TxMessage)
{
	uint8_t ubCounter = 0;
	
	/* ���÷������ݽṹ�� */
	TxMessage->DLC = 8;            //���ݳ���Ϊ8�ֽ�
	TxMessage->ExtId = 0x1314;      //ʹ�õ���չID
	TxMessage->IDE = CAN_ID_EXT;   //��չģʽ
	TxMessage->RTR=CAN_RTR_DATA;				 //���͵�������
	
	for(ubCounter = 0 ; ubCounter <8 ; ubCounter ++)
	{
		TxMessage->Data[ubCounter] = ubCounter;
	}
}


