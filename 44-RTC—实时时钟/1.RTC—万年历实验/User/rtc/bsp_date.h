#ifndef __BSP_DATE_H
#define __BSP_DATE_H

#include "stm32f10x.h"

struct rtc_time{
	int tm_sec;     //ʱ��������Ա
	int tm_min;     //ʱ����ķֳ�Ա
	int tm_hour;    //ʱ�����ʱ��Ա
	int tm_mday;    //ʱ������ճ�Ա
	int tm_mon;     //ʱ������³�Ա
	int tm_year;    //ʱ��������Ա
	int tm_wday;    //ʱ��������ڳ�Ա
};


void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);

#endif /* __BSP_DATE_H */

