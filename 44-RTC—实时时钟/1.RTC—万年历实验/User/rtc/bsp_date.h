#ifndef __BSP_DATE_H
#define __BSP_DATE_H

#include "stm32f10x.h"

struct rtc_time{
	int tm_sec;     //时间戳的秒成员
	int tm_min;     //时间戳的分成员
	int tm_hour;    //时间戳的时成员
	int tm_mday;    //时间戳的日成员
	int tm_mon;     //时间戳的月成员
	int tm_year;    //时间戳的年成员
	int tm_wday;    //时间戳的星期成员
};


void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);

#endif /* __BSP_DATE_H */

