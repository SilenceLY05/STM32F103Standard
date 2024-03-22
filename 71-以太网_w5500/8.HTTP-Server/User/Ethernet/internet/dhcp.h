#ifndef __DHCP_H__
#define __DHCP_H__

#include "stdio.h"
#include "types.h"
#include "stm32f10x.h"

#define DEVICE_ID  "W5500"

typedef struct _DHCP_GET
{
	uint8 mac[6];
	uint8 lip[4];
	uint8 sub[4];
	uint8 gw[4];
	uint8 dns[4];
}DHCP_Get;

extern DHCP_Get DHCP_GET;
extern uint32   dhcp_time;



/* 使用UDP协议工作，统一使用两个IANA分配的端口 67服务器端 68客户端 */
#define DHCP_SERVER_PORT                      67//从服务器到客户
#define DHCP_CLIENT_PORT                      68//从客户到服务器


#define EXTERN_DHCP_MAC                       DHCP_GET.mac
#define EXTERN_DHCP_SN                        DHCP_GET.sub
#define EXTERN_DHCP_GW                        DHCP_GET.gw
#define EXTERN_DHCP_DNS                       DHCP_GET.dns
#define EXTERN_DHCP_NAME                      "iweb"   //ConfigMas.domain
#define EXTERN_DHCP_SIP                       DHCP_GET.lip


#define DHCP_RET_NONE                         0
#define DHCP_RET_ERR                          1
#define DHCP_RET_TIMEOUT                      2
#define DHCP_RET_UPDATE                       3
#define DHCP_RET_CONFLICT                     4

/* DHCP状态机 */
#define STATE_DHCP_READY                      0
#define STATE_DHCP_DISCOVER                   1
#define STATE_DHCP_REQUESET                   2
#define STATE_DHCP_LEASED                     3
#define STATE_DHCP_REREQUEST                  4
#define STATE_DHCP_RELEASE                    5


#define MAX_DHCP_RETRY                        3
#define DHCP_WAIT_TIME                        5


#define DEFAULT_LEASETIME                     0xFFFFFFFF  /* 无限租赁时间 */


/* DHCP基本报文格式 */
typedef struct _RIP_MSG
{
	uint8   op;                                //表示DHCP/BOOTP报文类型-取值1表示为BOOTREQUEST(Client发给Server),取2表示BOOTREPLY(Server发给Client)	
	uint8   htype;                             //取值为1表示10M以太网
	uint8   hlen;                              //取值6表示10M以太网
	uint8   hops;                              //Client通常设备为0
	uint32  xid;                               //Transaction ID，Client选择的随机数，交互需要保持一致
	uint16  secs;                              //由Client填写自Client开始地址获取或续订过程地址以来过去了几秒钟
	uint16  flags;                             //目前仅第一个bit使用，取1时标识报文为广播报文
	uint8   ciaddr[4];                         //Client IP address。仅当Client处于绑定、续订或重新绑定状态并且可以响应ARP请求时才填写
	uint8   yiaddr[4];                         //'your'(client)IP address。也即DHCP Server告知Client分配到的地址
	uint8   siaddr[4];                         //bootstrap引导所使用的下一个服务器的地址。在DHCPOFFER，DHCPACK中携带
	uint8   giaddr[4];                         //Relay agent IP address在中继场景下使用
	uint8   chaddr[16];                        //Client hardware address
	uint8   sname[64];                         //Server Host Name 可选的Server主机名，以Null结尾的字符串
	uint8   file[128];                         //Boot file name 以Null结尾
	uint8   OPT[312];                          //DHCP Client必须准备好接受长度至少为312*8字节的‘选项’字段的DHCP消息
}RIP_MSG;

#define MAX_DHCP_OPT                         16


/* DHCP报文OP代码 */
#define DHCP_BOOTREQUEST                     1
#define DHCP_BOOTREPLY                       2

/* DHCP报文Htype取值 */
#define DHCP_HTYPE10MB                       1
#define DHCP_HTYPE100MB                      2

/* DHCP报文Hlen取值 */
#define DHCP_HLEN                            6

/* DHCP报文HOPS取值 */
#define DHCP_HOPS                            0

/* DHCP报文默认XID取值 */
#define DHCP_DEFAULT_XID                     0x12345678

/* DHCP报文SECS取值 */
#define DHCP_SECS                            0

/* DHCP报文Flag表示为广播报文 */
#define DHCP_FLAGSBROADCAST                  0x8000

/* OPT前4个字节为magic cookie 通常为固定的 */
#define MAGIC_COOKIE                         0x63825363

/* Option 53 DHCP Message Type具体的类型 */
#define DHCP_DISCOVER                        1      //Client广播以发现Server存在
#define DHCP_OFFER                           2      //Server携带相应的网络参数回应Client的DISCOVER消息
#define DHCP_REQUEST                         3      //1.Client携带特定一台Server提供的网络参数用于隐式拒绝其他Server提供的网络参数 2.Client重启等情况，用于直接确认先前获得地址的可用性 3.动态分配方式下，Client用于租期续约
#define DHCP_DECLINE                         4      //Client向Server广播通知分配地址的不可用性
#define DHCP_ACK                             5      //Server携带网络参数向Client固化网络配置
#define DHCP_NAK                             6      //Server向Client表明获得地址的错误性
#define DHCP_RELIEASE                        7      //Client向Server单播通知撤销所分配地址并释放租约
#define DHCP_INFORM                          8      //Client向Server广播请求获取其他本地配置参数


/* Option Code */
enum
{
	padOption                                = 0,     //pad选项可用于使后续字段按单词边界对齐 代码为0，长度为一个8位字节
	subnetMask                               = 1,     //子网掩码选项，如果DHCP应答中同时指定了子网掩码和路由器选项，则子网掩码必须优先，代码为1，长度为4个字节
	timeOffset                               = 2,     //指定客户端子网与UTC的秒差，偏移量表示一个带符号的32位整数，代码为2，长度为4个字节
	routerOnSubnet                           = 3,     //为客户端子网上的路由器指定IP地址列表，路由器按优先顺序出。代码为3，最小长度是4字节，必须是4的整数倍
	timeServer                               = 4,     //时间服务器选项指定客户端可用的时间服务器列表，代码为4，最小长度是4字节，必须是4的整数倍
	nameServer                               = 5,     //名称服务器选项，指定客户端可用的名称服务器列表，代码为5，最小长度是4字节，必须是4的整数倍
	dns                                      = 6,     //域名服务器选项指定客户端可用的域名系统名称服务器列表，代码为6，最小长度是4字节，必须是4的整数倍
	logServer                                = 7,     //日志服务器选项指定客户端可用的入职服务器列表，代码为7，最小长度是4字节，必须是4的整数倍
	cookieServer                             = 8,     //指定客户端可用的cookie服务器列表，代码为8，最小长度是4字节，必须是4的整数倍
	lprServer                                = 9,     //指定客户端可用的打印机服务器列表，代码为9，最小长度是4字节，必须是4的整数倍
	impressServer                            = 10,    //指定客户端可用的Imagen Impress 服务器，代码为10， 最小长度是4字节，必须是4的整数倍
	resourcelocationServer                   = 11,    //指定客户端可用的资源位置服务器列表，代码为11，最小长度是4字节，必须是4的整数倍
	hostName                                 = 12,    //指定客户端名称，可以使用本地域名或不是本地域名，代码为12，最小长度为1
	bootfileSize                             = 13,    //指定客户端默认引导映像的长度，以512字节为单位，文件长度指定为无符号16位整数，代码13，长度为2
	meritdumpFile                            = 14,    //指定客户机崩溃时核心映像存储到该文件的路径名，代码是14，最小长度为1
	domainName                               = 15,    //指定客户端通过域名系统解析主机名时应使用的域名，代码是15，最小长度是1
	swapServer                               = 16,    //指定客户端的交换服务器的IP地址，代码为16，长度为4
	rootPath                                 = 17,    //指定客户端根据磁盘的路径名，代码为17，最小长度为1
	extensionsPath                           = 18,    //指定文件的字符串，代码是18，最小长度为1
	IPforwarding                             = 19,    //IP转发使能/禁用选项，取值为0表示禁止IP转发，取值为1表示启用IP转发，代码为19，长度为1
	nolocalsourceRouting                     = 20,    //是否将其IP层配置为允许通过非本地源路由转发数据报，0表示不允许，1表示允许，代码为20，长度为1
	policyFilter                             = 21,    //指定非本地源路由的策略过滤器，过滤器由IP地址和掩码列表组成代码为21，最小长度为8，长度必须是8的倍数
	maxDgramReasmSize                        = 22,    //指定应该准备重新组装的最大数据报大小，指定为16位无符号整数，法定最小值为576，代码为22，长度是2
	defaultIPTTL                             = 23,    //指定应在传出数据报上使用的默认生存时间，取值为1~255，代码为23，长度为1
	pathMTUagingTimeout                      = 24,    //指定发现Path MTU值老化时使用的超时时间(秒为单位)，指定为32为无符号整数，代码为24，长度是4
  pathMTUplateauTable                      = 25,    //指定在执行MTU发现时使用的MTU大小表，格式为16位无符号整数列表，按从小到大排序，最小不能小于68，代码为25，最小长度为2，必须是2的倍数
  ifMTU                                    = 26,    //指定在接口上使用的MTU，MTU指定为16位无符号整数，最小值为68，代码为26，长度为2
  allSubnetsLocal                          = 27,    //是否可以客户端与IP网络的子网使用相同的MTU，取值1表示相同，代码为27，长度是1
  broadcastAddr                            = 28,    //指定客户端子网上使用的广播地址，代码为28，长度是4
  performMaskDiscovery                     = 29,    //指定客户端是否使用ICMP执行子网掩码发现，1表示使用，代码为29，长度为1
  maskSupplier                             = 30,    //指定客户端是否使用ICMP响应子网掩码请求，1表示响应，代码为30，长度为1
  performRouterDiscovery                   = 31,    //是否使用路由器发现机制请求路由器，其表示应该执行路由器发现，代码为31，长度为1
  routerSolicitationAddr                   = 32,    //指定客户端发送路由器请求的地址，代码为32，长度是4
  staticRoute                              = 33,    //指定客户端在其路由缓存中安装的静态路由列表，路由由目的地址和目的路由器组成，代码为33，长度为8
  trailerEncapsulation                     = 34,    //指定客户端在使用ARP协议时是否应该协商使用延迟，0表示不尝试，代码为34，长度是1
  arpCacheTimeout                          = 35,    //以秒为单位指定ARP缓存项的超时时间，指定为32位无符号整数，代码为35，长度是4
  ethernetEncapsulation                    = 36,    //如果接口是以太网，是否使用RFC894或1042封装，0用894,1用1042，代码为36，长度是1
  tcpDefaultTTL                            = 37,    //在发送TCP段时应该使用默认的TTL，该值为8位无符号整数，最小值是1，代码为37，长度是1
  tcpKeepaliveInterval                     = 38,    //客户端TCP在TCP连接上发送keepalive消息前应等待的时间，时间为32位无符号整数，值为0表示不应该，代码为38，长度是4
  tcpKeepaliveGarbage                      = 39,    //指定客户端是否发送带有8字节垃圾的TCP keepalive消息，0表示不发送1表示发送一个字节，代码是39，长度是1
  nisDomainName                            = 40,    //指定客户端的NIS[17]域名称，代码为40，最小长度是1
  nisServers                               = 41,    //指定一个IP地址列表，指示客户机可用的NIS服务器，代码是41，最小长度是4，必须是4的倍数
  ntpServers                               = 42,    //指定一个IP地址列表，指示客户机可用的NTP[18]服务器，代码是41，最小长度是4，必须是4的倍数
  vendorSpecificInfo                       = 43,    //客户端和服务器交换特定域供应商的信息，代码为43，长度为1
  netBIOSnameServer                        = 44,    //NetBIOS名称服务器选项，代码为44，最小长度是4，必须是4的倍数
	netBIOSdgramDistServer                   = 45,    //NetBIOS数据报分发服务器选项，代码为45，最小长度是4，必须是4的倍数
	netBIOSnodeType                          = 46,    //NETBIOS节点类型选项允许NetBIOS通过TCP、IP客户端进行配置，代码是46，长度是1
  netBIOSscope                             = 47,    //NetBIOS scope选项指定客户端的NetBIOS over TCP/IP作用域参数,代码为47，最小长度为1
  xFontServer                              = 48,    //指定客户端可用的X Window System [21] Font服务器列表，代码为48，最小长度是4，必须是4的倍数
  xDisplayManager                          = 49,    //指定正在运行X Window System Display Manager并且对客户机可用的系统的IP地址列表，代码为49，最小长度是4，必须是4的倍数
  dhcpRequestedIPaddr                      = 50,    //用于客户端请求，允许客户端请求分配特定的IP地址，代码是50，长度是4
  dhcpIPaddrLeaseTime                      = 51,    //定义客户端请求IP地址的租用时间，以秒为单位，32位无符号整数，代码是51，长度是4
  dhcpOptionOverload                       = 52,    //指示DHCP的"sname""file"字段，代码是52，长度是1，值为1表示“file”字段用于保存选项2 .“sname”字段用于保存选项3 .两个字段都用于保存选项
  dhcpMessageType                          = 53,    //传递DHCP报文类型，代码为53，长度是1,
  dhcpServerIdentifier                     = 54,    //客户端通过在DHCPREQUEST消息中包含此选项来指示正在接受的几个租约提议中的哪一个,标识符为IP地址，代码为54，长度为4
  dhcpParamRequest                         = 55,    //DHCP客户端请求指定配置参数的值，代码为55，最小长度是1
  dhcpMsg                                  = 56,    //指示客户端拒绝提供参数的原因，代码是56，最小长度是1
  dhcpMaxMsgSize                           = 57,    //愿意接受DHCP消息的最大长度，长度为16位无符号整数，代码是57，长度是2
  dhcpT1value                              = 58,    //指定从地址分配到客户端转换到renew状态的时间间隔，32位无符号整型，单位s，代码是58，长度是4
  dhcpT2value                              = 59,    //指定从地址分配到客户端转换到REBINDING状态的时间间隔。32位无符号整型，单位s，代码是58，长度是4
  dhcpClassIdentifier                      = 60,    //标识DHCP客户端的类型和配置，代码为60，最小长度为1
  dhcpClientIdentifier                     = 61,    //DHCP客户端指定其唯一标识符。用这个值来索引地址绑定数据库，代码为61，最小长度为2
	endOption                                = 255    //标记了供应商字段中有效信息的结束，后续的8位用pad填充，代码为255，长度为一个8位字节
};


void DHCP_ResetTime(void);
void DHCP_SendDiscover(void);
void DHCP_SendRequest(void);
void DHCP_SendReleaseDecline(char msgtype);
uint8 DHCP_ParseMessage(uint16 length);
uint8 DHCP_CheckState(SOCKET s);
void DHCP_CheckTimeout(void);
uint8 Check_LeaseIP(void);
void DHCP_TimerHandler(void);
void DHCP_InitClient(void);
void Do_DHCP(void);



#endif /* __DHCP_H__ */

