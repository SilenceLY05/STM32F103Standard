#include <string.h>
#include <stdio.h>
#include "ntp.h"
#include "bsp_w5500_conf.h"
#include "bsp_w5500.h"
#include "types.h"
#include "socket.h"


NTPformat ntpformat;
DataTime nowdata;
uint8 ntp_message[48];
tstamp total_seconds=0;
uint8 time_zone = 39;
uint8 ntp_server_ip[4] = {202,120,2,101};                  //NTP��������ַ
uint8 ntp_port = 123;                                      //NTP�������˿ں�
uint8 buffpub[2048];
uint8 ntp_debuge = 0;



/* ��������ʱ�� seconds UTC�����׼ʱ�� */
void calc_date_time(tstamp seconds)
{
	uint8 yf=0;                   //�������
	uint32 p_year_total_sec;      //ƽ���ж�����
	uint32 r_year_total_sec;      //�����ж�����
	tstamp n=0,d=0,total_d=0,rz=0; //UTCʱ�� ����ȥ������ܹ��ж����죬������Ϊֹ�ж����죬������˶�����
	uint16 y=0,yr=0;               //�����ǵڼ��꣬����
	signed long long yd=0;         //����ڼ���
	
	n = seconds;
	total_d = seconds / SECS_PERDAY;
	d= 0;
	p_year_total_sec = 365*SECS_PERDAY;
	r_year_total_sec = 366 * SECS_PERDAY;
	while(n>p_year_total_sec)
	{
		if((EPOCH+y)%400 == 0 || ((EPOCH+y)%100!=0 && (EPOCH+y)%4==0))
		{
			n = n - r_year_total_sec;
			d = d+ 366;
		}
		else
		{
			n = n - p_year_total_sec;
			d = d+ 365;
		}
		y+=1;
	}
	y+=EPOCH;
	
	nowdata.year[0] = (uint8)((y&0xFF00)>8);
	nowdata.year[1] = (uint8)(y&0xFF);
	
	yd = 0;
	yd = total_d - d;
	
	yf = 1;
	while(yd>28)
	{
		if(yf == 1 || yf == 3 || yf == 5 || yf == 7 || yf == 8 || yf == 10 || yf ==12)
		{
			yd -= 31;
			if(yd<0)
				break;
			rz += 31;
		}
		if(yf ==2)
		{
			if(y %400 == 0 || (y%100 != 0 && y%4 == 0))
			{
				yd -= 29;
				if(yd <0)
					break;
				rz+=29;
			}
			else
			{
				yd -= 28;
				if(yd < 0)
					break;
				rz+=28;
			}
			
		}
		
		if(yf == 4 || yf == 6 || yf == 9 || yf == 11)
		{
			yd -= 30;
			if(yd <0)
				break;
			rz +=30;
		}
		
		yf+=1;
	}
	nowdata.month = yf;
	yr = total_d - rz-d;
	yr +=1;        //��0�ż�Ϊ1��
		
	nowdata.day = yr;
	
	//����ʱ��
	seconds = seconds % SECS_PERDAY;
	nowdata.hour = seconds % 3600;
	nowdata.minute = (seconds % 3600)/60;
	nowdata.second = (seconds %3600) % 60;
		
}


/* ��ʼ��NTP Client��Ϣ */
void ntp_client_init(void)
{
	uint8 flag;
	NTP_Timer_Init();
	ntpformat.dstaddr [0] = ntp_server_ip[0];
	ntpformat.dstaddr [1] = ntp_server_ip[1];
	ntpformat.dstaddr [2] = ntp_server_ip[2];
	ntpformat.dstaddr [3] = ntp_server_ip[3];
	
	ntpformat.leap = 0;
	ntpformat.version = 4;
	ntpformat.mode = 3;
	ntpformat.stratum = 0;
	ntpformat.poll = 0;
	ntpformat.precision = 0;
	ntpformat.rootdelay = 0;
	ntpformat.rootdisp = 0;
	ntpformat.refid = 0;
	ntpformat.reftime = 0;
	ntpformat.org = 0;
	ntpformat.rec = 0;
	ntpformat.xmt = 1;
	
	flag = (ntpformat.leap << 6)+(ntpformat.version<<3)+ntpformat.mode;
	memcpy(ntp_message,(void const*)(&flag),1);
	
}


/* �ı�����Ϊ�� */
tstamp change_datetime_to_seconds(void)
{
	tstamp seconds = 0;
	uint32 total_day = 0;
	uint16 i = 0,r_year_cnt = 0,l = 0;
	
	l = nowdata.year[0];
	l = (l<<8) + nowdata.year[1];
	
	for(i = EPOCH;i<l;i++)
	{
		if(i%400 ==0 || ((i %100 != 0)&& (i%4 ==0)))
		{
			r_year_cnt += 1;
		}
	}
	
	total_day = (l - EPOCH -r_year_cnt)*365 + r_year_cnt *366;
	
	for( i = 1; i<=nowdata.month;i++)
	{
		if( i == 5 || i == 7 || i == 10 || i == 12)
		{
			total_day += 30;
		}
		if( i == 3)
		{
			if( l %400 == 0 || ( i%100 != 0 && i%4 ==0))
			{
				total_day += 29;
			}
			else
			{
				total_day += 28; 
			}
		}
		if( i == 2 || i == 4 || i == 6 || i== 8 || i==9|| i==11 )
		{
			total_day +=31;
		}
	}
	seconds = (total_day+nowdata.day-1)*24*3600;  
	seconds += nowdata.second;
	seconds += nowdata.minute*60;
	seconds += nowdata.hour*3600;
	
	return seconds;
}



/* ��NTP��������ȡʱ�� buff����Ż��� idx���������ݴ����ʼλ�� */
void get_seconds_from_ntp_server(uint8 *buff,uint16 idx)
{
	tstamp seconds = 0;
	uint8 i =0;
	for( i = 0; i<4;i++)
	{
		seconds = (seconds <<8) | buff[idx+i];
	}
	
	switch(time_zone)
	{
		case 0:
			seconds -=  12*3600;
		break;
		case 1:
			seconds -=  11*3600;
		break;
		case 2:
			seconds -=  10*3600;
		break;
		case 3:
			seconds -=  (9*3600+30*60);
		break;
		case 4:
			seconds -=  9*3600;
		break;
		case 5:
		case 6:
			seconds -=  8*3600;
		break;
		case 7:
		case 8:
			seconds -=  7*3600;
		break;
		case 9:
		case 10:
		seconds -=  6*3600;
		break;
		case 11:
		case 12:
		case 13:
			seconds -= 5*3600;
		break;
		case 14:
			seconds -=  (4*3600+30*60);
		break;
		case 15:
		case 16:
			seconds -=  4*3600;
		break;
		case 17:
			seconds -=  (3*3600+30*60);
		break;
		case 18:
			seconds -=  3*3600;
		break;
		case 19:
			seconds -=  2*3600;
		break;
		case 20:
			seconds -=  1*3600;
		break;
		case 21:                           
		case 22:
		break;
		case 23:
		case 24:
		case 25:
			seconds +=  1*3600;
		break;
		case 26:
		case 27:
			seconds +=  2*3600;
		break;
		case 28:
		case 29:
			seconds +=  3*3600;
		break;
		case 30:
			seconds +=  (3*3600+30*60);
		break;
		case 31:
			seconds +=  4*3600;
		break;
		case 32:
			seconds +=  (4*3600+30*60);
		break;
		case 33:
			seconds +=  5*3600;
		break;
		case 34:
			seconds +=  (5*3600+30*60);
		break;
		case 35:
			seconds +=  (5*3600+45*60);
		break;
		case 36:
			seconds +=  6*3600;
		break;
		case 37:
			seconds +=  (6*3600+30*60);
		break;
		case 38:
			seconds +=  7*3600;
		break;
		case 39:
			seconds +=  8*3600;
		break;
		case 40:
			seconds +=  9*3600;
		break;
		case 41:
			seconds +=  (9*3600+30*60);
		break;
		case 42:
			seconds +=  10*3600;
		break;
		case 43:
			seconds +=  (10*3600+30*60);
		break;
		case 44:
			seconds +=  11*3600;
		break;
		case 45:
			seconds +=  (11*3600+30*60);
		break;
		case 46:
			seconds +=  12*3600;
		break;
		case 47:
			seconds +=  (12*3600+45*60);
		break;
		case 48:
			seconds +=  13*3600;
		break;
		case 49:
			seconds +=  14*3600;
		break;
		
	}
	total_seconds = seconds;
	calc_date_time(seconds);              //��UTCʱ���������
}


/* ִ��NTP Client������ */
uint8 ntp_try_times = 0;
void NTP_Client_Test(void)
{
	if(total_seconds > 0 )                  //�ѻ�ȡʱ������ִ��NTP����
		return;
	else
	{
		uint16 len;
		uint8 * data_buff = buffpub;
		uint32 destip=0;
		uint16 destport;
		uint16 startindex = 40;                 //�ظ�����ʱ�������׵�ַ
		switch(getSn_SR(SOCK_NTP))
		{
			case Sn_SR_SOCK_UDP:                  //UDPģʽ����
				if(total_seconds > 0)
					return;
				
				if(ntp_try_times <100)
				{
					if(ntptimer>0)                      //1������һ��
					{
						/* ��������NTP������ */
						SendTo(SOCK_NTP,ntp_message,sizeof(ntp_message),ntp_server_ip,ntp_port);
						ntp_try_times++;
						if(ntp_debuge)
							printf("ntp_try_times:%d\r\n",ntp_try_times);
						
						ntptimer =0;                      //�ڶ�ʱ��2�ж��м�1
					}
				
				}
				else
				{
					ntp_try_times = 0;
					if(ntp_debuge)
						printf("ntp retry failed!\r\n");
				}
				if((len = getSn_RX_FSR(SOCK_NTP))>0)
				{
					if(len > TX_RX_MAX_BUFF_SIZE)
						len = TX_RX_MAX_BUFF_SIZE;
					
					RecvFrom(SOCK_NTP,data_buff,len,(uint8*)&destip,&destport);        //����NTP�������ظ�����
					get_seconds_from_ntp_server(data_buff,startindex);                 //��NTP��������ȡʱ��
					printf("����ʱ�䣺%d-%02d-%02d %02d:%02d:%02d\r\n",  
					(nowdata.year[0]<<8)+nowdata.year[1],							
					nowdata.month,
					nowdata.day,
					nowdata.hour,
					nowdata.minute,
					nowdata.second);
					ntp_try_times=0;
				}
				
				break;
				
			case Sn_SR_SOCK_CLOSE:
				socket(SOCK_NTP,Sn_MR_UDP,ntp_port,0);
			break;
		}
	}
		
}



