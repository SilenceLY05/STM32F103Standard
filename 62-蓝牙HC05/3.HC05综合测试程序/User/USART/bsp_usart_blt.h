#ifndef __BSP_USART_BLT_H
#define __BSP_USART_BLT_H

#include "stm32f10x.h"
#include <stdio.h>

#define BLT_USART_BAUDRATE                         38400

#define BLT_USARTx                                 USART2
#define BLT_USART_APBxClockCmd                     RCC_APB1PeriphClockCmd
#define BLT_USART_CLK                              RCC_APB1Periph_USART2
#define BLT_USART_GPIO_APBxClockCmd                RCC_APB2PeriphClockCmd
#define BLT_USART_GPIO_CLK                         RCC_APB2Periph_GPIOA
#define BLT_USART_TX_PORT                          GPIOA
#define BLT_SUART_TX_PIN                           GPIO_Pin_2
#define BLT_USART_RX_PORT                          GPIOA
#define BLT_SUART_RX_PIN                           GPIO_Pin_3
#define BLT_USART_IRQ                              USART2_IRQn
#define BLT_USART_IRQHandler                       USART2_IRQHandler

#define USART_BUFF_SIZE   1024



void BLT_USART_Config(void);
void USART_SendByte(USART_TypeDef *pUSARTx,uint8_t ch);
void USART_SendHalfWord(USART_TypeDef *pUSARTx,uint16_t ch);
void USART_SendArray(USART_TypeDef *pUSARTx,uint8_t *array,uint16_t num);
void USART_SendString(USART_TypeDef *pUSARTx,uint8_t *str);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);
void USART_SendStr_Length(USART_TypeDef *pUSARTx,uint8_t *str,uint32_t strlen);


#endif /* __BSP_USART_BLT_H */



