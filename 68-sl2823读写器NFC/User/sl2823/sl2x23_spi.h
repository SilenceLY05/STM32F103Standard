#ifndef __BSP_SPI_FLASH_H
#define __BSP_SPI_FLASH_H

#include "stm32f10x.h"
#include <stdio.h>


/* 硬件/软件SPI 切换 */
//#define HARDWARE_SPI    //软硬件SPI两种都可以使用



/*  SPI接口定义-开头  */
#define SL2x23_SPI                    SPI2
#define SL2x23_SPI_CLK                RCC_APB1Periph_SPI2

//CS(NSS)引脚 片选选普通GPIO即可
#define SL2x23_GPIO_CS_CLK_FUN         RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_CS_CLK             RCC_APB2Periph_GPIOA
#define SL2x23_GPIO_CS_PORT            GPIOA
#define SL2x23_GPIO_CS_PIN             GPIO_Pin_4
#define SL2x23_GPIO_CS_Mode            GPIO_Mode_Out_PP

#define SL2x23_GPIO_SCK_CLK_FUN         RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_SCK_CLK             RCC_APB2Periph_GPIOB
#define SL2x23_GPIO_SCK_PORT            GPIOB
#define SL2x23_GPIO_SCK_PIN             GPIO_Pin_13
#define SL2x23_GPIO_SCK_Mode            GPIO_Mode_Out_PP

#define SL2x23_GPIO_MISO_CLK_FUN         RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_MISO_CLK             RCC_APB2Periph_GPIOB
#define SL2x23_GPIO_MISO_PORT            GPIOB
#define SL2x23_GPIO_MISO_PIN             GPIO_Pin_14
#define SL2x23_GPIO_MISO_Mode            GPIO_Mode_IN_FLOATING

#define SL2x23_GPIO_MOSI_CLK_FUN         RCC_APB2PeriphClockCmd
#define SL2x23_GPIO_MOSI_CLK             RCC_APB2Periph_GPIOB
#define SL2x23_GPIO_MOSI_PORT            GPIOB
#define SL2x23_GPIO_MOSI_PIN             GPIO_Pin_15
#define SL2x23_GPIO_MOSI_Mode            GPIO_Mode_Out_PP


#define SL2x23_CS_Enable()               GPIO_ResetBits(SL2x23_GPIO_CS_PORT,SL2x23_GPIO_CS_PIN)
#define SL2x23_CS_Disable()              GPIO_SetBits(SL2x23_GPIO_CS_PORT,SL2x23_GPIO_CS_PIN)

#define SL2x23_SPI_CS_LOW                SL2x23_CS_Enable()
#define SL2x23_SPI_CS_HIGH               SL2x23_CS_Disable()

#define SL2x23_SCK_0()                   GPIO_ResetBits(SL2x23_GPIO_SCK_PORT,SL2x23_GPIO_SCK_PIN)
#define SL2x23_SCK_1()                   GPIO_SetBits(SL2x23_GPIO_SCK_PORT,SL2x23_GPIO_SCK_PIN)

#define SL2x23_MOSI_0()                   GPIO_ResetBits(SL2x23_GPIO_MOSI_PORT,SL2x23_GPIO_MOSI_PIN)
#define SL2x23_MOSI_1()                   GPIO_SetBits(SL2x23_GPIO_MOSI_PORT,SL2x23_GPIO_MOSI_PIN)

#define SL2x23_MISO_GET()                 GPIO_ReadInputDataBit(SL2x23_GPIO_MOSI_PORT,SL2x23_GPIO_MOSI_PIN)
																		 


uint8_t SPI_ReadWriteByte(uint8_t byte);
uint8_t DEV_ReadSR(uint8_t sr);
void DEV_SPI_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint32_t NumByteToRead);
void DEV_SPI_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint32_t NumByteToRead);
void SL2x23_SPI_Config(void);



#endif /* __SPI_FLASH_H */
