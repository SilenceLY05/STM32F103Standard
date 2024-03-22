#ifndef __BSP_W5500_CONF_H__
#define __BSP_W5500_CONF_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "types.h"

#define __GNUC__

typedef void (*pFunction)(void);

extern uint8   remote_ip[4];                                     //远端IP地址
extern uint16  remote_port;                                      //远端端口号
extern uint16  local_port;                                       //定义本地端口
extern uint8   use_dhcp;                                         //是否使用DHCP获取IP
extern uint8   use_eeprom;                                       //是否使用EEPROM中的IP配置信息


#define STM32F103VET6
#undef  STM32F103ZET6


/* 定义SPI2作为W5500的硬件接口 */
#define WIZ_SPIx                     SPI2                        //定义W5500所用的SPI接口
#define WIZ_SPI_APBxClock            RCC_APB1PeriphClockCmd      
#define WIZ_SPI_CLK                  RCC_APB1Periph_SPI2         //定义W5500所用的SPI接口时钟
#define WIZ_SPI_GPIO_APBxClock       RCC_APB2PeriphClockCmd
#define WIZ_SPI_GPIO_CLK             RCC_APB2Periph_GPIOB        //GPIO端口时钟
#define WIZ_SPI_GPIO_PORT            GPIOB                       //GPIO端口
#define WIZ_SPI_GPIO_SCLK            GPIO_Pin_13                 //定义W5500的时钟管脚
#define WIZ_SPI_GPIO_MISO            GPIO_Pin_14                 //定义W5500的MISO管脚
#define WIZ_SPI_GPIO_MOSI            GPIO_Pin_15                 //定义W5500的MOSI管脚



#ifdef  STM32F103ZET6
#define WIZ_SPI_SCS_CLK              RCC_APB2Periph_GPIOG        //GPIO端口时钟
#define WIZ_SPI_SCS_PORT             GPIOG                       //GPIO端口
#define WIZ_SPI_SCS_PIN              GPIO_Pin_9                  //定义W5500片选引脚

#define WIZ_SPI_RESET_CLK            RCC_APB2Periph_GPIOG        //GPIO端口时钟
#define WIZ_SPI_RESET_PORT           GPIOG                       //GPIO端口
#define WIZ_SPI_RESET_PIN            GPIO_Pin_15                 //定义W5500 RESET引脚

#define WIZ_SPI_INT_CLK              RCC_APB2Periph_GPIOG        //GPIO端口时钟
#define WIZ_SPI_INT_PORT             GPIOG                       //GPIO端口
#define WIZ_SPI_INT_PIN              GPIO_Pin_8                  //定义W5500 INT引脚
#endif



#ifdef  STM32F103VET6
#define WIZ_SPI_SCS_CLK              RCC_APB2Periph_GPIOB        //GPIO端口时钟
#define WIZ_SPI_SCS_PORT             GPIOB                       //GPIO端口
#define WIZ_SPI_SCS_PIN              GPIO_Pin_12                 //定义W5500片选引脚

#define WIZ_SPI_RESET_CLK            RCC_APB2Periph_GPIOC        //GPIO端口时钟
#define WIZ_SPI_RESET_PORT           GPIOC                       //GPIO端口
#define WIZ_SPI_RESET_PIN            GPIO_Pin_7                  //定义W5500 RESET引脚

#define WIZ_SPI_INT_CLK              RCC_APB2Periph_GPIOC        //GPIO端口时钟
#define WIZ_SPI_INT_PORT             GPIOC                       //GPIO端口
#define WIZ_SPI_INT_PIN              GPIO_Pin_6                  //定义W5500 INT引脚
#endif


#define FW_VER_HIGH                  1
#define FW_VER_LOW                   0
#define ON                           1
#define OFF                          0
#define HIGH                         1
#define LOW                          0

#define MAX_BUFF_SIZE                1460                        //定义每个数据包的大小
#define KEEP_ALIVE_TIME              30
#define TX_RX_MAX_BUFF_SIZE          2048
#define EEPROM_MSG_LEN               sizeof(EEPROM_MSG)
	
#define IP_FROM_DEFINE               0                           //使用初始定义的IP信息
#define IP_FROM_DHCP                 1                           //使用DHCP获取IP信息
#define IP_FROM_EEPROM               2                           //使用EEPROM定义的IP信息
extern uint8 ip_from;                                            //选择IP信息配置源

#pragma pack(1)
/* 此结构体定义了W5500可供配置的主要参数 */
typedef struct _CONFIG_MSG
{
	uint8 mac[6];                                                  //MAC地址
	uint8 lip[4];                                                  //local IP本地IP地址
	uint8 sub[4];                                                  //子网掩码
	uint8 gw[4];                                                   //网关
	uint8 dns[4];                                                  //DNS服务器地址
	uint8 rip[4];                                                  //remote IP远程IP地址
	uint8 sw_ver[2];                                               //软件版本号
}CONFIG_MSG;
#pragma pack()


#pragma pack(1)
/* 此结构定义了EEPROM写入的几个变量，可按需修改 */
typedef struct _EEPROM_MSG
{
	uint8 mac[6];                                                  //MAC地址
	uint8 lip[4];                                                  //locak IP本地IP地址
	uint8 sub[4];                                                  //子网掩码
	uint8 gw[4];                                                   //网关
}EEPROM_MSG_STR;
#pragma pack()



extern EEPROM_MSG_STR EEPROM_MSG;
extern CONFIG_MSG ConfigMsg;
extern uint8 dhcp_ok;                                             //DHCP运行成功
extern uint32 dhcp_time;                                          //DHCP运行计数
extern vu8 ntptimer;                                              //NPT秒计数


/*MCU配置相关函数*/
void W5500_GPIO_Config(void);
void Timer2_Init(void);
void Timer2_Interrupt(void);
void WriteConfigToEeprom(void);
void ReadConfigFromEeprom(void);
void reboot(void);


/*W5500SPI相关函数*/
void IINCHIP_WRITE(uint32 address,uint8 data);
uint8 IINCHIP_READ(uint32 address);
uint16_t WIZ_WriteBuff(uint32 address,uint8 *buff,uint16 len);
uint16_t WIZ_ReadBuff(uint32 address,uint8 *buff,uint16 len);


/*W5500基本配置相关函数*/
void W5500_Reset(void);
void Set_W5500_MAC(void);
void Set_W5500_IP(void);

/*需要用定时的的应用函数*/
void NTP_Timer_Init(void);
void DHCP_Timer_Init(void);


void WIZ_CS(uint8_t val);
void IINCHIP_CSOFF(void);
void IINCHIP_CSON(void);
uint8_t SPI_SendByte(uint8_t byte);
uint8 IINCHIP_SpiSendData(uint8 data);




#endif /* __BSP_W5500_CONF_H__ */

