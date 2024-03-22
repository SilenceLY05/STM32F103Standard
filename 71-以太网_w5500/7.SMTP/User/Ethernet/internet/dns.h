#ifndef __DNS_H__
#define __DNS_H__

#include "types.h"


extern uint8 BUFFPUB[1024];

#define DNS_RESPONSE_TIMEOUT        3   //3s
#define DNS_RETRY                   3   //3times
#define DNS_RET_PROGRESS            0
#define DNS_RET_FAIL                1
#define DNS_RET_SUCCESS             2

#define EXTERN_DNS_SERVERIP         ConfigMsg.dns
#define EXTERN_DNS_SOCKBUFF         gBUFFPUBLIC
#define EXTERN_DNS_ANYPORT          2000


/* DNS�������ֵ */
#define MAX_DNS_BUFF_SIZE           256

#define INITRTT                     2000L   /* ��ʼƽ����Ӧʱ�� */
#define MAXCNAME                    10      /* cname�ݹ��������� */


/* ��Դ��¼���� */
#define TYPE_A       1  //����������ΪIPv4
#define TYPE_NS      2  //ָ�����ڸ����DNS���Ʒ�����
#define TYPE_MD      3  //�ʼ�Ŀ�ĵ�(�ѷ���,MX���)
#define TYPE_MF      4  //�ʼ�ת��������(�ѷ���,MX���)
#define TYPE_CNAME   5  //Ϊһ���������ñ���
#define TYPE_SOA     6  //��������DNS�������Ȩ��Ϣ
#define TYPE_MB      7  //ָ����������(�ѹ�ʱ������ʹ��)
#define TYPE_MG      8  //ָ���������Ա(�ѹ�ʱ������ʹ��)
#define TYPE_MR      9  //��������������(�ѹ�ʱ��������)
#define TYPE_NULL    10 //����ĳЩʵ��
#define TYPE_WKS     11 //������֪���������(�ѹ�ʱ)
#define TYPE_PTR     12 //����IP��ַ�������ķ����ѯ
#define TYPE_HINFO   13 //�ṩ�й���������������Ӳ���Ͳ���ϵͳ����Ϣ
#define TYPE_MINFO   14 //�ṩ����������б����Ϣ���ѹ�ʱ������ʹ�ã�
#define TYPE_MX      15 //�����ʼ�ת��
#define TYPE_TXT     16 //�洢��������ص��κ��ı���Ϣ
#define TYPE_ANY     255//���ڲ�ѯ���еļ�¼����



/* ����Э������ */
#define CLASS_IN     1  //IN����Internet


/* ����ʱ����� */
#define AGAIN        8  //Average RTT gain = 1/8 
#define LAGAIN       3  //Log2(AGAIN)
#define DGAIN        4  //ƽ��ƫ������ = 1/4
#define LDGAIN       2  //log2(DGAIN)

#define IPPORT_DOMAIN   53



/* ����DNS���ĵ��ײ����� */
struct DNSHMG
{
	uint16       id;      /* ����ID */
	/* �����еı�־�ֶ� */
	uint8        qr;      /* ��ѯ/��Ӧ��־ */
#define QUERY        0  //��ѯ
#define RESPONSE     1  //��Ӧ
	uint8    opcode;      /* ��ѯ���� */
#define NQUERY       0  //��׼��ѯ
#define IQUERY       1  //�����ѯ
#define STAQUERY     2  //������״̬����
	uint8        aa;      /* ��ȨӦ��1ʱ��ʾȨ����������0ʱ��ʾ��Ȩ�� */
	uint8        tc;      /* �Ƿ񱻽ضϣ�1ʱ��ʾ����512�ֽڲ����ضϣ�ֻ����ǰ512�ֽ� */
	uint8        rd;      /* �����ݹ飬��ѯ�����ã���Ӧ�з��� 1�ݹ��ѯ��0������ѯ */
	uint8        ra;      /* ���õݹ� ֻ��������Ӧ�У�Ϊ1��ʾ֧�ֵݹ��ѯ*/
	uint8     rcode;      /* �������ʾ��Ӧ���״̬ */
#define NO_ERROR     0  //��ʾû�д���
#define FORMAT_ERROR 1  //�������޷���������
#define SERVER_ERROR 2  //�������������󣬲��ܴ���
#define NAME_ERROR   3  //��Ӧ������������
#define NOT_IMPL     4  //��֧����������
#define REFUSED      5  //�������ܾ�ִ������
	/* �����е���������ֶ� */
	uint16   qcount;      //DNS��ѯ�������Ŀ
	/* �����еĻش���Դ�����ֶ� */
	uint16   acount;      //DNS��Ӧ����Ŀ
	/* �����е�Ȩ�������������ֶ� */
	uint16  nscount;      //Ȩ�����Ʒ���������Ŀ
	/* �����еĸ�����Դ�����ֶ� */
	uint16  arcount;      //����ļ�¼��Ŀ(Ȩ�����Ʒ�������ӦIP��ַ����Ŀ)
	
};



int DNS_MakeQuery(uint16 op,uint8 *name,uint8 *buff,uint16 len);
int Parse_Name(uint8 *msg,uint8 *compressed,uint16 len);
uint8 *DNS_Question(uint8 *msg,uint8 *cp);
uint8 *DNS_Answer(uint8 *msg,uint8 *cp);
uint8 parseMSG(struct DNSHMG *pdhdr,uint8 *pbuff);
uint8 DNS_Query(uint8 s,uint8*name);
void DNS_Test(void);


#endif  /* __DNS_H__ */

