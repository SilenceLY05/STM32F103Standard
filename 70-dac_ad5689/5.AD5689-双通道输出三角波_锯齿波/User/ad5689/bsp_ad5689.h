#ifndef __BSP_AD5689_H
#define __BSP_AD5689_H

#include "stm32f10x.h"


/* AD5689 GPIO�������� */
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



//д������Ĵ���n
#define CMD_WRITEn                                       1
//������Ĵ���n�����ݸ���DAC�Ĵ���n
#define CMD_WRITEn_UPDATAn                               2
//д�벢����DACͨ��n
#define CMD_WRITE_UPDATAn                                3
//DAC����/�ϵ�
#define CMD_POWER_DOWN                                   4
//Ӳ��LDAC���μĴ���
#define CMD_LDAC_MASK                                    5
//�����λ
#define CMD_RESET                                        6
//����DCEN�Ĵ���(�ջ���ʹ��)
#define CMD_DCEN_REG                                     8
//���ûض��Ĵ���(�ض�ʹ��)
#define CMD_READ_BACK                                    9


#define DAC_A                                            0x01       //��λ0001
#define DAC_B                                            0x08       //��λ1000
#define DAC_A_B                                          0x09       //��λ1001


//�ջ���ģʽ
#define DCENMODE_EN                                      1
//����ģʽ
#define DCENMODE_DIS                                     0

//��������ģʽ
#define NORMALOPERATION                                  0        //��λ00

//����ģʽ 1K����GND
#define R1KTOGND                                         1        //��λ01
//����ģʽ 100K����GND
#define R100KTOGND                                       2        //��λ10
//����ģʽ ��̬
#define THREESTATE                                       3        //��λ11


//LDACʹ��λ(DB3��DB0)
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

