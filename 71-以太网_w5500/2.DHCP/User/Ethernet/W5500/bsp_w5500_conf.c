#include "bsp_w5500_conf.h"
#include "bsp_w5500.h"
#include "bsp_i2c_ee.h"
#include "utility.h"
#include "bsp_TimeBase.h"
#include "dhcp.h"
#include <string.h>
#include <stdio.h>

CONFIG_MSG  ConfigMsg;                      /*配置结构体*/
EEPROM_MSG_STR   EEPROM_MSG;                /*EEPROM存储信息结构体*/

/* 定义MAC地址 如果多块W5500网络适配板在一起工作，使用不同的MAC地址 */
uint8 mac[6] = {0x00,0x08,0xdc,0x11,0x11,0x11};


/* 定义默认IP信息 */
uint8 local_ip[4] = {192,168,3,88};                              //定义W5500默认IP地址
uint8 subnet[4] = {255,255,255,0};                               //定义W5500默认子网掩码
uint8 gateway[4] = {192,168,3,1};                                //定义W5500默认网关
uint8 dns_server[4] = {114,114,114,114};                         //定义W5500默认DNS

uint16 locak_port = 5000;                                        //定义本地端口

/* 定义远端IP信息 */
uint8 remote_ip[4] = {192,168,3,8};                              //远端IP地址
uint16 remote_port = 5000;                                       //远端端口号


/* IP配置方法选择 */
uint8 ip_from = IP_FROM_DEFINE;

uint8  dhcp_ok   = 0;                                             //dhcp成功获取IP
uint32 ms        = 0;                                             //毫秒计时
uint32 dhcp_time = 0;                                             //DHCP运行计数
vu8    ntptimer  = 0;                                             //NPT秒计数



/* 配置W5500的IP地址 */
void Set_W5500_IP(void)
{
	/* 复制定义的配置信息到配置结构体 */
	memcpy(ConfigMsg.mac,mac,6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	
	if(ip_from == IP_FROM_DEFINE)
		printf("使用定义的IP信息配置W5500 \r\n");
	
	/* 使用EEPROM存储的IP参数 */
	if(ip_from == IP_FROM_EEPROM)
	{
		/* 从EEPROM中读取IP配置信息 */
		ReadConfigFromEeprom();
		
		/* 如果读取EEPROM中MAC信息，如果已配置，则可使用 */
		if(*(EEPROM_MSG.mac) == 0x00 && *(EEPROM_MSG.mac+1) == 0x08 && *(EEPROM_MSG.mac+2) == 0xdc)
		{
			printf("IP from EEPROM \r\n");
			
			/* 复制EEPROM配置信息到配置的结构体变量 */
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip,4);
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub,4);
			memcpy(ConfigMsg.gw,EEPROM_MSG.gw,4);
			
		}
		else
		{
			printf("EEPROM未配置，使用定义的IP信息配置W5500，并写入EEPROM\r\n");
			WriteConfigToEeprom();//使用默认的IP信息，并初始化EEPROM中的数据
		}
	}
	
	/* 使用DHCP获取IP参数，需调用DHCP子函数 */
	if(ip_from == IP_FROM_DHCP)
	{
		/* 复制DHCP获取的配置信息到配置结构体 */
		if(dhcp_ok ==1)
		{
			printf("IP from DHCP \r\n");
			memcpy(ConfigMsg.lip,DHCP_GET.lip,4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub,4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw,4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
			
		}
		else
		{
			printf("DHCP子程序未运行，或者不成功\r\n");
			printf("使用定义的IP信息配置W5500\r\n");
		}
	}
	
	/* 以下配置信息，根据需要选用 */
	ConfigMsg.sw_ver[0] = FW_VER_HIGH;
	ConfigMsg.sw_ver[1] = FW_VER_LOW;
	
	/* 将IP配置信息写入W5500相应寄存器 */
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	setSIPR(local_ip);
	printf("W5500 IP地址:%d.%d.%d.%d\r\n",local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("W5500 子网掩码:%d.%d.%d.%d\r\n",subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("W5500 网关:%d.%d.%d.%d\r\n",gateway[0],gateway[1],gateway[2],gateway[3]);
}

/* 配置W5500的MAC地址 */
void Set_W5500_MAC(void)
{
	memcpy(ConfigMsg.mac,mac,6);
	setSHAR(ConfigMsg.mac);
	memcpy(DHCP_GET.mac,mac,6);
}



/* 配置W5500的GPIO接口 */
void W5500_GPIO_Config(void)
{
	GPIO_InitTypeDef                 GPIO_InitStructure;
	SPI_InitTypeDef                  SPI_InitStructure;
	
	WIZ_SPI_APBxClock(WIZ_SPI_CLK,ENABLE);
	WIZ_SPI_GPIO_APBxClock(WIZ_SPI_GPIO_CLK | RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_GPIO_SCLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIZ_SPI_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_GPIO_MISO;
	GPIO_Init(WIZ_SPI_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_GPIO_MOSI;
	GPIO_Init(WIZ_SPI_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_SCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(WIZ_SPI_SCS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIZ_SPI_RESET_PORT,&GPIO_InitStructure);
	GPIO_SetBits(WIZ_SPI_RESET_PORT,WIZ_SPI_RESET_PIN);
	
	GPIO_InitStructure.GPIO_Pin = WIZ_SPI_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIZ_SPI_INT_PORT,&GPIO_InitStructure);
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Cmd(WIZ_SPIx,ENABLE);
}


/* W5500片选信号设置函数 val为0表示片选端口为低 为1表示片选端口为高 */
void WIZ_CS(uint8_t val)
{
	if(val == HIGH)
	{
		GPIO_SetBits(WIZ_SPI_SCS_PORT,WIZ_SPI_SCS_PIN);
	}
	else if(val == LOW)
	{
		GPIO_ResetBits(WIZ_SPI_SCS_PORT,WIZ_SPI_SCS_PIN);
	}
}


/* 设置W5500的片选端口SCSn为低 */
void IINCHIP_CSOFF(void)
{
	WIZ_CS(LOW);
}

/* 设置W5500的片选端口SCSn为高 */
void IINCHIP_CSON(void)
{
	WIZ_CS(HIGH);
}


/* W5500复位设置函数 */
void W5500_Reset(void)
{
	GPIO_ResetBits(WIZ_SPI_RESET_PORT,WIZ_SPI_RESET_PIN);
	Delay_Us(2);
	GPIO_SetBits(WIZ_SPI_RESET_PORT,WIZ_SPI_RESET_PIN);
	Delay_Ms(1600);
}


/* 使用SPI发送一字节的数据 Byte要发送的数据 */
uint8_t SPI_SendByte(uint8_t byte)
{
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
	SPI_I2S_SendData(WIZ_SPIx,byte);
	
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx,SPI_I2S_FLAG_RXNE) == RESET);
	
	return SPI_I2S_ReceiveData(WIZ_SPIx);
}



/* STM32 SPI2写入8位数据 */
uint8 IINCHIP_SpiSendData(uint8 data)
{
	return SPI_SendByte(data);
}


/* 写入一个8位数据到W5500 SCS为低代表SPI帧开始 为高表示结束 address:写入数据的地址 data:写入的8位数据 */
void IINCHIP_WRITE(uint32 address,uint8 data)
{
	IINCHIP_CSOFF();
	IINCHIP_SpiSendData((address & 0x00FF0000)>>16);
	IINCHIP_SpiSendData((address & 0x0000FF00)>>8);
	IINCHIP_SpiSendData((address & 0x000000F8) +4);   //加4表示写操作
	IINCHIP_SpiSendData(data);
	IINCHIP_CSON();
}


/* 从W5500读出一个8为数据  输入address:读取数据的地址 返回值data8位数据 */
uint8 IINCHIP_READ(uint32 address)
{
	uint8 data;
	IINCHIP_CSOFF();
	IINCHIP_SpiSendData((address & 0x00FF0000)>>16);
	IINCHIP_SpiSendData((address & 0x0000FF00)>>8);
	IINCHIP_SpiSendData((address & 0x000000F8));
	data = IINCHIP_SpiSendData(0x00);
	IINCHIP_CSON();
	return data;
}


/* 向W5500写入len个字节数据 address:写入数据的地址 buff:写入 字符串 len:字符串长度 返回len字符串长度 */
uint16_t WIZ_WriteBuff(uint32 address,uint8 *buff,uint16 len)
{
	uint16 idx = 0;
	if(len == 0)
	{
		printf("Unexpected length 0\r\n");
	}
	IINCHIP_CSOFF();
	IINCHIP_SpiSendData((address & 0x00FF0000)>>16);
	IINCHIP_SpiSendData((address & 0x0000FF00)>>8);
	IINCHIP_SpiSendData((address & 0x000000F8) +4);   //加4表示写操作
	for(idx = 0;idx<len;idx++)
	{
		IINCHIP_SpiSendData(buff[idx]);
	}
	IINCHIP_CSON();
	
	return len;
}


/* 从W5500读出len个字节数据 address:读取数据的地址 buff:存放读取数据 len:字符串长度 返回len字符串长度 */
uint16_t WIZ_ReadBuff(uint32 address,uint8 *buff,uint16 len)
{
	uint16 idx = 0;
	if(len == 0)
	{
		printf("Unexpected2 length 0\r\n");
	}
	IINCHIP_CSOFF();
	IINCHIP_SpiSendData((address & 0x00FF0000)>>16);
	IINCHIP_SpiSendData((address & 0x0000FF00)>>8);
	IINCHIP_SpiSendData((address & 0x000000F8) );   //加4表示写操作
	for(idx = 0;idx<len;idx++)
	{
		buff[idx] = IINCHIP_SpiSendData(0x00);
	}
	IINCHIP_CSON();
	
	return len;
}


/* 写配置信息到EEPROM */
void WriteConfigToEeprom(void)
{
	uint16 Addr = 0;
	ee_WriteBytes(ConfigMsg.mac,Addr,(uint8)EEPROM_MSG_LEN);
	Delay_Ms(10);
}

/* 从EEPROM读取配置信息 */
void ReadConfigFromEeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	Delay_Us(10);
}


/* STM32定时器初始化 */
void Timer2_Init(void)
{
	BASIC_TIM_Mode_Config();                                       /* TIM2 定时配置 */
	BASIC_TIM_NCIV_Config();                                       /* 定时器的中断优先级 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);            /* TIM2 重新开时钟，开始计时 */
}


/* DHCP用到的定时器初始化 */
void DHCP_Timer_Init(void)
{
	Timer2_Init();
}

/* NTP用到的定时器初始化 */
void NTP_Timer_Init(void)
{
	Timer2_Init();
}



/* 定时器2中断函数 */
void Timer2_Interrupt(void)
{
	ms++;
	if(ms>=1000)
	{
		ms=0;
		dhcp_time++;
		#ifndef __NTP_H__
		ntptimer++;
		#endif
	}
}

/* STM32系统软复位函数 */
void reboot(void)
{
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	printf("系统重启中.....\r\n");
	JumpAddress = *(vu32*)(0x00000004);
	Jump_To_Application = (pFunction)JumpAddress;
	Jump_To_Application();
}

