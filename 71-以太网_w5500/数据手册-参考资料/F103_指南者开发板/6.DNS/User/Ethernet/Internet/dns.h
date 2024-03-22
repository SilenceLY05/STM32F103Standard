#ifndef	_DNS_H_
#define	_DNS_H_

#include "types.h"


extern uint8 BUFPUB[1024];

#define  DNS_RESPONSE_TIMEOUT 3 // 3 seconds
#define  DNS_RETRY            3//3 times
#define  DNS_RET_PROGRESS  0
#define  DNS_RET_FAIL      1
#define  DNS_RET_SUCCESS   2

#define  EXTERN_DNS_SERVERIP  ConfigMsg.dns
#define  EXTERN_DNS_SOCKBUF   gBUFPUBLIC
#define  EXTERN_DNS_ANYPORT   2000//tsvDEVCONFnew.any_port

#define	MAX_DNS_BUF_SIZE	256		/* DNS数据的最大值. */


#define  INITRTT     2000L	/* 初始平滑响应时间 */
#define  MAXCNAME    10	/* cname递归的最大数量 */
         
/* 资源记录类型 */				 
#define  TYPE_A		1	/* 主机地址 */
#define  TYPE_NS		2	/* 记录哪些DNS服务器负责解析 */
#define  TYPE_MD		3	/* 邮件目的地 (已废弃) */
#define  TYPE_MF		4	/* Mail forwarder (obsolete) */
#define  TYPE_CNAME	5	/* 别名对应的规范名称 */
#define  TYPE_SOA    6	/* 起始授权机构记录 */
#define  TYPE_MB		7	/* 邮箱名(实验性) */
#define  TYPE_MG		8	/* 邮件组成员(实验) */
#define  TYPE_MR		9	/* 邮件重命名(实验性) */
#define  TYPE_NULL	10	/* Null (experimental) */
#define  TYPE_WKS    11	/* 著名的 sockets */
#define  TYPE_PTR    12	/* IP地址反向解析 */
#define  TYPE_HINFO	13	/* 主机信息 */
#define  TYPE_MINFO	14	/* 邮箱信息 (experimental)*/
#define  TYPE_MX		15	/* 邮件交换记录 */
#define  TYPE_TXT    16	/* 文本资源记录 */
#define  TYPE_ANY    255/* 匹配任何类型 */

/* 网络协议类型 */
#define  CLASS_IN    1  /* The ARPA Internet */

/* 往返时间参数 */
#define  AGAIN       8	/* Average RTT gain = 1/8 */
#define  LAGAIN      3	/* Log2(AGAIN) */
#define  DGAIN       4  /* 平均偏差增益 = 1/4 */
#define  LDGAIN      2  /* log2(DGAIN) */
         
#define  IPPORT_DOMAIN	53

/* 所有域消息的标头 */
struct dhdr
{
	uint16  id;		/* id */
	uint8 	qr;		/* 查询/响应 */
#define	QUERY		0
#define	RESPONSE	1
	uint8 	opcode;
#define	IQUERY		1
	uint8 	aa;		/* Authoratative answer */
	uint8 	tc;		/* Truncation */
	uint8 	rd;		/* Recursion desired */
	uint8 	ra;		/* Recursion available */
	uint8 	rcode;		/* Response code */
#define	NO_ERROR	0
#define	FORMAT_ERROR	1
#define	SERVER_FAIL	2
#define	NAME_ERROR	3
#define	NOT_IMPL	4
#define	REFUSED		5
	uint16  qdcount;	/* Question count */
	uint16  ancount;	/* Answer count */
	uint16  nscount;	/* Authority (name server) count */
	uint16  arcount;	/* Additional record count */
};

extern uint8 dns_get_ip[4];
extern uint8 domain_name[];
int dns_makequery(uint16 op, uint8 * name, uint8 * buf, uint16 len);
int parse_name(uint8 * msg, uint8 * compressed, /*char * buf,*/ uint16 len);
uint8 * dns_question(uint8 * msg, uint8 * cp);
uint8 * dns_answer(uint8 * msg, uint8 * cp);
uint8 parseMSG(struct dhdr * pdhdr, uint8 * pbuf);

uint8 dns_query(uint8 s, uint8 * name);
void do_dns(void);

#endif	/* _DNS_H_ */
