#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "types.h"



void SysTick_Init(uint8_t systick);
void Delay_Us(uint32 time_us);
void Delay_Ms(uint32 time_ms);
void Delay_S(uint32 time_s);
char c2d(uint8 c);
void replacetochar(char *str,char oldchar ,char newchar);
uint16 atoi16(char *str,uint16 base);
uint32 atoi32(char *str,uint16 base);
void itos(uint16 n,uint8 str[5],uint8 len);
int validatoi(char *str,int base,int*ret);
uint16 swaps(uint16 i);
uint32 swap32(uint32 i);
void mid(int8 *src,int8 *s1,int8 *s2,int8 *sub);
void internet_addr(unsigned char* addr,unsigned char *ip);
char *internet_ntoa(unsigned long addr);
char *internet_ntoa_pad(unsigned long addr);
char verify_ip_address(char* addr,uint8 *ip);
uint16 htons(uint16 hostshort);
unsigned long htonl(unsigned long hostlong);
unsigned long ntohs(unsigned short netshort);
unsigned long ntohl(unsigned long netlong);
unsigned short checksun(unsigned char *src,unsigned int len);
u_char check_dest_in_local(u_long destip);




#endif /* __UTILITY_H__ */

