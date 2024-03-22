#ifndef __NTP_H__
#define __NTP_H__

#include "types.h"


extern uint8 ntp_server_ip[4];
extern uint8 ntp_port;
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;


#pragma pack(1)
typedef struct _NPTformat{       //�˽ṹ�嶨����NTPʱ��ͬ�����ĵ���ر���
	uint8 dstaddr[4];      //
	char version;          //��ʾNTP�汾��
	char leap;             //�����ʶ�� ֵΪ11ʱ��ʾ���棬ʱ��δ��ͬ��
	char mode;             //ģʽ:0δ���壬1��ʾ�����Ե�ģʽ��2��ʾ�����Եȣ�3��ʾ�ͻ�ģʽ��4��ʾ������ģʽ��5��ʾ�㲥���鲥ģʽ��6��ʾNTP���Ʊ��ģ�7Ԥ���ڲ�ʹ��
	char stratum;          //ϵͳʱ�ӵĲ���1~16��׼ȷ�ȴ�1��16�ݼ�������16ʱ����δͬ��״̬
	char poll;             //��ѯʱ�䣬��������NTP����֮���ʱ����
	s_char precision;      //ϵͳʱ�ӵľ���
	tdist rootdelay;       //���ӳ٣����ص����ο�ʱ��Դ������ʱ��
	tdist rootdisp;        //��������ϵͳʱ����������ο�ʱ�ӵ�������
	char refid;            //�ο���ʶ�����ο�ʱ��Դ�ı�ʶ
	tstamp reftime;        //�ο�ʱ�����ϵͳʱ�����һ�α��趨����µ�ʱ��
	tstamp org;            //ԭʼʱ�����NTP�������뿪���Ͷ�ʱ���Ͷ˵ı���ʱ��
	tstamp rec;            //����ʱ�����NTP�����ĵ�����ն�ʱ���ն˵ı���ʱ��
	tstamp xmt;            //����ʱ�����Ӧ�����뿪Ӧ����ʱӦ���ߵı���ʱ��
	
}NTPformat;

#pragma pack()


#pragma pack(1)
typedef struct _DataTime{             //�ṹ�嶨����NTPʱ��ͬ������ر���
	uint16 year[2];    //��Ϊ2�ֽڣ��ߵ���λ
	uint8 month;       //��
	uint8 day;         //��
	uint8 hour;        //Сʱ
	uint8 minute;      //����
	uint8 second;      //��
	
}DataTime;
#pragma pack()


#define SECS_PERDAY           86400UL    //һ�������
#define UTC_ADJ_HRS           9          //GMT+9
#define EPOCH                 1900       //NTP start year
	

void calc_date_time(tstamp seconds);
void ntp_client_init(void);
tstamp change_datetime_to_seconds(void);
void get_seconds_from_ntp_server(uint8 *buff,uint16 idx);
void NTP_Client_Test(void);


#endif /* __NTP_H__ */

