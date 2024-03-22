#ifndef __NTP_H__
#define __NTP_H__

#include "types.h"


extern uint8 ntp_server_ip[4];
extern uint8 ntp_port;
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;


#pragma pack(1)
typedef struct _NPTformat{       //此结构体定义了NTP时钟同步报文的相关变量
	uint8 dstaddr[4];      //
	char version;          //表示NTP版本号
	char leap;             //润秒标识器 值为11时表示警告，时钟未被同步
	char mode;             //模式:0未定义，1表示主动对等模式，2表示被动对等，3表示客户模式，4表示服务器模式，5表示广播或组播模式，6表示NTP控制报文，7预留内部使用
	char stratum;          //系统时钟的层数1~16，准确度从1到16递减，层数16时处于未同步状态
	char poll;             //轮询时间，两个连续NTP报文之间的时间间隔
	s_char precision;      //系统时钟的精度
	tdist rootdelay;       //根延迟，本地到主参考时钟源的往返时间
	tdist rootdisp;        //根差量，系统时钟相对于主参考时钟的最大误差
	char refid;            //参考标识符，参考时钟源的标识
	tstamp reftime;        //参考时间戳，系统时钟最后一次被设定或更新的时间
	tstamp org;            //原始时间戳，NTP请求报文离开发送端时发送端的本地时间
	tstamp rec;            //接收时间戳，NTP请求报文到达接收端时接收端的本地时间
	tstamp xmt;            //传送时间戳，应答报文离开应答者时应答者的本地时间
	
}NTPformat;

#pragma pack()


#pragma pack(1)
typedef struct _DataTime{             //结构体定义了NTP时间同步的相关变量
	uint16 year[2];    //年为2字节，高低两位
	uint8 month;       //月
	uint8 day;         //日
	uint8 hour;        //小时
	uint8 minute;      //分钟
	uint8 second;      //秒
	
}DataTime;
#pragma pack()


#define SECS_PERDAY           86400UL    //一天多少秒
#define UTC_ADJ_HRS           9          //GMT+9
#define EPOCH                 1900       //NTP start year
	

void calc_date_time(tstamp seconds);
void ntp_client_init(void);
tstamp change_datetime_to_seconds(void);
void get_seconds_from_ntp_server(uint8 *buff,uint16 idx);
void NTP_Client_Test(void);


#endif /* __NTP_H__ */

