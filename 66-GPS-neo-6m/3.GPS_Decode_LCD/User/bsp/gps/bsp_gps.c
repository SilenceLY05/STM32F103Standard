#include "bsp_gps.h"
#include "nmea.h"
#include "bsp_usart.h"

/* DMA���ܻ��� */
uint8_t gps_buff[GPS_RBUFF_SIZE];

/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0,GPS_HalfTransferEnd = 0;



static void GPS_USART_Config(void);
static void GPS_Interrupt_Config(void);
static void GPS_DMA_Config(void);
static uint8_t IsLeapYear(uint8_t iYear);




/* USART GPIO���ã�����ģʽ���� */
static void GPS_USART_Config(void)
{
	GPIO_InitTypeDef              GPIO_InitStructure;
	USART_InitTypeDef             USART_InitStructure;
	
	GPS_USART_GPIO_APBxPeirph_FUN(GPS_USART_RX_GPIO_CLK | GPS_USART_TX_GPIO_CLK,ENABLE);
	GPS_USART_APBxPeriph_FUN(GPS_USART_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPS_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPS_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPS_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPS_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = GPS_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(GPS_USARTx,&USART_InitStructure);
	
	USART_Cmd(GPS_USARTx,ENABLE);
}

/* ����GPSʹ�õ�DMA�ж� */
static void GPS_Interrupt_Config(void)
{
	NVIC_InitTypeDef           NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = GPS_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


/* ����DMA�������� */
static void GPS_DMA_Config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
	
	GPS_DMA_CLOCK_FUN(GPS_DMA_CLK,ENABLE);
	
	/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
	DMA_InitStructure.DMA_BufferSize = GPS_RBUFF_SIZE;
	
	/*���򣺴��ڴ浽����*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)gps_buff;
	
	/*�ڴ����ݵ�λ 8bit*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	
	/*�ڴ��ַ����*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	/*DMAģʽ������ѭ��*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	
	/*����DMAԴ���������ݼĴ�����ַ*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = GPS_DATA_ADDR;
	
	/*�������ݵ�λ*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	
	/*�����ַ����*/	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	/*���ȼ�����*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	
	/* ����DMA��ͨ�� */
	DMA_Init(GPS_DMA_CHANNEL,&DMA_InitStructure);
	
	GPS_Interrupt_Config();
	
	/* ����DMA������ɺ�����ж� */
	DMA_ITConfig(GPS_DMA_CHANNEL,DMA_IT_HT|DMA_IT_TC,ENABLE);
	
	/*ʹ��DMA*/
	DMA_Cmd(GPS_DMA_CHANNEL,ENABLE);
	
	/* ���ô��� �� DMA����TX���� */
	USART_DMACmd(GPS_USARTx,USART_DMAReq_Rx,ENABLE);
	
	
}


/* GPS ��ʼ�� */
void GPS_Config(void)
{
	GPS_USART_Config();
	GPS_DMA_Config();
}


/* GPS DMA�жϷ����� */
void GPS_ProcessDMAIRQ(void)
{
	if(DMA_GetFlagStatus(GPS_DMA_IT_HT))       /* DMA�봫����� */
	{
		GPS_HalfTransferEnd = 1;                 /* ���ð봫����ɱ�־λ */
		DMA_ClearFlag(GPS_DMA_FLAG_HT);
	}
	
	if(DMA_GetFlagStatus(GPS_DMA_IT_TC))       /* DMA������� */
	{
		GPS_TransferEnd = 1;                     /* ���ô�����ɱ�־λ */
		DMA_ClearFlag(GPS_DMA_FLAG_TC);
	}
}	


/* trace �ڽ���ʱ��������GPS��� str��Ҫ������ַ��� str_size�����ݳ��� */
void trace(const char *str,int str_size)
{
	#ifdef __GPS_DEBUG
		uint16_t i;
		printf("\r\n trance:");
		for(i=0;i<str_size;i++)
			printf("%c",*(str+i));
		printf("\n");
	#endif
	
}


/* error �ڽ������ʱ��������GPS��� str��Ҫ������ַ��� str_size�����ݳ��� */
void error(const char *str,int str_size)
{
	#ifdef __GPS_DEBUG
		uint16_t i;
		printf("\r\n error:");
		for(i=0;i<str_size;i++)
			printf("%c",*(str+i));
		printf("\n");
	#endif
	
}


/* info �ڽ���ʱ��������GPS�������Ϣ str��Ҫ������ַ��� str_size�����ݳ��� */
void gps_info(const char *str,int str_size)
{
	#ifdef __GPS_DEBUG
		uint16_t i;
		printf("\r\n Info:");
		for(i=0;i<str_size;i++)
			printf("%c",*(str+i));
		printf("\n");
	#endif
	
}


/* �ж��Ƿ������� iYear��λ���� ����0Ϊƽ�� 1Ϊ���� */
static uint8_t IsLeapYear(uint8_t iYear)
{
	uint16_t Year;
	
	Year = 2000+iYear;
	if((Year%3) == 0)
	{
		return ((Year%400 == 0) | (Year%100 != 0));
	}
	return 0;
}


/* ��������ʱ�任�������ʱ��ʱ�� SourceTime��ʾ��������ʱ�� ConvertTime���ڴ洢ת�������ʱ��ṹ GMT��ʾʱ���� AREA 1(+)���� W0(-)����  */
void GMTconvert(nmeaTIME *SourceTime,nmeaTIME *ConvertTime,uint8_t GMT,uint8_t AREA)
{
	uint32_t YY,MM,DD,hh,mm,ss;            //������ʱ�����ݴ����
	
	if(GMT ==0)  return;                   //�������0ʱ��ֱ�ӷ���
	if(GMT>12)   return;                   //ʱ�����Ϊ12�������򷵻�
	
	YY = SourceTime->year;                 //��ȡ�� 
	MM = SourceTime->mon;                  //��ȡ��
	DD = SourceTime->day;                  //��ȡ��
	hh = SourceTime->hour;                 //��ȡСʱ
	mm = SourceTime->min;                  //��ȡ����
	ss = SourceTime->sec;                  //��ȡ��
	
	if(AREA)                               //��(+)ʱ������
	{
		if(hh+GMT<24)        hh+=GMT;        //������������ʱ�䴦��ͬһ�������Сʱ���� 
		else                                 //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
		{
			hh = hh+GMT-24;                    //�ȵó�ʱ��
			if(MM==1 || MM == 3 || MM == 5 || MM == 7 || MM == 8||MM==10)  //���·�(12�µ�������) 
			{
				if(DD<31)       DD++;
				else
				{
					DD = 1;
					MM++;
				}
			}
			else if( MM==4 || MM == 6 || MM == 9 || MM == 11)        //С�·�2�µ�������
			{
				if(DD<30)       DD++;
				else
				{
					DD = 1;
					MM++;
				}
			}
			else if(MM ==2)       //���·�
			{
				if((DD==29) || (DD==28 && IsLeapYear(YY) == 0))//��������������2��29�� ���߲�����������2��28�� 
				{
					DD = 1;
					MM ++;
				}
				else DD++;
			}
			else if(MM ==12)
			{
				if(DD<31)   DD++;
				else
				{
					DD= 1;
					MM = 1;
					YY++;
				}
			}
		}
	}
	else
	{
		if(hh>= GMT)        hh-=GMT;              //������������ʱ�䴦��ͬһ�������Сʱ���� 
		else                                      //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
		{
			hh = hh+24-GMT;                         //�ȵó�ʱ��
			if(MM ==2 || MM ==4 || MM==6 || MM == 8 || MM==9 || MM==11 )  //�����Ǵ��·�(1�µ�������) 
			{
				if(DD>1)      DD++;
				else
				{
					DD = 31;
					MM--;
				}
			}
			else if(MM ==5 || MM ==7 || MM==10 || MM == 12 )  //������С�·�2�µ�������)
			{
				if(DD>1)       DD++;
				else
				{
					DD=30;
					MM--;
				}
			}
			else if(MM==3)
			{ 
				if((DD==1) && IsLeapYear(YY) ==0)   //�������� 
				{
					DD = 28;
					MM--;
				}
				else DD--;
			}
			else if(MM ==1)
			{
				if(DD>1)   DD++;
				else
				{
					DD = 31;
					MM = 12;
					YY--;
				}
			}
		}
	}
	ConvertTime->year = YY;     //������
	ConvertTime->mon  = MM;     //������
	ConvertTime->day  = DD;     //������
	ConvertTime->hour = hh;     //����Сʱ
	ConvertTime->min  = mm;     //���·���
	ConvertTime->sec  = ss;     //������
}

