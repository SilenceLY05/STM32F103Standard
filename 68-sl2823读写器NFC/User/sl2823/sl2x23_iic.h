#ifndef   __SL2X23_IIC_H__
#define   __SL2X23_IIC_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f10x.h"

#define SL2x23_DEV_ADDRESS    0x28   /* SL2823���豸��ַ */


/* SL2x23 I2C��ʱ */
#define SL2x23_FLAG_TIMEOUT       ((uint32_t)0x1000)
#define SL2x23_LONG_TIMEOUT       ((uint32_t)(10 * SL2x23_FLAG_TIMEOUT))


/* �ȴ���־λ */
#define WAIT_FOR_FLAG(flag,value,timeout)       SL2x23_Timeout = timeout;\
						while(I2C_GetFlagStatus(SL2x23_I2C,flag) != value){\
							if((SL2x23_Timeout--) == 0) return SL2x23_TIMEOUT_UserCallBack();\
						}
						
						
/* �����У�����ADDR��־λ */
#define CLEAR_ADDR_BIT       I2C_ReadRegister(SL2x23_I2C,I2C_Register_SR1);\
						                 I2C_ReadRegister(SL2x23_I2C,I2C_Register_SR2);



/* ����I2C�������ӵ�GPIO�˿ڣ��û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA���� */
#define SL2x23_I2C                      I2C2
#define SL2x23_I2C_CLK                  RCC_APB1Periph_I2C2

#define SL2x23_I2C_SCL_GPIO_CLK         RCC_APB2Periph_GPIOB
#define SL2x23_I2C_SCL_GPIO_PORT        GPIOB
#define SL2x23_I2C_SCL_PIN              GPIO_Pin_10
#define SL2x23_I2C_SCL_SOURCE           GPIO_PinSource10
						
#define SL2x23_I2C_SDA_GPIO_CLK         RCC_APB2Periph_GPIOB
#define SL2x23_I2C_SDA_GPIO_PORT        GPIOB
#define SL2x23_I2C_SDA_PIN              GPIO_Pin_11
#define SL2x23_I2C_SDA_SOURCE           GPIO_PinSource11



#define SL2x23_I2C_SCL_1()  SL2x23_I2C_SCL_GPIO_PORT->BSRR = SL2x23_I2C_SCL_PIN   /* SCL = 1 */
#define SL2x23_I2C_SCL_0()  SL2x23_I2C_SCL_GPIO_PORT->BRR = SL2x23_I2C_SCL_PIN   /* SCL = 0 */
	
#define SL2x23_I2C_SDA_1()  SL2x23_I2C_SDA_GPIO_PORT->BSRR = SL2x23_I2C_SDA_PIN   /* SDA = 1 */
#define SL2x23_I2C_SDA_0()  SL2x23_I2C_SDA_GPIO_PORT->BRR = SL2x23_I2C_SDA_PIN   /* SDA = 0 */
	
#define SL2x23_I2C_SDA_READ()   ((SL2x23_I2C_SDA_GPIO_PORT->IDR & SL2x23_I2C_SDA_PIN) != 0)   /* ��SDA����״̬ */

/* SDA IO�������ã���©�������Ҫ��������� */
#define SL2x23_I2C_SDA_IN()   {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(uint32_t)8<<28;}
#define SL2x23_I2C_SDA_OUT()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(uint32_t)3<<28;}
	

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
void SL2x23_I2C_Init(void);


uint32_t SL2x23_TIMEOUT_UserCallBack(void);
uint32_t i2c_write_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t reg_value);
uint32_t i2c_read_word(uint8_t i2c_dev_addr,uint8_t reg_addr,uint8_t *reg_value);
uint8_t i2c_CheckDevice(uint8_t _Address);


#endif   /* __SL2X23_IIC_H__  */
