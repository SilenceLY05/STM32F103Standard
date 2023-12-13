#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"
#include <stdio.h>


//串口1—USART1
#define DEBUG_USARTx                  USART1
#define DEBUG_USART_CLK               RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd        RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE          115200

//USART GPIO引脚宏定义
#define DEBUG_USART_GPIO_CLK          (RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd   RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT      GPIOA
#define DEBUG_USART_TX_GPIO_PIN       GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT      GPIOA
#define DEBUG_USART_RX_GPIO_PIN       GPIO_Pin_10


////串口2—USART1
//#define DEBUG_USARTx                  USART2
//#define DEBUG_USART_CLK               RCC_APB2Periph_USART2
//#define DEBUG_USART_APBxClkCmd        RCC_APB2PeriphClockCmd
//#define DEBUG_USART_BAUDRATE          115200

////USART GPIO引脚宏定义
//#define DEBUG_USART_GPIO_CLK          (RCC_APB2Periph_GPIOA)
//#define DEBUG_USART_GPIO_APBxClkCmd   RCC_APB2PeriphClockCmd

//#define DEBUG_USART_TX_GPIO_PORT      GPIOA
//#define DEBUG_USART_TX_GPIO_PIN       GPIO_Pin_2
//#define DEBUG_USART_RX_GPIO_PORT      GPIOA
//#define DEBUG_USART_RX_GPIO_PIN       GPIO_Pin_3

//#define DEBUG_USART_IRQ               USART2_IRQn
//#define DEBUG_USART_IRQHandler        USART2_IRQHandler

////串口3—USART1
//#define DEBUG_USARTx                  USART3
//#define DEBUG_USART_CLK               RCC_APB1Periph_USART3
//#define DEBUG_USART_APBxClkCmd        RCC_APB1PeriphClockCmd
//#define DEBUG_USART_BAUDRATE          115200

////USART GPIO引脚宏定义
//#define DEBUG_USART_GPIO_CLK          (RCC_APB2Periph_GPIOB)
//#define DEBUG_USART_GPIO_APBxClkCmd   RCC_APB2PeriphClockCmd

//#define DEBUG_USART_TX_GPIO_PORT      GPIOB
//#define DEBUG_USART_TX_GPIO_PIN       GPIO_Pin_10
//#define DEBUG_USART_RX_GPIO_PORT      GPIOB
//#define DEBUG_USART_RX_GPIO_PIN       GPIO_Pin_11

//#define DEBUG_USART_IRQ               USART3_IRQn
//#define DEBUG_USART_IRQHandler        USART3_IRQHandler

////串口4—USART1
//#define DEBUG_USARTx                  USART4
//#define DEBUG_USART_CLK               RCC_APB1Periph_USART4
//#define DEBUG_USART_APBxClkCmd        RCC_APB1PeriphClockCmd
//#define DEBUG_USART_BAUDRATE          115200

////USART GPIO引脚宏定义
//#define DEBUG_USART_GPIO_CLK          (RCC_APB2Periph_GPIOC)
//#define DEBUG_USART_GPIO_APBxClkCmd   RCC_APB2PeriphClockCmd

//#define DEBUG_USART_TX_GPIO_PORT      GPIOC
//#define DEBUG_USART_TX_GPIO_PIN       GPIO_Pin_10
//#define DEBUG_USART_RX_GPIO_PORT      GPIOC
//#define DEBUG_USART_RX_GPIO_PIN       GPIO_Pin_11

//#define DEBUG_USART_IRQ               USART4_IRQn
//#define DEBUG_USART_IRQHandler        USART4_IRQHandler

////串口5—USART1
//#define DEBUG_USARTx                  USART5
//#define DEBUG_USART_CLK               RCC_APB1Periph_USART5
//#define DEBUG_USART_APBxClkCmd        RCC_APB1PeriphClockCmd
//#define DEBUG_USART_BAUDRATE          115200

////USART GPIO引脚宏定义
//#define DEBUG_USART_GPIO_CLK          (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD)
//#define DEBUG_USART_GPIO_APBxClkCmd   RCC_APB2PeriphClockCmd

//#define DEBUG_USART_TX_GPIO_PORT      GPIOC
//#define DEBUG_USART_TX_GPIO_PIN       GPIO_Pin_12
//#define DEBUG_USART_RX_GPIO_PORT      GPIOD
//#define DEBUG_USART_RX_GPIO_PIN       GPIO_Pin_2

//#define DEBUG_USART_IRQ               USART5_IRQn
//#define DEBUG_USART_IRQHandler        USART5_IRQHandler


void Debug_USART_Config(void);



#endif /* __BSP_USART_H */

