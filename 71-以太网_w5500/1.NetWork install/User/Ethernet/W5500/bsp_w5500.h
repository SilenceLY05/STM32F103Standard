#ifndef __BSP_W5500_H__
#define __BSP_W5500_H__


#include "stm32f10x.h"
#include "types.h"

/********************************** ͨ�üĴ�����ַƫ�� ************************************/
/* ģʽ�Ĵ��� */
#define MR                                         (0x000000)
/* ����IP��ַ�Ĵ��� (����Ĭ�����ص�ַ) */
#define GAR0                                       (0x000100)
#define GAR1                                       (0x000200)
#define GAR2                                       (0x000300)
#define GAR3                                       (0x000400)
/* ��������Ĵ��� (�������������ַ) */
#define SUBR0                                      (0x000500)
#define SUBR1                                      (0x000600)
#define SUBR2                                      (0x000700)
#define SUBR3                                      (0x000800)
/* ԴMAC��ַ�Ĵ��� */
#define SHAR0                                      (0x000900)
#define SHAR1                                      (0x000A00)
#define SHAR2                                      (0x000B00)
#define SHAR3                                      (0x000C00)
#define SHAR4                                      (0x000D00)
#define SHAR5                                      (0x000E00)
/* ԴIP��ַ�Ĵ��� */
#define SIPR0                                      (0x000F00)
#define SIPR1                                      (0x001000)
#define SIPR2                                      (0x001100)
#define SIPR3                                      (0x001200)
/* �͵�ƽ�жϼĴ�����ַ */
#define INTLEVEL0                                  (0x001300)
#define INTLEVEL1                                  (0x001400)
/* �жϼĴ��� */
#define IR                                         (0x001500)
/* �ж����μĴ��� */
#define IMR                                        (0x001600)
/* Socket�жϼĴ��� */
#define SIR                                        (0x001700) 
/* Socket �ж����μĴ��� */
#define SIMR                                       (0x001800)
/* �ش���ʱ�Ĵ���( 1 is 100us )��ʼ��ʱΪ2000��0x7D0 200���� */
#define RTR0                                       (0x001900)
#define RTR1                                       (0x001A00)
/* ���Լ����Ĵ��� �����ش����� */
#define WIZ_RCR                                    (0x001B00)
/* PPP���ӿ���Э������ʱ�Ĵ��� LCP Echo��������� */
#define PTIMER                                     (0x001C00)
/* PPP���ӿ���Э������Ĵ��� ����LCP��Ӧ�����4�ֽڻ��� */
#define PMAGIC                                     (0x001D00)
/* PPPoEģʽ��Ŀ��MAC�Ĵ��� д��PPPoE�����������MAC��ַ */
#define PDHAR0                                     (0x001E00)
#define PDHAR1                                     (0x001F00)
#define PDHAR2                                     (0x002000)
#define PDHAR3                                     (0x002100)
#define PDHAR4                                     (0x002200)
#define PDHAR5                                     (0x002300)
/* PPPoEģʽ�»ỰID�Ĵ��� ����PPPoE�������ỰID */
#define PSID0                                      (0x002400)
#define PSID1                                      (0x002500)
/* PPPoEģʽ�������յ�Ԫ */
#define PMR0                                       (0x002600)
#define PMR1                                       (0x002700)
/* �޷��ִ�IP��ַ�Ĵ��� */
#define UIPR0                                      (0x002800)
#define UIPR1                                      (0x002900)
#define UIPR2                                      (0x002A00)
#define UIPR3                                      (0x002B00)
/* �޷��ִ�˿ڼĴ��� */
#define UPORT0                                     (0x002C00)
#define UPORT1                                     (0x002D00)
/* PHY���üĴ��� */
#define PHYCFGR                                    (0x002E00)
/* оƬ�汾�Ĵ�����ַ */
#define VERSIONR                                   (0x003900)  



/********************************** socket�Ĵ�����ַƫ�� ************************************/
/* socket nģʽ�Ĵ��� */
#define Sn_MR(ch)                                  (0x000008+(ch<<5))
/* Socket n���üĴ��� */
#define Sn_CR(ch)                                  (0x000108 + (ch<<5))
/* ͨ���жϼĴ��� */
#define Sn_IR(ch)                                  (0x000208 + (ch<<5))
/* ͨ��״̬�Ĵ��� */
#define Sn_SR(ch)                                  (0x000308 + (ch<<5))
/* Դ�˿ڼĴ��� */
#define Sn_PORT0(ch)                               (0x000408 + (ch<<5))
#define Sn_PORT1(ch)                               (0x000508 + (ch<<5))
/* Ŀ��MAC�Ĵ�����ַ */
#define Sn_DHAR0(ch)                               (0x000608 + (ch<<5))
#define Sn_DHAR1(ch)                               (0x000708 + (ch<<5))
#define Sn_DHAR2(ch)                               (0x000808 + (ch<<5))
#define Sn_DHAR3(ch)                               (0x000908 + (ch<<5))
#define Sn_DHAR4(ch)                               (0x000A08 + (ch<<5))
#define Sn_DHAR5(ch)                               (0x000B08 + (ch<<5))
/* Ŀ��IP�Ĵ�����ַ */
#define Sn_DIPR0(ch)                               (0x000C08 + (ch<<5))
#define Sn_DIPR1(ch)                               (0x000D08 + (ch<<5))
#define Sn_DIPR2(ch)                               (0x000E08 + (ch<<5))
#define Sn_DIPR3(ch)                               (0x000F08 + (ch<<5))
/* Ŀ��˿ڼĴ��� */
#define Sn_DPORT0(ch)                              (0x001008 + (ch<<5))
#define Sn_DPORT1(ch)                              (0x001108 + (ch<<5))
/* ���ֶμĴ��� */
#define Sn_MSSR0(ch)                               (0x001208 + (ch<<5))
#define Sn_MSSR1(ch)                               (0x001308 + (ch<<5))

#define Sn_PROTO(ch)                               (0x001408 + (ch<<5))
/* Socket IP�������ͼĴ��� */
#define Sn_TOS(ch)                                 (0x001508 + (ch<<5))
/* Socket IP����ʱ��Ĵ��� */
#define Sn_TTL(ch)                                 (0x001608 + (ch<<5))
/* ���ջ����С�Ĵ��� */
#define Sn_RXBUF_SIZE(ch)                          (0x001E08 + (ch<<5))
/* ���ͻ����С�Ĵ��� */
#define Sn_TXBUF_SIZE(ch)                          (0x001F08 + (ch<<5))
/* ���з��ͻ���Ĵ��� */
#define Sn_TX_FSR0(ch)                             (0x002008 + (ch<<5))
#define Sn_TX_FSR1(ch)                             (0x002108 + (ch<<5))
/* ���Ͷ�ָ��Ĵ�����ַ */
#define Sn_TX_RD0(ch)                              (0x002208 + (ch<<5))
#define Sn_TX_RD1(ch)                              (0x002308 + (ch<<5))
/* ����дָ��Ĵ�����ַ */
#define Sn_TX_WR0(ch)                              (0x002408 + (ch<<5))
#define Sn_TX_WR1(ch)                              (0x002508 + (ch<<5))
/* ���н��ջ���Ĵ��� */
#define Sn_RX_RSR0(ch)                             (0x002608 + (ch<<5))
#define Sn_RX_RSR1(ch)                             (0x002708 + (ch<<5))
/* ���ն�ָ��Ĵ�����ַ */
#define Sn_RX_RD0(ch)                              (0x002808 + (ch<<5))
#define Sn_RX_RD1(ch)                              (0x002908 + (ch<<5))
/* ����дָ��Ĵ�����ַ */
#define Sn_RX_WR0(ch)                              (0x002A08 + (ch<<5))
#define Sn_RX_WR1(ch)                              (0x002B08 + (ch<<5))
/* socket �ж����μĴ���*/
#define Sn_IMR(ch)                                 (0x002C08 + (ch<<5))
/* �ֶ�IP��ͷ�Ĵ��� */
#define Sn_FRAG(ch)                                (0x002D08 + (ch<<5))
/* Socket����ʱ��Ĵ��� */
#define Sn_KPALVTR(ch)                             (0x002F08 + (ch<<5))



/********************************** ͨ�üĴ��������Ĵ��� ************************************/
/* ģʽ�Ĵ��� */
#define MR_RST                                     0x80       //�ڲ��Ĵ�����ʼ������λ������
#define MR_WOL                                     0x20       //���绽�ѣ�0�ر�1���� 
#define MR_PB                                      0x10       //Ping Blockģʽ 0�ر�1���� ����ʱping����û����Ӧ
#define MR_PPPoE                                   0x08       //����PPPoEģʽ ʹ��ADSL��Ҫ����
#define MR_FARP                                    0x02       //ǿ��ARPģʽ ����ʱ�����Ƿ������ݶ���ǿ�ȷ���ARP����

/* �жϼĴ��� */
#define IR_CONFLICT                                0x80       //IP��ͻ ����APR����ʱ�������ͷ�IP�뱾��IP�ظ���1
#define IR_UNREACH                                 0x40       //Ŀ�겻�ɵִ� ���յ�ICMP������1 ��1��ͨ��UIPR��UPORTR�ɲ�ѯ��IP��ַ�Ͷ˿ںŵ���Ϣ
#define IR_PPPoE                                   0x20       //PPPoE���ӹر� PPP0Eģʽ�£��Ͽ�PPPoE����ʱ��Ч
#define IR_MP                                      0x10       //Magic Packet �����绽��ģʽ���ò�ͨ�� UDP ���յ� Magic Packet ���绽�Ѱ�ʱ



/********************************** ͨ�üĴ��������Ĵ��� ************************************/
/* Socketģʽ�Ĵ��� */
#define Sn_MR_MULTI                                0x80       //UDPģʽ 1�����ಥ
#define Sn_MR_BCASTB                               0x40       //MACRAW��UDPģʽ�µ��������� 1�����㲥����
#define Sn_MR_MC                                   0x20       //�ಥ  1ʹ��IGMP�汾1 0ʹ��IGMP�汾2
#define Sn_MR_ND                                   0x20       //ʹ������ʱACK��1��������ʱACKѡ��
#define Sn_MR_UCASTB                               0x10       //UDPģʽ�µĵ������� 1������������
#define Sn_MR_PPPOE                                0x05       //PPPoE
#define Sn_MR_MACRAW                               0x04       //MAC LAYER RAW SOCK
#define Sn_MR_IPRAW                                0x03       //IP LAYER RAW SOCK
#define Sn_MR_UDP                                  0x02       //UDP
#define Sn_MR_TCP                                  0x01       //TCP
#define Sn_MR_CLOSE                                0x00       //��ʹ��socket

/* Socketģʽ�Ĵ�����MACRAWģʽ�� */
#define Sn_MR_MFEN                                 0x80       //��MACRAWģʽ�¿���MAC��ַ���� 1����
#define Sn_MR_MMB                                  0x20       //MACRAWģʽ�µĶಥ���� 1�����ಥ���� IPv4
#define Sn_MR_MIP6B                                0x10       //MACRAWģʽ��IPv6������ 1����



/* Socket���üĴ��� */
#define Sn_CR_OPEN                                 0x01       //����Sn_MR(P3:P0)��Э������ʼ���ʹ�
#define Sn_CR_LISTEN                               0x02       //(Sn_MR(P3:P0) = Sn_MR_TCP)����Ч��������Ϊһ��TCP���������ȴ�TCP��������
#define Sn_CR_CONNECT                              0x04       //��TPCģʽ�����У���TCP����������һ����������
#define Sn_CR_DISCON                               0x08       //��TCPģʽ�·��ͶϿ����Ӳ���
#define Sn_CR_CLOSE                                0x10       //�ر�Socket n   ʹ�����ַ����Ͽ�ʱû�жϿ�����
#define Sn_CR_SEND                                 0x20       //����Socket n �����ڴ��е����л�������
#define Sn_CR_SEND_MAC                             0x21       //ֻ��UDPģʽ����Ч��������SENDһ�£�����Ҫʹ���Զ�ARP����
#define Sn_CR_SEND_KEEP                            0x22       //ֻ����TCPģʽ����Ч��ͨ������1�ֽ��������������������״̬
#define Sn_CR_RECV                                 0x40       //ͨ�����ն�ָ��Ĵ����ж�Socket n���ջ����Ƿ����


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

/* Socket n�жϼĴ��� */
#define Sn_IR_SEND_OK                              0x10       //SENDOK�жϣ���SEND�������ʱ����λ��Ч
#define Sn_IR_TIMEOUT                              0x08       //��ARP����TCP��ʱ������ʱ����λ��Ч
#define Sn_IR_RECV                                 0x04       //���ۺ�ʱ��ֻҪ���յ��˶Է������ݣ���λ��Ч
#define Sn_IR_DISCON                               0x02       //�����յ��Է���FIN �� FIN/ACK��ʱ����λ��Ч
#define Sn_IR_CON                                  0x01       //���ɹ���Է��������ӣ���Sn_SR���SOCK_ESTABLISHED״̬ʱ����λ��Ч


/* Socket n״̬�Ĵ��� */
#define Sn_SR_SOCK_CLOSE                           0x00       //��ʾSocket n���ڹر�װ���Դ���ͷ�
#define Sn_SR_SOCK_INIT                            0x13       //Socket n�˿ڴ򿪲�����TCP����ģʽ
#define Sn_SR_SOCK_LISTEN                          0x14       //Socket n������TCPģʽ���ȴ�TCP�ͻ��˵���������
#define Sn_SR_SOCK_SYNSENT                         0x15       //��ʾSocket n�Ѿ������������󵽶Է�
#define Sn_SR_SOCK_SYNRECV                         0x16       //��ʾSocket n�ɹ��ĴӶԷ��յ������������
#define Sn_SR_SOCK_ESTABLISHED                     0x17       //��ʾSocket n����״̬ ��״̬�¿���ʹ��SEND����RECV����������ݰ�����
#define Sn_SR_SOCK_FIN_WAIT                        0x18       //��ʾSocket n���ڹر�
#define Sn_SR_SOCK_CLOSING                         0x1A       //��ʾSocket n���ڹر�
#define Sn_SR_SOCK_TIME_WAIT                       0x1B       //��ʾSocket n���ڹر�
#define Sn_SR_SOCK_CLOSE_WAIT                      0x1C       //ָʾ�� Socket n ���յ����������ӶԷ������ĶϿ���������FIN packet��
#define Sn_SR_SOCK_LAST_ACK                        0x1D       //Socket n�ڱ����ر�״̬�µȴ��ԶϿ���������������Ӧ
#define Sn_SR_SOCK_UDP                             0x22       //��ʾSocket n ������UDPģʽ��
#define Sn_SR_SOCK_IPRAW                           0x32       //ip raw mode socket 
#define Sn_SR_SOCK_MACRAW                          0x42       //��ʾSocket 0������MACRAWģʽ�� ����Socket0����Ч
#define Sn_SR_SOCK_PPPOE                           0x5F       //pppoe socket



/* IP Э�� */
#define IPPROTO_IP                                 0          //����ռλ��
#define IPPROTO_ICMP                               1          //����ICMP�����ڴ��󱨸�����
#define IPPROTO_IGMP                               2          //����IGMP�����ڶಥ�����
#define IPPROTO_GGP                                3          //�����ѷ�����GGP
#define IPPROTO_TCP                                6          //����TCP
#define IPPROTO_PUP                                12         //����PUP
#define IPPROTO_UDP                                17         //����UDP
#define IPPROTO_IDP                                22         //����IDP
#define IPPROTO_ND                                 77         //����ǹٷ����������Э��
#define IPPROTO_RAW                                255        //����ֱ�Ӳ���IP���ݰ����ƹ������



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

