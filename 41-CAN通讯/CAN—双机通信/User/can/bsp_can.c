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
	
	/* CAN寄存器初始化 */
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN单元初始化 */
	CAN_InitStructure.CAN_ABOM = ENABLE;                     //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM = ENABLE;                     //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART = DISABLE;                    //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;            //正常工作模式
	CAN_InitStructure.CAN_RFLM = DISABLE;                    //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文 
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;                 //BTR-SJW 重新同步跳跃宽度 1个时间单元
	CAN_InitStructure.CAN_TTCM = DISABLE;                    //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_TXFP = DISABLE;
	
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;                 //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;                 //BTR-TS1 时间段2 占用了3个时间单元
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB1 = 36 MHz) */
	CAN_InitStructure.CAN_Prescaler = 4;                     //BTR-BRP 波特率分频器  定义了时间单元的时间长度 36/(1+5+3)/4=1 Mbps
	CAN_Init(CANx,&CAN_InitStructure);
	
	
}

static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef      CAN_FilerInitStructure;
	
	/* CAN筛选器初始化 */
	CAN_FilerInitStructure.CAN_FilterNumber = 0;                              //筛选器组0
	CAN_FilerInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;            //工作在掩码模式
	CAN_FilerInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;           //筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
	
	CAN_FilerInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA ) &0xFFFF0000 ) >> 16;  //要筛选的ID高位 
	CAN_FilerInitStructure.CAN_FilterIdLow = (((uint32_t)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF; //要筛选的ID低位
	CAN_FilerInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;   //筛选器高16位每位必须匹配
	CAN_FilerInitStructure.CAN_FilterMaskIdLow = 0xFFFF;    //筛选器低16位每位必须匹配
	CAN_FilerInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;          //筛选器被关联到FIFO0
	CAN_FilerInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilerInitStructure);
	
	/*CAN通信中断使能*/
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);

}

void CAN_Config(void)
{
	CAN_GPIO_Config();
	NVIC_Config();
	CAN_Mode_Config();
	CAN_Filter_Config();
	
}

/* 初始化RxMessage数据结构体 */
void Init_RxMessage(CanRxMsg *RxMessage)
{
	uint8_t ubCounter = 0;
	
	/* 把接收结构体清零 */
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

/* 初始化TxMessage数据结构体 */
void CAN_SetMsg(CanTxMsg *TxMessage)
{
	uint8_t ubCounter = 0;
	
	/* 设置发送数据结构体 */
	TxMessage->DLC = 8;            //数据长度为8字节
	TxMessage->ExtId = 0x1314;      //使用的扩展ID
	TxMessage->IDE = CAN_ID_EXT;   //扩展模式
	TxMessage->RTR=CAN_RTR_DATA;				 //发送的是数据
	
	for(ubCounter = 0 ; ubCounter <8 ; ubCounter ++)
	{
		TxMessage->Data[ubCounter] = ubCounter;
	}
}


