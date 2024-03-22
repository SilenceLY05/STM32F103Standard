#include "bsp_w5500_conf.h"
#include "bsp_w5500.h"
#include "bsp_i2c_ee.h"
#include "utility.h"
#include "bsp_TimeBase.h"
#include "dhcp.h"
#include <string.h>
#include <stdio.h>

CONFIG_MSG  ConfigMsg;                      /*���ýṹ��*/
EEPROM_MSG_STR   EEPROM_MSG;                /*EEPROM�洢��Ϣ�ṹ��*/

/* ����MAC��ַ ������W5500�����������һ������ʹ�ò�ͬ��MAC��ַ */
uint8 mac[6] = {0x00,0x08,0xdc,0x11,0x11,0x11};


/* ����Ĭ��IP��Ϣ */
uint8 local_ip[4] = {192,168,3,88};                              //����W5500Ĭ��IP��ַ
uint8 subnet[4] = {255,255,255,0};                               //����W5500Ĭ����������
uint8 gateway[4] = {192,168,3,1};                                //����W5500Ĭ������
uint8 dns_server[4] = {114,114,114,114};                         //����W5500Ĭ��DNS

uint16 locak_port = 5000;                                        //���屾�ض˿�

/* ����Զ��IP��Ϣ */
uint8 remote_ip[4] = {192,168,3,8};                              //Զ��IP��ַ
uint16 remote_port = 5000;                                       //Զ�˶˿ں�


/* IP���÷���ѡ�� */
uint8 ip_from = IP_FROM_DEFINE;

uint8  dhcp_ok   = 0;                                             //dhcp�ɹ���ȡIP
uint32 ms        = 0;                                             //�����ʱ
uint32 dhcp_time = 0;                                             //DHCP���м���
vu8    ntptimer  = 0;                                             //NPT�����



/* ����W5500��IP��ַ */
void Set_W5500_IP(void)
{
	/* ���ƶ����������Ϣ�����ýṹ�� */
	memcpy(ConfigMsg.mac,mac,6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	
	if(ip_from == IP_FROM_DEFINE)
		printf("ʹ�ö����IP��Ϣ����W5500 \r\n");
	
	/* ʹ��EEPROM�洢��IP���� */
	if(ip_from == IP_FROM_EEPROM)
	{
		/* ��EEPROM�ж�ȡIP������Ϣ */
		ReadConfigFromEeprom();
		
		/* �����ȡEEPROM��MAC��Ϣ����������ã����ʹ�� */
		if(*(EEPROM_MSG.mac) == 0x00 && *(EEPROM_MSG.mac+1) == 0x08 && *(EEPROM_MSG.mac+2) == 0xdc)
		{
			printf("IP from EEPROM \r\n");
			
			/* ����EEPROM������Ϣ�����õĽṹ����� */
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip,4);
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub,4);
			memcpy(ConfigMsg.gw,EEPROM_MSG.gw,4);
			
		}
		else
		{
			printf("EEPROMδ���ã�ʹ�ö����IP��Ϣ����W5500����д��EEPROM\r\n");
			WriteConfigToEeprom();//ʹ��Ĭ�ϵ�IP��Ϣ������ʼ��EEPROM�е�����
		}
	}
	
	/* ʹ��DHCP��ȡIP�����������DHCP�Ӻ��� */
	if(ip_from == IP_FROM_DHCP)
	{
		/* ����DHCP��ȡ��������Ϣ�����ýṹ�� */
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
			printf("DHCP�ӳ���δ���У����߲��ɹ�\r\n");
			printf("ʹ�ö����IP��Ϣ����W5500\r\n");
		}
	}
	
	/* ����������Ϣ��������Ҫѡ�� */
	ConfigMsg.sw_ver[0] = FW_VER_HIGH;
	ConfigMsg.sw_ver[1] = FW_VER_LOW;
	
	/* ��IP������Ϣд��W5500��Ӧ�Ĵ��� */
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	setSIPR(local_ip);
	printf("W5500 IP��ַ:%d.%d.%d.%d\r\n",local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("W5500 ��������:%d.%d.%d.%d\r\n",subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("W5500 ����:%d.%d.%d.%d\r\n",gateway[0],gateway[1],gateway[2],gateway[3]);
}

/* ����W5500��MAC��ַ */
void Set_W5500_MAC(void)
{
	memcpy(ConfigMsg.mac,mac,6);
	setSHAR(ConfigMsg.mac);
	memcpy(DHCP_GET.mac,mac,6);
}



/* ����W5500��GPIO�ӿ� */
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


/* W5500Ƭѡ�ź����ú��� valΪ0��ʾƬѡ�˿�Ϊ�� Ϊ1��ʾƬѡ�˿�Ϊ�� */
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


/* ����W5500��Ƭѡ�˿�SCSnΪ�� */
void IINCHIP_CSOFF(void)
{
	WIZ_CS(LOW);
}

/* ����W5500��Ƭѡ�˿�SCSnΪ�� */
void IINCHIP_CSON(void)
{
	WIZ_CS(HIGH);
}


/* W5500��λ���ú��� */
void W5500_Reset(void)
{
	GPIO_ResetBits(WIZ_SPI_RESET_PORT,WIZ_SPI_RESET_PIN);
	Delay_Us(2);
	GPIO_SetBits(WIZ_SPI_RESET_PORT,WIZ_SPI_RESET_PIN);
	Delay_Ms(1600);
}


/* ʹ��SPI����һ�ֽڵ����� ByteҪ���͵����� */
uint8_t SPI_SendByte(uint8_t byte)
{
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
	SPI_I2S_SendData(WIZ_SPIx,byte);
	
	while(SPI_I2S_GetFlagStatus(WIZ_SPIx,SPI_I2S_FLAG_RXNE) == RESET);
	
	return SPI_I2S_ReceiveData(WIZ_SPIx);
}



/* STM32 SPI2д��8λ���� */
uint8 IINCHIP_SpiSendData(uint8 data)
{
	return SPI_SendByte(data);
}


/* д��һ��8λ���ݵ�W5500 SCSΪ�ʹ���SPI֡��ʼ Ϊ�߱�ʾ���� address:д�����ݵĵ�ַ data:д���8λ���� */
void IINCHIP_WRITE(uint32 address,uint8 data)
{
	IINCHIP_CSOFF();
	IINCHIP_SpiSendData((address & 0x00FF0000)>>16);
	IINCHIP_SpiSendData((address & 0x0000FF00)>>8);
	IINCHIP_SpiSendData((address & 0x000000F8) +4);   //��4��ʾд����
	IINCHIP_SpiSendData(data);
	IINCHIP_CSON();
}


/* ��W5500����һ��8Ϊ����  ����address:��ȡ���ݵĵ�ַ ����ֵdata8λ���� */
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


/* ��W5500д��len���ֽ����� address:д�����ݵĵ�ַ buff:д�� �ַ��� len:�ַ������� ����len�ַ������� */
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
	IINCHIP_SpiSendData((address & 0x000000F8) +4);   //��4��ʾд����
	for(idx = 0;idx<len;idx++)
	{
		IINCHIP_SpiSendData(buff[idx]);
	}
	IINCHIP_CSON();
	
	return len;
}


/* ��W5500����len���ֽ����� address:��ȡ���ݵĵ�ַ buff:��Ŷ�ȡ���� len:�ַ������� ����len�ַ������� */
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
	IINCHIP_SpiSendData((address & 0x000000F8) );   //��4��ʾд����
	for(idx = 0;idx<len;idx++)
	{
		buff[idx] = IINCHIP_SpiSendData(0x00);
	}
	IINCHIP_CSON();
	
	return len;
}


/* д������Ϣ��EEPROM */
void WriteConfigToEeprom(void)
{
	uint16 Addr = 0;
	ee_WriteBytes(ConfigMsg.mac,Addr,(uint8)EEPROM_MSG_LEN);
	Delay_Ms(10);
}

/* ��EEPROM��ȡ������Ϣ */
void ReadConfigFromEeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	Delay_Us(10);
}


/* STM32��ʱ����ʼ�� */
void Timer2_Init(void)
{
	BASIC_TIM_Mode_Config();                                       /* TIM2 ��ʱ���� */
	BASIC_TIM_NCIV_Config();                                       /* ��ʱ�����ж����ȼ� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);            /* TIM2 ���¿�ʱ�ӣ���ʼ��ʱ */
}


/* DHCP�õ��Ķ�ʱ����ʼ�� */
void DHCP_Timer_Init(void)
{
	Timer2_Init();
}

/* NTP�õ��Ķ�ʱ����ʼ�� */
void NTP_Timer_Init(void)
{
	Timer2_Init();
}



/* ��ʱ��2�жϺ��� */
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

/* STM32ϵͳ��λ���� */
void reboot(void)
{
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	printf("ϵͳ������.....\r\n");
	JumpAddress = *(vu32*)(0x00000004);
	Jump_To_Application = (pFunction)JumpAddress;
	Jump_To_Application();
}

