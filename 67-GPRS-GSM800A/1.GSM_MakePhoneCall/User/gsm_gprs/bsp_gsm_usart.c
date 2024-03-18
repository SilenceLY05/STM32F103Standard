#include "bsp_gsm_usart.h"
#include <stdarg.h>



static char *                 itoa                                ( int value, char * string, int radix );
static void GSM_NVIC_Config(void);

/* 配置USART接收中断 */
static void GSM_NVIC_Config(void)
{
	NVIC_InitTypeDef			   NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	NVIC_InitStructure.NVIC_IRQChannel = GSM_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}


/* GSM USART GPIO配置 */
void GSM_USART_Config(void)
{
	GPIO_InitTypeDef               GPIO_InitStructure;
	USART_InitTypeDef              USART_InitStructure;
	
	GSM_USART_GPIO_APBxClockCmd(GSM_USART_GPIO_CLK,ENABLE);
	GSM_USART_APBxClockCmd(GSM_USART_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GSM_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GSM_TX_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GSM_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GSM_RX_GPIO_PORT,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = GSM_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(GSM_USARTx,&USART_InitStructure);
	
	/* 配置中断优先级 */
	GSM_NVIC_Config();
	
	/* 使能串口2中断优先级 */
	USART_ITConfig(GSM_USARTx,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(GSM_USARTx,ENABLE);
}




/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */


#if 1
//中断缓存串口数据
#define USART_BUFF_SIZE            512
volatile     uint16_t usart_p = 0;
uint8_t      usart_buff[USART_BUFF_SIZE];

void bsp_GSM_USART_IRQHandler(void)
{
	if(usart_p<USART_BUFF_SIZE)
	{
		if(USART_GetITStatus(GSM_USARTx,USART_IT_RXNE) != RESET)
		{
			usart_buff[usart_p] = USART_ReceiveData(GSM_USARTx);
			usart_p++;
		}
	}
}

/* 获取接收到的数据和长度 */
char *get_rebuff(uint16_t *len)
{
	*len = usart_p;
	return (char *)&usart_buff;
}

void clean_rebuff(void)
{
	uint16_t i = USART_BUFF_SIZE;
	usart_p = 0;
	while(i)
		usart_buff[--i] = 0;
}

#endif



/*
 * 函数名：GSM_USARTx_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即GSM_USARTx
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用GSM_USARTx_printf( GSM_USARTx, "\r\n this is a demo \r\n" );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %d \r\n", i );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %s \r\n", j );
 */
void GSM_USARTx_printf (char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     // 判断是否到达字符串结束符
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
				USART_SendData(GSM_USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //换行符
				USART_SendData(GSM_USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}			 
		}
		
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(GSM_USARTx,*s);
					while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//十进制
				d = va_arg(ap, int);
				
				itoa(d, buf, 10);
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(GSM_USARTx,*s);
					while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;
				
				default:
				Data++;
				
				break;
				
			}		 
		}
		
		else USART_SendData(GSM_USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( GSM_USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}

