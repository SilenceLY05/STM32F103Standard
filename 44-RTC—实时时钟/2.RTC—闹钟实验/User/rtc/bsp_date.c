#include "bsp_date.h"

#define FEBRUARY              2
#define STARTOFTIME           1970
#define SECDAY                86400L      /* 一天有多少秒 */
#define SECYR                 (SECDAY*356)
#define leapyear(year)        ((year) % 4 == 0)
#define days_in_year(a)       (leapyear(a) ? 366 : 365)
#define days_in_month(a)      (month_days[(a) - 1])

static int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* 计算公历 */
void GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = {0,31,59,90,120,151,181,212,243,273,304,334};
	
	lastYear = tm->tm_year - 1;
	
	/* 计算从公元元年到记数的前一年中共经历了多少个闰年 */
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;

	/* 如果计数的这一年为闰年，且计数的月份在2月以后，则日数+1 否则不加1 */
	if((tm->tm_year%4==0)&&((tm->tm_year%100 !=0 ) || (tm->tm_year%400 == 0)) && (tm->tm_mon>2) )
	{
		day = 1;
	}
	else
	{
		day = 0;
	}
	
	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon - 1] + tm->tm_mday; // 计算从公元元年元旦到计数日期一共有多少天
	tm->tm_wday = day%7;
	
}


/* 将格里高利历日期转换为自1970-01-01 00:00:00以来的秒数。假设以正常日期格式输入，
*即1980-12-31 23:59:59 => year=1980, mon=12, day=31, hour=23, min=59, sec=59。
*
*[对于儒略历(在1917年之前在俄罗斯使用，在1752年之前在英国和殖民地使用，在1582年之
前在其他地方使用，并且仍然被一些社区使用)，省略年/100+年/400的条款，并添加10。]
*
*这个算法最早是由高斯(我想)发表的
*
*警告:此函数将在2106-02-07 06:28:16在长度为32位的机器上溢出!(然而，由于time_t被签名
*，我们已经在2038-01-19 03:14:08的其他地方遇到了问题)*/
u32 mktimev(struct rtc_time * tm)
{
	if(0 >= (int)(tm->tm_mon -= 2))  //月份值（tm->tm_mon）先减去2，然后检查结果是否小于等于0
	{
		/* 如果是，它就将12加回到月份，并从年份值中减去1 */
		tm->tm_mon += 12;
		tm->tm_year -= 1;
		/* 将-1和0变为11和12，即原来的1月和2月被移动到一年的最后，成为新的11月和12月 */
	}
	return (((
					(u32)(tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday)
					+ tm->tm_year*365 -719499
					)*24 + tm->tm_hour   /* now have hours */
					)*60 + tm->tm_min    /* now have minutes */
					)*60+tm->tm_sec;     /* finally seconds */
}

void to_tm(u32 tim, struct rtc_time * tm)
{
	register u32 i;
	register long   hms,day;
	
	day = tim / SECDAY;        /* 有多少天 */
	hms = tim % SECDAY;        /* 今天的时间，单位s */
	
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;
	
	/* 计算出现在年份，起始的计数年份为1970年 */
	for(i = STARTOFTIME; day >= days_in_year(i); i++)
	{
		day -= days_in_year(i);
	}
	tm->tm_year = i;
	
	/* 计算当前月份 */
	if(leapyear (tm->tm_year))
	{
		days_in_month(FEBRUARY) = 29;
	}
	for( i = 1; day >= days_in_month(i); i++)
	{
		day -= days_in_month(i);
	}
	days_in_month(FEBRUARY) = 28;
	tm->tm_mon = i;
	
	/* 计算当前日期 */
	tm->tm_mday  = day +1;
	
	GregorianDay(tm);
	
}



