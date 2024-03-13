#ifndef __BSP_HC05_H
#define __BSP_HC05_H

#include "stm32f10x.h"
#include "core_delay.h"


#define HC05_DELAY_MS          Delay_ms
#define HC05_USART             BLT_USARTx

#define BLT_GPIO_APBxPeriph_FUN           RCC_APB2PeriphClockCmd
#define BLT_GPIO_INT_CLK                  RCC_APB2Periph_GPIOB
#define BLT_GPIO_INT_PORT                 GPIOB
#define BLT_GPIO_INT_PIN                  GPIO_Pin_13

#define BLT_GPIO_KEY_CLK                  RCC_APB2Periph_GPIOB
#define BLT_GPIO_KEY_PORT                 GPIOB
#define BLT_GPIO_KEY_PIN                  GPIO_Pin_14 

#define BLT_KEY_HIGHT                     GPIO_SetBits(BLT_GPIO_KEY_PORT,BLT_GPIO_KEY_PIN)
#define BLT_KEY_LOW                       GPIO_ResetBits(BLT_GPIO_KEY_PORT,BLT_GPIO_KEY_PIN)

//IS_HC05_CONNECTED���ڼ��ģ���Ƿ������״̬
#define IS_HC05_CONNECTED                 GPIO_ReadInputDataBit(BLT_GPIO_INT_PORT,BLT_GPIO_INT_PIN)

#define ENABLE_LCD_DISPLAY             //�л�Һ����ʾ�� ʹ��Ұ�𡾵��败����ILI9341_XPT2046_3.2_2.8�硿

//��������豸����
#define BLTDEV_MAX_NUM             10

//������ַ��������ʽ����NAP��UAP��LAP��
typedef struct
{
	uint8_t    num;                            //ɨ�赽�������豸����
	char unpraseAddr[BLTDEV_MAX_NUM][50];      //�����豸��ַ���ַ�����ʽ������ɨ��ʱ������ʱʹ��
	char name[BLTDEV_MAX_NUM][50];             //�����豸����
}BLTDev;

//�����豸�б�
extern BLTDev   bltDevList;




/*��Ϣ���*/
#define HC05_DEBUG_ON         0
#define HC05_DEBUG_FUNC_ON    0

#define HC05_INFO(fmt,arg...)           printf("<<-HC05-INFO->> "fmt"\n",##arg)
#define HC05_ERROR(fmt,arg...)          printf("<<-HC05-ERROR->> "fmt"\n",##arg)
#define HC05_DEBUG(fmt,arg...)          do{\
                                          if(HC05_DEBUG_ON)\
                                          printf("<<-HC05-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define HC05_DEBUG_FUNC()               do{\
                                         if(HC05_DEBUG_FUNC_ON)\
                                         printf("<<-HC05-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

																			 
void WriteCommand(const char *command,const char*arg);
uint8_t HC05_Send_CMD(char *cmd,uint8_t clean);
uint8_t HC05_Init(void);
uint8_t HC05_Send_CMD_Wait(char *cmd,uint8_t clean,uint32_t delayms);
void HC05_SendString(char *str);
int get_line(char *line,char *stream,int max_size);
void rawReplaceChar(char *rawstr,char delimiter);
uint8_t parseBltAddr(void);
uint8_t getRomoteDeviceName(void);
void printfBLTInfo(void);
uint8_t linkHC05(void);
																			 

#endif /* __BSP_HC05_H */

