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
	GSM_DATA_SEND          =0x1A,         //发送数据(ctrl + z)
	GSM_DATA_CANCLE        =0x1B,         //发送数据(Esc)
}gsm_cmd_end_e;




//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK


uint8_t gsm_cmd_check(char *reply);                             // 检查命令返回值
uint8_t gsm_cmd(char *cmd,char *reply,uint32_t waittime);       // 发送GSM指令
char *gsm_waitask(uint8_t waitask_hook(void));                  // 等待有数据应答



#define GSM_CLEAN_RX()                         clean_rebuff()
#define gsm_ate0()                             gsm_cmd("ATE0\r","OK",100)              //关闭显示
#define GSM_TX(cmd)                            GSM_USARTx_printf("%s",cmd)
#define GSM_IS_RX()                            (USART_GetFlagStatus(GSM_USARTx,USART_FLAG_RXNE) != RESET)
#define GSM_RX(len)                            ((char *)get_rebuff(&(len)))
#define GSM_DELAY(time)                        Delay_ms(time)
#define GSM_SWAP16(data)    			          	 __REVSH(data)



/*************************** 电话 功能 ***************************/
#define GSM_HANGON()                   GSM_TX("ATA\r");//接电话
#define GSM_HANGOFF()                  GSM_TX("ATH\r");//挂电话

uint8_t gsm_cnum(char *num);           // 本机号码指令
uint8_t gsm_init(void);                // 初始化并检测模块
void gsm_call(char *num);              // 发起拨打电话
uint8_t IsRing(char *num);             // 来电电话
uint8_t IsInserCard(void);             // 检测是否有卡



/***************************  短信功能  ****************************/
uint8_t IsASSIC(char *str);                                 // 判断是否是纯ASSIC编码
void gsm_gbk2usc2(char *ucs2,char *gbk);                    // 把GBK编码转换成UCS2编码
void gsm_char2hex(char *hex,char ch);                       // 把字符转换成16进制字符
void gsm_gbk2ucs2hex(char *ucs2hex,char *gbk);              // GBK编码转ucs2编码再转成16进制字符
uint8_t gsm_sms(char *num,char *smstext);                   // 发送短信（支持中文，中文为GBK码）
uint8_t IsReceiveMS(void);                                  // 查询是否接收到新短信
uint8_t IsReceiveMS_Test(void);                             // 查询是否接收到新短信
uint8_t readmessage(uint8_t messadd,char *num,char *str);   // 读取短信内容
uint8_t hexuni2gbk(char *hexuni,char *chgbk);



/*************************** GPRS 功能 ***************************/
uint8_t gsm_gprs_init(void);                                                  // 初始化GPRS网络
uint8_t gsm_gprs_tcp_link(char *localport,char *serverip,char *serverport);   //  建立TCP连接
uint8_t gsm_gprs_udp_link(char *localport,char *serverip,char *serverport);   //  建立UDP连接
uint8_t gsp_gprs_send(const char *str);                                       // 使用GPRS发送数据
uint8_t gsm_gprs_link_close(void);                                            // 断开网络连接
uint8_t gsm_gprs_shut_close(void);                                            // 关闭场景
uint8_t PostGPRS(void);                                                       // 返回从GSM模块接收到的网络数据，打印到串口


/*调试用串口*/

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

