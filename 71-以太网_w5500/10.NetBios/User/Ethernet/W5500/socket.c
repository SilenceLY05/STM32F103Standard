#include "socket.h"
#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "stdio.h"

/* �ر�Socket�˿� ����s:��ʾҪ�رյ�Socket�˿ں� */
void Close(SOCKET s)
{
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_CLOSE);
	
	/* �ȴ������ ʶ�������Ĵ������Զ�����Ϊ0x00 */
	while(IINCHIP_READ(Sn_CR(s)));
	
	
	/* ���Sn_IR�Ĵ��� */
	IINCHIP_WRITE(Sn_IR(s),0xFF);
}



/* ���ض�ģʽ��ʼ��ͨ���������ö˿� s:socket�˿ںţ�protocol:socket��Э��,port:Ҫ�󶨵Ķ˿ڣ�flag:����MR�Ĵ�����һЩλ ����1�ɹ�0ʧ�� */
uint8 socket(SOCKET s,uint8 protocol,uint16 port,uint8 flag)
{
	uint8 ret;
	uint16 local_port = 0;
	
	if(((protocol & 0x0F) == Sn_MR_TCP) || ((protocol & 0x0F) == Sn_MR_UDP) || ((protocol & 0x0F) == Sn_MR_IPRAW) ||
		 ((protocol & 0x0F) == Sn_MR_MACRAW) ||((protocol & 0x0F) == Sn_MR_PPPOE) )
	{
		/* �رյ�ǰsocket */
		Close(s);
		
		/* ����socket��ģʽ�Ĵ��� */
		IINCHIP_WRITE(Sn_MR(s),protocol | flag);
		if(port != 0)
		{
			IINCHIP_WRITE(Sn_PORT0(s),(uint8)((port&0xFF00)>>8));
			IINCHIP_WRITE(Sn_PORT1(s),(uint8)(port&0x00FF));
		}
		else
		{
			/* ���������Դ�˿�������local_port */
			local_port++;
			IINCHIP_WRITE(Sn_PORT0(s),(uint8)((local_port&0xFF00)>>8));
			IINCHIP_WRITE(Sn_PORT1(s),(uint8)(local_port&0x00FF));
		}
		/* ����Sn_MR��Э������ʼ���ʹ�Socket */
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_OPEN);
		
		/* �ȴ������ ʶ�������Ĵ������Զ�����Ϊ0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		ret = 1;
		
	}
	else
	{
		ret = 0;
	}
	return ret;
}


/* �ڱ��������½���ͨ������ �ȴ��Է������� s:Socket��� ����1�ɹ� 0ʧ�� */
uint8 Listen(SOCKET s)
{
	uint8 ret;
	
	/* Sn_CR�Ĵ����е�LISTENλֻ��TCPģʽ����Ч */
	if(IINCHIP_READ(Sn_SR(s)) == Sn_SR_SOCK_INIT)
	{
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_LISTEN);
		
		/* �ȴ������ ʶ�������Ĵ������Զ�����Ϊ0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}	


/* ��ActiveģʽΪͨ���������ӣ�������һֱ�ȴ�ֱ�����ӽ��� s:Socket��ţ�addr:Ҫ���ӵķ�����IP��ַ port:Ҫ���ӵķ�����IP�˿� */
uint8 Connect(SOCKET s,uint8 *addr,uint16 port)
{
	uint8 ret;
	
	/* �жϵ�ַ�Ƿ�Ϊȫ0xFF����0x00���˿��Ƿ�Ϊ0x00 */
	if(((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
		 ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) || (port==0x00))
	{
		ret = 0;
	}
	else
	{
		ret = 1;
		
		IINCHIP_WRITE(Sn_DIPR0(s),addr[0]);
		IINCHIP_WRITE(Sn_DIPR1(s),addr[1]);
		IINCHIP_WRITE(Sn_DIPR2(s),addr[2]);
		IINCHIP_WRITE(Sn_DIPR3(s),addr[3]);
		IINCHIP_WRITE(Sn_DPORT0(s),((port&0xFF00)>>8));
		IINCHIP_WRITE(Sn_DPORT1(s),(port&0x00FF));
		
		/* �ȴ������ ʶ�������Ĵ������Զ�����Ϊ0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		/* ��Socket n�������������ᴦ��SOCK_SYNSENT��ʱ״̬ ��������������� SOCK_ESTABLISHED ����ᳬʱ�ж� */
		while(IINCHIP_READ(Sn_SR(s)) != Sn_SR_SOCK_SYNSENT)
		{
			if(IINCHIP_READ(Sn_SR(s)) == Sn_SR_SOCK_ESTABLISHED)
			{
				break;
			}
			
			if(getSn_IR(s) & Sn_IR_TIMEOUT)
			{
				/* �����ʱ�ж� */
				IINCHIP_WRITE(Sn_IR(s),Sn_IR_TIMEOUT);
				ret = 0;
				break;
			}
		}
	}
	return ret;
}

/* �Ͽ���Socker �˿ڵ����� s:Socket��� */
void Disconnect(SOCKET s)
{
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_DISCON);
	
	while(IINCHIP_READ(Sn_CR(s)));
}


/* ��TCP��ʽ�������� s:Socket��� buf:Ҫ���͵����ݻ����� len�����ݳ��� ����1��ʾ���ͳɹ� 0��ʾʧ�� */
uint16 Send(SOCKET s,const uint8 *buff,uint16 len)
{
	uint8 status = 0;
	uint16 ret = 0;        //ʵ�ʷ��͵����ݴ�С
	uint16 freesize = 0;  //���õķ��ͻ�������С
	
	/* ������ݳ���û�г����˿ڵ����ݻ����� */
	if(len>getIINCHIP_TxMAX(s))
		ret = getIINCHIP_TxMAX(s);
	else
		ret = len;
	
	/* ����Ƿ����㹻�Ŀ��з��ͻ����� */
	do
	{
		freesize = getSn_TX_FSR(s);
		
		/* ��ȡSn_SR�Ĵ�����״̬ */
		status = IINCHIP_READ(Sn_SR(s));
		
		/* ���û�����Ӳ���û���յ�����������ʧ�� */
		if((status != Sn_SR_SOCK_ESTABLISHED) && (status != Sn_SR_SOCK_CLOSE_WAIT))
		{
			ret = 0;
			break;
		}
	}while(freesize <len);
	
	/* �������ݲ�����Ƿ������ */
	send_data_processing(s,(uint8 *)buff,len);
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);
	
	while(IINCHIP_READ(Sn_CR(s)));
	
	/* ��ȡIR�Ĵ������SEND_OKλ�Ƿ���Ч */
	while((IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK)
	{
		/* ����Ƿ������� */
		status = IINCHIP_READ(Sn_SR(s));
		if((status != Sn_SR_SOCK_ESTABLISHED) && (status != Sn_SR_SOCK_CLOSE_WAIT ))
		{
			printf("SEND_OK problem\r\n!!!");
			Close(s);
			return 0;
		}
		
	}
	IINCHIP_WRITE(Sn_IR(s),Sn_IR_SEND_OK);
	
#ifdef __DEF_IINCHIP_INT__
	putISR(s,getISR(s)&(~Sn_IR_SEND_OK));
#else
	IINCHIP_WRITE(Sn_IR(s),Sn_IR_SEND_OK);
#endif
	return ret;
}

/* ������TCPģʽ�������� �ȴ�Ӧ�ó�����Ҫ���յ����� s:Socket��� buf:Ҫ���յ����ݻ����� len�����ݳ��� �������ݳ��� 0��ʾʧ�� */
uint16 Receive(SOCKET s,uint8 *buff,uint16 len)
{
	uint16 ret = 0;
	
	if(len>0)
	{
		recv_data_processing(s,buff,len);
		
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);
		
		while(IINCHIP_READ(Sn_CR(s)));
		
		ret = len;
		
	}
	return ret;
}

/* ���ͷ�TCPģʽ������ s:Socket��� buf:Ҫ���͵����� len�����͵����ݳ��� addr:Ŀ��ĵ�ַ port:Ҫ���͵�IP�˿� ����ʵ�ʷ��͵����ݳ��� 0��ʾʧ�� */
uint16 SendTo(SOCKET s,const uint8 *buff,uint16 len,uint8 *addr,uint16 port)
{
	uint16 ret=0;

   if (len > getIINCHIP_TxMAX(s)) 
   ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

   if( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) || ((port == 0x00)) )//||(ret == 0) )
   {
      /* added return value */
      ret = 0;
   }
   else
   {
      IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
      IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
      IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
      IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
      IINCHIP_WRITE( Sn_DPORT0(s),(uint8)((port & 0xff00) >> 8));
      IINCHIP_WRITE( Sn_DPORT1(s),(uint8)(port & 0x00ff));
      // copy data
      send_data_processing(s, (uint8 *)buff, ret);
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);
      /* wait to process the command... */
      while( IINCHIP_READ( Sn_CR(s) ) )
	  ;
      /* ------- */
     while( (IINCHIP_READ( Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
     {
      if (IINCHIP_READ( Sn_IR(s) ) & Sn_IR_TIMEOUT)
      {
            /* clear interrupt */
      IINCHIP_WRITE( Sn_IR(s) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
      return 0;
      }
     }
      IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
   }
   return ret;
}


/* �������շ�TCPģʽ�����ݣ�����UDP��IP_RAW,MAC_RAWģʽ��������ͷ s:Socket��� buf:Ҫ���յ����ݻ����� len���������յ����ݳ��� addr:�洢ԴIP��ַ port:�洢ԴIP�˿� ���سɹ����յ����ݴ�С 0��ʾʧ�� */
uint16 RecvFrom(SOCKET s,uint8*buff,uint16 len,uint8 *addr,uint16 *port)
{
	uint8 head[8];
	uint16 data_len = 0;
	uint16 ptr = 0;
	uint32 addrbsb=0;
	
	if(len>0)
	{
		ptr = IINCHIP_READ(Sn_RX_RD0(s));
		ptr += (ptr<<8) + IINCHIP_READ(Sn_RX_RD1(s));
		
		addrbsb = (uint32)(ptr<<8) + (s<<5) +0x18;
		
		switch(IINCHIP_READ(Sn_MR(s)) & 0x07)
		{
			case Sn_MR_UDP:
				WIZ_ReadBuff(addrbsb,head,0x08);
				ptr += 8;
			
				addr[0] = head[0];
				addr[1] = head[1];
				addr[2] = head[2];
				addr[3] = head[3];
				*port   = head[4];
				*port   = (*port << 8) + head[5];
				data_len = head[6];
				data_len = (data_len << 8) + head[7];
			
				addrbsb = (uint32)(ptr<<8) + (s<<5) +0x18;
				WIZ_ReadBuff(addrbsb,buff,data_len);
				ptr += data_len;
			
				IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr&0xFF00)>>8));
				IINCHIP_WRITE(Sn_RX_RD1(s),(uint8)(ptr&0x00FF));
			break;
			
			case Sn_MR_IPRAW:
				WIZ_ReadBuff(addrbsb,head,0x06);
				ptr += 6;
			
				addr[0] = head[0];
				addr[1] = head[1];
				addr[2] = head[2];
				addr[3] = head[3];
				data_len = head[4];
				data_len = (data_len << 8) + head[5];
			
				addrbsb = (uint32)(ptr<<8) + (s<<5) +0x18;
				WIZ_ReadBuff(addrbsb,buff,data_len);
				ptr += data_len;
			
				IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr&0xFF00)>>8));
				IINCHIP_WRITE(Sn_RX_RD1(s),(uint8)(ptr&0x00FF));
			break;
			
			case Sn_MR_MACRAW:
				WIZ_ReadBuff(addrbsb,head,0x02);
				ptr += 2;
			
				data_len = head[0];
				data_len = (data_len << 8) + head[1] - 2;
			
				if(data_len > 1514)
				{
					printf("data_len over 1514\r\n");
					while(1);
				}
			
				addrbsb = (uint32)(ptr<<8) + (s<<5) +0x18;
				WIZ_ReadBuff(addrbsb,buff,data_len);
				ptr += data_len;
			
				IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr&0xFF00)>>8));
				IINCHIP_WRITE(Sn_RX_RD1(s),(uint8)(ptr&0x00FF));
			break;
				
			default:
				break;
		}
		
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);
		
		while(IINCHIP_READ(Sn_CR(s)));
	}
	return data_len;
}


#ifdef __MACRAW__

/* ��SOCKET 0��MACRAWģʽ */
void MACRAW_Open(void)
{
	uint8 sock_num = 0;
	uint16 dummyPort = 0;
	uint8 mFlag = 0;
	sock_num = 0;
	Close(sock_num);
	socket(sock_num,Sn_MR_MACRAW,dummyPort,mFlag);
}


/* ��MACRAWģʽ�������� buff:Ҫ���͵����ݻ����� len:���ݳ��� */
uint16 MACRAW_Send(const uint8*buff,uint16 len)
{
	uint16 ret = 0;
	uint8 sock_num;
	sock_num;
	
	if(len>getIINCHIP_TxMAX(sock_num))
		ret = getIINCHIP_TxMAX(sock_num);
	else
		ret = len;
	
	send_data_processing(sock_num,(uint8 *)buff,len);
	
	IINCHIP_WRITE(Sn_CR(sock_num),Sn_CR_SEND);
	while(IINCHIP_READ(Sn_CR(sock_num)));
	while((IINCHIP_READ(Sn_IR(sock_num)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK);
	IINCHIP_WRITE(Sn_IR(sock_num),Sn_IR_SEND_OK);
	
	return ret;
}


/* ��MACRAWģʽ�������� buff:Ҫ���յ����ݻ��棬len:���ݳ��� */
uint16 MACRAW_Recv(uint8 *buff,uint16 len)
{
	uint8 sock_num;
	uint16 data_len;
	uint16 dummyPort = 0;
	uint16 ptr = 0;
	uint8 mFlag = 0;
	sock_num = 0;
	
	if(len > 0)
	{
		data_len = 0;
		
		ptr = IINCHIP_READ(Sn_RX_RD0(sock_num));
		ptr = (uint16)((ptr &0xFF00)<<8) + IINCHIP_READ(Sn_RX_RD1(sock_num));
		
		data_len = IINCHIP_READ_RXBUFF(0,ptr);
		ptr++;
		
		data_len = ((data_len << 8) + IINCHIP_READ_RXBUFF(0,ptr) ) -2;
		ptr++;
		
		if(len >1514)
		{
			printf("data_len over 1514 \r\n");
			printf("\r\n ptr:%X,dat_len:%X \r\n",ptr,data_len);
			
			Close(sock_num);
			socket(sock_num,Sn_MR_MACRAW,dummyPort,mFlag);
			
			return 0;
		}
		
		IINCHIP_READ_RXBUFF_BURST(sock_num,ptr,data_len,(uint8 *)(buff));
		ptr += data_len;
		
		IINCHIP_WRITE(Sn_RX_RD0(sock_num),(uint8)((ptr & 0xFF00)>>8));
		IINCHIP_WRITE(Sn_RX_RD1(sock_num),(uint8)(ptr & 0x00FF));
		IINCHIP_WRITE(Sn_CR(sock_num),Sn_CR_RECV);
		while(IINCHIP_READ(Sn_CR(sock_num)));
	}
	return data_len;
}





#endif



