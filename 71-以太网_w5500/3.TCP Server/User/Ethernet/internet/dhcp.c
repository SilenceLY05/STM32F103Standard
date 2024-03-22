#include <stdio.h>
#include <string.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "dhcp.h"
#include "utility.h"
#include "bsp_W5500_conf.h"

DHCP_Get DHCP_GET;


uint8 *SRC_MAC_ADDR   = EXTERN_DHCP_MAC;         /* 本地MAC地址 */
uint8 *GET_SN_MASK    = EXTERN_DHCP_SN;          /* 从DHCP服务器获取到的子网掩码 */
uint8 *GET_GW_IP      = EXTERN_DHCP_GW;          /* 从DHCP服务器获取到的网关地址 */
uint8 *GET_DNS_IP     = EXTERN_DHCP_DNS;         /* 从DHCP服务器获取到的DNS服务器地址 */
uint8 *DHCP_HOST_NAME = EXTERN_DHCP_NAME;        /* 主机名 */
uint8 *GET_SIP        = EXTERN_DHCP_SIP;         /* 从DHCP服务器获取到的本机IP */
uint8  DHCP_SIP[4]    = {0,};                    /* 已发现的DNS服务器地址 */
uint8  DHCP_REAL_SIP[4] = {0,};                  /* 从DHCP列表中选择使用的DHCP服务器 */
uint8  OLD_SIP[4];                               /* 最初获取到的IP地址 */

uint8  dhcp_state     = STATE_DHCP_READY;        /* DHCP客户端状态 */
uint8  dhcp_retry_count = 0;                     /* DHCP重试次数 */
uint8  dhcp_timeout   = 0;                       /* DHCP超时标志位 */
uint32 dhcp_lease_time;                          /* 租约时间 */
uint32 next_dhcp_time = 0;                       /* DHCP超时时间 */
uint32 dhcp_tick_cnt  = 0;                       
uint8  DHCP_timer;

uint8  Conflict_flag = 0;
uint32 DHCP_XID      = DHCP_DEFAULT_XID;         /* DHCP报文默认XID取值 */
uint8  EXTERN_DHCPBUFF[1024];
RIP_MSG* pRIPMSG = (RIP_MSG*)EXTERN_DHCPBUFF;    /* DHCP消息指针 */


/* DHCP定时初始化 */
void DHCP_ResetTime(void)
{
	/* dhcp运行计数 */
	dhcp_time = 0;
	dhcp_tick_cnt = 0;
	/* 超时时间 = 运行时间+ */
	next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
	/* 初始化重试次数 */
	dhcp_retry_count = 0;
}


/* 发送DISCOVER信息给DHCP服务器 */
void DHCP_SendDiscover(void)
{
	/* 初始化IP数组 */
	uint8 ip[4] = {255,255,255,255};
	uint16 i = 0;
	/* 初始化客户端名称数组 */
	uint8 host_name[12];
	
	/* 清空pRIPMSG的 sizeof(RIP_MSG)个空间 */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	/* 选择Client发给Server报文类型 */
	pRIPMSG->op = DHCP_BOOTREQUEST;
	/* 使用10M 以太网 */
	pRIPMSG->htype = DHCP_HTYPE10MB;
	/* 使用10M 以太网 */
	pRIPMSG->hlen = DHCP_HLEN;
	/* Client设备为0 */
	pRIPMSG->hops = DHCP_HOPS;
	/* 将xid转换为大端格式后存储 */
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	/* DHCP报文SECS转换为大端格式并保存 */
	pRIPMSG->secs = htons(DHCP_SECS);
	/* 第一个bit使用，转换为大端格式 广播报文 */
	pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
	/* 客户端硬件地址 6个元素 */
	pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
	pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
	pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
	pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
	pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
	pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];
	
	/* OPT前4个字节为magic cookie 通常为固定的 */
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 24) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 16) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)((MAGIC_COOKIE >> 8) & 0xFF);
	pRIPMSG->OPT[i++] = (uint8)(MAGIC_COOKIE  & 0xFF);
	
	/* option 53 传递DHCP报文类型 */
	pRIPMSG->OPT[i++] = dhcpMessageType;
	/* 长度 */
	pRIPMSG->OPT[i++] = 0x01;
	/* DHCP Message Type具体的类型 */
	pRIPMSG->OPT[i++] = DHCP_DISCOVER;
	
	/* option 61 DHCP客户端指定其唯一标识符 */
	pRIPMSG->OPT[i++] = dhcpClientIdentifier;
	/* 长度 */
	pRIPMSG->OPT[i++] = 0x07;
	/* 硬件类型 0x01为以太网 */
	pRIPMSG->OPT[i++] = 0x01;
	/* Client MAC Address */
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->OPT[i++] = SRC_MAC_ADDR[5];
	
	/* option 12 客户端名称数组 */
	pRIPMSG->OPT[i++] = hostName;
	
	sprintf((char *)host_name,"%.4s-%02x%02x%02x",DEVICE_ID,SRC_MAC_ADDR[3],SRC_MAC_ADDR[4],SRC_MAC_ADDR[5]);
	/* 保存客户端名称长度 */
	pRIPMSG->OPT[i++] = (uint8)strlen((char*)host_name);
	
	strcpy((char*)(&(pRIPMSG->OPT[i])),(char*)host_name);
	
	i+=(uint16)strlen((char*)host_name);
	
	/* option 55 DHCP客户端请求指定配置参数的值 */
	pRIPMSG->OPT[i++] = dhcpParamRequest;
	/* 长度 */
	pRIPMSG->OPT[i++] = 0x06;
	pRIPMSG->OPT[i++] = subnetMask;              /* option 1 */
	pRIPMSG->OPT[i++] = routerOnSubnet;          /* option 3 */
	pRIPMSG->OPT[i++] = dns;                     /* option 6 */
	pRIPMSG->OPT[i++] = domainName;              /* option 15 */
	pRIPMSG->OPT[i++] = dhcpT1value;             /* option 58 */
	pRIPMSG->OPT[i++] = dhcpT2value;             /* option 59 */
	pRIPMSG->OPT[i++] = endOption;               /* option 61 */
	
	/* 将数据发送出去 */
	SendTo(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	
	#ifdef DHCP_DEBUG	
	printf("sent DHCP_DISCOVER\r\n");
	#endif
	
}


/* 将请求消息发送到DHCP服务器 */
void DHCP_SendRequest(void)
{
	uint8 ip[4];
	uint16 i = 0;
	
	uint8 host_name[12];
	
	/* 清空pRIPMSG的 sizeof(RIP_MSG)个空间 */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLEN;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	
	/* 如果DHCP的状态为 准备、发现、请求 则为广播模式，否则为单播模式 */
	if(dhcp_state < STATE_DHCP_LEASED)
		pRIPMSG->flags = htons(DHCP_FLAGSBROADCAST);
	else
	{
		pRIPMSG->flags = 0;   //for unicast
		/* 从DHCP服务器获取到的本机IP */
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
	
	/* 如果DHCP的状态为 准备、发现、请求 则进行option 50和54 */
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
	
	/* 发送广播包 */
	if(dhcp_state < STATE_DHCP_LEASED)
		memset(ip,0xFF,4);
	else
		memcpy(ip,DHCP_SIP,4);
		SendTo(SOCK_DHCP, (uint8 *)pRIPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
	
	#ifdef DHCP_DEBUG	
	printf("sent DHCP_DISCOVER\r\n");
	#endif
}


/* 发送释放信息 输入msgtype是否为7 */
void DHCP_SendReleaseDecline(char msgtype)
{
	uint8 ip[4];
	uint16 i = 0;
	
	/* 清空pRIPMSG的 sizeof(RIP_MSG)个空间 */
	memset((void*)pRIPMSG,0,sizeof(RIP_MSG));
	
	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLEN;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = htonl(DHCP_DEFAULT_XID);
	pRIPMSG->secs = htons(DHCP_SECS);
	/* 单播 */
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
	/* 如果msgtype为非0值，则分配地址不可用，否则撤销地址并释放租约 */
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


/* 解析获得的DHCP信息 输入length期望接收的DHCP消息的最大值 返回0解析失败其他值成功 */
uint8 DHCP_ParseMessage(uint16 length)
{
	uint8 svr_addr[6];  //源IP地址
	uint16 svr_port;    //源端口号
	uint16 len;         //接收到的消息长度
	uint8 *p;           
	uint8 *e;
	uint8 type;         //DHCP消息类型
	uint8 opt_len;      //DHCP选项长度
	
	/* 接收DHCP数据存储在pRIPMSG并返回长度 */
	len = RecvFrom(SOCK_DHCP,(uint8 *)pRIPMSG,length,svr_addr,&svr_port);
	
	#ifdef DHCP_DEBUG
	/* 已发现的DNS服务器地址  */
	printf("DHCP_SIP:%u.%u.%u.%u\r\n",DHCP_SIP[0],DHCP_SIP[1],DHCP_SIP[2],DHCP_SIP[3]);
	/* 选择使用的DNS地址 */
	printf("DHCP_REAL_SIP:%u.%u.%u.%u\r\n",DHCP_REAL_SIP[0],DHCP_REAL_SIP[1],DHCP_REAL_SIP[2],DHCP_REAL_SIP[3]);
	printf("svr_addr:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
	#endif
	
	/* 如果OP不是Client发给Server 或者 UDP协议工作不是从服务器到客户端 */
	if(pRIPMSG->op != DHCP_BOOTREPLY || svr_port != DHCP_SERVER_PORT)
	{
		#ifdef DHCP_DEBUG
		printf("DHCP:NO DHCP MSG\r\n");
		#endif
		return 0;
	}
	
	/* 本地的MAC地址不为0或者Transaction ID不等于默认值 */
	if(memcpy(pRIPMSG->chaddr,SRC_MAC_ADDR,6)!= 0 || pRIPMSG->xid != htonl(DHCP_XID))
	{
		/* 输出本地MAC地址、客户端硬件地址、XID、客户端IP地址 */
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
	
	/* 将DNS服务器地址转换为uint32的指针并解引用判断是否其值等于0 */
	if(*((uint32 *)DHCP_SIP) != 0x00000000)
	{
		/* 如果选择使用的DHCP服务器地址与DNS服务器地址都与源IP地址不同 */
		if(*((uint32*)DHCP_REAL_SIP) != *((uint32*)svr_addr) && *((uint32*)DHCP_SIP) != *((uint32*)svr_addr))
		{
			#ifdef DHCP_DEBUG
			printf("Another DHCP sever send a response message. thie is ignores\r\n");
			printf("\tIP:%u.%u.%u.%u\r\n",svr_addr[0],svr_addr[1],svr_addr[2],svr_addr[3]);
			#endif 
			return 0;
		}
	}
	/* 将Server告知Client分配到的地址复制到服务器获取的本机IP */
	memcpy(GET_SIP,pRIPMSG->yiaddr,4);
	
	#ifdef DHCP_DEBUG
	printf("DHCP MAG received \r\n");
	printf("yiaddr:%u.%u.%u.%u\r\n",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
	#endif
	
	type = 0;
	p = (uint8 *)(&pRIPMSG->op);    //DHCP报文的起始地址
	p = p+240;                      //option信息的起始位置(除去magic code)
	e = p +(len -240);              //DHCP消息选项字段的结束位置    
	
	#ifdef DHCP_DEBUG
	printf("p:%08x e:%08x len : %d\r\n",(uint32)p,(uint32)e,len);
	#endif
	
	while(p<e)
	{
		/* p指针自增，查看magic code后的数据 */
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
			
				/* 如果DNS地址为0 或者DHCP服务器地址等于源IP地址 或者DNS地址等于源IP地址  */
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


/* 检查DHCP状态 输入值s:Socket号，返回DHCP状态 DHCP_RET_NONE: 未获取到IP地址 DHCP_RET_TIMEOUT:超时 DHCP_RET_UPDATE: 获取成功 DHCP_RET_CONFLICT:IP地址冲突 */
uint8 DHCP_CheckState(SOCKET s)
{
	uint16 len;        /*定义一个表示接收数据大小变量*/
	uint8 type;        /*定义一个表示接收封包类型变量*/
	
	type = 0;
	
	if(getSn_SR(s) != Sn_SR_SOCK_CLOSE)          /*socket处于打开状态*/
	{
		if((len = getSn_RX_FSR(s))>0)              /*接收到数据*/
		{
			type = DHCP_ParseMessage(len);           /*解析收到的封包类型*/
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
		if(!socket(SOCK_DHCP,Sn_MR_UDP,DHCP_CLIENT_PORT,0x00))/*初始化socket和端口*/
		{
			#ifdef DHCP_DEBUG
			printf("Fail to create eht DHCP_SOCK(%u)\r\n",SOCK_DHCP);
			#endif
			return DHCP_RET_ERR;                                /*socket初始化错误*/
		}
	}
	switch(dhcp_state)
	{
		case STATE_DHCP_READY:                          /*DHCP初始化状态*/
			dhcp_timeout = 0;                             /*DHCP超时标志设置为1*/
			DHCP_ResetTime();                             /*复位超时时间*/
			DHCP_SendDiscover();                          /*发送DISCOVER包*/
		
			DHCP_timer = 0;                               /*set_timer0(DHCP_timer_handler);  */ 	
			dhcp_state = STATE_DHCP_DISCOVER;             /*DHCP的DISCOVER状态*/
		break;
		
		case STATE_DHCP_DISCOVER:                       /* DHCP识别状态 */
			if(type == DHCP_OFFER)
			{
				DHCP_ResetTime();                           /*复位超时时间*/
				DHCP_SendDiscover();                        /*发送REQUEST包*/
				dhcp_state = STATE_DHCP_REQUESET;           /* 状态变为请求 */
				#ifdef DHCP_DEBUG
				printf("state : STATE_DHCP_REQUEST\r\n");
				#endif
			}
			else
				DHCP_CheckTimeout();                        /*检查是否超时*/
		break;
			
		case STATE_DHCP_REQUESET:                       /*DHCP的REQUEST状态*/
			if(type == DHCP_ACK)                          /*接收到DHCP服务器回应的off封包*/
			{
				DHCP_ResetTime();                           /*复位超时时间*/
				if(Check_LeaseIP())                         /* 如果IP冲突 */
				{
					#ifdef DHCP_DEBUG
					printf("state : STATE_DHCP_LEASED\r\n");
					#endif
					dhcp_state = STATE_DHCP_LEASED;           /* 租赁状态 */
					return DHCP_RET_UPDATE;                   /* 更新租赁 */
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
			/* 如果租赁时间不是永久 或者 使用时间过半 */
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

/* 检查DHCP请求释放超时 */
void DHCP_CheckTimeout(void)
{
	/* 重试次数未达到最大值 */
	if(dhcp_retry_count < MAX_DHCP_RETRY)
	{
		/* DHCP运行计数大于超时时间 */
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


/* 检查获取IP是否冲突 */
uint8 Check_LeaseIP(void)
{
	#ifdef DHCP_DEBUG
	printf("<Check the IP Conflict %d.%d.%d.%d:",GET_SIP[0],GET_SIP[1],GET_SIP[2],GET_SIP[3]);
	#endif
	
	/* SendTo完成，意味着有一个节点具有相同的IP */
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


/* DHCP定时器操作 */
void DHCP_TimerHandler(void)
{
	/* 如果计数器大于1000置0并将DHCP运行次数加1 */
	if(dhcp_tick_cnt++ >1000)
	{
		dhcp_tick_cnt = 0;
		dhcp_time++;
	}
}


/* 初始化DHCP客户端 */
void DHCP_InitClient(void)
{
	uint8 ip_0[4] = {0,};
	DHCP_XID= 0x12345678;
	memset(OLD_SIP,0,sizeof(OLD_SIP));
	memset(DHCP_SIP,0,sizeof(DHCP_SIP));
	memset(DHCP_REAL_SIP,0,sizeof(DHCP_REAL_SIP));
	
	/* 清除IP设置标志 */
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

/* 执行DHCP Client */
void Do_DHCP(void)
{
	uint8 dhcpret = 0;
	ip_from = IP_FROM_DHCP;       //IP配置方法选择DHCP
	DHCP_Timer_Init();            /* 初始化DHCP定时器 */
	
	if(Conflict_flag == 1)
	{
		DHCP_InitClient();          /* 初始化DHCP客户端 */
		Conflict_flag = 0;
	}
	
	dhcpret = DHCP_CheckState(SOCK_DHCP);          /* 获取DHCP服务状态 */
	
	switch(dhcpret)
	{
		case DHCP_RET_NONE:                         //IP地址获取不成功
			break;
		
		case DHCP_RET_TIMEOUT:                     //IP地址获取超时
			break;
		
		case DHCP_RET_UPDATE:                       /*成功获取到IP地址*/ 
			dhcp_ok = 1;
			Set_W5500_IP();                           /*将获取到的IP地址写入W5500寄存器*/ 
			printf("已从DHCP服务器成功获得IP地址\r\n");
		break;
		
		case DHCP_RET_CONFLICT:
			printf("从DHCP获取IP地址失败\r\n");
			dhcp_state = STATE_DHCP_READY;
			printf("重试中\r\n");
			dhcp_ok = 0;
		break;
		
		default:
			break;
	}
}





