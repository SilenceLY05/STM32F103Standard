#ifndef __BSP_GSP_H
#define __BSP_GSP_H

#include "stm32f10x.h"
#include "nmea.h"

//#define __GPS_DEBUG                  //定义这个宏，在nmea解码时会输出调试信息
#define __GPS_LOG_FILE               //定义这个宏，对SD卡上的gpslog.txt文件进行解码；不定义的话使用串口接收GPS信息解码


/* GPS接口配置 使用不用的串口时，要修改对应的接口 */
#define GPS_DR_Base                (USART2_BASE+0x04)      //串口的数据寄存器地址

#define GPS_DATA_ADDR              GPS_DR_Base             //GPS使用的串口的数据寄存器地址
#define GPS_RBUFF_SIZE             512                     //串口接收缓冲区大小
#define HALF_GPS_RBUFF_SIZE        (GPS_RBUFF_SIZE/2)      //串口接收缓冲区的一半



/* 外设 */
//GPS串口配置
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


//GPSDMA配置
#define GPS_DMAx                                 DMA1
#define GPS_DMA_CLOCK_FUN                        RCC_AHBPeriphClockCmd
#define GPS_DMA_CLK                              RCC_AHBPeriph_DMA1
#define GPS_DMA_CHANNEL                          DMA1_Channel6
#define GPS_DMA_IRQn                             DMA1_Channel6_IRQn

//外设标志
#define GPS_DMA_FLAG_TC                          DMA1_FLAG_TC6
#define GPS_DMA_FLAG_TE                          DMA1_FLAG_TE6
#define GPS_DMA_FLAG_HT                          DMA1_FLAG_HT6
#define GPS_DMA_FLAG_GL                          DMA1_FLAG_GL6
#define GPS_DMA_IT_HT                            DMA1_IT_HT6
#define GPS_DMA_IT_TC                            DMA1_IT_TC6


//中断函数
#define GPS_DMA_IRQHandler                       DMA1_Channel6_IRQHandler     //GPS使用的DMA中断服务函数



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

