#ifndef __NETBIOS_H__
#define __NETBIOS_H__

#include "utility.h"


#define NETBIOS_W5500_NAME            "W5500"      //定义NetBIOS名字
#define NETBIOS_SOCK                  6            //定义NetBIOS采用的socket
#define NETBIOS_PORT                  137          //NetBIOS Name service 的默认端口

#define NETBIOS_NAME_LEN              16           //NetBIOS名字的最大长度
#define NETBIOS_MSG_MAX_LEN           512          //NetBIOS报文的最大长度

#define NETBIOS_NAME_TTL              10           //NetBIOS响应时间


/* NetBIOS header OPCODE+NM_FLAGS_RCODE */
#define NETB_HFLAG_RESPONSE           0x8000U      //OPCODE R=1 响应数据包
#define NETB_HFLAG_OPCODE             0x7800U      //操作位 更新
#define NETB_HFLAG_OPCODE_NAME_QUERY  0x0000U      //操作位 请求
#define NETB_HFLAG_AUTHORATIVE        0x0400U      //权威回答位 OPCODE R=0 必须为0 R=1且该位为1则响应的节点是域名的权威
#define NETB_HFLAG_TRUNCATED          0x0200U      //截止位，消息长度大于576置1
#define NETB_HFLAG_RECURS_DESIRED     0x0100U      //递归所需标志，将状态复制到响应包，置1，NBNS迭代查询注册和发布
#define NETB_HFLAG_RECURS_AVAILABLE   0x0080U      //递归可用标志，仅在Name Server中有效，其他必须为0
#define NETB_HFLAG_BROADCAST          0x0010U      //该位为1表示广播或组播，0表示单播
#define NETB_HFLAG_REPLYCODE          0x0008U      //请求结果代码位
#define NETB_HFLAG_REPLYCODE_NOERROR  0x0000U      //请求无错




/* NetBIOS RRname flag */
#define NETB_NFLAG_UNIQUE             0x8000U      //组名称
#define NETB_NFLAG_NODETYPE           0x6000U      //节点类型
#define NETB_NFLAG_NODETYPE_HNODE     0x6000U      //保留
#define NETB_NFLAG_NODETYPE_MNODE     0x4000U      //M节点
#define NETB_NFLAG_NODETYPE_PNODE     0x2000U      //P节点
#define NETB_NFLAG_NODETYPE_BNODE     0x0000U      //B节点



#pragma pack(1)
/* NetBIOS名字报文格式 */
typedef struct _NETBIOS_HDR{
	uint16 trans_id;         //事务ID
	uint16 flags;            //通用标志
	uint16 questions;        //问题记录个数
	uint16 answerRRs;        //回答记录个数
	uint16 authorityRRs;     //权威记录个数
	uint16 additionalRRs;    //附加记录个数
}NETBIOS_HDR;


/* NetBIOS 信息名字报文格式 */
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


/* NETBIOS报文结构体 */
typedef struct _NETBIOS_RESP{
	NETBIOS_HDR resp_hdr;
	NETBIOS_NAME_HDR resp_name;
}NETBIOS_RESP;

#pragma pack()
	
void NetBIOS_Test(void);

#endif /* __NETBIOS_H__ */

