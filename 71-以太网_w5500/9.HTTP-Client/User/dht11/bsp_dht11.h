#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#include "stm32f10x.h"
#include "bsp_w5500_conf.h"

/************************** DHT11 数据类型定义********************************/
typedef struct 
{
	uint8_t humi_int;     //湿度的整数部分
	uint8_t humi_deci;    //湿度的小数部分
	uint8_t temp_int;     //温度的整数部分
	uint8_t temp_deci;    //温度的小数部分
	uint8_t check_sum;    //校验和
}DHT11_Data_Typedef;

/******************************** DHT11 引脚定义 ************************************/
#define DHT11_Dout_SCK_ABPxClock_FUN                 RCC_APB2PeriphClockCmd
#define DHT11_Dout_GPIO_CLK                          RCC_APB2Periph_GPIOE
#define DHT11_Dout_GPIO_PORT                         GPIOE
#define DHT11_Dout_GPIO_PIN                          GPIO_Pin_6

/******************************** DHT11 宏函数定义 ************************************/
#define DHT11_Dout_0                   GPIO_ResetBits(DHT11_Dout_GPIO_PORT,DHT11_Dout_GPIO_PIN)
#define DHT11_Dout_1                   GPIO_SetBits(DHT11_Dout_GPIO_PORT,DHT11_Dout_GPIO_PIN)

#define DHT11_Dout_IN()                GPIO_ReadInputDataBit(DHT11_Dout_GPIO_PORT,DHT11_Dout_GPIO_PIN)

#define DHT11_HIGH         1
#define DHT11_LOW          0

void                     DHT11_Init                      ( void );
uint8_t                  DHT11_Read_TempAndHumidity      ( DHT11_Data_Typedef * DHT11_Data );



#endif  /* __BSP_DHT11_H */

