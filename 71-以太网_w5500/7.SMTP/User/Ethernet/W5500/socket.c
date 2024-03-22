#include "socket.h"
#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "stdio.h"

/* 关闭Socket端口 输入s:表示要关闭的Socket端口号 */
void Close(SOCKET s)
{
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_CLOSE);
	
	/* 等待命令处理 识别命令后寄存器会自动清零为0x00 */
	while(IINCHIP_READ(Sn_CR(s)));
	
	
	/* 清除Sn_IR寄存器 */
	IINCHIP_WRITE(Sn_IR(s),0xFF);
}



/* 以特定模式初始化通道，并设置端口 s:socket端口号，protocol:socket的协议,port:要绑定的端口，flag:设置MR寄存器的一些位 返回1成功0失败 */
uint8 socket(SOCKET s,uint8 protocol,uint16 port,uint8 flag)
{
	uint8 ret;
	uint16 local_port = 0;
	
	if(((protocol & 0x0F) == Sn_MR_TCP) || ((protocol & 0x0F) == Sn_MR_UDP) || ((protocol & 0x0F) == Sn_MR_IPRAW) ||
		 ((protocol & 0x0F) == Sn_MR_MACRAW) ||((protocol & 0x0F) == Sn_MR_PPPOE) )
	{
		/* 关闭当前socket */
		Close(s);
		
		/* 设置socket的模式寄存器 */
		IINCHIP_WRITE(Sn_MR(s),protocol | flag);
		if(port != 0)
		{
			IINCHIP_WRITE(Sn_PORT0(s),(uint8)((port&0xFF00)>>8));
			IINCHIP_WRITE(Sn_PORT1(s),(uint8)(port&0x00FF));
		}
		else
		{
			/* 如果不设置源端口则设置local_port */
			local_port++;
			IINCHIP_WRITE(Sn_PORT0(s),(uint8)((local_port&0xFF00)>>8));
			IINCHIP_WRITE(Sn_PORT1(s),(uint8)(local_port&0x00FF));
		}
		/* 按照Sn_MR的协议来初始化和打开Socket */
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_OPEN);
		
		/* 等待命令处理 识别命令后寄存器会自动清零为0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		ret = 1;
		
	}
	else
	{
		ret = 0;
	}
	return ret;
}


/* 在被动连接下建立通道连接 等待对方的请求 s:Socket编号 返回1成功 0失败 */
uint8 Listen(SOCKET s)
{
	uint8 ret;
	
	/* Sn_CR寄存器中的LISTEN位只在TCP模式下生效 */
	if(IINCHIP_READ(Sn_SR(s)) == Sn_SR_SOCK_INIT)
	{
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_LISTEN);
		
		/* 等待命令处理 识别命令后寄存器会自动清零为0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}	


/* 以Active模式为通道建立连接，函数会一直等待直到连接建立 s:Socket编号，addr:要连接的服务器IP地址 port:要连接的服务器IP端口 */
uint8 Connect(SOCKET s,uint8 *addr,uint16 port)
{
	uint8 ret;
	
	/* 判断地址是否为全0xFF或者0x00，端口是否为0x00 */
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
		
		/* 等待命令处理 识别命令后寄存器会自动清零为0x00 */
		while(IINCHIP_READ(Sn_CR(s)));
		
		/* 当Socket n发送连接请求后会处于SOCK_SYNSENT临时状态 若接收请求则会变成 SOCK_ESTABLISHED 否则会超时中断 */
		while(IINCHIP_READ(Sn_SR(s)) != Sn_SR_SOCK_SYNSENT)
		{
			if(IINCHIP_READ(Sn_SR(s)) == Sn_SR_SOCK_ESTABLISHED)
			{
				break;
			}
			
			if(getSn_IR(s) & Sn_IR_TIMEOUT)
			{
				/* 清除超时中断 */
				IINCHIP_WRITE(Sn_IR(s),Sn_IR_TIMEOUT);
				ret = 0;
				break;
			}
		}
	}
	return ret;
}

/* 断开与Socker 端口的连接 s:Socket编号 */
void Disconnect(SOCKET s)
{
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_DISCON);
	
	while(IINCHIP_READ(Sn_CR(s)));
}


/* 以TCP方式发送数据 s:Socket编号 buf:要发送的数据缓冲区 len：数据长度 返回1表示发送成功 0表示失败 */
uint16 Send(SOCKET s,const uint8 *buff,uint16 len)
{
	uint8 status = 0;
	uint16 ret = 0;        //实际发送的数据大小
	uint16 freesize = 0;  //可用的发送缓冲区大小
	
	/* 检查数据长度没有超过端口的数据缓冲区 */
	if(len>getIINCHIP_TxMAX(s))
		ret = getIINCHIP_TxMAX(s);
	else
		ret = len;
	
	/* 检查是否有足够的空闲发送缓冲区 */
	do
	{
		freesize = getSn_TX_FSR(s);
		
		/* 读取Sn_SR寄存器的状态 */
		status = IINCHIP_READ(Sn_SR(s));
		
		/* 如果没有连接并且没有收到连接请求则失败 */
		if((status != Sn_SR_SOCK_ESTABLISHED) && (status != Sn_SR_SOCK_CLOSE_WAIT))
		{
			ret = 0;
			break;
		}
	}while(freesize <len);
	
	/* 发送数据并检查是否发送完成 */
	send_data_processing(s,(uint8 *)buff,len);
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);
	
	while(IINCHIP_READ(Sn_CR(s)));
	
	/* 读取IR寄存器检查SEND_OK位是否生效 */
	while((IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK)
	{
		/* 检查是否建立连接 */
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

/* 用于以TCP模式接受数据 等待应用程序想要接收的数据 s:Socket编号 buf:要接收的数据缓冲区 len：数据长度 返回数据长度 0表示失败 */
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

/* 发送非TCP模式的数据 s:Socket编号 buf:要发送的数据 len：发送的数据长度 addr:目标的地址 port:要发送的IP端口 返回实际发送的数据长度 0表示失败 */
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


/* 用来接收非TCP模式的数据，接收UDP，IP_RAW,MAC_RAW模式，并处理报头 s:Socket编号 buf:要接收的数据缓冲区 len：期望接收的数据长度 addr:存储源IP地址 port:存储源IP端口 返回成功接收的数据大小 0表示失败 */
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

/* 打开SOCKET 0的MACRAW模式 */
void MACRAW_Open(void)
{
	uint8 sock_num = 0;
	uint16 dummyPort = 0;
	uint8 mFlag = 0;
	sock_num = 0;
	Close(sock_num);
	socket(sock_num,Sn_MR_MACRAW,dummyPort,mFlag);
}


/* 在MACRAW模式发送数据 buff:要发送的数据缓冲区 len:数据长度 */
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


/* 在MACRAW模式接收数据 buff:要接收的数据缓存，len:数据长度 */
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



