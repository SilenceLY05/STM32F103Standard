#ifndef __BSP_AD5689_H
#define __BSP_AD5689_H

#include "stm32f10x.h"


/* AD5689 GPIO引脚设置 */
#define AD5689_SPIx                                      SPI1
#define AD5689_SPI_APBxClockCmd                          RCC_APB2PeriphClockCmd
#define AD5689_SPI_CLK                                   RCC_APB2Periph_SPI1


#define AD5689_GPIO_APBxClockCmd                         RCC_APB2PeriphClockCmd
#define AD5689_GPIO_CLK                                  RCC_APB2Periph_GPIOA
//SYNC
#define AD5689_GPIO_SYNC_PORT                            GPIOA
#define AD5689_GPIO_SYNC_PIN                             GPIO_Pin_4
//SCLK
#define AD5689_GPIO_SCK_PORT                             GPIOA
#define AD5689_GPIO_SCK_PIN                              GPIO_Pin_5
//SDO
#define AD5689_GPIO_MISO_PORT                            GPIOA
#define AD5689_GPIO_MISO_PIN                             GPIO_Pin_6
//SDIN
#define AD5689_GPIO_MOSI_PORT                            GPIOA
#define AD5689_GPIO_MOSI_PIN                             GPIO_Pin_7
//LDAC
#define AD5689_GPIO_LDAC_PORT                            GPIOA
#define AD5689_GPIO_LDAC_PIN                             GPIO_Pin_14
//RESET
#define AD5689_GPIO_RESET_PORT                           GPIOA
#define AD5689_GPIO_RESET_PIN                            GPIO_Pin_15


#define AD5689_SCLK_H                                    GPIO_SetBits(AD5689_GPIO_SCK_PORT,AD5689_GPIO_SCK_PIN)
#define AD5689_SCLK_L                                    GPIO_ResetBits(AD5689_GPIO_SCK_PORT,AD5689_GPIO_SCK_PIN)

#define AD5689_RESET_H                                   GPIO_SetBits(AD5689_GPIO_RESET_PORT,AD5689_GPIO_RESET_PIN)
#define AD5689_RESET_L                                   GPIO_ResetBits(AD5689_GPIO_RESET_PORT,AD5689_GPIO_RESET_PIN)

#define AD5689_SYNC_H                                    GPIO_SetBits(AD5689_GPIO_SYNC_PORT,AD5689_GPIO_SYNC_PIN)
#define AD5689_SYNC_L                                    GPIO_ResetBits(AD5689_GPIO_SYNC_PORT,AD5689_GPIO_SYNC_PIN)



//写入输入寄存器n
#define CMD_WRITEn                                       1
//以输入寄存器n的内容更新DAC寄存器n
#define CMD_WRITEn_UPDATAn                               2
//写入并更新DAC通道n
#define CMD_WRITE_UPDATAn                                3
//DAC掉电/上电
#define CMD_POWER_DOWN                                   4
//硬件LDAC屏蔽寄存器
#define CMD_LDAC_MASK                                    5
//软件复位
#define CMD_RESET                                        6
//设置DCEN寄存器(菊花链使能)
#define CMD_DCEN_REG                                     8
//设置回读寄存器(回读使能)
#define CMD_READ_BACK                                    9


#define DAC_A                                            0x01       //四位0001
#define DAC_B                                            0x08       //四位1000
#define DAC_A_B                                          0x09       //四位1001


//菊花链模式
#define DCENMODE_EN                                      1
//独立模式
#define DCENMODE_DIS                                     0

//正常工作模式
#define NORMALOPERATION                                  0        //两位00

//掉电模式 1KΩ接GND
#define R1KTOGND                                         1        //两位01
//掉电模式 100KΩ接GND
#define R100KTOGND                                       2        //两位10
//掉电模式 三态
#define THREESTATE                                       3        //两位11


//LDAC使能位(DB3、DB0)
#define LDAC_EN                                          1
#define LDAC_DIS                                         0





void AD5689_SetSPIDirection(uint16_t direction);
void AD5689_Config(void);
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char NumberOfByte,uint32_t SPITimeOut);
void AD5689_SetRegisterValue(uint8_t command,uint16_t channel,uint16_t data);
void AD5689_LDAC_MASK(uint16_t ldac1,uint16_t ldac2);
void AD5689_SoftReset(void);
void AD5689_PowerDown(uint16_t channel1,uint16_t channel2);
void AD5689_WriteUpdata_DACREG(uint16_t channel,uint16_t data);
void AD5689_Write_InputREG(uint16_t channel,uint16_t data);
void AD5689_InputREG_toUpdata_DACREG(uint16_t channel,uint16_t data);
void AD5689_DaisyChan_Operation(uint16_t decn);
void AD5689_Init(void);





#endif /* __BSP_AD5689_H */

