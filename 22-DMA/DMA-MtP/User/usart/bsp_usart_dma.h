#ifndef __BSP_USART_DMA_H
#define __BSP_USART_DMA_H

#include "stm32f10x.h"
#include <stdio.h>


//串口工作参数宏定义
#define DEBUG_USARTx                 USART1
#define DEBUG_USART_CLK              RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd       RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE         115200

//USART GPIO引脚宏定义
#define DEBUG_USART_GPIO_CLK         (RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT     GPIOA
#define DEBUG_USART_TX_GPIO_PIN      GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT     GPIOA
#define DEBUG_USART_RX_GPIO_PIN      GPIO_Pin_10


//串口对应的DMA请求通道
#define USART_TX_DMA_CHANNEL         DMA1_Channel4
//外设寄存器地址
#define USART_DR_ADDRESS             (USART1_BASE+0x04)
//一次发送的数据量
#define SENDBUFF_SIZE                5000

#define USART_TX_DMA_FLAG_TC         DMA1_FLAG_TC4

void USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void USARTx_DMA_Config(void);

#endif /* __BSP_USART_DMA_H */

