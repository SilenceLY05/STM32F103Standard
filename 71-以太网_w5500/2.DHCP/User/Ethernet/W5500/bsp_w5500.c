#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
#ifdef __DEF_IINCHIP_PPP__
#include "md5.h"
#endif


uint16 SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};  //���ͻ��������ֵ
uint16 RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0};  //���ܻ��������ֵ
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};  //�����ͻ�������ʼ������Ϊ2�ֽ�
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};  //�����ջ�������ʼ������Ϊ2�ֽ�

void IINCHIP_INIT(void)
{
	setMR(MR_RST);
#ifdef __DEF_IINCHIP_DBG__
	printf("MR value is %02x\r\n",IINCHIP_READ_COMMON(MR));
#endif
}


/* ��ȡ���յĻ�������С s:socket��� �������õĽ��ջ�������С */
uint16 getIINCHIP_RxMAX(SOCKET s)
{
	return RSIZE[s];
}

/* ��ȡ���͵Ļ�������С s:socket��� �������õķ��ͻ�������С */
uint16 getIINCHIP_TxMAX(SOCKET s)
{
	return SSIZE[s];
}


/* ��������IP��ַ addrָ��4�ֽ������ָ�룬��������IP��ַ */
void setGAR(uint8 *addr)
{
	WIZ_WriteBuff(GAR0,addr,4);
}


/* ��ȡ����IP��ַ addrָ��4�ֽ������ָ�룬��ȡ����IP��ַ */
void getGAR(uint8 *addr)
{
	WIZ_ReadBuff(GAR0,addr,4);
}


/* �������������ַ addrָ��4�ֽ������ָ�룬������������ */
void setSUBR(uint8 *addr)
{
	WIZ_WriteBuff(SUBR0,addr,4);
}

/* ��ȡ���������ַ addrָ��4�ֽ������ָ�룬��ȡ���������ַ */
void getSUBR(uint8 *addr)
{
	WIZ_ReadBuff(SUBR0,addr,4);
}


/* ����ԴMAC��ַ addrָ��6�ֽ������ָ�룬����ԴMAC��ַ */
void setSHAR(uint8 *addr)
{
	WIZ_WriteBuff(SHAR0,addr,6);
}

/* ��ȡԴMAC��ַ addrָ��6�ֽ������ָ�룬��ȡԴMAC��ַ */
void getSHAR(uint8 *addr)
{
	WIZ_ReadBuff(SHAR0,addr,6);
}


/* ����ԴIP��ַ addrָ��4�ֽ������ָ�룬������������ */
void setSIPR(uint8 *addr)
{
	WIZ_WriteBuff(SIPR0,addr,4);
}

/* ��ȡԴIP��ַ addrָ��4�ֽ������ָ�룬��ȡԴIP��ַ */
void getSIPR(uint8 *addr)
{
	WIZ_ReadBuff(SIPR0,addr,4);
}

/* ����ģʽ�Ĵ��� valд��ģʽ�Ĵ�����ֵ */
void setMR(uint8 val)
{
	IINCHIP_WRITE(MR,val);
}


/* ��ͨ�üĴ����л�ȡ�жϼĴ�����ֵ */
uint8 getIR(void)
{
	return IINCHIP_READ(IR);
}


/* �����ش�ʱ�� ����Է�û����Ӧ����Ӧ�ӳ٣������RTR���ý����ش� timeoutд���ش���ʱ�Ĵ�����ֵ */
void setRTR(uint16 timeout)
{
	IINCHIP_WRITE(RTR0,(uint8)((timeout&0xFF00)>>8));
	IINCHIP_WRITE(RTR0,(uint8)(timeout &0x00FF));
}


/* �����ش�����������Է�û����Ӧ����Ӧ�ӳ٣�����RTR��RCR�Ĵ������ü�¼ʱ�䣬��ᷢ����ʱ retry���Դ���*/
void setRCR(uint8 retry)
{
	IINCHIP_WRITE(WIZ_RCR,retry);
}


/* ����/�����жϣ����IMR�е��κ�λ��0���򾡹ܸ�λ������IR�Ĵ���Ҳû���ж��ź� maskҪ������ж�λ */
void clearIR(uint8 mask)
{
	IINCHIP_WRITE(IR,~mask | getIR());
}

/* ����TCP������ģʽ�µ�����Ķδ�С���ڱ���ģʽ���ɶԷ����� s:socket��� sn_mssr:���δ�С */
void setSn_MSS(SOCKET s,uint16 Sn_MSSR)
{
	IINCHIP_WRITE(Sn_MSSR0(s),(uint8)((Sn_MSSR & 0xFF00)>>8));
	IINCHIP_WRITE(Sn_MSSR1(s),(uint8)(Sn_MSSR & 0x00FF));
}

/* ����IP����ʱ��Ĵ��� s:socket��� ttl:����ʱ�� */
void setSn_TTL(SOCKET s,uint8 ttl)
{
	IINCHIP_WRITE(Sn_TTL(s),ttl);
}


/* ��ȡSocket���жϼĴ��� s:socket��� �����жϼĴ�����״̬ */
uint8 getSn_IR(SOCKET s)
{
	return IINCHIP_READ(Sn_IR(s));
}


/* дSocket���жϼĴ�������ж� s:socket��� valд���ֵ */
void setSn_IR(uint8 s,uint8 val)
{
	IINCHIP_WRITE(Sn_IR(s),val);
}

/* ��ȡsocket ��״̬ s:socket��� ���ؼĴ�����״̬ */
uint8 getSn_SR(SOCKET s)
{
	return IINCHIP_READ(Sn_SR(s));
}


/* ��ȡ���ͻ���Ĵ�����С s:socket��� ���ػ����С */
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


/* ��ȡ���ջ���Ĵ�����С s:socket��� ���ػ����С */
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


/* ��ȡTXдָ��Ĵ��������ڸ��ƻ������е����ݺ����TXдָ��Ĵ��� s:socket��� dataҪ���͵����� len���ݳ��� ���ؿ��з��ͻ����С */
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


/* ��ȡRXдָ��Ĵ������ӽ��ջ������������ݺ󣬸���RXдָ��Ĵ��� s:socket��� dataҪ���͵����� len���ݳ��� ���ؿ��з��ͻ����С */
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


/* ������ʹ�õ�ͨ�����÷��ͺͽ��ջ������Ĵ�С tx_size: tx buffer size to set=tx_size[s]*(1024) rx_size: rx buffer size to set=rx_size[s]*(1024) */
void socket_buf_init(uint8 *tx_size,uint8 *rx_size)
{
	int16 i;
	int16 ssum=0,rsum=0;
	
	for(i=0;i<MAX_BUFF_SIZE;i++)   //ͨ��ÿ��ͨ������Tx��Rx�ڴ�Ĵ�С������ͻ���ַ
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





