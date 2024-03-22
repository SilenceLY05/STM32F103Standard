#include <stdio.h>
#include <string.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "dhcp.h"
#include "utility.h"
#include "bsp_W5500_conf.h"

DHCP_Get DHCP_GET;


uint8 *SRC_MAC_ADDR   = EXTERN_DHCP_MAC;         /* ����MAC��ַ */
uint8 *GET_SN_MASK    = EXTERN_DHCP_SN;          /* ��DHCP��������ȡ������������ */
uint8 *GET_GW_IP      = EXTERN_DHCP_GW;          /* ��DHCP��������ȡ�������ص�ַ */
uint8 *GET_DNS_IP     = EXTERN_DHCP_DNS;         /* ��DHCP��������ȡ����DNS��������ַ */
uint8 *DHCP_HOST_NAME = EXTERN_DHCP_NAME;        /* ������ */
uint8 *GET_SIP        = EXTERN_DHCP_SIP;         /* ��DHCP��������ȡ���ı���IP */
uint8  DHCP_SIP[4]    = {0,};                    /* �ѷ��ֵ�DNS��������ַ */
uint8  DHCP_REAL_SIP[4] = {0,};                  /* ��DHCP�б���ѡ��ʹ�õ�DHCP������ */
uint8  OLD_SIP[4];                               /* �����ȡ����IP��ַ */

uint8  dhcp_state     = STATE_DHCP_READY;        /* DHCP�ͻ���״̬ */
uint8  dhcp_retry_count = 0;                     /* DHCP���Դ��� */
uint8  dhcp_timeout   = 0;                       /* DHCP��ʱ��־λ */
uint32 dhcp_lease_time;                          /* ��Լʱ�� */
uint32 next_dhcp_time = 0;                       /* DHCP��ʱʱ�� */
uint32 dhcp_tick_cnt  = 0;                       
uint8  DHCP_timer;

uint8  Conflict_flag = 0;
uint32 DHCP_XID      = DHCP_DEFAULT_XID;         /* DHCP����Ĭ��XIDȡֵ */
uint8  EXTERN_DHCPBUFF[1024];
RIP_MSG* pRIPMSG = (RIP_MSG*)EXTERN_DHCPBUFF;    /* DHCP��Ϣָ�� */


/* DHCP��ʱ��ʼ�� */
void DHCP_ResetTime(void)
{
	/* dhcp���м��� */
	dhcp_time = 0;
	dhcp_tick_cnt = 0;
	/* ��ʱʱ�� = ����ʱ��+ */
	next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
	/* ��ʼ�����Դ��� */
	dhcp_retry_count = 0;
}


/* ����DISCOVER��Ϣ��DHCP������ */
void DHCP_SendDiscover(void)
{
	/* ��ʼ��IP���� */
	uint8 ip[4] = {255,255,255,255};
	uint16 i = 0;
	/* ��ʼ���ͻ����������� */
	uint8 host_name[12];
	
	/* ���pRIPMSG�� sizeof(RIP_MSG)���ռ� */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	/* ѡ��Client����Server�������� */
	pRIPMSG->op = DHCP_BOOTREQUEST;
	/* ʹ��10M ��̫�� */
	pRIPMSG->htype = DHCP_HTYPE10MB;
	/* ʹ��10M ��̫�� */
	pRIPMSG->hlen = DHCP_HLEN;
	/* Client�豸Ϊ0 */
	pRIPMSG->hops = DHCP_HOPS;
	/* ��xidת��Ϊ��˸�ʽ��洢 */
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	/* DHCP����SECSת��Ϊ��˸�ʽ������ */
	pRIPMSG->secs = htons(DHCP_SECS);
	/* ��һ��bitʹ�ã�ת��Ϊ��˸�ʽ �㲥���� */
	pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
	/* �ͻ���Ӳ����ַ 6��Ԫ�� */
	pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
	pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
	pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
	pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
	pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
	pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];
	
	/* OPTǰ4���ֽ�Ϊmagic cookie ͨ��Ϊ�̶��� */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE  & 0xFF);
	
	/* option 53 ����DHCP�������� */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	/* ���� */
	pRIPMSG->OPT[i++] = 0x01;
	/* DHCP Message Type��������� */
	pRIPMSG->OPT[i++] = DHCP_DISCOVER;
	
	/* option 61 DHCP�ͻ���ָ����Ψһ��ʶ�� */
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	/* ���� */
	pRIPMSG->OPT[i++] = 0x07;
	/* Ӳ������ 0x01Ϊ��̫�� */
	pRIPMSG->OPT[i++] = 0x01;
	/* Client MAC Address */
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
	
	/* option 12 �ͻ����������� */
	pRIPMSG->OPT[i++] = hostName;
	
	sprintf((char *)host_name,"%.4s-%02x%02x%02x",DEVICE_ID,SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
	/* ����ͻ������Ƴ��� */
	pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);
	
	strcpy((char*)(&(pRIPMSG->OPT[i])),(char*)host_name);
	
	i+=(uint16)strlen((char*)host_name);
	
	/* option 55 DHCP�ͻ�������ָ�����ò�����ֵ */
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	/* ���� */
	pRIPMSG->OPT[i++] = 0x06;
	pRIPMSG->OPT[i++] = subnetMask;              /* option 1 */
	pRIPMSG->OPT[i++] = routerOnSubnet;          /* option 3 */
	pRIPMSG->OPT[i++] = dns;                     /* option 6 */
	pRIPMSG->OPT[i++] = domainName;              /* option 15 */
	pRIPMSG->OPT[i++] = dhcpT1value;             /* option 58 */
	pRIPMSG->OPT[i++] = dhcpT2value;             /* option 59 */
	pRIPMSG->OPT[i++] = endOption;               /* option 61 */
	
	/* �����ݷ��ͳ�ȥ */
	SendTo(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	
	#ifdef DHCP_DEBUG	
	printf("sent DHCP_DISCOVER\r\n");
	#endif
	
}


/* ��������Ϣ���͵�DHCP������ */
void DHCP_SendRequest(void)
{
	uint8 ip[4];
	uint16 i = 0;
	
	uint8 host_name[12];
	
	/* ���pRIPMSG�� sizeof(RIP_MSG)���ռ� */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLEN;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	
	/* ���DHCP��״̬Ϊ ׼�������֡����� ��Ϊ�㲥ģʽ������Ϊ����ģʽ */
	if(dhcp_state < STATE_DHCP_LEASED)
		pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
	else
	{
		pRIPMSG->flags = 0;   //for unicast
		/* ��DHCP��������ȡ���ı���IP */
		memcpy(pRIPMSG->ciaddr,GET_SIP,4);
	}
	
	memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6);
	
	/* magic cookie */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE  & 0xFF);
	
	/* option 53 */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = DHCP_REQUEST;
	
	/* option 61 */
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
	
	/* ���DHCP��״̬Ϊ ׼�������֡����� �����option 50��54 */
	if(dhcp_state < STATE_DHCP_LEASED)
	{
		pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
		pRIPMSG->OPT[i++] = 0x04;
		pRIPMSG->OPT[i++] = GET_SIP[0];
		pRIPMSG->OPT[i++] = GET_SIP[1];
		pRIPMSG->OPT[i++] = GET_SIP[2];
		pRIPMSG->OPT[i++] = GET_SIP[3];
		
		pRIPMSG->OPT[i++] = dhcpServerIdentifier;
		pRIPMSG->OPT[i++] = 0x04;
		pRIPMSG->OPT[i++] = DHCP_SIP[0];
		pRIPMSG->OPT[i++] = DHCP_SIP[1];
		pRIPMSG->OPT[i++] = DHCP_SIP[2];
		pRIPMSG->OPT[i++] = DHCP_SIP[3];
		
	}
	
	/* host name */
	pRIPMSG->OPT[i++] = hostName;
	
	sprintf((char *)host_name,"%.4s-%02x%02x%02x",DEVICE_ID,SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
	
	pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);
	
	strcpy((char*)(&(pRIPMSG->OPT[i])),(char*)host_name);
	
	i+=(uint16)strlen((char*)host_name);
	
	/* option 55 */
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	pRIPMSG->OPT[i++] = 0x06;
	pRIPMSG->OPT[i++] = subnetMask;              /* option 1 */
	pRIPMSG->OPT[i++] = routerOnSubnet;          /* option 3 */
	pRIPMSG->OPT[i++] = dns;                     /* option 6 */
	pRIPMSG->OPT[i++] = domainName;              /* option 15 */
	pRIPMSG->OPT[i++] = dhcpT1value;             /* option 58 */
	pRIPMSG->OPT[i++] = dhcpT2value;             /* option 59 */
	pRIPMSG->OPT[i++] = performMaskDiscovery;    /* option 31 */
	pRIPMSG->OPT[i++] = staticRoute;             /* option 33 */
	pRIPMSG->OPT[i++] = endOption;               /* option 61 */
	
	/* ���͹㲥�� */
	if(dhcp_state < STATE_DHCP_LEASED)
		memset(ip,0xFF,4);
	else
		memcpy(ip,DHCP_SIP,4);
		SendTo(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	
	#ifdef DHCP_DEBUG	
	printf("sent DHCP_DISCOVER\r\n");
	#endif
}


/* �����ͷ���Ϣ ����msgtype�Ƿ�Ϊ7 */
void DHCP_SendReleaseDecline(char msgtype)
{
	uint8 ip[4];
	uint16 i = 0;
	
	/* ���pRIPMSG�� sizeof(RIP_MSG)���ռ� */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLEN;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	/* ���� */
	pRIPMSG->flags = 0;
	memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6);
	
	/* magic cookie */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE  & 0xFF);
	
	/* option 53 */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	pRIPMSG->OPT[i++] = 0x01;
	/* ���msgtypeΪ��0ֵ��������ַ�����ã���������ַ���ͷ���Լ */
	pRIPMSG->OPT[i++] = ((!msgtype) ? DHCP_RELIEASE : DHCP_DECLINE);
	
	/* option 61 */
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	pRIPMSG->OPT[i++] = 0x07;
	pRIPMSG->OPT[i++] = 0x01;
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
	
	pRIPMSG->OPT[i++] = dhcpServerIdentifier;
	pRIPMSG->OPT[i++] = 0x04;
	pRIPMSG->OPT[i++] = DHCP_SIP[0];
	pRIPMSG->OPT[i++] = DHCP_SIP[1];
	pRIPMSG->OPT[i++] = DHCP_SIP[2];
	pRIPMSG->OPT[i++] = DHCP_SIP[3];
	
	if(msgtype)
	{
		pRIPMSG->OPT[i++] = dhcpRequestedIPaddr;
		pRIPMSG->OPT[i++] = 0x04;
		pRIPMSG->OPT[i++] = GET_SIP[0];
		pRIPMSG->OPT[i++] = GET_SIP[1];
		pRIPMSG->OPT[i++] = GET_SIP[2];
		pRIPMSG->OPT[i++] = GET_SIP[3];
		
		#ifdef DHCP_DEBUG
		printf("sent DHCP_DECLINE\r\n");
		#endif
	}
	#ifdef DHCP_DEBUG
	printf("send DHCP_RELEASE\r\n");
	#endif
	
	pRIPMSG->OPT[i++] = endOption; 
	
	if(msgtype)
		memset(ip,0xFF,4);
	else
		memcpy(ip,DHCP_SIP,4);
	SendTo(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);

}


/* ������õ�DHCP��Ϣ ����length�������յ�DHCP��Ϣ�����ֵ ����0����ʧ������ֵ�ɹ� */
uint8 DHCP_ParseMessage(uint16 length)
{
	uint8 svr_addr[6];  //ԴIP��ַ
	uint16 svr_port;    //Դ�˿ں�
	uint16 len;         //���յ�����Ϣ����
	uint8 *p;           
	uint8 *e;
	uint8 type;         //DHCP��Ϣ����
	uint8 opt_len;      //DHCPѡ���
	
	/* ����DHCP���ݴ洢��pRIPMSG�����س��� */
	len = RecvFrom(SOCK_DHCP,(uint8 *)pRIPMSG,length,svr_addr,&svr_port);
	
	#ifdef DHCP_DEBUG
	/* �ѷ��ֵ�DNS��������ַ  */
	printf("DHCP_SIP:%u.%u.%u.%u\r\n",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
	/* ѡ��ʹ�õ�DNS��ַ */
	printf("DHCP_REAL_SIP:%u.%u.%u.%u\r\n",DHCP_REAL_SIP[0],DHCP_REAL_SIP[1],DHCP_REAL_SIP[2],DHCP_REAL_SIP[3]);
	printf("svr_addr:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
	#endif
	
	/* ���OP����Client����Server ���� UDPЭ�鹤�����Ǵӷ��������ͻ��� */
	if(pRIPMSG->op != DHCP_BOOTREPLY || svr_port != DHCP_SERVER_PORT)
	{
		#ifdef DHCP_DEBUG
		printf("DHCP:NO DHCP MSG\r\n");
		#endif
		return 0;
	}
	
	/* ���ص�MAC��ַ��Ϊ0����Transaction ID������Ĭ��ֵ */
	if(memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6)!= 0 || pRIPMSG->xid != htonl(DHCP_XID))
	{
		/* �������MAC��ַ���ͻ���Ӳ����ַ��XID���ͻ���IP��ַ */
		#ifdef DHCP_DEBUG
		printf(" No My DHCP Message . This message is ignored\r\n ");
		printf("\tSRC_MAC_ADDR(%02x.%02x.%02x.",SRC_MAC_ADDR[0],SRC_MAC_ADDR[1],SRC_MAC_ADDR[2]);
		printf("%02x.%02x.%02x)",SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
		printf(",pRIPMSG->chaddr(%02x.%02x.%02x.",(char)pRIPMSG->chaddr[0],(char)pRIPMSG->chaddr[1],(char)pRIPMSG->chaddr[2]);
		printf("%02x.%02x.%02x)",(char)pRIPMSG->chaddr[3],(char)pRIPMSG->chaddr[4],(char)pRIPMSG->chaddr[5]);
		printf("\tpRIPMSG->xid(%08x),DHCP_XID(%08x)",pRIPMSG->yiaddr,(DHCP_XID));
		printf("\tRIMPMSG->yiaddr:%d.%d.%d.%d\r\n",pRIPMSG->yiaddr[0],pRIPMSG->yiaddr[1],pRIPMSG->yiaddr[2],pRIPMSG->yiaddr[3]);
		#endif 
		return 0;
	}
	
	/* ��DNS��������ַת��Ϊuint32��ָ�벢�������ж��Ƿ���ֵ����0 */
	if(*((uint32 *)DHCP_SIP) != 0x00000000)
	{
		/* ���ѡ��ʹ�õ�DHCP��������ַ��DNS��������ַ����ԴIP��ַ��ͬ */
		if(*((uint32*)DHCP_REAL_SIP) != *((uint32*)svr_addr) && *((uint32*)DHCP_SIP) != *((uint32*)svr_addr))
		{
			#ifdef DHCP_DEBUG
			printf("Another DHCP sever send a response message. thie is ignores\r\n");
			printf("\tIP:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
			#endif 
			return 0;
		}
	}
	/* ��Server��֪Client���䵽�ĵ�ַ���Ƶ���������ȡ�ı���IP */
	memcpy(GET_SIP,pRIPMSG->yiaddr,4);
	
	#ifdef DHCP_DEBUG
	printf("DHCP MAG received \r\n");
	printf("yiaddr:%u.%u.%u.%u\r\n",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
	#endif
	
	type = 0;
	p = (uint8 *)(&pRIPMSG->op);    //DHCP���ĵ���ʼ��ַ
	p = p+240;                      //option��Ϣ����ʼλ��(��ȥmagic code)
	e = p +(len -240);              //DHCP��Ϣѡ���ֶεĽ���λ��    
	
	#ifdef DHCP_DEBUG
	printf("p:%08x e:%08x len : %d\r\n",(uint32)p,(uint32)e,len);
	#endif
	
	while(p<e)
	{
		/* pָ���������鿴magic code������� */
		switch(*p++)
		{
			
			case endOption:
				return type;
			
			case padOption:
				break;
			
			case dhcpMessageType:
				opt_len = *p++;
				type = *p;
				#ifdef DHCP_DEBUG
				printf("dhcpMessageType:%02X\r\n",type);
				#endif
			break;
			
			case subnetMask:
				opt_len = *p++;
				memcpy(GET_SN_MASK,p,4);
				#ifdef DHCP_DEBUG
				printf("subnetMask:");
				printf("%u.%u.%u.%u\r\n",GET_SN_MASK[0],GET_SN_MASK[1],GET_SN_MASK[2],GET_SN_MASK[3]);
				#endif
			break;
			
			case routerOnSubnet:
				opt_len = *p++;
				memcpy(GET_GW_IP,p,4);
				#ifdef DHCP_DEBUG
				printf("routerOnSubnet:");
				printf("%u.%u.%u.%u\r\n",GET_GW_IP[0],GET_GW_IP[1],GET_GW_IP[2],GET_GW_IP[3]);
				#endif
			break;
			
			case dns:
				opt_len = *p++;
				memcpy(GET_DNS_IP,p,4);
			break;
			
			case dhcpIPaddrLeaseTime:
				opt_len = *p++;
				dhcp_lease_time = ntohl(*((uint32*)p));
				#ifdef DHCP_DEBUG
				printf("dhcpIPaddrLeaseTime :%d\r\n",dhcp_lease_time);
				#endif
			break;
			
			case dhcpServerIdentifier:
				opt_len = *p++;
				#ifdef DHCP_DEBUG
				printf("DHCP_SIP:%u.%u.%u.%u\r\n",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
				#endif
			
				/* ���DNS��ַΪ0 ����DHCP��������ַ����ԴIP��ַ ����DNS��ַ����ԴIP��ַ  */
				if(*((uint32*)DHCP_SIP) == 0 || *((uint32*)DHCP_REAL_SIP) == *((uint32*)svr_addr)  || *((uint32*)DHCP_SIP) == *((uint32*)svr_addr) )
				{
					memcpy(DHCP_SIP,p,4);
					memcpy(DHCP_REAL_SIP,svr_addr,4);
					#ifdef DHCP_DEBUG
					printf("dhcpServerIdentifier:");
					printf("%u.%u.%u.%u\r\n",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
					printf("My DHCP server real IP address:");
					printf("%u.%u.%u.%u\r\n",DHCP_REAL_SIP[0],DHCP_REAL_SIP[1],DHCP_REAL_SIP[2],DHCP_REAL_SIP[3]);
					#endif
				}
				else
				{
					#ifdef DHCP_DEBUG
					printf("Another dhcpServerIdentifier:\r\n");
					printf("\tMY(%u.%u.%u.%u)",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
					printf("My DHCP server real IP address:");
					printf("Another(%u.%u.%u.%u)",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
					#endif
				}
			break;
				
			default:
				opt_len = *p++;
				#ifdef DHCP_DEBUG
				printf("opt_len:%u\r\n",opt_len);
				#endif
			break;
		}
		p+=opt_len;
	}
	return 0;
}


/* ���DHCP״̬ ����ֵs:Socket�ţ�����DHCP״̬ DHCP_RET_NONE: δ��ȡ��IP��ַ DHCP_RET_TIMEOUT:��ʱ DHCP_RET_UPDATE: ��ȡ�ɹ� DHCP_RET_CONFLICT:IP��ַ��ͻ */
uint8 DHCP_CheckState(SOCKET s)
{
	uint16 len;        /*����һ����ʾ�������ݴ�С����*/
	uint8 type;        /*����һ����ʾ���շ�����ͱ���*/
	
	type = 0;
	
	if(getSn_SR(s) != Sn_SR_SOCK_CLOSE)          /*socket���ڴ�״̬*/
	{
		if((len = getSn_RX_FSR(s))>0)              /*���յ�����*/
		{
			type = DHCP_ParseMessage(len);           /*�����յ��ķ������*/
		}
	}
	else
	{
		if(dhcp_state == STATE_DHCP_READY)
		{
			#ifdef DHCP_DEBUG
			printf("state : STATE_DHCP_READY\r\n");
			#endif
		}
		if(!socket(SOCK_DHCP,Sn_MR_UDP,DHCP_CLIENT_PORT,0x00))/*��ʼ��socket�Ͷ˿�*/
		{
			#ifdef DHCP_DEBUG
			printf("Fail to create eht DHCP_SOCK(%u)\r\n",SOCK_DHCP);
			#endif
			return DHCP_RET_ERR;                                /*socket��ʼ������*/
		}
	}
	switch(dhcp_state)
	{
		case STATE_DHCP_READY:                          /*DHCP��ʼ��״̬*/
			dhcp_timeout = 0;                             /*DHCP��ʱ��־����Ϊ1*/
			DHCP_ResetTime();                             /*��λ��ʱʱ��*/
			DHCP_SendDiscover();                          /*����DISCOVER��*/
		
			DHCP_timer = 0;                               /*set_timer0(DHCP_timer_handler);  */ 	
			dhcp_state = STATE_DHCP_DISCOVER;             /*DHCP��DISCOVER״̬*/
		break;
		
		case STATE_DHCP_DISCOVER:                       /* DHCPʶ��״̬ */
			if(type == DHCP_OFFER)
			{
				DHCP_ResetTime();                           /*��λ��ʱʱ��*/
				DHCP_SendDiscover();                        /*����REQUEST��*/
				dhcp_state = STATE_DHCP_REQUESET;           /* ״̬��Ϊ���� */
				#ifdef DHCP_DEBUG
				printf("state : STATE_DHCP_REQUEST\r\n");
				#endif
			}
			else
				DHCP_CheckTimeout();                        /*����Ƿ�ʱ*/
		break;
			
		case STATE_DHCP_REQUESET:                       /*DHCP��REQUEST״̬*/
			if(type == DHCP_ACK)                          /*���յ�DHCP��������Ӧ��off���*/
			{
				DHCP_ResetTime();                           /*��λ��ʱʱ��*/
				if(Check_LeaseIP())                         /* ���IP��ͻ */
				{
					#ifdef DHCP_DEBUG
					printf("state : STATE_DHCP_LEASED\r\n");
					#endif
					dhcp_state = STATE_DHCP_LEASED;           /* ����״̬ */
					return DHCP_RET_UPDATE;                   /* �������� */
				}
				else
				{
					#ifdef DHCP_DEBUG
					printf("state: STATE_DHCP_DISCOVER\r\n");
					#endif
					dhcp_state = STATE_DHCP_DISCOVER;
					return DHCP_RET_CONFLICT;
				}
			}
			else if(type == DHCP_NAK)
			{
				DHCP_ResetTime();
				dhcp_state = STATE_DHCP_DISCOVER;
				#ifdef DHCP_DEBUG
				printf("state : STATE_DHCP_DISCOVER\r\n");
				#endif
			}
			else
				DHCP_CheckTimeout();
		break;
			
		case STATE_DHCP_LEASED:
			/* �������ʱ�䲻������ ���� ʹ��ʱ����� */
			if((dhcp_lease_time != 0xFFFFFFFF) && (dhcp_time>(dhcp_lease_time/2)))
			{
				type = 0;
				memcpy(OLD_SIP,GET_SIP,4);
				DHCP_SendRequest();
				dhcp_state = STATE_DHCP_REREQUEST;
				#ifdef DHCP_DEBUG
				printf("state:STATE_DHCP_REREQUEST\r\n");
				#endif
				DHCP_ResetTime();
			}
		break;
			
		case STATE_DHCP_REREQUEST:
			if(type == DHCP_ACK)
			{
				if(memcpy(OLD_SIP,GET_SIP,4)!= 0)
				{
					#ifdef DHCP_DEBUG
					printf("The IP address from the DHCP server is updated\r\n");
					printf("OLD_SIP:%u.%u.%u.%u",OLD_SIP[0],OLD_SIP[1],OLD_SIP[2],OLD_SIP[3]);
					printf("GET_SIP:%u.%u.%u.%u",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
					#endif
					return DHCP_RET_UPDATE;
				}
				#ifdef DHCP_DEBUG
				else
				{
					printf("state :STATE_DHCP_LEASED:same IP\r\n");
				}
				#endif
				
				DHCP_ResetTime();
				dhcp_state = STATE_DHCP_LEASED;
			}
			else if(type == DHCP_NAK)
			{
				DHCP_ResetTime();
				dhcp_state = STATE_DHCP_DISCOVER;
				#ifdef DHCP_DEBUG
				printf("state: STATE_DHCP_DISCOVER\r\n");
				#endif
			}
			else
				DHCP_CheckTimeout();
		break;
			
		default:
			dhcp_state = STATE_DHCP_READY;
		break;
			
	}
	if(dhcp_timeout ==1)
	{
		dhcp_state = STATE_DHCP_READY;
		return DHCP_RET_TIMEOUT;
	}
	return DHCP_RET_NONE;
}

/* ���DHCP�����ͷų�ʱ */
void DHCP_CheckTimeout(void)
{
	/* ���Դ���δ�ﵽ���ֵ */
	if(dhcp_retry_count < MAX_DHCP_RETRY)
	{
		/* DHCP���м������ڳ�ʱʱ�� */
		if(dhcp_time > next_dhcp_time)
		{
			dhcp_time = 0;
			next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
			dhcp_retry_count++;
			
			switch(dhcp_state)
			{
				case STATE_DHCP_DISCOVER:
					#ifdef DHCP_DEBUG
						printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
					#endif
					DHCP_SendDiscover();
				break;
				
				case STATE_DHCP_REQUESET:
					#ifdef DHCP_DEBUG
						printf("<<timeout>> state : STATE_DHCP_REQUESET\r\n");
					#endif
					DHCP_SendRequest();
				break;
				
				case STATE_DHCP_REREQUEST:
					#ifdef DHCP_DEBUG
						printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
					#endif
					DHCP_SendRequest();
				break;
				
				default:
					break;
			}
		}
	}
	else
	{
		DHCP_ResetTime();
		dhcp_timeout = 1;
		
		DHCP_SendDiscover();
		dhcp_state = STATE_DHCP_DISCOVER;
		
		#ifdef STATE_DHCP_DISCOVER
		printf("timeout:state:STATE_DHCP_DISCOVER\r\n");
		#endif
		
	}
}


/* ����ȡIP�Ƿ��ͻ */
uint8 Check_LeaseIP(void)
{
	#ifdef DHCP_DEBUG
	printf("<Check the IP Conflict %d.%d.%d.%d:",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
	#endif
	
	/* SendTo��ɣ���ζ����һ���ڵ������ͬ��IP */
	if(SendTo(SOCK_DHCP,(const uint8*)"CHECK)IP_CONFLICT",17,GET_SIP,5000))
	{
		#ifdef DHCP_DEBUG
		printf("Conflict>\r\n");
		#endif
		DHCP_SendReleaseDecline(1);
		return 0;
	}
	#ifdef DHCP_DEBUG
	printf("No Conflict>\r\n");
	#endif
	
	return 1;
}


/* DHCP��ʱ������ */
void DHCP_TimerHandler(void)
{
	/* �������������1000��0����DHCP���д�����1 */
	if(dhcp_tick_cnt++ >1000)
	{
		dhcp_tick_cnt = 0;
		dhcp_time++;
	}
}


/* ��ʼ��DHCP�ͻ��� */
void DHCP_InitClient(void)
{
	uint8 ip_0[4] = {0,};
	DHCP_XID= 0x12345678;
	memset(OLD_SIP,0,sizeof(OLD_SIP));
	memset(DHCP_SIP,0,sizeof(DHCP_SIP));
	memset(DHCP_REAL_SIP,0,sizeof(DHCP_REAL_SIP));
	
	/* ���IP���ñ�־ */
	IINCHIP_INIT();
	setSUBR(ip_0);
	setGAR(ip_0);
	setSIPR(ip_0);
	
	printf("mac= %02x:%02x:%02x:%02x:%02x:%02x\r\n",SRC_MAC_ADDR[0],SRC_MAC_ADDR[1],SRC_MAC_ADDR[2],SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
	dhcp_state = STATE_DHCP_READY;
	#ifdef DHCP_DEBUG
	printf("init_dhcp_client:%u\r\n",SOCK_DHCP);
	#endif
}

/* ִ��DHCP Client */
void Do_DHCP(void)
{
	uint8 dhcpret = 0;
	ip_from = IP_FROM_DHCP;       //IP���÷���ѡ��DHCP
	DHCP_Timer_Init();            /* ��ʼ��DHCP��ʱ�� */
	
	if(Conflict_flag == 1)
	{
		DHCP_InitClient();          /* ��ʼ��DHCP�ͻ��� */
		Conflict_flag = 0;
	}
	
	dhcpret = DHCP_CheckState(SOCK_DHCP);          /* ��ȡDHCP����״̬ */
	
	switch(dhcpret)
	{
		case DHCP_RET_NONE:                         //IP��ַ��ȡ���ɹ�
			break;
		
		case DHCP_RET_TIMEOUT:                     //IP��ַ��ȡ��ʱ
			break;
		
		case DHCP_RET_UPDATE:                       /*�ɹ���ȡ��IP��ַ*/ 
			dhcp_ok = 1;
			Set_W5500_IP();                           /*����ȡ����IP��ַд��W5500�Ĵ���*/ 
			printf("�Ѵ�DHCP�������ɹ����IP��ַ\r\n");
		break;
		
		case DHCP_RET_CONFLICT:
			printf("��DHCP��ȡIP��ַʧ��\r\n");
			dhcp_state = STATE_DHCP_READY;
			printf("������\r\n");
			dhcp_ok = 0;
		break;
		
		default:
			break;
	}
}





