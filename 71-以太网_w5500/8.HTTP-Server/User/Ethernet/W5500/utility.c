#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


static uint8_t fac_us=0;                   /* us延时倍乘数 */
static uint16_t fac_ms = 0;                /* ms延时倍乘数 */

/* 初始化延迟函数 SysTick系统时钟 */
void SysTick_Init(uint8_t systick)
{
	SysTick->CTRL &= 0xFFFFFFFB;
	fac_us = systick/8;
	fac_ms = (uint16_t)fac_us*1000;
}



/* 微秒延时函数 time_us要延时微秒时间数 */
void Delay_Us(uint32 time_us)
{
	uint32_t temp;                              
	SysTick->LOAD = time_us*fac_us;             //时间加载
	SysTick->VAL = 0x00;                        //清空计数器  
	SysTick->CTRL = 0x01;                       //开始倒数
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));       //等待时间到达
	SysTick->CTRL = 0x00;                       //关闭计数器
	SysTick->VAL = 0x00;                        //清空计数器
}



/* 微秒延时函数 time_us要延时毫秒时间数 */
void Delay_Ms(uint32 time_ms)
{
	uint32_t temp;                              
	SysTick->LOAD = time_ms*fac_ms;             //时间加载
	SysTick->VAL = 0x00;                        //清空计数器  
	SysTick->CTRL = 0x01;                       //开始倒数
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));       //等待时间到达
	SysTick->CTRL = 0x00;                       //关闭计数器
	SysTick->VAL = 0x00;                        //清空计数器
}

/* 秒延时函数 */
void Delay_S(uint32 time_s)
{
	for(;time_s!=0;time_s--)
		Delay_Ms(1000);
}


/* 把十进制数转化为字符型 c要转化十进制数据 返回一个字符型数据 */
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


/* 用新的字符替代字符串中特殊的字符 str:要替换的字符串，oldchar特殊的字符 newchar新的字符 */
void replacetochar(char *str,char oldchar ,char newchar)
{
	int x;
	for(x=0;str[x];x++)
		if(str[x] == oldchar) str[x] = newchar;
}


/* 字符串转化为16位无符号整型函数 str要转化的字符串 base转化的基数 返回转化后的整型数 */
uint16 atoi16(char *str,uint16 base)
{
	uint32 num = 0;
	while(*str != 0)
		num = num*base + c2d(*str++);
	
	return num;
}


/* 字符串转化为32位无符号整型函数 str要转化的字符串 base转化的基数 返回转化后的整型数 */
uint32 atoi32(char *str,uint16 base)
{
	uint32 num = 0;
	while(*str != 0)
		num = num*base + c2d(*str++);
	
	return num;
}



/* 整型数转化为字符串函数 n:要转化整数 str[5]:存放转化后的字符串 len整型数长度 */
void itos(uint16 n,uint8 str[5],uint8 len)
{
	uint8 i = len -1;
	
	//将0x20空格赋值给str内的len个元素
	memset(str,0x20,len);
	
	do
	{
		str[i--] = n%10+'0';
	}while((n/=10)>0);
	return;
}



/* 把字符串转化为十进制或十六进制函数 str:要转换的字符串 base转化的基数 ret保存转化后的整数值 返回1成功 返回0失败 */
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


/* 16位字符高8位低8位转换 i要转换的数据 返回转换后的值 */
uint16 swaps(uint16 i) 
{
	uint16 ret = 0;
	
	ret = (i& 0xFF) <<8;
	ret |= ((i>>8) & 0xFF);
	return ret;
}

/* 32位字符高低位变化，i要转换的数据 返回转换后的值 */
uint32 swap32(uint32 i)
{
	uint32_t ret = 0;
	
	ret = (i&0xFF)<<24;
	ret |= ((i>>8)&0xFF)<<16;
	ret |= ((i>>16)&0xFF)<<8;
	ret |= ((i>>24)&0xFF);
	
	return ret;
}


/* 提取字符串中间的几个字符 src目标字符串 s1 s2是操作字符 sub是提取后的字符串 */
void mid(int8 *src,int8 *s1,int8 *s2,int8 *sub)
{
	int8 *sub1;
	int8 *sub2;
	uint16 n;
	
	//获取s1字符串出现在src字符串中的位置
	sub1 = strstr(src,s1);
	//再加上s1字符串的长度指向s1后面的字符串
	sub1 += strlen(s1);
	sub2 = strstr(src,s2);
	n = sub2 - sub1;
	
	//将找到的子字符串共n个复制到sub
	strncpy(sub,sub1,n);
	//添加字符串结束符
	sub[n] = 0;
}


/* ip网络地址转换 addr：地址 ip:ip */
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
		//分解字符串为一组字符串 ,nexttok位要分解的字符串 .为分隔符字符
		nexttok = strtok(nexttok,".");
		
		if(nexttok[0] == '0' && nexttok[1] == 'x')
			num = atoi16(nexttok+2,0x10);
		else
			num = atoi16(nexttok,10);
		
		ip[i] = num;
		nexttok = NULL;
	}
}


/* 将32位地址转化为十进制格式 addr：要转化的地址 返回十进制地址 */
char *internet_ntoa(unsigned long addr)
{
	static char addr_str[32];
	
	//将addr_str当前位置后面的32个字节替换为0并返回
	memset(addr_str,0,32);
	sprintf(addr_str,"%d.%d.%d.%d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	
	return addr_str;
}


/* 将16位地址转化为十进制格式 addr：要转化的地址 返回十进制地址 */
char *internet_ntoa_pad(unsigned long addr)
{
	static char addr_str[16];
	
	//将addr_str当前位置后面的16个字节替换为0并返回
	memset(addr_str,0,16);
	sprintf(addr_str,"%03d.%03d.%03d.%03d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	
	return addr_str;
}


/* 验证IP地址 addr：地址 ip:ip 返回1成功 0失败*/
char verify_ip_address(char* addr,uint8 *ip)
{
	int i;
	char taddr[50];
	char *tok = taddr;
	int tnum;
	
	strcpy(taddr,addr);
	
	
	for(i = 0;i<4;i++)
	{
		//分解字符串为一组字符串 ,nexttok位要分解的字符串 .为分隔符字符
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


/* 将一个主机模式的unsigned short型数据转换到大端模式的TCP/IP 网络字节格式的数据 输入要转换的数据 返回大端模式的数据 */
/**< A 16-bit number in host byte order.  */
uint16 htons(uint16 hostshort)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return swaps(hostshort);
	#else
		return hostshort;
	#endif
}


/* 将一个主机模式的unsigned long型数据转换到大端模式的TCP/IP 网络字节格式的数据 输入要转换的数据 返回大端模式的数据 */
/* 大端模式的数据是指将数据的高字节保存在内存的低地址中，低字节保存在高地址中 */
/**< hostshort  - A 32-bit number in host byte order.  */
unsigned long htonl(unsigned long hostlong)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return swap32(hostlong);
	#else
		return hostlong;
	#endif
}


/* 将一个大端模式的TCP/IP 网络字节格式的数据转换到主机模式的unsigned short型数据 输入要转换的数据 返回unsigned short模式的数据 */
/**< netshort - network odering 16bit value */
unsigned long ntohs(unsigned short netshort)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return htons(netshort);
	#else
		return netshort;
	#endif
}



/* 将一个大端模式的TCP/IP 网络字节格式的数据转换到主机模式的unsigned long型数据 输入要转换的数据 返回unsigned long模式的数据 */
/**< netshort - network odering 16bit value */
unsigned long ntohl(unsigned long netlong)
{
	#if(SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
		return htonl(netlong);
	#else
		return netlong;
	#endif
}


/* 计算字符串校验和 src:要检验的数据流 len:数据流长度  */
unsigned short checksun(unsigned char *src,unsigned int len)
{
	u_int sum,tsum,i,j;
	u_long lsum;
	
	//存储数据流中16位数据单元的数据量
	j = len>>1;
	
	lsum = 0;
	
	for(i = 0;i<j;i++)
	{
		//获取当前16位数据单元的高8位
		tsum = src[i*2];
		//将高8位左移8位
		tsum = tsum << 8;
		//加入当前16位数据单元的低8位
		tsum += src[i*2 +1];
		lsum += tsum;
	}
	
	if(len % 2)//如果数据流长度是奇数，则还有一个字节未处理
	{
		//获取这个字节
		tsum = src[i*2];
		//将这个字节作为16位数的高8位，加入lsum
		lsum += (tsum << 8);
	}
	
	sum = lsum;
	sum =~(sum + (lsum >>16));
	return (u_short)sum;
}


/* 检测地址是否批评 destip要转换的数据 返回1成功 0失败 */
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



	



