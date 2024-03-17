#ifndef __BSP_GSP_H
#define __BSP_GSP_H

#include "stm32f10x.h"
#include "nmea.h"

//#define __GPS_DEBUG                  //��������꣬��nmea����ʱ�����������Ϣ
#define __GPS_LOG_FILE               //��������꣬��SD���ϵ�gpslog.txt�ļ����н��룻������Ļ�ʹ�ô��ڽ���GPS��Ϣ����


/* GPS�ӿ����� ʹ�ò��õĴ���ʱ��Ҫ�޸Ķ�Ӧ�Ľӿ� */
#define GPS_DR_Base                (USART2_BASE+0x04)      //���ڵ����ݼĴ�����ַ

#define GPS_DATA_ADDR              GPS_DR_Base             //GPSʹ�õĴ��ڵ����ݼĴ�����ַ
#define GPS_RBUFF_SIZE             512                     //���ڽ��ջ�������С
#define HALF_GPS_RBUFF_SIZE        (GPS_RBUFF_SIZE/2)      //���ڽ��ջ�������һ��



/* ���� */
//GPS��������
#define GPS_USART_BAUDRATE                       9600
#define GPS_USARTx                               USART2
#define GPS_USART_APBxPeriph_FUN                 RCC_APB1PeriphClockCmd
#define GPS_USART_CLK                            RCC_APB1Periph_USART2


#define GPS_USART_GPIO_APBxPeirph_FUN            RCC_APB2PeriphClockCmd
#define GPS_USART_RX_GPIO_CLK                    RCC_APB2Periph_GPIOA
#define GPS_USART_RX_GPIO_PORT                   GPIOA
#define GPS_USART_RX_GPIO_PIN                    GPIO_Pin_3

#define GPS_USART_TX_GPIO_CLK                    RCC_APB2Periph_GPIOA
#define GPS_USART_TX_GPIO_PORT                   GPIOA
#define GPS_USART_TX_GPIO_PIN                    GPIO_Pin_2


//GPSDMA����
#define GPS_DMAx                                 DMA1
#define GPS_DMA_CLOCK_FUN                        RCC_AHBPeriphClockCmd
#define GPS_DMA_CLK                              RCC_AHBPeriph_DMA1
#define GPS_DMA_CHANNEL                          DMA1_Channel6
#define GPS_DMA_IRQn                             DMA1_Channel6_IRQn

//�����־
#define GPS_DMA_FLAG_TC                          DMA1_FLAG_TC6
#define GPS_DMA_FLAG_TE                          DMA1_FLAG_TE6
#define GPS_DMA_FLAG_HT                          DMA1_FLAG_HT6
#define GPS_DMA_FLAG_GL                          DMA1_FLAG_GL6
#define GPS_DMA_IT_HT                            DMA1_IT_HT6
#define GPS_DMA_IT_TC                            DMA1_IT_TC6


//�жϺ���
#define GPS_DMA_IRQHandler                       DMA1_Channel6_IRQHandler     //GPSʹ�õ�DMA�жϷ�����



extern uint8_t gps_buff[GPS_RBUFF_SIZE];
extern __IO uint8_t GPS_TransferEnd;
extern __IO uint8_t GPS_HalfTransferEnd;



void GPS_Config(void);
void GPS_ProcessDMAIRQ(void);
void trace(const char *str,int str_size);
void error(const char *str,int str_size);
void gps_info(const char *str,int str_size);
void GMTconvert(nmeaTIME *SourceTime,nmeaTIME *ConvertTime,uint8_t GMT,uint8_t AREA);




#endif /* __BSP_GSP_H */

