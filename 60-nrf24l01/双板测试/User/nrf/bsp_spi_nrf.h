#ifndef __BSP_SPI_NRF_H
#define	__BSP_SPI_NRF_H


#include "stm32f10x.h"


#define TX_ADR_WIDTH       5    //发射地址宽度
#define TX_PLOAD_WIDTH     32   //发射数据通道有效数据宽度0~32byte

#define RX_ADR_WIDTH       5
#define RX_PLOAD_WIDTH     32


#define CHANNAL            40   //频道选择



// SPI(nRF24L01) commands ,	NRF的SPI命令宏定义，详见NRF功能使用文档
#define NRF_READ_REG    0x00  // 定义要注册的读命令
#define NRF_WRITE_REG   0x20  // 定义要注册的写命令
#define RD_RX_PLOAD 0x61  // 定义RX有效负载寄存器地址
#define WR_TX_PLOAD 0xA0  // 定义TX有效负载寄存器地址
#define FLUSH_TX    0xE1  // 定义flush TX寄存器命令
#define FLUSH_RX    0xE2  // 定义flush RX寄存器命令
#define REUSE_TX_PL 0xE3  // 定义重用TX负载注册命令
#define NOP         0xFF  // 定义无操作，可用于读取状态寄存器

// SPI(nRF24L01) registers(addresses) ，NRF24L01 相关寄存器地址的宏定义
#define CONFIG      0x00  // 'Config' register address
#define EN_AA       0x01  // '启用自动确认注册地址
#define EN_RXADDR   0x02  // '使能RX地址' register address
#define SETUP_AW    0x03  // 'Setup address width' register address
#define SETUP_RETR  0x04  // 'Setup Auto. Retrans' register address
#define RF_CH       0x05  // 'RF channel' register address
#define RF_SETUP    0x06  // 'RF setup' register address
#define STATUS      0x07  // 'Status' register address
#define OBSERVE_TX  0x08  // 'Observe TX' register address
#define CD          0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  // 'RX address pipe5' register address
#define TX_ADDR     0x10  // 'TX address' register address
#define RX_PW_P0    0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  // 'FIFO Status Register' register address


#define MAX_RT      0x10  //达到最大重发次数中断标志位
#define TX_DS       0x20  //发送完成中断标志位

#define RX_DR       0x40  //接收完成中断标志位


/* NRF1 GPIO引脚 */
#define NRF_GPIO_ClockCmd      RCC_APB2PeriphClockCmd
#define NRF_SPI_ClockCmd       RCC_APB2PeriphClockCmd
#define NRF_SPIx               SPI1
#define NRF_SPI_CLK            RCC_APB2Periph_SPI1

#define NRF_SCK_GPIO_CLK       RCC_APB2Periph_GPIOA
#define NRF_SCK_GPIO_PORT      GPIOA
#define NRF_SCK_GPIO_PIN       GPIO_Pin_5

#define NRF_MOSI_GPIO_CLK      RCC_APB2Periph_GPIOA
#define NRF_MOSI_GPIO_PORT     GPIOA
#define NRF_MOSI_GPIO_PIN      GPIO_Pin_7

#define NRF_MISO_GPIO_CLK      RCC_APB2Periph_GPIOA
#define NRF_MISO_GPIO_PORT     GPIOA
#define NRF_MISO_GPIO_PIN      GPIO_Pin_6

#define NRF_CSN_GPIO_CLK       RCC_APB2Periph_GPIOC
#define NRF_CSN_GPIO_PORT      GPIOC
#define NRF_CSN_GPIO_PIN       GPIO_Pin_6

#define NRF_CE_GPIO_CLK        RCC_APB2Periph_GPIOC
#define NRF_CE_GPIO_PORT       GPIOC
#define NRF_CE_GPIO_PIN        GPIO_Pin_5

#define NRF_IRQ_GPIO_CLK       RCC_APB2Periph_GPIOC
#define NRF_IRQ_GPIO_PORT      GPIOC
#define NRF_IRQ_GPIO_PIN       GPIO_Pin_4


#define NRF_CSN_HIGH()         GPIO_SetBits(NRF_CSN_GPIO_PORT,NRF_CSN_GPIO_PIN)
#define NRF_CSN_LOW()          GPIO_ResetBits(NRF_CSN_GPIO_PORT,NRF_CSN_GPIO_PIN)             //CSN置低
#define NRF_CE_HIGH()          GPIO_SetBits(NRF_CE_GPIO_PORT,NRF_CE_GPIO_PIN)
#define NRF_CE_LOW()           GPIO_ResetBits(NRF_CE_GPIO_PORT,NRF_CE_GPIO_PIN)               //CE置低
#define NRF_Read_IRQ()         GPIO_ReadInputDataBit(NRF_IRQ_GPIO_PORT,NRF_IRQ_GPIO_PIN)      //中断引脚


void Delay(__IO uint32_t nCount);

void SPI_NRF_Config(void);
uint8_t SPI_NRF_ReadWrite(uint8_t data);
uint8_t SPI_NRF_WriteReg(uint8_t reg,uint8_t data);
uint8_t SPI_NRF_ReadReg(uint8_t reg);
uint8_t SPI_NRF_WriteBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num);
uint8_t SPI_NRF_ReadBuffer(uint8_t reg,uint8_t *pBuffer,uint8_t Num);
void NRF_RX_Mode(void);
void NRF_TX_Mode(void);
uint8_t NRF_Check(void);
uint8_t NRF_TX_Data(uint8_t *txBuffer);
uint8_t NRF_RX_Data(uint8_t *rxBuffer);



#endif /* __BSP_SPI_NRF_H */
