#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


static uint8_t fac_us=0;                   /* us��ʱ������ */
static uint16_t fac_ms = 0;                /* ms��ʱ������ */

/* ��ʼ���ӳٺ��� SysTickϵͳʱ�� */
void SysTick_Init(uint8_t systick)
{
	SysTick->CTRL &= 0xFFFFFFFB;
	fac_us = systick/8;
	fac_ms = (uint16_t)fac_us*1000;
}



/* ΢����ʱ���� time_usҪ��ʱ΢��ʱ���� */
void Delay_Us(uint32 time_us)
{
	uint32_t temp;                              
	SysTick->LOAD = time_us*fac_us;             //ʱ�����
	SysTick->VAL = 0x00;                        //��ռ�����  
	SysTick->CTRL = 0x01;                       //��ʼ����
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));       //�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00;                       //�رռ�����
	SysTick->VAL = 0x00;                        //��ռ�����
}



/* ΢����ʱ���� time_usҪ��ʱ����ʱ���� */
void Delay_Ms(uint32 time_ms)
{
	uint32_t temp;                              
	SysTick->LOAD = time_ms*fac_ms;             //ʱ�����
	SysTick->VAL = 0x00;                        //��ռ�����  
	SysTick->CTRL = 0x01;                       //��ʼ����
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));       //�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00;                       //�رռ�����
	SysTick->VAL = 0x00;                        //��ռ�����
}

/* ����ʱ���� */
void Delay_S(uint32 time_s)
{
	for(;time_s!=0;time_s--)
		Delay_Ms(1000);
}


/* ��ʮ������ת��Ϊ�ַ��� cҪת��ʮ�������� ����һ���ַ������� */
char c2d(uint8 c)
{
	if(c>='0' && c<= '9')
		return c-'0';
	if(c>= 'a' && c<= 'f')
		return 10+c-'a';
	if(c>= 'A' && c<= 'F')
		return 10+c -'A';
	
	return (char)c;
}


/* ���µ��ַ�����ַ�����������ַ� str:Ҫ�滻���ַ�����oldchar������ַ� newchar�µ��ַ� */
void replacetochar(char *str,char oldchar ,char newchar)
{
	int x;
	for(x=0;str[x];x++)
		if(str[x] == oldchar) str[x] = newchar;
}


/* �ַ���ת��Ϊ16λ�޷������ͺ��� strҪת�����ַ��� baseת���Ļ��� ����ת����������� */
uint16 atoi16(char *str,uint16 base)
{
	uint32 num = 0;
	while(*str != 0)
		num = num*base + c2d(*str++);
	
	return num;
}


/* �ַ���ת��Ϊ32λ�޷������ͺ��� strҪת�����ַ��� baseת���Ļ��� ����ת����������� */
uint32 atoi32(char *str,uint16 base)
{
	uint32 num = 0;
	while(*str != 0)
		num = num*base + c2d(*str++);
	
	return num;
}



/* ������ת��Ϊ�ַ������� n:Ҫת������ str[5]:���ת������ַ��� len���������� */
void itos(uint16 n,uint8 str[5],uint8 len)
{
	uint8 i = len -1;
	
	//��0x20�ո�ֵ��str�ڵ�len��Ԫ��
	memset(str,0x20,len);
	
	do
	{
		str[i--] = n%10+'0';
	}while((n/=10)>0);
	return;
}



/* ���ַ���ת��Ϊʮ���ƻ�ʮ�����ƺ��� str:Ҫת�����ַ��� baseת���Ļ��� ret����ת���������ֵ ����1�ɹ� ����0ʧ�� */
int validatoi(char *str,int base,int*ret)
{
	int c;
	char *tstr = str;
	
	if(str == 0 || *str == '\0')
		return 0;
	
	while(*tstr != '\0')
	{
		c = c2d(*tstr);
		if(c>= 0 && c<base)
			tstr++;
		else
			return 0;
	}
	
	*ret = atoi16(str,base);
	return 1;
	
}


/* 16λ�ַ���8λ��8λת�� iҪת�������� ����ת�����ֵ */
uint16 swaps(uint16 i) 
{
	uint16 ret = 0;
	
	ret = (i& 0xFF) <<8;
	ret |= ((i>>8) & 0xFF);
	return ret;
}

/* 32λ�ַ��ߵ�λ�仯��iҪת�������� ����ת�����ֵ */
uint32 swap32(uint32 i)
{
	uint32_t ret = 0;
	
	ret = (i&0xFF)<<24;
	ret |= ((i>>8)&0xFF)<<16;
	ret |= ((i>>16)&0xFF)<<8;
	ret |= ((i>>24)&0xFF);
	
	return ret;
}


/* ��ȡ�ַ����м�ļ����ַ� srcĿ���ַ��� s1 s2�ǲ����ַ� sub����ȡ����ַ��� */
void mid(int8 *src,int8 *s1,int8 *s2,int8 *sub)
{
	int8 *sub1;
	int8 *sub2;
	uint16 n;
	
	//��ȡs1�ַ���������src�ַ����е�λ��
	sub1 = strstr(src,s1);
	//�ټ���s1�ַ����ĳ���ָ��s1������ַ���
	sub1 += strlen(s1);
	sub2 = strstr(src,s2);
	n = sub2 - sub1;
	
	//���ҵ������ַ�����n�����Ƶ�sub
	strncpy(sub,sub1,n);
	//����ַ���������
	sub[n] = 0;
}


/* ip�����ַת�� addr����ַ ip:ip */
void internet_addr(unsigned char* addr,unsigned char *ip)
{
	int i;
	char taddr[30];
	char *nexttok;
	char num;
	
	strcpy(taddr,(char *)addr);
	
	nexttok = taddr;
	
	for(i = 0;i<4;i++)
	{
		//�ֽ��ַ���Ϊһ���ַ��� ,nexttokλҪ�ֽ���ַ��� .Ϊ�ָ����ַ�
		nexttok = strtok(nexttok,".");
		
		if(nexttok[0] == '0' && nexttok[1] == 'x')
			num = atoi16(nexttok+2,0x10);
		else
			num = atoi16(nexttok,10);
		
		ip[i] = num;
		nexttok = NULL;
	}
}


/* ��32λ��ַת��Ϊʮ���Ƹ�ʽ addr��Ҫת���ĵ�ַ ����ʮ���Ƶ�ַ */
char *internet_ntoa(unsigned long addr)
{
	static char addr_str[32];
	
	//��addr_str��ǰλ�ú����32���ֽ��滻Ϊ0������
	memset(addr_str,0,32);
	sprintf(addr_str,"%d.%d.%d.%d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	
	return addr_str;
}


/* ��16λ��ַת��Ϊʮ���Ƹ�ʽ addr��Ҫת���ĵ�ַ ����ʮ���Ƶ�ַ */
char *internet_ntoa_pad(unsigned long addr)
{
	static char addr_str[16];
	
	//��addr_str��ǰλ�ú����16���ֽ��滻Ϊ0������
	memset(addr_str,0,16);
	sprintf(addr_str,"%03d.%03d.%03d.%03d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	
	return addr_str;
}


/* ��֤IP��ַ addr����ַ ip:ip ����1�ɹ� 0ʧ��*/
char verify_ip_address(char* addr,uint8 *ip)
{
	int i;
	char taddr[50];
	char *tok = taddr;
	int tnum;
	
	strcpy(taddr,addr);
	
	
	for(i = 0;i<4;i++)
	{
		//�ֽ��ַ���Ϊһ���ַ��� ,nexttokλҪ�ֽ���ַ��� .Ϊ�ָ����ַ�
		tok = strtok(tok,".");
		
		if(!tok) return 0;
		
		if(tok[0] == '0' && tok[1] == 'x')
		{
			if(!validatoi(tok+2,0x10,&tnum)) return 0;
		}
		else if(!validatoi(tok,10,&tnum)) return 0;
		
		ip[i] = tnum;
		
		if(tnum<0 || tnum>255) return 0;
		
		tok = NULL;
	}
	return 1;
}


/* ��һ������ģʽ��unsigned short������ת�������ģʽ��TCP/IP �����ֽڸ�ʽ������ ����Ҫת�������� ���ش��ģʽ������ */
/**< A 16-bit number in host byte order.  */
uint16 htons(uint16 hostshort)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return swaps(hostshort);
	#else
		return hostshort;
	#endif
}


/* ��һ������ģʽ��unsigned long������ת�������ģʽ��TCP/IP �����ֽڸ�ʽ������ ����Ҫת�������� ���ش��ģʽ������ */
/* ���ģʽ��������ָ�����ݵĸ��ֽڱ������ڴ�ĵ͵�ַ�У����ֽڱ����ڸߵ�ַ�� */
/**< hostshort  - A 32-bit number in host byte order.  */
unsigned long htonl(unsigned long hostlong)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return swap32(hostlong);
	#else
		return hostlong;
	#endif
}


/* ��һ�����ģʽ��TCP/IP �����ֽڸ�ʽ������ת��������ģʽ��unsigned short������ ����Ҫת�������� ����unsigned shortģʽ������ */
/**< netshort - network odering 16bit value */
unsigned long ntohs(unsigned short netshort)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return htons(netshort);
	#else
		return netshort;
	#endif
}



/* ��һ�����ģʽ��TCP/IP �����ֽڸ�ʽ������ת��������ģʽ��unsigned long������ ����Ҫת�������� ����unsigned longģʽ������ */
/**< netshort - network odering 16bit value */
unsigned long ntohl(unsigned long netlong)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return htonl(netlong);
	#else
		return netlong;
	#endif
}


/* �����ַ���У��� src:Ҫ����������� len:����������  */
unsigned short checksun(unsigned char *src,unsigned int len)
{
	u_int sum,tsum,i,j;
	u_long lsum;
	
	//�洢��������16λ���ݵ�Ԫ��������
	j = len>>1;
	
	lsum = 0;
	
	for(i = 0;i<j;i++)
	{
		//��ȡ��ǰ16λ���ݵ�Ԫ�ĸ�8λ
		tsum = src[i*2];
		//����8λ����8λ
		tsum = tsum << 8;
		//���뵱ǰ16λ���ݵ�Ԫ�ĵ�8λ
		tsum += src[i*2 +1];
		lsum += tsum;
	}
	
	if(len % 2)//�������������������������һ���ֽ�δ����
	{
		//��ȡ����ֽ�
		tsum = src[i*2];
		//������ֽ���Ϊ16λ���ĸ�8λ������lsum
		lsum += (tsum << 8);
	}
	
	sum = lsum;
	sum =~(sum + (lsum >>16));
	return (u_short)sum;
}


/* ����ַ�Ƿ����� destipҪת�������� ����1�ɹ� 0ʧ�� */
u_char check_dest_in_local(u_long destip)
{
	int i = 0;
	u_char *pdestip = (u_char*)&destip;
	
	for(i=0;i<4;i++)
	{
		if((pdestip[i] & IINCHIP_READ(SUBR0+i)) != (IINCHIP_READ(SIPR0+i) & IINCHIP_READ(SUBR0+i)))
			return 1;	// Remote
	}
	return 0;
}



	



