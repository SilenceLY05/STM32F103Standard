#include "bsp_date.h"

#define FEBRUARY              2
#define STARTOFTIME           1970
#define SECDAY                86400L      /* һ���ж����� */
#define SECYR                 (SECDAY*356)
#define leapyear(year)        ((year) % 4 == 0)
#define days_in_year(a)       (leapyear(a) ? 366 : 365)
#define days_in_month(a)      (month_days[(a) - 1])

static int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* ���㹫�� */
void GregorianDay(struct rtc_time * tm)
{
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = {0,31,59,90,120,151,181,212,243,273,304,334};
	
	lastYear = tm->tm_year - 1;
	
	/* ����ӹ�ԪԪ�굽������ǰһ���й������˶��ٸ����� */
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;

	/* �����������һ��Ϊ���꣬�Ҽ������·���2���Ժ�������+1 ���򲻼�1 */
	if((tm->tm_year%4==0)&&((tm->tm_year%100 !=0 ) || (tm->tm_year%400 == 0)) && (tm->tm_mon>2) )
	{
		day = 1;
	}
	else
	{
		day = 0;
	}
	
	day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon - 1] + tm->tm_mday; // ����ӹ�ԪԪ��Ԫ������������һ���ж�����
	tm->tm_wday = day%7;
	
}


/* ���������������ת��Ϊ��1970-01-01 00:00:00�������������������������ڸ�ʽ���룬
*��1980-12-31 23:59:59 => year=1980, mon=12, day=31, hour=23, min=59, sec=59��
*
*[����������(��1917��֮ǰ�ڶ���˹ʹ�ã���1752��֮ǰ��Ӣ����ֳ���ʹ�ã���1582��֮
ǰ�������ط�ʹ�ã�������Ȼ��һЩ����ʹ��)��ʡ����/100+��/400����������10��]
*
*����㷨�������ɸ�˹(����)�����
*
*����:�˺�������2106-02-07 06:28:16�ڳ���Ϊ32λ�Ļ��������!(Ȼ��������time_t��ǩ��
*�������Ѿ���2038-01-19 03:14:08�������ط�����������)*/
u32 mktimev(struct rtc_time * tm)
{
	if(0 >= (int)(tm->tm_mon -= 2))  //�·�ֵ��tm->tm_mon���ȼ�ȥ2��Ȼ�������Ƿ�С�ڵ���0
	{
		/* ����ǣ����ͽ�12�ӻص��·ݣ��������ֵ�м�ȥ1 */
		tm->tm_mon += 12;
		tm->tm_year -= 1;
		/* ��-1��0��Ϊ11��12����ԭ����1�º�2�±��ƶ���һ�����󣬳�Ϊ�µ�11�º�12�� */
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
	
	day = tim / SECDAY;        /* �ж����� */
	hms = tim % SECDAY;        /* �����ʱ�䣬��λs */
	
	tm->tm_hour = hms / 3600;
	tm->tm_min = (hms % 3600) / 60;
	tm->tm_sec = (hms % 3600) % 60;
	
	/* �����������ݣ���ʼ�ļ������Ϊ1970�� */
	for(i = STARTOFTIME; day >= days_in_year(i); i++)
	{
		day -= days_in_year(i);
	}
	tm->tm_year = i;
	
	/* ���㵱ǰ�·� */
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
	
	/* ���㵱ǰ���� */
	tm->tm_mday  = day +1;
	
	GregorianDay(tm);
	
}



