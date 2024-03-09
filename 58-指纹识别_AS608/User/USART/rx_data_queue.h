#ifndef  __RX_DATA_QUEUE_H
#define  __RX_DATA_QUEUE_H

#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>

/* ������еĸ�����Ҫ��2���� */
#define QUEUE_NODE_NUM           (2)                /* ������еĴ�С���ж��ٸ��������� */
#define QUEUE_NODE_DATA_LEN      (30)               /* �������ջ�������С */

/* ���е������������ͽӿ� */
#define QUEUE_DATA_TYPE               ESP_USART_FRAME

/* ���еĵ�������ӿ� */
#define DATA_QUEUE_LOG                QUEUE_DEBUG
#define DATA_QUEUE_LOG_ARRAY          QUEUE_DEBUG_ARRAY


/* �������� */
typedef struct
{
	unsigned char* head;                         /* ������ͷָ�� */
	uint16_t len;                       /* ���յ������ݳ��� */
}ESP_USART_FRAME;   


/* ���нṹ */
typedef struct 
{
	int     size;                                       /* ��������С*/
	int     read;                                       /* ��ָ��*/
	int     write;                                      /* дָ��*/
	int     read_using;                                 /*���ڶ�ȡ�Ļ�����ָ��*/
	int     write_using;                                /*����д��Ļ�����ָ��*/
	QUEUE_DATA_TYPE     *elems[QUEUE_NODE_NUM];         /* ��������ַ */
}QueueBuffer;

extern QueueBuffer  rx_queue;


/*��Ϣ���*/
#define QUEUE_DEBUG_ON          0
#define QUEUE_DEBUG_ARRAY_ON		0

#define QUEUE_INFO(fmt,arg...)           printf("<<-QUEUE-INFO->> "fmt"\n",##arg)
#define QUEUE_ERROR(fmt,arg...)          printf("<<-QUEUE-ERROR->> "fmt"\n",##arg)
#define QUEUE_DEBUG(fmt,arg...)          do{\
                                          if(QUEUE_DEBUG_ON)\
                                          printf("<<-QUEUE-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define QUEUE_DEBUG_ARRAY(array, num)    do{\
																									 int32_t i;\
																									 uint8_t* a = array;\
																									 if(QUEUE_DEBUG_ARRAY_ON)\
																									 {\
																											printf("\n<<-QUEUE-DEBUG-ARRAY->>\n");\
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

//������е�״̬��Ϣ
#define cbPrint(cb)		    DATA_QUEUE_LOG("size=0x%x, read=%d, write=%d\n", cb.size, cb.read, cb.write);\
	  DATA_QUEUE_LOG("size=0x%x, read_using=%d, write_using=%d\n", cb.size, cb.read_using, cb.write_using);


																								 QUEUE_DATA_TYPE* cbWrite(QueueBuffer *cb);
QUEUE_DATA_TYPE* cbRead(QueueBuffer *cb);
void cbReadFinish(QueueBuffer *cb);
void cbWriteFinish(QueueBuffer *cb);
//void cbPrint(QueueBuffer *cb);
QUEUE_DATA_TYPE* cbWriteUsing(QueueBuffer *cb) ;
int cbIsFull(QueueBuffer *cb) ; 
int cbIsEmpty(QueueBuffer *cb) ;

void rx_queue_init(void);
void pull_data_from_queue(void);
void push_data_to_queue(char *src_dat,uint16_t src_len);
																								 
#endif /* __RX_DATA_QUEUE_H */
