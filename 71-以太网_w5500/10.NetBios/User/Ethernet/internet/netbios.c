#include <string.h>
#include <stdio.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "netbios.h"

char NetBIOS_RX_BUFF[NETBIOS_MSG_MAX_LEN];
char NetBIOS_TX_BUFF[NETBIOS_MSG_MAX_LEN];


/* NetBIOS���� ���������name_enc����ǰ��NetBIOS���� name_dec������NetBIOS���� name_dec_len����������ַ�����ĳ��� */
static int NetBIOS_NameDecoding(char *name_enc,char *name_dec,int name_dec_len)
{
	char *pname;
	char cname;
	char cnbname;
	int index = 0;
	
	/* ����NetBIOS���� */
	pname = name_enc;
	
	for(;;)
	{
		/* ��һ���������������е�ÿ�����ַ���ɽ�����һ���ַ� */
		cname = *pname;
		if(cname == '\0')   //û��Ԫ����
			break;
		if(cname == '.')    //������������ID
 			break;       
		if(cname < 'A' || cname > 'Z')     //������Э��
			return -1;
		
		cname -= 'A';    //����ʱ�Ὣ8λ��Ϊ����4λ ÿ��4λ�ټ��ϡ�A��
		cnbname = cname<<4;
		pname++;
		
		
		cname = *pname;
		if(cname == '\0' || cname == '.')    //������û�и�����ַ��ˡ���������������һ���ַ����м䡣���Ϸ���
			return  -1;
		
		if(cname < 'A'||cname > 'Z')
			return -1;
		
		cname -= 'A';
		cnbname |= cname;
		pname++;
		
		
		/* �Ƿ��пռ�洢 */
		if(index < NETBIOS_NAME_LEN)
		{
			name_dec[index++] = (cnbname != ' '?cnbname:'\0');   //cnbname��Ϊ' '����cnbname��ֵ������ͽ���
		}
			
	}
	return 0;
}


/* ִ��NetBIOS���� */
void NetBIOS_Test(void)
{
	unsigned char state;
	unsigned int len;
	state = getSn_SR(NETBIOS_SOCK);
	switch(state)
	{
		case Sn_SR_SOCK_UDP:                 //������UDPģʽ��
			if((len = getSn_RX_FSR(NETBIOS_SOCK))>0)
			{
				unsigned char rem_ip_addr[4];           //��¼IP��ַ
				uint16 rem_udp_port;                    //��¼�˿�
				char netbios_name[NETBIOS_NAME_LEN +1]; 
				NETBIOS_HDR* netbios_hdr;
				NETBIOS_NAME_HDR* netbios_name_hdr;
				
				/* ���յ������ݴ�С */
				len = RecvFrom(NETBIOS_SOCK,(unsigned char*)&NetBIOS_RX_BUFF,len,rem_ip_addr,&rem_udp_port);
				printf("rem_ip_addr = %d.%d.%d.%d",rem_ip_addr[0],rem_ip_addr[1],rem_ip_addr[2],rem_ip_addr[3]);
				netbios_hdr = (NETBIOS_HDR*)NetBIOS_RX_BUFF;
				netbios_name_hdr = (NETBIOS_NAME_HDR*)netbios_hdr+1;
				/* ������ݰ�ʱNetBIOS��ѯ�� */
				if(((netbios_hdr->flags & ntohs(NETB_HFLAG_OPCODE)) == ntohs(NETB_HFLAG_OPCODE_NAME_QUERY)) &&
						((netbios_hdr->flags & ntohs(NETB_HFLAG_REPLYCODE)) == 0) && (netbios_hdr->questions == ntohs(1)))
				{
					printf("netbios name query question\r\n");
					
					/* ��NetBIOS�����н��� */
					NetBIOS_NameDecoding((char*)(netbios_name_hdr->encname),netbios_name,sizeof(netbios_name));
					printf("netbios_name = %s\r\n",netbios_name);
					
					/* �������Ա�����NetBIOS��ѯ */
					if(strcmp(netbios_name,NETBIOS_W5500_NAME) == 0)
					{
						uint8 ip_addr[4];
						
						NETBIOS_RESP *resp = (NETBIOS_RESP*)NetBIOS_TX_BUFF;
						
						/* ����NetBIOS��Ӧ����header */
						resp->resp_hdr .trans_id = netbios_hdr->trans_id;
						resp->resp_hdr.flags = ntohs(NETB_HFLAG_RESPONSE | NETB_HFLAG_OPCODE_NAME_QUERY | NETB_HFLAG_AUTHORATIVE | NETB_HFLAG_RECURS_DESIRED);
						resp->resp_hdr.questions = 0;
						resp->resp_hdr.answerRRs = htons(1);
						resp->resp_hdr.additionalRRs = 0;
						resp->resp_hdr.authorityRRs = 0;
						
						/* ����NetBIOS��Ӧ����header���� */
						memcpy(resp->resp_name.encname,netbios_name_hdr->encname,sizeof(netbios_name_hdr->encname));
						resp->resp_name.nametype = netbios_name_hdr->nametype;
						resp->resp_name.type = netbios_name_hdr->type;
						resp->resp_name.cls = netbios_name_hdr->cls;
						resp->resp_name.ttl = htonl(NETBIOS_NAME_TTL);
						resp->resp_name.datalen = htonl(sizeof(resp->resp_name.flags)+sizeof(resp->resp_name.addr));
						resp->resp_name.flags = htons(NETB_NFLAG_NODETYPE_BNODE);
						getSIPR(ip_addr);
						
						memcpy(resp->resp_name.addr,ip_addr,4);
						
						/* ����NetBIOS��Ӧ */
						SendTo(NETBIOS_SOCK,(unsigned char*)resp,sizeof(NETBIOS_RESP),rem_ip_addr,rem_udp_port);
						printf("send response\r\n");
					}
				}
			}
			
			break;
			
		case Sn_SR_SOCK_CLOSE:
			Close(NETBIOS_SOCK);
			socket(NETBIOS_SOCK,Sn_MR_UDP,NETBIOS_PORT,0);
		break;
		
		default:
			break;
	}
}





