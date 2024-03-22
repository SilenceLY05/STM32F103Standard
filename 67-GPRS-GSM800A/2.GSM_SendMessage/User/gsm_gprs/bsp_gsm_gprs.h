#ifndef __BSP_GSM_GPRS_H
#define __BSP_GSM_GPRS_H

#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_gsm_usart.h"
#include "ff.h"

#define MaxMessAdd    100
#define MAXADDSIZE    10


extern uint8_t currentAddIndex;
extern uint8_t address[MAXADDSIZE];


typedef enum
{
	GSM_TRUE,
	GSM_FALSE,
}gsm_res_e;

typedef enum
{
	GSM_NULL               =0,
	GSM_CMS_SEND           ='\r',
	GSM_DATA_SEND          =0x1A,         //��������(ctrl + z)
	GSM_DATA_CANCLE        =0x1B,         //��������(Esc)
}gsm_cmd_end_e;




//                  ָ��             ��������
//��������          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //�ܶ�SIM��Ĭ�϶���û���ñ�������ģ������������ http://www.multisilicon.com/blog/a21234642.html
//SIMӪ����         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM��״̬         AT+CPIN?\r        +CPIN: READY   OK
//SIM���ź�ǿ��     AT+CSQ\r          +CSQ: 8,0   OK


uint8_t gsm_cmd_check(char *reply);                             // ��������ֵ
uint8_t gsm_cmd(char *cmd,char *reply,uint32_t waittime);       // ����GSMָ��
char *gsm_waitask(uint8_t waitask_hook(void));                  // �ȴ�������Ӧ��



#define GSM_CLEAN_RX()                         clean_rebuff()
#define gsm_ate0()                             gsm_cmd("ATE0\r","OK",100)              //�ر���ʾ
#define GSM_TX(cmd)                            GSM_USARTx_printf("%s",cmd)
#define GSM_IS_RX()                            (USART_GetFlagStatus(GSM_USARTx,USART_FLAG_RXNE) != RESET)
#define GSM_RX(len)                            ((char *)get_rebuff(&(len)))
#define GSM_DELAY(time)                        Delay_ms(time)
#define GSM_SWAP16(data)    			          	 __REVSH(data)



/*************************** �绰 ���� ***************************/
#define GSM_HANGON()                   GSM_TX("ATA\r");//�ӵ绰
#define GSM_HANGOFF()                  GSM_TX("ATH\r");//�ҵ绰

uint8_t gsm_cnum(char *num);           // ��������ָ��
uint8_t gsm_init(void);                // ��ʼ�������ģ��
void gsm_call(char *num);              // ���𲦴�绰
uint8_t IsRing(char *num);             // ����绰
uint8_t IsInserCard(void);             // ����Ƿ��п�



/***************************  ���Ź���  ****************************/
uint8_t IsASSIC(char *str);                                 // �ж��Ƿ��Ǵ�ASSIC����
void gsm_gbk2usc2(char *ucs2,char *gbk);                    // ��GBK����ת����UCS2����
void gsm_char2hex(char *hex,char ch);                       // ���ַ�ת����16�����ַ�
void gsm_gbk2ucs2hex(char *ucs2hex,char *gbk);              // GBK����תucs2������ת��16�����ַ�
uint8_t gsm_sms(char *num,char *smstext);                   // ���Ͷ��ţ�֧�����ģ�����ΪGBK�룩
uint8_t IsReceiveMS(void);                                  // ��ѯ�Ƿ���յ��¶���
uint8_t IsReceiveMS_Test(void);                             // ��ѯ�Ƿ���յ��¶���
uint8_t readmessage(uint8_t messadd,char *num,char *str);   // ��ȡ��������
uint8_t hexuni2gbk(char *hexuni,char *chgbk);



/*************************** GPRS ���� ***************************/
uint8_t gsm_gprs_init(void);                                                  // ��ʼ��GPRS����
uint8_t gsm_gprs_tcp_link(char *localport,char *serverip,char *serverport);   //  ����TCP����
uint8_t gsm_gprs_udp_link(char *localport,char *serverip,char *serverport);   //  ����UDP����
uint8_t gsp_gprs_send(const char *str);                                       // ʹ��GPRS��������
uint8_t gsm_gprs_link_close(void);                                            // �Ͽ���������
uint8_t gsm_gprs_shut_close(void);                                            // �رճ���
uint8_t PostGPRS(void);                                                       // ���ش�GSMģ����յ����������ݣ���ӡ������


/*�����ô���*/

#define GSM_DEBUG_ON         	0
#define GSM_DEBUG_ARRAY_ON    0
#define GSM_DEBUG_FUNC_ON   	0
// Log define
#define GSM_INFO(fmt,arg...)           printf("<<-GSM-INFO->> "fmt"\n",##arg)
#define GSM_ERROR(fmt,arg...)          printf("<<-GSM-ERROR->> "fmt"\n",##arg)
#define GSM_DEBUG(fmt,arg...)          do{\
                                         if(GSM_DEBUG_ON)\
                                         printf("<<-GSM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																					}while(0)

#define GSM_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(GSM_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-GSM-DEBUG-ARRAY->> [%d]\n",__LINE__);\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define GSM_DEBUG_FUNC()               do{\
                                         if(GSM_DEBUG_FUNC_ON)\
                                         printf("<<-GSM-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

#endif /* __BSP_GSM_GPRS_H */
