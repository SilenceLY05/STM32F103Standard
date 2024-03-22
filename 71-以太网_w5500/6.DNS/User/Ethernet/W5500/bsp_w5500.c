#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
#ifdef __DEF_IINCHIP_PPP__
#include "md5.h"
#endif


uint16 SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};  //发送缓冲区最大值
uint16 RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};  //接受缓冲区最大值
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};  //将发送缓冲区初始化分配为2字节
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};  //将接收缓冲区初始化分配为2字节

void IINCHIP_INIT(void)
{
	setMR(MR_RST);
#ifdef __DEF_IINCHIP_DBG__
	printf("MR value is %02x\r\n",IINCHIP_READ_COMMON(MR));
#endif
}


/* 获取接收的缓冲区大小 s:socket标号 返回设置的接收缓冲区大小 */
uint16 getIINCHIP_RxMAX(SOCKET s)
{
	return RSIZE[s];
}

/* 获取发送的缓冲区大小 s:socket标号 返回设置的发送缓冲区大小 */
uint16 getIINCHIP_TxMAX(SOCKET s)
{
	return SSIZE[s];
}


/* 设置网关IP地址 addr指向4字节数组的指针，设置网关IP地址 */
void setGAR(uint8 *addr)
{
	WIZ_WriteBuff(GAR0,addr,4);
}


/* 获取网关IP地址 addr指向4字节数组的指针，获取网关IP地址 */
void getGAR(uint8 *addr)
{
	WIZ_ReadBuff(GAR0,addr,4);
}


/* 设置子网掩码地址 addr指向4字节数组的指针，设置子网掩码 */
void setSUBR(uint8 *addr)
{
	WIZ_WriteBuff(SUBR0,addr,4);
}

/* 获取子网掩码地址 addr指向4字节数组的指针，获取子网掩码地址 */
void getSUBR(uint8 *addr)
{
	WIZ_ReadBuff(SUBR0,addr,4);
}


/* 设置源MAC地址 addr指向6字节数组的指针，设置源MAC地址 */
void setSHAR(uint8 *addr)
{
	WIZ_WriteBuff(SHAR0,addr,6);
}

/* 获取源MAC地址 addr指向6字节数组的指针，获取源MAC地址 */
void getSHAR(uint8 *addr)
{
	WIZ_ReadBuff(SHAR0,addr,6);
}


/* 设置源IP地址 addr指向4字节数组的指针，设置子网掩码 */
void setSIPR(uint8 *addr)
{
	WIZ_WriteBuff(SIPR0,addr,4);
}

/* 获取源IP地址 addr指向4字节数组的指针，获取源IP地址 */
void getSIPR(uint8 *addr)
{
	WIZ_ReadBuff(SIPR0,addr,4);
}

/* 设置模式寄存器 val写入模式寄存器的值 */
void setMR(uint8 val)
{
	IINCHIP_WRITE(MR,val);
}


/* 在通用寄存器中获取中断寄存器的值 */
uint8 getIR(void)
{
	return IINCHIP_READ(IR);
}


/* 设置重传时间 如果对方没有响应或响应延迟，则根据RTR设置进行重传 timeout写入重传超时寄存器的值 */
void setRTR(uint16 timeout)
{
	IINCHIP_WRITE(RTR0,(uint8)((timeout&0xFF00)>>8));
	IINCHIP_WRITE(RTR0,(uint8)(timeout &0x00FF));
}


/* 设置重传次数，如果对方没有响应或响应延迟，则按照RTR和RCR寄存器设置记录时间，则会发生超时 retry重试次数*/
void setRCR(uint8 retry)
{
	IINCHIP_WRITE(WIZ_RCR,retry);
}


/* 启用/禁用中断，如果IMR中的任何位置0，则尽管该位设置在IR寄存器也没有中断信号 mask要清除的中断位 */
void clearIR(uint8 mask)
{
	IINCHIP_WRITE(IR,~mask | getIR());
}

/* 设置TCP在主动模式下的最大报文段大小，在被动模式下由对方设置 s:socket编号 sn_mssr:最大段大小 */
void setSn_MSS(SOCKET s,uint16 Sn_MSSR)
{
	IINCHIP_WRITE(Sn_MSSR0(s),(uint8)((Sn_MSSR & 0xFF00)>>8));
	IINCHIP_WRITE(Sn_MSSR1(s),(uint8)(Sn_MSSR & 0x00FF));
}

/* 设置IP生存时间寄存器 s:socket编号 ttl:生存时间 */
void setSn_TTL(SOCKET s,uint8 ttl)
{
	IINCHIP_WRITE(Sn_TTL(s),ttl);
}


/* 读取Socket的中断寄存器 s:socket编号 返回中断寄存器的状态 */
uint8 getSn_IR(SOCKET s)
{
	return IINCHIP_READ(Sn_IR(s));
}


/* 写Socket的中断寄存器清除中断 s:socket编号 val写入的值 */
void setSn_IR(uint8 s,uint8 val)
{
	IINCHIP_WRITE(Sn_IR(s),val);
}

/* 获取socket 的状态 s:socket编号 返回寄存器的状态 */
uint8 getSn_SR(SOCKET s)
{
	return IINCHIP_READ(Sn_SR(s));
}


/* 获取发送缓存寄存器大小 s:socket编号 返回缓存大小 */
uint16 getSn_TX_FSR(SOCKET s)
{
	uint16 val,val1;
	
	do
	{
		val1= IINCHIP_READ(Sn_TX_FSR0(s));
		val1 = (val1<<8) + IINCHIP_READ(Sn_TX_FSR1(s));
		if(val1 !=0)
		{
			val= IINCHIP_READ(Sn_TX_FSR0(s));
			val = (val1<<8) + IINCHIP_READ(Sn_TX_FSR1(s));
		}
	}while(val != val1);
	return val;
}


/* 获取接收缓存寄存器大小 s:socket编号 返回缓存大小 */
uint16 getSn_RX_FSR(SOCKET s)
{
	uint16 val,val1;
	
	do
	{
		val1= IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1<<8) + IINCHIP_READ(Sn_RX_RSR1(s));
		if(val1 !=0)
		{
			val= IINCHIP_READ(Sn_RX_RSR0(s));
			val = (val1<<8) + IINCHIP_READ(Sn_RX_RSR1(s));
		}
	}while(val != val1);
	return val;
}


/* 读取TX写指针寄存器，并在复制缓冲区中的数据后更新TX写指针寄存器 s:socket编号 data要发送的数据 len数据长度 返回空闲发送缓存大小 */
void send_data_processing(SOCKET s,uint8 *data,uint16 len)
{
	uint16 ptr = 0;
	uint32 address = 0;
	if(len == 0)
	{
		printf("CH : %d Unexpected1 length 0\r\n",s);
		return;
	}
	
	ptr = IINCHIP_READ(Sn_TX_WR0(s));
	ptr = ((ptr & 0x00FF)<<8) + IINCHIP_READ(Sn_TX_WR1(1));
	
	address = (uint32)(ptr<<8) + (s<<5)+0x10;
	WIZ_WriteBuff(address,data,len);
	
	ptr += len;
	
	IINCHIP_WRITE(Sn_TX_WR0(s),(uint8)((ptr&0x00FF)>>8));
	IINCHIP_WRITE(Sn_TX_WR1(s),(uint8)(ptr & 0x00FF));
}


/* 读取RX写指针寄存器，从接收缓冲区复制数据后，更新RX写指针寄存器 s:socket编号 data要发送的数据 len数据长度 返回空闲发送缓存大小 */
void recv_data_processing(SOCKET s,uint8 *data,uint16 len)
{
	uint16 ptr = 0;
	uint32 address = 0;
	if(len == 0)
	{
		printf("CH : %d Unexpected2 length 0\r\n",s);
		return;
	}
	
	ptr = IINCHIP_READ(Sn_RX_RD0(s));
	ptr = ((ptr & 0x00FF)<<8) + IINCHIP_READ(Sn_RX_RD1(1));
	
	address = (uint32)(ptr<<8) + (s<<5)+0x10;
	WIZ_WriteBuff(address,data,len);
	
	ptr += len;
	
	IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr&0x00FF)>>8));
	IINCHIP_WRITE(Sn_RX_RD1(s),(uint8)(ptr & 0x00FF));
}


/* 根据所使用的通道设置发送和接收缓冲区的大小 tx_size: tx buffer size to set=tx_size[s]*(1024) rx_size: rx buffer size to set=rx_size[s]*(1024) */
void socket_buf_init(uint8 *tx_size,uint8 *rx_size)
{
	int16 i;
	int16 ssum=0,rsum=0;
	
	for(i=0;i<MAX_BUFF_SIZE;i++)   //通过每个通道设置Tx和Rx内存的大小，掩码和基地址
	{
		IINCHIP_WRITE(Sn_TXBUF_SIZE(i),tx_size[i]);
		IINCHIP_WRITE(Sn_RXBUF_SIZE(i),rx_size[i]);
		
#ifdef __DEF_IINCHIP_DBG__
		printf("tx_size[%d]:%d,Sn_TXBUF_SIZE = %d\r\n",i,tx_size[i],IINCHIP_READ(Sn_TXBUF_SIZE(i)));
		printf("rx_size[%d]:%d,Sn_RXBUF_SIZE = %d\r\n",i,rx_size[i],IINCHIP_READ(Sn_RXBUF_SIZE(i)));
#endif
		SSIZE[i] = (int16)(0);
		RSIZE[i] = (int16)(0);
		
		if(ssum<= 16384)
		{
			SSIZE[i] = (int16)tx_size[i]*(1024);
		}
		
		if(rsum<= 16384)
		{
			RSIZE[i] = (int16)rx_size[i]*(1024);
		}
		
		ssum += SSIZE[i];
		rsum += RSIZE[i];
	}
}





