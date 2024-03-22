#include "ping.h"


#define PING_DEBUG
PINGMSG PingRequest;
PINGMSG PingReply;

static uint16 RandomID = 0x1234;
static uint16 RandomSeqNum = 0x4321;
uint8 ping_reply_received = 0;
uint8 req = 0;
uint8 rep = 0;



/* ping��Ӧ���� ���룺s-socket addr-Ping��ַ */
uint8 Ping_Request(uint8 s,uint8 *addr)
{
	uint16 i;
	
	ping_reply_received = 0;              //ping�ظ���ʼ����־λ
	PingRequest.Type = PING_REQUEST;    
	PingRequest.Code = CODE_ZERO;
	PingRequest.ID = htons(RandomID++);   //����Ping��ӦIDΪ��������ͱ���
	PingRequest.SeqNum = htons(RandomSeqNum++);    //����Ping��Ӧ�����к�Ϊ������ͱ���
	
	for(i=0;i<BUFF_LEN;i++)
	{
		PingRequest.Data[i] = (i)%8;
	}
	PingRequest.CheckSum = 0;
	
	/* ������Ӧ���� */
	PingRequest.CheckSum = htons(checksun((uint8 *)&Ping_Request,sizeof(PingRequest)));
	
	/* ����PIng��Ӧ��Ŀ�ķ� */
	if(SendTo(s,(uint8 *)&PingRequest,sizeof(PingRequest),addr,3000) == 0)
	{
		printf("\r\n Fail to send ping-reply packet \r\n");
	}
	else
	{
		printf("����PING��%d.%d.%d.%d\r\n",addr[0],addr[1],addr[2],addr[3]);
	}
	return 0;
	

}


/* ����PING�ظ� ���룺s-socket addr-Ping��ַ rlen-�������յ����ݳ��� */
uint8 Ping_Reply(uint8 s,uint8 *addr,uint16 rlen)
{
	uint16 temp_checksum;
	uint16 len;
	uint16 i;
	uint8 data_buff[128];
	
	uint16 port = 3000;
	PINGMSG PingReply;
	len = RecvFrom(s,(uint8 *)data_buff,rlen,addr,&port);
	if(data_buff[0] == PING_REPLY)
	{
		PingReply.Type = data_buff[0];
		PingReply.Code = data_buff[1];
		PingReply.CheckSum = (data_buff[3]<<8) + data_buff[2];
		PingReply.ID = (data_buff[5]<<8)+data_buff[4];
		PingReply.SeqNum = (data_buff[7]<<8) + data_buff[6];
		
		for(i = 0;i<len-8;i++)
		{
			PingReply.Data[i] = data_buff[8+i];
		}
		temp_checksum = ~checksun(data_buff,len);
		if(temp_checksum != 0xFFFF)
			printf("temp_checksum = %x\r\n",temp_checksum);
		else
		{
			printf("����%d.%d.%d.%d�Ļظ���ID = %x �ֽ� = %d \r\n",addr[0],addr[1],addr[2],addr[3],htons(PingReply.ID),rlen+6);
			ping_reply_received = 1;              //���˳�ping�ظ�ѭ��ʱ������ping�ظ���־λ1 
		}
		
	}
	else if(data_buff[0] == PING_REQUEST)
	{
		PingReply.Type = data_buff[0];
		PingReply.Code = data_buff[1];
		PingReply.CheckSum = (data_buff[3]<<8) + data_buff[2];
		PingReply.ID = (data_buff[5]<<8)+data_buff[4];
		PingReply.SeqNum = (data_buff[7]<<8) + data_buff[6];
		
		for(i = 0;i<len-8;i++)
		{
			PingReply.Data[i] = data_buff[8+i];
		}
		temp_checksum = PingReply.CheckSum;
		PingReply.CheckSum = 0;
		if(temp_checksum != PingReply.CheckSum)
			printf("CheckSum is in correct %x should be %x \r\n",temp_checksum,htons(PingReply.CheckSum));
		else
		{
			printf("����%d.%d.%d.%d�Ļظ���ID = %x SeqSum = %x �ֽ� = %d \r\n",addr[0],addr[1],addr[2],addr[3],PingReply.ID,PingReply.SeqNum,rlen+6);
			ping_reply_received = 1;              //���˳�ping�ظ�ѭ��ʱ������ping�ظ���־λ1 
		}
	}
	else
	{
		printf("Unknow msg\r\n");
	}
	return 0;
}



/* �Զ�PING����IP���� ���룺s-socket addr-Ping��ַ */
void Ping_Auto(uint8 s,uint8 *addr)
{
	uint8 i;
	int32_t len = 0;
	uint8 cnt = 0;
	
	for(i = 0; i<3;i++)
	{
		Delay_Ms(10);
		switch(getSn_SR(s))
		{
			case Sn_SR_SOCK_CLOSE:
				Close(s);
				IINCHIP_WRITE(Sn_PROTO(s),IPPROTO_ICMP);            //����ICMPЭ��
				if(socket(s,Sn_MR_IPRAW,3000,0) != 0)               //�ж� ip rawģʽsocket�Ƿ���
				{}
				while(getSn_SR(s) != Sn_SR_SOCK_IPRAW);
				Delay_Ms(1000);
				Delay_Ms(1000);
			break;
					
			case Sn_SR_SOCK_IPRAW:                                //ip rawģʽ
				Ping_Request(s,addr);
				req++;
				while(1)
				{
					if((len =getSn_RX_FSR(s))>0)
					{
						Ping_Reply(s,addr,len);
						Delay_Ms(50);
						rep++;
						break;
					}
					else if(cnt > 200)
					{
						printf("Request Time out \r\n");
						cnt = 0;
						break;
					}
					else
					{
						cnt++;
						Delay_Ms(1);
					}
				}
			break;
			default:
				break;
		}
		#ifdef PING_DEBUG
		if(rep != 0)
		{
			printf("Ping Request = %d,PING_Relpy = %d\r\n",req,rep);
			if(rep == req)
				printf("PING SUCCESS \r\n");
			else
				printf("REPLY_ERROR\r\n");
		}
		#endif
	}
}



/* �趨����PING����IP���� ���룺s-socket pCount-PING�Ĵ��� addr-Ping��ַ */
void Ping_Count(uint8 s,uint16 pCount,uint8 *addr)
{
	uint16 rlen,cnt,i;
	
	cnt = 0;
	
	for( i = 0; i< pCount;i++)
	{
		if( i!= 0)
		{
			printf("\r\n NO.%d \r\n",(i-1));
		}
		switch(getSn_SR(s))
		{
			case Sn_SR_SOCK_CLOSE:
				Close(s);
				IINCHIP_WRITE(Sn_PROTO(s),IPPROTO_ICMP);            //����ICMPЭ��
				if(socket(s,Sn_MR_IPRAW,3000,0) != 0)               //�ж� ip rawģʽsocket�Ƿ���
				{}
				while(getSn_SR(s) != Sn_SR_SOCK_IPRAW);
				Delay_Ms(1000);
				Delay_Ms(1000);
			break;
					
			case Sn_SR_SOCK_IPRAW:                                //ip rawģʽ
				Ping_Request(s,addr);
				req++;
				while(1)
				{
					if((rlen =getSn_RX_FSR(s))>0)
					{
						Ping_Reply(s,addr,rlen);
						rep++;
						if(ping_reply_received) break;
					}
					if(cnt > 100)
					{
						printf("Request Time out \r\n");
						cnt = 0;
						break;
					}
					else
					{
						cnt++;
						Delay_Ms(50);
					}
				}
			break;
			default:
				break;
		}
		
		if(req>=pCount)
		{
			#ifdef PING_DEBUG
			printf("Ping Request = %d,PING_Relpy = %d\r\n",req,rep);
			if(rep == req)
				printf("PING SUCCESS \r\n");
			else
				printf("REPLY_ERROR\r\n");
			#endif
		}
		
	}
}



/* ִ��PING���� */
void Ping_Test(void)
{
	printf("--------------------------PING_TEST_START------------------------\r\n");
	Delay_Ms(1000);
	Ping_Auto(SOCK_PING,remote_ip);
}







