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

#define	MAX_DNS_BUF_SIZE	256		/* DNS���ݵ����ֵ. */


#define  INITRTT     2000L	/* ��ʼƽ����Ӧʱ�� */
#define  MAXCNAME    10	/* cname�ݹ��������� */
         
/* ��Դ��¼���� */				 
#define  TYPE_A		1	/* ������ַ */
#define  TYPE_NS		2	/* ��¼��ЩDNS������������� */
#define  TYPE_MD		3	/* �ʼ�Ŀ�ĵ� (�ѷ���) */
#define  TYPE_MF		4	/* Mail forwarder (obsolete) */
#define  TYPE_CNAME	5	/* ������Ӧ�Ĺ淶���� */
#define  TYPE_SOA    6	/* ��ʼ��Ȩ������¼ */
#define  TYPE_MB		7	/* ������(ʵ����) */
#define  TYPE_MG		8	/* �ʼ����Ա(ʵ��) */
#define  TYPE_MR		9	/* �ʼ�������(ʵ����) */
#define  TYPE_NULL	10	/* Null (experimental) */
#define  TYPE_WKS    11	/* ������ sockets */
#define  TYPE_PTR    12	/* IP��ַ������� */
#define  TYPE_HINFO	13	/* ������Ϣ */
#define  TYPE_MINFO	14	/* ������Ϣ (experimental)*/
#define  TYPE_MX		15	/* �ʼ�������¼ */
#define  TYPE_TXT    16	/* �ı���Դ��¼ */
#define  TYPE_ANY    255/* ƥ���κ����� */

/* ����Э������ */
#define  CLASS_IN    1  /* The ARPA Internet */

/* ����ʱ����� */
#define  AGAIN       8	/* Average RTT gain = 1/8 */
#define  LAGAIN      3	/* Log2(AGAIN) */
#define  DGAIN       4  /* ƽ��ƫ������ = 1/4 */
#define  LDGAIN      2  /* log2(DGAIN) */
         
#define  IPPORT_DOMAIN	53

/* ��������Ϣ�ı�ͷ */
struct dhdr
{
	uint16  id;		/* id */
	uint8 	qr;		/* ��ѯ/��Ӧ */
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
