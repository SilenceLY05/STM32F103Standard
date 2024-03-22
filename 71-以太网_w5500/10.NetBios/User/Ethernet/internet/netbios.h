#ifndef __NETBIOS_H__
#define __NETBIOS_H__

#include "utility.h"


#define NETBIOS_W5500_NAME            "W5500"      //����NetBIOS����
#define NETBIOS_SOCK                  6            //����NetBIOS���õ�socket
#define NETBIOS_PORT                  137          //NetBIOS Name service ��Ĭ�϶˿�

#define NETBIOS_NAME_LEN              16           //NetBIOS���ֵ���󳤶�
#define NETBIOS_MSG_MAX_LEN           512          //NetBIOS���ĵ���󳤶�

#define NETBIOS_NAME_TTL              10           //NetBIOS��Ӧʱ��


/* NetBIOS header OPCODE+NM_FLAGS_RCODE */
#define NETB_HFLAG_RESPONSE           0x8000U      //OPCODE R=1 ��Ӧ���ݰ�
#define NETB_HFLAG_OPCODE             0x7800U      //����λ ����
#define NETB_HFLAG_OPCODE_NAME_QUERY  0x0000U      //����λ ����
#define NETB_HFLAG_AUTHORATIVE        0x0400U      //Ȩ���ش�λ OPCODE R=0 ����Ϊ0 R=1�Ҹ�λΪ1����Ӧ�Ľڵ���������Ȩ��
#define NETB_HFLAG_TRUNCATED          0x0200U      //��ֹλ����Ϣ���ȴ���576��1
#define NETB_HFLAG_RECURS_DESIRED     0x0100U      //�ݹ������־����״̬���Ƶ���Ӧ������1��NBNS������ѯע��ͷ���
#define NETB_HFLAG_RECURS_AVAILABLE   0x0080U      //�ݹ���ñ�־������Name Server����Ч����������Ϊ0
#define NETB_HFLAG_BROADCAST          0x0010U      //��λΪ1��ʾ�㲥���鲥��0��ʾ����
#define NETB_HFLAG_REPLYCODE          0x0008U      //����������λ
#define NETB_HFLAG_REPLYCODE_NOERROR  0x0000U      //�����޴�




/* NetBIOS RRname flag */
#define NETB_NFLAG_UNIQUE             0x8000U      //������
#define NETB_NFLAG_NODETYPE           0x6000U      //�ڵ�����
#define NETB_NFLAG_NODETYPE_HNODE     0x6000U      //����
#define NETB_NFLAG_NODETYPE_MNODE     0x4000U      //M�ڵ�
#define NETB_NFLAG_NODETYPE_PNODE     0x2000U      //P�ڵ�
#define NETB_NFLAG_NODETYPE_BNODE     0x0000U      //B�ڵ�



#pragma pack(1)
/* NetBIOS���ֱ��ĸ�ʽ */
typedef struct _NETBIOS_HDR{
	uint16 trans_id;         //����ID
	uint16 flags;            //ͨ�ñ�־
	uint16 questions;        //�����¼����
	uint16 answerRRs;        //�ش��¼����
	uint16 authorityRRs;     //Ȩ����¼����
	uint16 additionalRRs;    //���Ӽ�¼����
}NETBIOS_HDR;


/* NetBIOS ��Ϣ���ֱ��ĸ�ʽ */
typedef struct _NETBIOS_NAME_HDR{
	uint8 nametype;
	uint8 encname[(NETBIOS_NAME_LEN*2)+1];
	uint16 type;
	uint16 cls;
	uint32 ttl;
	uint16 datalen;
	uint16 flags;
	uint8 addr[4];
}NETBIOS_NAME_HDR;


/* NETBIOS���Ľṹ�� */
typedef struct _NETBIOS_RESP{
	NETBIOS_HDR resp_hdr;
	NETBIOS_NAME_HDR resp_name;
}NETBIOS_RESP;

#pragma pack()
	
void NetBIOS_Test(void);

#endif /* __NETBIOS_H__ */

