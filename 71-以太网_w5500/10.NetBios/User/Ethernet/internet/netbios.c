#include <string.h>
#include <stdio.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "netbios.h"

char NetBIOS_RX_BUFF[NETBIOS_MSG_MAX_LEN];
char NetBIOS_TX_BUFF[NETBIOS_MSG_MAX_LEN];


/* NetBIOS解码 输入参数：name_enc解码前的NetBIOS名字 name_dec解码后的NetBIOS名字 name_dec_len解码后名称字符数组的长度 */
static int NetBIOS_NameDecoding(char *name_enc,char *name_dec,int name_dec_len)
{
	char *pname;
	char cname;
	char cnbname;
	int index = 0;
	
	/* 启动NetBIOS解码 */
	pname = name_enc;
	
	for(;;)
	{
		/* 第一个级别编码的名称中的每两个字符变成解码后的一个字符 */
		cname = *pname;
		if(cname == '\0')   //没有元素了
			break;
		if(cname == '.')    //后面是作用于ID
 			break;       
		if(cname < 'A' || cname > 'Z')     //不符合协议
			return -1;
		
		cname -= 'A';    //编码时会将8位分为两个4位 每个4位再加上‘A’
		cnbname = cname<<4;
		pname++;
		
		
		cname = *pname;
		if(cname == '\0' || cname == '.')    //名字里没有更多的字符了——但我们正处在一对字符的中间。不合法的
			return  -1;
		
		if(cname < 'A'||cname > 'Z')
			return -1;
		
		cname -= 'A';
		cnbname |= cname;
		pname++;
		
		
		/* 是否有空间存储 */
		if(index < NETBIOS_NAME_LEN)
		{
			name_dec[index++] = (cnbname != ' '?cnbname:'\0');   //cnbname不为' '就用cnbname的值，否则就结束
		}
			
	}
	return 0;
}


/* 执行NetBIOS解码 */
void NetBIOS_Test(void)
{
	unsigned char state;
	unsigned int len;
	state = getSn_SR(NETBIOS_SOCK);
	switch(state)
	{
		case Sn_SR_SOCK_UDP:                 //工作在UDP模式下
			if((len = getSn_RX_FSR(NETBIOS_SOCK))>0)
			{
				unsigned char rem_ip_addr[4];           //记录IP地址
				uint16 rem_udp_port;                    //记录端口
				char netbios_name[NETBIOS_NAME_LEN +1]; 
				NETBIOS_HDR* netbios_hdr;
				NETBIOS_NAME_HDR* netbios_name_hdr;
				
				/* 接收到的数据大小 */
				len = RecvFrom(NETBIOS_SOCK,(unsigned char*)&NetBIOS_RX_BUFF,len,rem_ip_addr,&rem_udp_port);
				printf("rem_ip_addr = %d.%d.%d.%d",rem_ip_addr[0],rem_ip_addr[1],rem_ip_addr[2],rem_ip_addr[3]);
				netbios_hdr = (NETBIOS_HDR*)NetBIOS_RX_BUFF;
				netbios_name_hdr = (NETBIOS_NAME_HDR*)netbios_hdr+1;
				/* 如果数据包时NetBIOS查询包 */
				if(((netbios_hdr->flags & ntohs(NETB_HFLAG_OPCODE)) == ntohs(NETB_HFLAG_OPCODE_NAME_QUERY)) &&
						((netbios_hdr->flags & ntohs(NETB_HFLAG_REPLYCODE)) == 0) && (netbios_hdr->questions == ntohs(1)))
				{
					printf("netbios name query question\r\n");
					
					/* 对NetBIOS包进行解析 */
					NetBIOS_NameDecoding((char*)(netbios_name_hdr->encname),netbios_name,sizeof(netbios_name));
					printf("netbios_name = %s\r\n",netbios_name);
					
					/* 如果是针对本机的NetBIOS查询 */
					if(strcmp(netbios_name,NETBIOS_W5500_NAME) == 0)
					{
						uint8 ip_addr[4];
						
						NETBIOS_RESP *resp = (NETBIOS_RESP*)NetBIOS_TX_BUFF;
						
						/* 处理NetBIOS响应包的header */
						resp->resp_hdr .trans_id = netbios_hdr->trans_id;
						resp->resp_hdr.flags = ntohs(NETB_HFLAG_RESPONSE | NETB_HFLAG_OPCODE_NAME_QUERY | NETB_HFLAG_AUTHORATIVE | NETB_HFLAG_RECURS_DESIRED);
						resp->resp_hdr.questions = 0;
						resp->resp_hdr.answerRRs = htons(1);
						resp->resp_hdr.additionalRRs = 0;
						resp->resp_hdr.authorityRRs = 0;
						
						/* 处理NetBIOS响应包的header数据 */
						memcpy(resp->resp_name.encname,netbios_name_hdr->encname,sizeof(netbios_name_hdr->encname));
						resp->resp_name.nametype = netbios_name_hdr->nametype;
						resp->resp_name.type = netbios_name_hdr->type;
						resp->resp_name.cls = netbios_name_hdr->cls;
						resp->resp_name.ttl = htonl(NETBIOS_NAME_TTL);
						resp->resp_name.datalen = htonl(sizeof(resp->resp_name.flags)+sizeof(resp->resp_name.addr));
						resp->resp_name.flags = htons(NETB_NFLAG_NODETYPE_BNODE);
						getSIPR(ip_addr);
						
						memcpy(resp->resp_name.addr,ip_addr,4);
						
						/* 发送NetBIOS响应 */
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





