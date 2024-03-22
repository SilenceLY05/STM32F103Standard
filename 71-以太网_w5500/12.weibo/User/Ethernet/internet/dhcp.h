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



/* ʹ��UDPЭ�鹤����ͳһʹ������IANA����Ķ˿� 67�������� 68�ͻ��� */
#define DHCP_SERVER_PORT                      67//�ӷ��������ͻ�
#define DHCP_CLIENT_PORT                      68//�ӿͻ���������


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

/* DHCP״̬�� */
#define STATE_DHCP_READY                      0
#define STATE_DHCP_DISCOVER                   1
#define STATE_DHCP_REQUESET                   2
#define STATE_DHCP_LEASED                     3
#define STATE_DHCP_REREQUEST                  4
#define STATE_DHCP_RELEASE                    5


#define MAX_DHCP_RETRY                        3
#define DHCP_WAIT_TIME                        5


#define DEFAULT_LEASETIME                     0xFFFFFFFF  /* ��������ʱ�� */


/* DHCP�������ĸ�ʽ */
typedef struct _RIP_MSG
{
	uint8   op;                                //��ʾDHCP/BOOTP��������-ȡֵ1��ʾΪBOOTREQUEST(Client����Server),ȡ2��ʾBOOTREPLY(Server����Client)	
	uint8   htype;                             //ȡֵΪ1��ʾ10M��̫��
	uint8   hlen;                              //ȡֵ6��ʾ10M��̫��
	uint8   hops;                              //Clientͨ���豸Ϊ0
	uint32  xid;                               //Transaction ID��Clientѡ����������������Ҫ����һ��
	uint16  secs;                              //��Client��д��Client��ʼ��ַ��ȡ���������̵�ַ������ȥ�˼�����
	uint16  flags;                             //Ŀǰ����һ��bitʹ�ã�ȡ1ʱ��ʶ����Ϊ�㲥����
	uint8   ciaddr[4];                         //Client IP address������Client���ڰ󶨡����������°�״̬���ҿ�����ӦARP����ʱ����д
	uint8   yiaddr[4];                         //'your'(client)IP address��Ҳ��DHCP Server��֪Client���䵽�ĵ�ַ
	uint8   siaddr[4];                         //bootstrap������ʹ�õ���һ���������ĵ�ַ����DHCPOFFER��DHCPACK��Я��
	uint8   giaddr[4];                         //Relay agent IP address���м̳�����ʹ��
	uint8   chaddr[16];                        //Client hardware address
	uint8   sname[64];                         //Server Host Name ��ѡ��Server����������Null��β���ַ���
	uint8   file[128];                         //Boot file name ��Null��β
	uint8   OPT[312];                          //DHCP Client����׼���ý��ܳ�������Ϊ312*8�ֽڵġ�ѡ��ֶε�DHCP��Ϣ
}RIP_MSG;

#define MAX_DHCP_OPT                         16


/* DHCP����OP���� */
#define DHCP_BOOTREQUEST                     1
#define DHCP_BOOTREPLY                       2

/* DHCP����Htypeȡֵ */
#define DHCP_HTYPE10MB                       1
#define DHCP_HTYPE100MB                      2

/* DHCP����Hlenȡֵ */
#define DHCP_HLEN                            6

/* DHCP����HOPSȡֵ */
#define DHCP_HOPS                            0

/* DHCP����Ĭ��XIDȡֵ */
#define DHCP_DEFAULT_XID                     0x12345678

/* DHCP����SECSȡֵ */
#define DHCP_SECS                            0

/* DHCP����Flag��ʾΪ�㲥���� */
#define DHCP_FLAGSBROADCAST                  0x8000

/* OPTǰ4���ֽ�Ϊmagic cookie ͨ��Ϊ�̶��� */
#define MAGIC_COOKIE                         0x63825363

/* Option 53 DHCP Message Type��������� */
#define DHCP_DISCOVER                        1      //Client�㲥�Է���Server����
#define DHCP_OFFER                           2      //ServerЯ����Ӧ�����������ӦClient��DISCOVER��Ϣ
#define DHCP_REQUEST                         3      //1.ClientЯ���ض�һ̨Server�ṩ���������������ʽ�ܾ�����Server�ṩ��������� 2.Client���������������ֱ��ȷ����ǰ��õ�ַ�Ŀ����� 3.��̬���䷽ʽ�£�Client����������Լ
#define DHCP_DECLINE                         4      //Client��Server�㲥֪ͨ�����ַ�Ĳ�������
#define DHCP_ACK                             5      //ServerЯ�����������Client�̻���������
#define DHCP_NAK                             6      //Server��Client������õ�ַ�Ĵ�����
#define DHCP_RELIEASE                        7      //Client��Server����֪ͨ�����������ַ���ͷ���Լ
#define DHCP_INFORM                          8      //Client��Server�㲥�����ȡ�����������ò���


/* Option Code */
enum
{
	padOption                                = 0,     //padѡ�������ʹ�����ֶΰ����ʱ߽���� ����Ϊ0������Ϊһ��8λ�ֽ�
	subnetMask                               = 1,     //��������ѡ����DHCPӦ����ͬʱָ�������������·����ѡ�����������������ȣ�����Ϊ1������Ϊ4���ֽ�
	timeOffset                               = 2,     //ָ���ͻ���������UTC����ƫ������ʾһ�������ŵ�32λ����������Ϊ2������Ϊ4���ֽ�
	routerOnSubnet                           = 3,     //Ϊ�ͻ��������ϵ�·����ָ��IP��ַ�б�·����������˳���������Ϊ3����С������4�ֽڣ�������4��������
	timeServer                               = 4,     //ʱ�������ѡ��ָ���ͻ��˿��õ�ʱ��������б�����Ϊ4����С������4�ֽڣ�������4��������
	nameServer                               = 5,     //���Ʒ�����ѡ�ָ���ͻ��˿��õ����Ʒ������б�����Ϊ5����С������4�ֽڣ�������4��������
	dns                                      = 6,     //����������ѡ��ָ���ͻ��˿��õ�����ϵͳ���Ʒ������б�����Ϊ6����С������4�ֽڣ�������4��������
	logServer                                = 7,     //��־������ѡ��ָ���ͻ��˿��õ���ְ�������б�����Ϊ7����С������4�ֽڣ�������4��������
	cookieServer                             = 8,     //ָ���ͻ��˿��õ�cookie�������б�����Ϊ8����С������4�ֽڣ�������4��������
	lprServer                                = 9,     //ָ���ͻ��˿��õĴ�ӡ���������б�����Ϊ9����С������4�ֽڣ�������4��������
	impressServer                            = 10,    //ָ���ͻ��˿��õ�Imagen Impress ������������Ϊ10�� ��С������4�ֽڣ�������4��������
	resourcelocationServer                   = 11,    //ָ���ͻ��˿��õ���Դλ�÷������б�����Ϊ11����С������4�ֽڣ�������4��������
	hostName                                 = 12,    //ָ���ͻ������ƣ�����ʹ�ñ����������Ǳ�������������Ϊ12����С����Ϊ1
	bootfileSize                             = 13,    //ָ���ͻ���Ĭ������ӳ��ĳ��ȣ���512�ֽ�Ϊ��λ���ļ�����ָ��Ϊ�޷���16λ����������13������Ϊ2
	meritdumpFile                            = 14,    //ָ���ͻ�������ʱ����ӳ��洢�����ļ���·������������14����С����Ϊ1
	domainName                               = 15,    //ָ���ͻ���ͨ������ϵͳ����������ʱӦʹ�õ�������������15����С������1
	swapServer                               = 16,    //ָ���ͻ��˵Ľ�����������IP��ַ������Ϊ16������Ϊ4
	rootPath                                 = 17,    //ָ���ͻ��˸��ݴ��̵�·����������Ϊ17����С����Ϊ1
	extensionsPath                           = 18,    //ָ���ļ����ַ�����������18����С����Ϊ1
	IPforwarding                             = 19,    //IPת��ʹ��/����ѡ�ȡֵΪ0��ʾ��ֹIPת����ȡֵΪ1��ʾ����IPת��������Ϊ19������Ϊ1
	nolocalsourceRouting                     = 20,    //�Ƿ���IP������Ϊ����ͨ���Ǳ���Դ·��ת�����ݱ���0��ʾ������1��ʾ��������Ϊ20������Ϊ1
	policyFilter                             = 21,    //ָ���Ǳ���Դ·�ɵĲ��Թ���������������IP��ַ�������б���ɴ���Ϊ21����С����Ϊ8�����ȱ�����8�ı���
	maxDgramReasmSize                        = 22,    //ָ��Ӧ��׼��������װ��������ݱ���С��ָ��Ϊ16λ�޷���������������СֵΪ576������Ϊ22��������2
	defaultIPTTL                             = 23,    //ָ��Ӧ�ڴ������ݱ���ʹ�õ�Ĭ������ʱ�䣬ȡֵΪ1~255������Ϊ23������Ϊ1
	pathMTUagingTimeout                      = 24,    //ָ������Path MTUֵ�ϻ�ʱʹ�õĳ�ʱʱ��(��Ϊ��λ)��ָ��Ϊ32Ϊ�޷�������������Ϊ24��������4
  pathMTUplateauTable                      = 25,    //ָ����ִ��MTU����ʱʹ�õ�MTU��С����ʽΪ16λ�޷��������б�����С����������С����С��68������Ϊ25����С����Ϊ2��������2�ı���
  ifMTU                                    = 26,    //ָ���ڽӿ���ʹ�õ�MTU��MTUָ��Ϊ16λ�޷�����������СֵΪ68������Ϊ26������Ϊ2
  allSubnetsLocal                          = 27,    //�Ƿ���Կͻ�����IP���������ʹ����ͬ��MTU��ȡֵ1��ʾ��ͬ������Ϊ27��������1
  broadcastAddr                            = 28,    //ָ���ͻ���������ʹ�õĹ㲥��ַ������Ϊ28��������4
  performMaskDiscovery                     = 29,    //ָ���ͻ����Ƿ�ʹ��ICMPִ���������뷢�֣�1��ʾʹ�ã�����Ϊ29������Ϊ1
  maskSupplier                             = 30,    //ָ���ͻ����Ƿ�ʹ��ICMP��Ӧ������������1��ʾ��Ӧ������Ϊ30������Ϊ1
  performRouterDiscovery                   = 31,    //�Ƿ�ʹ��·�������ֻ�������·���������ʾӦ��ִ��·�������֣�����Ϊ31������Ϊ1
  routerSolicitationAddr                   = 32,    //ָ���ͻ��˷���·��������ĵ�ַ������Ϊ32��������4
  staticRoute                              = 33,    //ָ���ͻ�������·�ɻ����а�װ�ľ�̬·���б�·����Ŀ�ĵ�ַ��Ŀ��·������ɣ�����Ϊ33������Ϊ8
  trailerEncapsulation                     = 34,    //ָ���ͻ�����ʹ��ARPЭ��ʱ�Ƿ�Ӧ��Э��ʹ���ӳ٣�0��ʾ�����ԣ�����Ϊ34��������1
  arpCacheTimeout                          = 35,    //����Ϊ��λָ��ARP������ĳ�ʱʱ�䣬ָ��Ϊ32λ�޷�������������Ϊ35��������4
  ethernetEncapsulation                    = 36,    //����ӿ�����̫�����Ƿ�ʹ��RFC894��1042��װ��0��894,1��1042������Ϊ36��������1
  tcpDefaultTTL                            = 37,    //�ڷ���TCP��ʱӦ��ʹ��Ĭ�ϵ�TTL����ֵΪ8λ�޷�����������Сֵ��1������Ϊ37��������1
  tcpKeepaliveInterval                     = 38,    //�ͻ���TCP��TCP�����Ϸ���keepalive��ϢǰӦ�ȴ���ʱ�䣬ʱ��Ϊ32λ�޷���������ֵΪ0��ʾ��Ӧ�ã�����Ϊ38��������4
  tcpKeepaliveGarbage                      = 39,    //ָ���ͻ����Ƿ��ʹ���8�ֽ�������TCP keepalive��Ϣ��0��ʾ������1��ʾ����һ���ֽڣ�������39��������1
  nisDomainName                            = 40,    //ָ���ͻ��˵�NIS[17]�����ƣ�����Ϊ40����С������1
  nisServers                               = 41,    //ָ��һ��IP��ַ�б�ָʾ�ͻ������õ�NIS��������������41����С������4��������4�ı���
  ntpServers                               = 42,    //ָ��һ��IP��ַ�б�ָʾ�ͻ������õ�NTP[18]��������������41����С������4��������4�ı���
  vendorSpecificInfo                       = 43,    //�ͻ��˺ͷ����������ض���Ӧ�̵���Ϣ������Ϊ43������Ϊ1
  netBIOSnameServer                        = 44,    //NetBIOS���Ʒ�����ѡ�����Ϊ44����С������4��������4�ı���
	netBIOSdgramDistServer                   = 45,    //NetBIOS���ݱ��ַ�������ѡ�����Ϊ45����С������4��������4�ı���
	netBIOSnodeType                          = 46,    //NETBIOS�ڵ�����ѡ������NetBIOSͨ��TCP��IP�ͻ��˽������ã�������46��������1
  netBIOSscope                             = 47,    //NetBIOS scopeѡ��ָ���ͻ��˵�NetBIOS over TCP/IP���������,����Ϊ47����С����Ϊ1
  xFontServer                              = 48,    //ָ���ͻ��˿��õ�X Window System [21] Font�������б�����Ϊ48����С������4��������4�ı���
  xDisplayManager                          = 49,    //ָ����������X Window System Display Manager���ҶԿͻ������õ�ϵͳ��IP��ַ�б�����Ϊ49����С������4��������4�ı���
  dhcpRequestedIPaddr                      = 50,    //���ڿͻ�����������ͻ�����������ض���IP��ַ��������50��������4
  dhcpIPaddrLeaseTime                      = 51,    //����ͻ�������IP��ַ������ʱ�䣬����Ϊ��λ��32λ�޷���������������51��������4
  dhcpOptionOverload                       = 52,    //ָʾDHCP��"sname""file"�ֶΣ�������52��������1��ֵΪ1��ʾ��file���ֶ����ڱ���ѡ��2 .��sname���ֶ����ڱ���ѡ��3 .�����ֶζ����ڱ���ѡ��
  dhcpMessageType                          = 53,    //����DHCP�������ͣ�����Ϊ53��������1,
  dhcpServerIdentifier                     = 54,    //�ͻ���ͨ����DHCPREQUEST��Ϣ�а�����ѡ����ָʾ���ڽ��ܵļ�����Լ�����е���һ��,��ʶ��ΪIP��ַ������Ϊ54������Ϊ4
  dhcpParamRequest                         = 55,    //DHCP�ͻ�������ָ�����ò�����ֵ������Ϊ55����С������1
  dhcpMsg                                  = 56,    //ָʾ�ͻ��˾ܾ��ṩ������ԭ�򣬴�����56����С������1
  dhcpMaxMsgSize                           = 57,    //Ը�����DHCP��Ϣ����󳤶ȣ�����Ϊ16λ�޷���������������57��������2
  dhcpT1value                              = 58,    //ָ���ӵ�ַ���䵽�ͻ���ת����renew״̬��ʱ������32λ�޷������ͣ���λs��������58��������4
  dhcpT2value                              = 59,    //ָ���ӵ�ַ���䵽�ͻ���ת����REBINDING״̬��ʱ������32λ�޷������ͣ���λs��������58��������4
  dhcpClassIdentifier                      = 60,    //��ʶDHCP�ͻ��˵����ͺ����ã�����Ϊ60����С����Ϊ1
  dhcpClientIdentifier                     = 61,    //DHCP�ͻ���ָ����Ψһ��ʶ���������ֵ��������ַ�����ݿ⣬����Ϊ61����С����Ϊ2
	endOption                                = 255    //����˹�Ӧ���ֶ�����Ч��Ϣ�Ľ�����������8λ��pad��䣬����Ϊ255������Ϊһ��8λ�ֽ�
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

