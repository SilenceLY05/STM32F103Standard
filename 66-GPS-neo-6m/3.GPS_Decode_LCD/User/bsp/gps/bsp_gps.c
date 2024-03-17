#include "bsp_gps.h"
#include "nmea.h"
#include "bsp_usart.h"

/* DMA接受缓冲 */
uint8_t gps_buff[GPS_RBUFF_SIZE];

/* DMA传输结束标志 */
__IO uint8_t GPS_TransferEnd = 0,GPS_HalfTransferEnd = 0;



static void GPS_USART_Config(void);
static void GPS_Interrupt_Config(void);
static void GPS_DMA_Config(void);
static uint8_t IsLeapYear(uint8_t iYear);




/* USART GPIO配置，工作模式配置 */
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

/* 配置GPS使用的DMA中断 */
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


/* 配置DMA接受配置 */
static void GPS_DMA_Config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
	
	GPS_DMA_CLOCK_FUN(GPS_DMA_CLK,ENABLE);
	
	/*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
	DMA_InitStructure.DMA_BufferSize = GPS_RBUFF_SIZE;
	
	/*方向：从内存到外设*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	/*禁止内存到内存的传输	*/
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	/*内存地址(要传输的变量的指针)*/
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)gps_buff;
	
	/*内存数据单位 8bit*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	
	/*内存地址自增*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	/*DMA模式：不断循环*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	
	/*设置DMA源：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = GPS_DATA_ADDR;
	
	/*外设数据单位*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	
	/*外设地址不增*/	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	/*优先级：中*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	
	/* 配置DMA的通道 */
	DMA_Init(GPS_DMA_CHANNEL,&DMA_InitStructure);
	
	GPS_Interrupt_Config();
	
	/* 配置DMA发送完成后产生中断 */
	DMA_ITConfig(GPS_DMA_CHANNEL,DMA_IT_HT|DMA_IT_TC,ENABLE);
	
	/*使能DMA*/
	DMA_Cmd(GPS_DMA_CHANNEL,ENABLE);
	
	/* 配置串口 向 DMA发出TX请求 */
	USART_DMACmd(GPS_USARTx,USART_DMAReq_Rx,ENABLE);
	
	
}


/* GPS 初始化 */
void GPS_Config(void)
{
	GPS_USART_Config();
	GPS_DMA_Config();
}


/* GPS DMA中断服务函数 */
void GPS_ProcessDMAIRQ(void)
{
	if(DMA_GetFlagStatus(GPS_DMA_IT_HT))       /* DMA半传输完成 */
	{
		GPS_HalfTransferEnd = 1;                 /* 设置半传输完成标志位 */
		DMA_ClearFlag(GPS_DMA_FLAG_HT);
	}
	
	if(DMA_GetFlagStatus(GPS_DMA_IT_TC))       /* DMA传输完成 */
	{
		GPS_TransferEnd = 1;                     /* 设置传输完成标志位 */
		DMA_ClearFlag(GPS_DMA_FLAG_TC);
	}
}	


/* trace 在解码时输出捕获的GPS语句 str：要输出的字符串 str_size：数据长度 */
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


/* error 在解码出错时输出捕获的GPS语句 str：要输出的字符串 str_size：数据长度 */
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


/* info 在解码时输出捕获的GPS语语句信息 str：要输出的字符串 str_size：数据长度 */
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


/* 判断是否是闰年 iYear两位年数 返回0为平面 1为闰年 */
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


/* 格林尼治时间换算世界各时区时间 SourceTime表示格林尼治时间 ConvertTime用于存储转换结果的时间结构 GMT表示时区数 AREA 1(+)东区 W0(-)西区  */
void GMTconvert(nmeaTIME *SourceTime,nmeaTIME *ConvertTime,uint8_t GMT,uint8_t AREA)
{
	uint32_t YY,MM,DD,hh,mm,ss;            //年月日时分秒暂存变量
	
	if(GMT ==0)  return;                   //如果处于0时区直接返回
	if(GMT>12)   return;                   //时区最大为12，超过则返回
	
	YY = SourceTime->year;                 //获取年 
	MM = SourceTime->mon;                  //获取月
	DD = SourceTime->day;                  //获取日
	hh = SourceTime->hour;                 //获取小时
	mm = SourceTime->min;                  //获取分钟
	ss = SourceTime->sec;                  //获取秒
	
	if(AREA)                               //东(+)时区处理
	{
		if(hh+GMT<24)        hh+=GMT;        //如果与格林尼治时间处于同一天则仅加小时即可 
		else                                 //如果已经晚于格林尼治时间1天则进行日期处理 
		{
			hh = hh+GMT-24;                    //先得出时间
			if(MM==1 || MM == 3 || MM == 5 || MM == 7 || MM == 8||MM==10)  //大月份(12月单独处理) 
			{
				if(DD<31)       DD++;
				else
				{
					DD = 1;
					MM++;
				}
			}
			else if( MM==4 || MM == 6 || MM == 9 || MM == 11)        //小月份2月单独处理
			{
				if(DD<30)       DD++;
				else
				{
					DD = 1;
					MM++;
				}
			}
			else if(MM ==2)       //二月份
			{
				if((DD==29) || (DD==28 && IsLeapYear(YY) == 0))//本来是闰年且是2月29日 或者不是闰年且是2月28日 
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
		if(hh>= GMT)        hh-=GMT;              //如果与格林尼治时间处于同一天则仅减小时即可 
		else                                      //如果已经早于格林尼治时间1天则进行日期处理 
		{
			hh = hh+24-GMT;                         //先得出时间
			if(MM ==2 || MM ==4 || MM==6 || MM == 8 || MM==9 || MM==11 )  //上月是大月份(1月单独处理) 
			{
				if(DD>1)      DD++;
				else
				{
					DD = 31;
					MM--;
				}
			}
			else if(MM ==5 || MM ==7 || MM==10 || MM == 12 )  //上月是小月份2月单独处理)
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
				if((DD==1) && IsLeapYear(YY) ==0)   //不是闰年 
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
	ConvertTime->year = YY;     //更新年
	ConvertTime->mon  = MM;     //更新月
	ConvertTime->day  = DD;     //更新日
	ConvertTime->hour = hh;     //更新小时
	ConvertTime->min  = mm;     //更新分钟
	ConvertTime->sec  = ss;     //更新秒
}

