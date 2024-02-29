#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "stm32f10x.h"
#include "bsp_usart.h"




#define SENSORS_I2Cx                     I2C1
#define SENSORS_I2C_APBxClock_FUN        RCC_APB1PeriphClockCmd
#define SENSORS_I2C_CLK                  RCC_APB1Periph_I2C1
#define SENSORS_I2C_GPIO_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define SENSORS_I2C_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SENSORS_I2C_SCL_PORT             GPIOB
#define SENSORS_I2C_SCL_PIN              GPIO_Pin_6
#define SENSORS_I2C_SDA_PORT             GPIOB
#define SENSORS_I2C_SDA_PIN              GPIO_Pin_7


/* STM32 I2C 快速模式 */
#define I2C_Speed            400000

/* 这个地址只要是与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7         0X0A


/* 等待超时时间 */
#define I2CT_FLAG_TIMEOUT           ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT           ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/* 信息输出 */
#define MPU_DEBUG_ON     0
#define MPU_INFO(fmt,arg...)      printf("<<-MPU-INFO->>"fmt"\n",##arg)
#define MPU_ERROR(fmt,arg...)     printf("<<-MPU-ERROR->>"fmt"\n",##arg)
#define MPU_DEBUG(fmt,arg...)     do{\
																		 if(MPU_DEBUG_ON)\
																		 printf("<<-MPU-DEBUG->> [%d]"fmt"\n",__LINE__,##arg);\
																		}while(0)

void I2C_Bus_Init(void);
uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr);
uint8_t I2C_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_WaitStandbyState(void);																		 

																		 
#endif  /*__BSP_I2C_H */

