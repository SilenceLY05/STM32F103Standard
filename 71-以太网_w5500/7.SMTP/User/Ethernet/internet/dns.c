#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "dns.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

uint8 domain_name[] = "www.baidu.com";
uint8 dns_get_ip[4];
uint16 MSG_ID = 0x1122;
uint8 BUFFPUB[1024];




/* ����DNS��ѯ���� op:�������� name:Ҫ��ѯ������ buff:���ڴ�Ź����õ�DNS������ len:�������ĳ��� ����ֵ��DNS��ѯ��Ϣ�ĳ��� */
int DNS_MakeQuery(uint16 op,uint8 *name,uint8 *buff,uint16 len)
{
	uint8 *cp;                 //������¼���ĳ���
	uint8 *cp1;                //������¼����������.֮��ĳ���
	uint8 *dname;              //������¼����������.֮�������
	uint16 p;                  //��¼����DNS�����ײ�����
	uint16 dlen;               //��¼��������
	
	cp = buff;
	
	MSG_ID++;                  //��ÿ�η��͵�����ID��ͬ
	
	*(uint16 *)&cp[0] = htons(MSG_ID);         //����ID
	p = (op<<11) | 0x0100;                     //��ѯ����+�ݹ��ѯ
	*(uint16 *)&cp[2] = htons(p);              //��־�ֶ�
	*(uint16 *)&cp[4] = htons(1);              //��������ֶ�
	*(uint16 *)&cp[6] = htons(0);              //�ش���Դ����
	*(uint16 *)&cp[8] = htons(0);              //Ȩ������������
	*(uint16 *)&cp[10] = htons(0);             //������Դ����
	
	/* ʹָ������ͷ����Ϣ */
	cp += sizeof(uint16)*6;
	dname = name;
	
	dlen = strlen((char*)dname);
	
	for(;;)
	{
		/* strchr��һ���ַ����в��Ҹ����ַ��ĵ�һ��ƥ��֮�� */
		cp1 = (unsigned char*)strchr((char *)dname,'.');
		
		/* ������һ��.ǰ���ַ��ĳ��� */
		if(cp1)
			len = cp1 - dname;
		else
			len = dlen;
		/* ��lenд��cpָ���λ�ò�ǰ��һλ */
		*cp++ = len;
		
		if(len == 0)
			break;
		
		/* ��ʾ��dname��ʼ��ǰlen���ֽڸ��Ƶ�cp�� */
		strncpy((char *)cp,(char *)dname,len);
		cp+=len;
		/* ���cp1Ϊ0 */
		if(!cp1)
		{
			*cp++ = 0;
			break;
			
		}
		dname += len+1;
		dlen -= len +1;
	}
	/* ������ǰ�Ѿ�����������������. */
	*(uint16 *)&cp[0] = htons(0x0001);
	/* ������dlen��ֵ */
	*(uint16 *)&cp[2] = htons(0x0001);
	
	cp += sizeof(uint16)*2;
	
	return ((int)((uint32)cp - (uint32)buff));
	
}



/* ����DNS�����е�ѹ�����ѹ�������ַ��� msg:ָ��DNS���ĵ���ʼλ��ָ��,compressed:ָ����Ҫ�����������ַ���ָ�룬len:���Խ�������󳤶� */
int Parse_Name(uint8 *msg,uint8 *compressed,uint16 len)
{
	uint16 slen;                 //��ǰ�γ���
	uint8 *cp;                   //ָ���ڱ����е�λ��
	int16 clen = 0;              //ѹ�����Ƶ��ܳ���
	int16 indirect = 0;          //���ָ����ʾ
	int16 nseg = 0;              //�μ���
	int8 name[MAX_DNS_BUFF_SIZE];
	int8 *buff;
	
	
	buff = name;
	cp = compressed;
	
	for(;;)
	{
		/* ��ȡ��ǰ�εĳ��� */
		slen = *cp++;
		
		/* ���û���������ָ�� ����ѹ�������ȼ��� */
		if(!indirect)
			clen++;
		
		/* ��⵱ǰ���Ƿ���һ��ѹ��ָ�� ǰ��λΪ11 */
		if((slen & 0xC0) == 0xC0)
		{
			if(!indirect)
				clen++;
			indirect = 1;
			
			/* ���¶�λcp�Ը��������� */
			cp = &msg[((slen&0x3F)<<8) + *cp];
			slen = *cp++;   //��ȡ�µ��ֶ�
		}
		
		/* ����γ�Ϊ0����ʾ�������� */
		if(slen == 0)
			break;
		
		/* ����ʣ��ɽ����ĳ��� */
		len -= slen +1;
		
		if(len <= 0)
			return -1;
		
		/* ���û������������ã�����ѹ�������ȼ��� */
		if(!indirect)
			clen += slen;
		
		/* ��ȡ���洢��ǰ�� */
		while(slen-- != 0)
			*buff++ = (int8)*cp++;
		
		*buff++ = '.';
		nseg++;
		
	}
	if(nseg == 0)
	{
		*buff++ = '.';
		len--;
	}
	*buff++ = '\0';
	len--;
	
	return clen;
	
}


/* �����ظ���Ϣ����ѯ��¼ msg:ָ��DNS���ĵ���ʼλ��ָ�� cp: ָ����Ҫ�����������ַ���ָ�� ������һ��ָ��λ��*/
uint8 *DNS_Question(uint8 *msg,uint8 *cp)
{
	int16 len;
	
	len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
	
	if(len == -1)
		return 0;
	
	cp += len;
	
	cp += 2;  //����TYPE����
	
	cp += 2;  //����class����
	
	return cp;
}


/* �����ظ���Ϣ��Ӧ���¼ msg:ָ��DNS���ĵ���ʼλ��ָ�� cp: ָ����Ҫ�����������ַ���ָ�� ������һ��ָ��λ��*/
uint8 *DNS_Answer(uint8 *msg,uint8 *cp)
{
	int16 len,type;
	
	len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
	
	if(len == -1)
		return 0;
	
	cp += len;
	type = ntohs(*((uint16 *)&cp[0]));
	cp += 2;  //����TYPE����
	cp += 2;  //����class����
	cp += 4;  //����ttl����
	cp += 2;  //����len����
	
	switch(type)
	{
		case TYPE_A:
			dns_get_ip[0] = *cp++;
			dns_get_ip[1] = *cp++;
			dns_get_ip[2] = *cp++;
			dns_get_ip[3] = *cp++;
			break;
		
		case TYPE_CNAME:
		case TYPE_MB:
		case TYPE_MG:
		case TYPE_MR:
		case TYPE_NS:
		case TYPE_PTR:
			/* ��Щ���Ͷ�����һ��������� ת��ΪASCII��ʽ */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			
			cp += len;
			break;
			
		case TYPE_HINFO:
			len = *cp++;
			cp += len;
			len = *cp++;
			cp += len;
			break;
		
		case TYPE_MX:
			cp += 2;
			/* ��ȡ���������� */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			cp += len;
			break;
			
		case TYPE_SOA:
			/* ��ȡ���Ʒ��������� */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			cp+=len;
			
			/* ��ȡ���������� */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			
			cp += len;
			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			break;
			
		case TYPE_TXT:
			break;
		
		default:
			break;
		
	}
	return cp;
}


/* ��������DNS�������Ļظ����� DNSHMG:ָ��DNS��Ϣͷ��ָ�� buff:���յĻظ���Ϣ  */
uint8 parseMSG(struct DNSHMG *pdhdr,uint8 *pbuff)
{
	uint16 tmp;
	uint16 i;
	uint8 *msg;
	uint8 *cp;
	
	msg = pbuff;
	memset(pdhdr,0,sizeof(pdhdr));
	
	/* ��ȡID */
	pdhdr->id = ntohs(*((uint16 *)&msg[0]));
	
	/* ��ȡFLAG���� */
	tmp = ntohs(*((uint16 *)&msg[2]));
	if(tmp & 0x8000)
		pdhdr->qr = 1;
	
	pdhdr->opcode = (tmp >> 11) & 0xF;
	
	if(tmp & 0x0400) 
		pdhdr->aa =1;
	
	if(tmp & 0x0200) 
		pdhdr->tc =1;
	
	if(tmp & 0x0100) 
		pdhdr->rd =1;
	
	if(tmp & 0x0080) 
		pdhdr->ra =1;
	
	pdhdr->rcode = tmp & 0xF;
	pdhdr->qcount = ntohs(*((uint16 *)&msg[4]));
	pdhdr->acount = ntohs(*((uint16 *)&msg[6]));
	pdhdr->nscount = ntohs(*((uint16 *)&msg[8]));
	pdhdr->arcount = ntohs(*((uint16 *)&msg[10]));
	
	/* �����ɱ䳤�� */
	cp = &msg[12];
	
	/* ���ⲿ�� */
	for( i = 0; i<pdhdr->qcount;i++)
	{
		cp = DNS_Question(msg,cp);
	}
	
	/* ��Ӧ���� */
	for(i = 0 ; i<pdhdr->acount; i++)
	{
		cp = DNS_Answer(msg,cp);
	}
	
	/* Ȩ�����Ʒ��������� */
	for(i = 0 ; i < pdhdr->nscount ;i++)
	{
		;
	}
	
	/* ����ļ�¼��Ŀ */
	for(i = 0 ;i<pdhdr->arcount;i++)
	{
		;
	}
	
	if(pdhdr->rcode == 0)
		return 1;
	else
		return 0;
	
	
}


/* ��ѯDNS������Ϣ����������DNS�������Ļظ� s:DNS������socket name:Ҫ��������Ϣ ����1�ɹ� */
uint8 DNS_Query(uint8 s,uint8*name)
{
	static uint32 dns_wait_time = 0;
	struct DNSHMG dhp;                           //����һ���ṹ������������ͷ����Ϣ
	uint8 ip[4];
	uint16 len,port;
	
	/* ��ȡsocket��Ϣ */
	switch(getSn_SR(s))
	{
		case Sn_SR_SOCK_CLOSE:
			dns_wait_time = 0;
			/* ��W5500 ��socket 3000�˿ڣ�������Ϊģʽ */
			socket(s,Sn_MR_UDP,3000,0);
			break;
		
		/* socket�Ѿ��� */
		case Sn_SR_SOCK_UDP:
			len = DNS_MakeQuery(0,name,BUFFPUB,MAX_DNS_BUFF_SIZE);  //����DNS�����Ĳ�����BUFFPUB
			SendTo(s,BUFFPUB,len,EXTERN_DNS_SERVERIP,IPPORT_DOMAIN);   //����DNS�����ĸ�DNS������
		
			if((len = getSn_RX_FSR(s)) > 0)
			{
				if(len > MAX_DNS_BUFF_SIZE)
					len = MAX_DNS_BUFF_SIZE;
				
				len = RecvFrom(s,BUFFPUB,len,ip,&port);       //����UDP��������ݲ�����BUFFPUB
				if(parseMSG(&dhp,BUFFPUB))                    //����DNS��Ӧ��Ϣ
				{
					Close(s);
					return DNS_RET_SUCCESS;                     //�ر�socket������DNS�����ɹ�������Ϣ
				}
				else
					dns_wait_time = DNS_RESPONSE_TIMEOUT;       //�ȴ���Ӧʱ������Ϊ��ʱ
			}
			else
			{
				Delay_Ms(1000);                               //û���յ�DNS��������UDP�ظ�������̫Ƶ������ʱ1s
				dns_wait_time++;                              //DNS��Ӧʱ���1
			}
			
			if(dns_wait_time >= DNS_RESPONSE_TIMEOUT)       //���DNS�ȴ�ʱ�䳬��3s
			{
				Close(s);
				return DNS_RET_FAIL;
			}
			break;
	}
	return DNS_RET_PROGRESS;
}


void DNS_Test(void)
{
	uint8 dns_retry_cnt = 0;
	uint8 dns_ok = 0;
	if((dns_ok == 1) || (dns_retry_cnt > DNS_RETRY))
	{
		return;
	}
	
	/* �ж�DNS��������IP��ַ�Ƿ�ƥ�� memcmp���ֽڱȽ�*/
	else if(memcmp(ConfigMsg.dns,"\x00\x00\x00\x00",4))
	{
		/* ����DNS�����ĺͽ���DNS��Ӧ���� */
		switch(DNS_Query(SOCK_DNS,domain_name))
		{
			/* DNS���������ɹ� */
			case DNS_RET_SUCCESS:
				dns_ok = 1;                              //DNS ���б�־λ��1
				memcpy(ConfigMsg.rip,dns_get_ip,4);      //�ѽ�������IP��ַ���Ƹ�ConfigMsg.rip
				dns_retry_cnt = 0;                       //DNS�����Ĵ�����0
				printf("Get [%s]' s IP address [%d.%d.%d.%d] from %d.%d.%d.%d",domain_name,
								ConfigMsg.rip[0],ConfigMsg.rip[1],ConfigMsg.rip[2],ConfigMsg.rip[3],
								ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
				break;
			
			case DNS_RET_FAIL:
				dns_ok = 0;
				dns_retry_cnt++;
				printf("Fail! Please check your network configuration or DNS server \r\n");
				break;
			
			default:
				break;
			
		}
	}
	else
		printf("Invalid DNS server [%d.%d.%d.%d]\r\n",ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
}






