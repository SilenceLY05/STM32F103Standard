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


/* DNS数组最大值 */
#define MAX_DNS_BUFF_SIZE           256

#define INITRTT                     2000L   /* 初始平滑响应时间 */
#define MAXCNAME                    10      /* cname递归的最大数量 */


/* 资源记录类型 */
#define TYPE_A       1  //将域名解析为IPv4
#define TYPE_NS      2  //指定用于该域的DNS名称服务器
#define TYPE_MD      3  //邮件目的地(已废弃,MX替代)
#define TYPE_MF      4  //邮件转发器域名(已废弃,MX替代)
#define TYPE_CNAME   5  //为一个域名设置别名
#define TYPE_SOA     6  //包含关于DNS区域的授权信息
#define TYPE_MB      7  //指定邮箱域名(已过时，很少使用)
#define TYPE_MG      8  //指定邮箱组成员(已过时，很少使用)
#define TYPE_MR      9  //重命名邮箱域名(已过时，很少用)
#define TYPE_NULL    10 //用于某些实验
#define TYPE_WKS     11 //描述已知服务的类型(已过时)
#define TYPE_PTR     12 //用于IP地址到域名的反向查询
#define TYPE_HINFO   13 //提供有关主机（服务器）硬件和操作系统的信息
#define TYPE_MINFO   14 //提供邮箱或邮箱列表的信息（已过时，很少使用）
#define TYPE_MX      15 //用于邮件转发
#define TYPE_TXT     16 //存储与域名相关的任何文本信息
#define TYPE_ANY     255//用于查询所有的记录类型



/* 网络协议类型 */
#define CLASS_IN     1  //IN就是Internet


/* 往返时间参数 */
#define AGAIN        8  //Average RTT gain = 1/8 
#define LAGAIN       3  //Log2(AGAIN)
#define DGAIN        4  //平均偏差增益 = 1/4
#define LDGAIN       2  //log2(DGAIN)

#define IPPORT_DOMAIN   53



/* 所有DNS报文的首部内容 */
struct DNSHMG
{
	uint16       id;      /* 事务ID */
	/* 报文中的标志字段 */
	uint8        qr;      /* 查询/响应标志 */
#define QUERY        0  //查询
#define RESPONSE     1  //响应
	uint8    opcode;      /* 查询类型 */
#define NQUERY       0  //标准查询
#define IQUERY       1  //反向查询
#define STAQUERY     2  //服务器状态请求
	uint8        aa;      /* 授权应答，1时表示权威服务器，0时表示非权威 */
	uint8        tc;      /* 是否被截断，1时表示超过512字节并被截断，只返回前512字节 */
	uint8        rd;      /* 期望递归，查询中设置，响应中返回 1递归查询，0迭代查询 */
	uint8        ra;      /* 可用递归 只出现在响应中，为1表示支持递归查询*/
	uint8     rcode;      /* 返回码表示响应差错状态 */
#define NO_ERROR     0  //表示没有错误
#define FORMAT_ERROR 1  //服务器无法解析请求
#define SERVER_ERROR 2  //服务器遇到错误，不能处理
#define NAME_ERROR   3  //相应的域名不存在
#define NOT_IMPL     4  //不支持请求类型
#define REFUSED      5  //服务器拒绝执行请求
	/* 报文中的问题计数字段 */
	uint16   qcount;      //DNS查询请求的数目
	/* 报文中的回答资源计数字段 */
	uint16   acount;      //DNS响应的数目
	/* 报文中的权威服务器计数字段 */
	uint16  nscount;      //权威名称服务器的数目
	/* 报文中的附加资源计数字段 */
	uint16  arcount;      //额外的记录数目(权威名称服务器对应IP地址书数目)
	
};



int DNS_MakeQuery(uint16 op,uint8 *name,uint8 *buff,uint16 len);
int Parse_Name(uint8 *msg,uint8 *compressed,uint16 len);
uint8 *DNS_Question(uint8 *msg,uint8 *cp);
uint8 *DNS_Answer(uint8 *msg,uint8 *cp);
uint8 parseMSG(struct DNSHMG *pdhdr,uint8 *pbuff);
uint8 DNS_Query(uint8 s,uint8*name);
void DNS_Test(void);


#endif  /* __DNS_H__ */

