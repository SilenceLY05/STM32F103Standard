#ifndef __BSP_W5500_H__
#define __BSP_W5500_H__


#include "stm32f10x.h"
#include "types.h"

/********************************** 通用寄存器地址偏移 ************************************/
/* 模式寄存器 */
#define MR                                         (0x000000)
/* 网关IP地址寄存器 (设置默认网关地址) */
#define GAR0                                       (0x000100)
#define GAR1                                       (0x000200)
#define GAR2                                       (0x000300)
#define GAR3                                       (0x000400)
/* 子网掩码寄存器 (设置子网掩码地址) */
#define SUBR0                                      (0x000500)
#define SUBR1                                      (0x000600)
#define SUBR2                                      (0x000700)
#define SUBR3                                      (0x000800)
/* 源MAC地址寄存器 */
#define SHAR0                                      (0x000900)
#define SHAR1                                      (0x000A00)
#define SHAR2                                      (0x000B00)
#define SHAR3                                      (0x000C00)
#define SHAR4                                      (0x000D00)
#define SHAR5                                      (0x000E00)
/* 源IP地址寄存器 */
#define SIPR0                                      (0x000F00)
#define SIPR1                                      (0x001000)
#define SIPR2                                      (0x001100)
#define SIPR3                                      (0x001200)
/* 低电平中断寄存器地址 */
#define INTLEVEL0                                  (0x001300)
#define INTLEVEL1                                  (0x001400)
/* 中断寄存器 */
#define IR                                         (0x001500)
/* 中断屏蔽寄存器 */
#define IMR                                        (0x001600)
/* Socket中断寄存器 */
#define SIR                                        (0x001700) 
/* Socket 中断屏蔽寄存器 */
#define SIMR                                       (0x001800)
/* 重传超时寄存器( 1 is 100us )初始化时为2000即0x7D0 200毫秒 */
#define RTR0                                       (0x001900)
#define RTR1                                       (0x001A00)
/* 重试计数寄存器 设置重传次数 */
#define WIZ_RCR                                    (0x001B00)
/* PPP连接控制协议请求定时寄存器 LCP Echo请求的期限 */
#define PTIMER                                     (0x001C00)
/* PPP连接控制协议幻数寄存器 配置LCP回应请求的4字节幻数 */
#define PMAGIC                                     (0x001D00)
/* PPPoE模式下目标MAC寄存器 写入PPPoE服务器所需的MAC地址 */
#define PDHAR0                                     (0x001E00)
#define PDHAR1                                     (0x001F00)
#define PDHAR2                                     (0x002000)
#define PDHAR3                                     (0x002100)
#define PDHAR4                                     (0x002200)
#define PDHAR5                                     (0x002300)
/* PPPoE模式下会话ID寄存器 填入PPPoE服务器会话ID */
#define PSID0                                      (0x002400)
#define PSID1                                      (0x002500)
/* PPPoE模式下最大接收单元 */
#define PMR0                                       (0x002600)
#define PMR1                                       (0x002700)
/* 无法抵达IP地址寄存器 */
#define UIPR0                                      (0x002800)
#define UIPR1                                      (0x002900)
#define UIPR2                                      (0x002A00)
#define UIPR3                                      (0x002B00)
/* 无法抵达端口寄存器 */
#define UPORT0                                     (0x002C00)
#define UPORT1                                     (0x002D00)
/* PHY配置寄存器 */
#define PHYCFGR                                    (0x002E00)
/* 芯片版本寄存器地址 */
#define VERSIONR                                   (0x003900)  



/********************************** socket寄存器地址偏移 ************************************/
/* socket n模式寄存器 */
#define Sn_MR(ch)                                  (0x000008+(ch<<5))
/* Socket n配置寄存器 */
#define Sn_CR(ch)                                  (0x000108 + (ch<<5))
/* 通道中断寄存器 */
#define Sn_IR(ch)                                  (0x000208 + (ch<<5))
/* 通道状态寄存器 */
#define Sn_SR(ch)                                  (0x000308 + (ch<<5))
/* 源端口寄存器 */
#define Sn_PORT0(ch)                               (0x000408 + (ch<<5))
#define Sn_PORT1(ch)                               (0x000508 + (ch<<5))
/* 目的MAC寄存器地址 */
#define Sn_DHAR0(ch)                               (0x000608 + (ch<<5))
#define Sn_DHAR1(ch)                               (0x000708 + (ch<<5))
#define Sn_DHAR2(ch)                               (0x000808 + (ch<<5))
#define Sn_DHAR3(ch)                               (0x000908 + (ch<<5))
#define Sn_DHAR4(ch)                               (0x000A08 + (ch<<5))
#define Sn_DHAR5(ch)                               (0x000B08 + (ch<<5))
/* 目标IP寄存器地址 */
#define Sn_DIPR0(ch)                               (0x000C08 + (ch<<5))
#define Sn_DIPR1(ch)                               (0x000D08 + (ch<<5))
#define Sn_DIPR2(ch)                               (0x000E08 + (ch<<5))
#define Sn_DIPR3(ch)                               (0x000F08 + (ch<<5))
/* 目标端口寄存器 */
#define Sn_DPORT0(ch)                              (0x001008 + (ch<<5))
#define Sn_DPORT1(ch)                              (0x001108 + (ch<<5))
/* 最大分段寄存器 */
#define Sn_MSSR0(ch)                               (0x001208 + (ch<<5))
#define Sn_MSSR1(ch)                               (0x001308 + (ch<<5))

#define Sn_PROTO(ch)                               (0x001408 + (ch<<5))
/* Socket IP服务类型寄存器 */
#define Sn_TOS(ch)                                 (0x001508 + (ch<<5))
/* Socket IP生存时间寄存器 */
#define Sn_TTL(ch)                                 (0x001608 + (ch<<5))
/* 接收缓存大小寄存器 */
#define Sn_RXBUF_SIZE(ch)                          (0x001E08 + (ch<<5))
/* 发送缓存大小寄存器 */
#define Sn_TXBUF_SIZE(ch)                          (0x001F08 + (ch<<5))
/* 空闲发送缓存寄存器 */
#define Sn_TX_FSR0(ch)                             (0x002008 + (ch<<5))
#define Sn_TX_FSR1(ch)                             (0x002108 + (ch<<5))
/* 发送读指针寄存器地址 */
#define Sn_TX_RD0(ch)                              (0x002208 + (ch<<5))
#define Sn_TX_RD1(ch)                              (0x002308 + (ch<<5))
/* 发送写指针寄存器地址 */
#define Sn_TX_WR0(ch)                              (0x002408 + (ch<<5))
#define Sn_TX_WR1(ch)                              (0x002508 + (ch<<5))
/* 空闲接收缓存寄存器 */
#define Sn_RX_RSR0(ch)                             (0x002608 + (ch<<5))
#define Sn_RX_RSR1(ch)                             (0x002708 + (ch<<5))
/* 接收读指针寄存器地址 */
#define Sn_RX_RD0(ch)                              (0x002808 + (ch<<5))
#define Sn_RX_RD1(ch)                              (0x002908 + (ch<<5))
/* 接收写指针寄存器地址 */
#define Sn_RX_WR0(ch)                              (0x002A08 + (ch<<5))
#define Sn_RX_WR1(ch)                              (0x002B08 + (ch<<5))
/* socket 中断屏蔽寄存器*/
#define Sn_IMR(ch)                                 (0x002C08 + (ch<<5))
/* 分段IP报头寄存器 */
#define Sn_FRAG(ch)                                (0x002D08 + (ch<<5))
/* Socket在线时间寄存器 */
#define Sn_KPALVTR(ch)                             (0x002F08 + (ch<<5))



/********************************** 通用寄存器—各寄存器 ************************************/
/* 模式寄存器 */
#define MR_RST                                     0x80       //内部寄存器初始化，复位后清零
#define MR_WOL                                     0x20       //网络唤醒，0关闭1开启 
#define MR_PB                                      0x10       //Ping Block模式 0关闭1开启 开启时ping请求没有响应
#define MR_PPPoE                                   0x08       //开启PPPoE模式 使用ADSL需要开启
#define MR_FARP                                    0x02       //强迫ARP模式 开启时无论是否发送数据都会强迫发送ARP请求

/* 中断寄存器 */
#define IR_CONFLICT                                0x80       //IP冲突 接收APR请求时，若发送方IP与本地IP重复置1
#define IR_UNREACH                                 0x40       //目标不可抵达 接收到ICMP包后置1 置1后通过UIPR和UPORTR可查询到IP地址和端口号等信息
#define IR_PPPoE                                   0x20       //PPPoE连接关闭 PPP0E模式下，断开PPPoE连接时生效
#define IR_MP                                      0x10       //Magic Packet 当网络唤醒模式启用并通过 UDP 接收到 Magic Packet 网络唤醒包时



/********************************** 通用寄存器—各寄存器 ************************************/
/* Socket模式寄存器 */
#define Sn_MR_MULTI                                0x80       //UDP模式 1开启多播
#define Sn_MR_BCASTB                               0x40       //MACRAW和UDP模式下的网络阻塞 1开启广播阻塞
#define Sn_MR_MC                                   0x20       //多播  1使用IGMP版本1 0使用IGMP版本2
#define Sn_MR_ND                                   0x20       //使用无延时ACK，1开启无延时ACK选项
#define Sn_MR_UCASTB                               0x10       //UDP模式下的单波阻塞 1开启单波阻塞
#define Sn_MR_PPPOE                                0x05       //PPPoE
#define Sn_MR_MACRAW                               0x04       //MAC LAYER RAW SOCK
#define Sn_MR_IPRAW                                0x03       //IP LAYER RAW SOCK
#define Sn_MR_UDP                                  0x02       //UDP
#define Sn_MR_TCP                                  0x01       //TCP
#define Sn_MR_CLOSE                                0x00       //不使用socket

/* Socket模式寄存器在MACRAW模式下 */
#define Sn_MR_MFEN                                 0x80       //在MACRAW模式下开启MAC地址过滤 1开启
#define Sn_MR_MMB                                  0x20       //MACRAW模式下的多播阻塞 1开启多播阻塞 IPv4
#define Sn_MR_MIP6B                                0x10       //MACRAW模式下IPv6包阻塞 1开启



/* Socket配置寄存器 */
#define Sn_CR_OPEN                                 0x01       //按照Sn_MR(P3:P0)的协议来初始化和打开
#define Sn_CR_LISTEN                               0x02       //(Sn_MR(P3:P0) = Sn_MR_TCP)下生效，被配置为一个TCP服务器，等待TCP连接请求
#define Sn_CR_CONNECT                              0x04       //在TPC模式下运行，向TCP服务器发送一个连接请求
#define Sn_CR_DISCON                               0x08       //在TCP模式下发送断开连接操作
#define Sn_CR_CLOSE                                0x10       //关闭Socket n   使用这种方法断开时没有断开过程
#define Sn_CR_SEND                                 0x20       //发送Socket n 发送内存中的所有缓冲数据
#define Sn_CR_SEND_MAC                             0x21       //只在UDP模式下有效，基本与SEND一致，不需要使用自动ARP请求
#define Sn_CR_SEND_KEEP                            0x22       //只有在TCP模式下有效，通过发送1字节在线心跳包来检查连接状态
#define Sn_CR_RECV                                 0x40       //通过接收读指针寄存器判断Socket n接收缓存是否完成


#ifdef __DEF_IINCHIP_PPP__
   #define Sn_CR_PCON                              0x23      
   #define Sn_CR_PDISCON                           0x24      
   #define Sn_CR_PCR                               0x25      
   #define Sn_CR_PCN                               0x26     
   #define Sn_CR_PCJ                               0x27     
#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
   #define Sn_IR_PRECV                             0x80     
   #define Sn_IR_PFAIL                             0x40     
   #define Sn_IR_PNEXT                             0x20     
#endif

/* Socket n中断寄存器 */
#define Sn_IR_SEND_OK                              0x10       //SENDOK中断，当SEND命令完成时，此位生效
#define Sn_IR_TIMEOUT                              0x08       //当ARP或者TCP超时被触发时，此位生效
#define Sn_IR_RECV                                 0x04       //无论何时，只要接收到了对方的数据，此位生效
#define Sn_IR_DISCON                               0x02       //当接收到对方的FIN 或 FIN/ACK包时，此位生效
#define Sn_IR_CON                                  0x01       //当成功与对方建立连接，且Sn_SR变成SOCK_ESTABLISHED状态时，此位生效


/* Socket n状态寄存器 */
#define Sn_SR_SOCK_CLOSE                           0x00       //表示Socket n处于关闭装填，资源被释放
#define Sn_SR_SOCK_INIT                            0x13       //Socket n端口打开并处于TCP工作模式
#define Sn_SR_SOCK_LISTEN                          0x14       //Socket n工作在TCP模式并等待TCP客户端的连接请求
#define Sn_SR_SOCK_SYNSENT                         0x15       //表示Socket n已经发送连接请求到对方
#define Sn_SR_SOCK_SYNRECV                         0x16       //表示Socket n成功的从对方收到了连接请求包
#define Sn_SR_SOCK_ESTABLISHED                     0x17       //表示Socket n工作状态 此状态下可以使用SEND或者RECV命令进行数据包传输
#define Sn_SR_SOCK_FIN_WAIT                        0x18       //表示Socket n正在关闭
#define Sn_SR_SOCK_CLOSING                         0x1A       //表示Socket n正在关闭
#define Sn_SR_SOCK_TIME_WAIT                       0x1B       //表示Socket n正在关闭
#define Sn_SR_SOCK_CLOSE_WAIT                      0x1C       //指示了 Socket n 接收到了来自连接对方发来的断开连接请求（FIN packet）
#define Sn_SR_SOCK_LAST_ACK                        0x1D       //Socket n在被动关闭状态下等待对断开连接请求做出回应
#define Sn_SR_SOCK_UDP                             0x22       //表示Socket n 工作在UDP模式下
#define Sn_SR_SOCK_IPRAW                           0x32       //ip raw mode socket 
#define Sn_SR_SOCK_MACRAW                          0x42       //表示Socket 0工作在MACRAW模式下 仅在Socket0下生效
#define Sn_SR_SOCK_PPPOE                           0x5F       //pppoe socket



/* IP 协议 */
#define IPPROTO_IP                                 0          //用作占位符
#define IPPROTO_ICMP                               1          //代表ICMP，用于错误报告和诊断
#define IPPROTO_IGMP                               2          //代表IGMP，用于多播组管理
#define IPPROTO_GGP                                3          //代表已废弃的GGP
#define IPPROTO_TCP                                6          //代表TCP
#define IPPROTO_PUP                                12         //代表PUP
#define IPPROTO_UDP                                17         //代表UDP
#define IPPROTO_IDP                                22         //代表IDP
#define IPPROTO_ND                                 77         //代表非官方的网络磁盘协议
#define IPPROTO_RAW                                255        //用于直接操作IP数据包，绕过传输层



void IINCHIP_INIT(void); // reset iinchip
void socket_buf_init(uint8 * tx_size, uint8 * rx_size); // setting tx/rx buf size
uint8 getISR(uint8 s);
void putISR(uint8 s, uint8 val);
uint16 getIINCHIP_RxMAX(uint8 s);
uint16 getIINCHIP_TxMAX(uint8 s);
void setMR(uint8 val);
void setRTR(uint16 timeout); // set retry duration for data transmission, connection, closing ...
void setRCR(uint8 retry); // set retry count (above the value, assert timeout interrupt)
void clearIR(uint8 mask); // clear interrupt
uint8 getIR( void );
void setSn_MSS(SOCKET s, uint16 Sn_MSSR); // set maximum segment size
uint8 getSn_IR(SOCKET s); // get socket interrupt status
uint8 getSn_SR(SOCKET s); // get socket status
uint16 getSn_TX_FSR(SOCKET s); // get socket TX free buf size
uint16 getSn_RX_FSR(SOCKET s); // get socket RX recv buf size
uint8 getSn_SR(SOCKET s);
void setSn_TTL(SOCKET s, uint8 ttl);
void send_data_processing(SOCKET s, uint8 *wizdata, uint16 len);
void recv_data_processing(SOCKET s, uint8 *wizdata, uint16 len);

void setGAR(uint8 * addr); // set gateway address
void setSUBR(uint8 * addr); // set subnet mask address
void setSHAR(uint8 * addr); // set local MAC address
void setSIPR(uint8 * addr); // set local IP address
void getGAR(uint8 * addr);
void getSUBR(uint8 * addr);
void getSHAR(uint8 * addr);
void getSIPR(uint8 * addr);

void setSn_IR(uint8 s, uint8 val);
void Reset_W5500(void);
extern uint8 txsize[];
extern uint8 rxsize[];





#endif /* __BSP_W5500_H__ */

