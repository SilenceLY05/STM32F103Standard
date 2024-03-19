#ifndef __SL2X23_IF_H__
#define __SL2X23_IF_H__


#include "sl2x23_spi.h"
#include "sl2x23_iic.h"
#include <string.h>

/* 切换通信方式 I2C、SPI、USART */
#define SL2X23_IIC_MODE
//#define SL2X23_SPI_MODE
//#define SL2X23_USART_MODE


//复位，即SL2x23模块的RST引脚，接STM32的普通IO即可
#define SL2x23_GPIO_RST_CLK_FUN                RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_RST_CLK                    RCC_APB2Periph_GPIOB
#define SL2x23_GPIO_RST_PORT                   GPIOB
#define SL2x23_GPIO_RST_PIN                    GPIO_Pin_12
#define SL2x23_GPIO_RST_Mode                   GPIO_Mode_Out_PP

// IRQ中断，即SL2x23模块的IRQ引脚，使用外部中断
#define SL2x23_GPIO_IRQ_CLK_FUN                RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_IRQ_CLK                    RCC_APB2Periph_GPIOB
#define SL2x23_GPIO_IRQ_PORT                   GPIOB
#define SL2x23_GPIO_IRQ_PIN                    GPIO_Pin_9
#define SL2x23_GPIO_IRQ_Mode                   GPIO_Mode_IPU

// IRQ中断线
#define SL2x23_IRQ_EXTI_CLK_FUN                RCC_APB2PeriphClockCmd
#define SL2x23_IRQ_EXTI_CLK                    RCC_APB2Periph_AFIO
#define SL2x23_IRQ_EXTI_IRQ                    EXTI9_5_IRQn
#define SL2x23_IRQ_EXTI_LINE                   EXTI_Line9
#define SL2x23_IRQ_EXTI_PORTSOURCE             GPIO_PortSourceGPIOB
#define SL2x23_IRQ_EXTI_PINSOURCE              GPIO_PinSource9
#define SL2x23_IRQPin_IRQHandler               EXTI9_5_IRQHandler


// RST复位
#define           SL2x23_Reset_Enable()      GPIO_ResetBits( SL2x23_GPIO_RST_PORT, SL2x23_GPIO_RST_PIN )
#define           SL2x23_Reset_Disable()     GPIO_SetBits ( SL2x23_GPIO_RST_PORT, SL2x23_GPIO_RST_PIN )

void SL2x23_RST_IRQ_Config(void);
void SL2x23_IRQ_EXTI_Config(void);
void SL2x23_IRQPin_IRQHandler(void);
void SL2x23_Init(void);
void write_reg(uint8_t addr,uint8_t RegValue);
uint8_t read_reg(uint8_t addr);
void set_bit_mask(uint8_t reg,uint8_t mask);
void write_buf(uint8_t addr,uint8_t *data,uint32_t lenth);
void read_buf(uint8_t addr,uint8_t *data,uint32_t lenth);
void clear_bit_mask(uint8_t reg,uint8_t mask);
uint8_t sl2x23_read_reg(uint8_t addr);
void Communication_Test(void);


#endif /* __SL2X23_IF_H__ */

