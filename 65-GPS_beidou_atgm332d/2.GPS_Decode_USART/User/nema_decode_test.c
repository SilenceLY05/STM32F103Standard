#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_gps.h"
#include "nmea.h"
#include "ff.h"


#ifdef  __GPS_LOG_FILE       //��SD���ϵ�gpslog.txt�ļ����н��룻����Ҫ��sd���ϴ��gpslog.txt�ļ���

FATFS       fs;
FIL         log_file;
FRESULT     res;
UINT        br,bw;           //file R/W count

/* ����GPS�ļ���Ϣ */
void nmea_decode_test(void)
{
	double deg_lat;        //ת����[degree].[degree]��ʽ��γ��
	double deg_lon;        //ת����[degree].[degree]��ʽ�ľ���
	
	nmeaINFO info;         //GPS�����õ�����Ϣ
	nmeaPARSER parser;     //����ʱʹ�õ����ݽṹ
	
	nmeaTIME  beiJingTime; //����ʱ��
	
	char buff[2048];
	
	/* ע���̷� */
	res = f_mount(&fs,"0:",1);
	
	if(res != FR_OK)
	{
		printf("\r\n ����SD�������ļ�ϵͳʧ�ܡ�(%d)���뽫SD�����뿪���� \r\n",res);
		while(1);
	}
	
	/* �򿪼�¼��GPS��Ϣ���ļ� */
	res = f_open(&log_file,"0:gpslog.txt",FA_OPEN_EXISTING | FA_READ);
	
	if(!(res == FR_OK))
	{
		printf("\r\n ��gpslog.txt�ļ�ʧ�ܣ�����SD���ĸ�Ŀ¼�Ƿ�����gpslog.txt�ļ��� \r\n");
		return;
	}
	
	/* �����������������Ϣ�ĺ��� */
	nmea_property()->trace_func = &trace;
	nmea_property()->error_func = &error;
	nmea_property()->info_func = &gps_info;
	
	/* ��ʼ��GPS���ݽṹ */
	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);
	
	while(!f_eof(&log_file))
	{
		f_read(&log_file,&buff[0],100,&br);
		
		/* ����nmea��ʽ���� */
		nmea_parse(&parser,&buff[0],br,&info);
		
		/* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
		GMTconvert(&info.utc,&beiJingTime,8,1);
		
		/* �������õ�����Ϣ */
		printf("\r\n ʱ��%d-%02d-%02d,%d:%d:%d \r\n",beiJingTime.year,beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
		
		/* info.lat lon�еĸ�ʽΪ[degree][min].[sec/60] */
		deg_lat = nmea_ndeg2degree(info.lat);
		deg_lon = nmea_ndeg2degree(info.lon);
		
		printf("\r\n γ�ȣ�%f ,���ȣ�%f \r\n",deg_lat,deg_lon);
		printf("\r\n ���θ߶�: %f �� \r\n",info.elv);
		printf("\r\n �ٶ�: %f km/h \r\n",info.speed);
		printf("\r\n ����: %f �� \r\n", info.direction);
		
		printf("\r\n ����ʹ�õ�GPS����: %d,�ɼ�GPS����:%d \r\n",info.satinfo.inuse,info.satinfo.inview);
		
		printf("\r\n ����ʹ�õı�������: %d ,�ɼ��������ǣ� %d \r\n",info.BDsatinfo.inuse,info.BDsatinfo.inview);
		
		printf("\r\n PDOP: %f , HDOP : %f , VDOP : %f \r\n",info.PDOP,info.HDOP,info.VDOP);
	}
	
	f_lseek(&log_file,f_size(&log_file));
	
	/* �ͷ�GPS���ݽṹ */
	nmea_parser_destroy(&parser);
	
	/* �ر��ļ� */
	f_close(&log_file);
}

#else

/* ����GPS�ļ���Ϣ */
int nmea_decode_test(void)
{
	double deg_lat;        //ת����[degree].[degree]��ʽ��γ��
	double deg_lon;        //ת����[degree].[degree]��ʽ�ľ���
	
	nmeaINFO info;         //GPS�����õ�����Ϣ
	nmeaPARSER parser;     //����ʱʹ�õ����ݽṹ
	uint8_t new_parse = 0; //�Ƿ����µĽ������ݱ�־
	
	nmeaTIME  beiJingTime; //����ʱ��
	
	
	/* �����������������Ϣ�ĺ��� */
	nmea_property()->trace_func = &trace;
	nmea_property()->error_func = &error;
	nmea_property()->info_func = &gps_info;
	
	/* ��ʼ��GPS���ݽṹ */
	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);
	
	while(1)
	{
		if(GPS_HalfTransferEnd)
		{
			/* ����nmea��ʽ���� */
			nmea_parse(&parser,(const char*)&gps_buff[0],HALF_GPS_RBUFF_SIZE,&info);
			
			GPS_HalfTransferEnd = 0;   //��ձ�־λ
			new_parse = 1;             //���ý�����Ϣ��־
		}
		else if(GPS_TransferEnd)
		{
			nmea_parse(&parser,(const char*)&gps_buff[HALF_GPS_RBUFF_SIZE],HALF_GPS_RBUFF_SIZE,&info);
			
			GPS_TransferEnd = 0;
			new_parse = 1;
		}
		
		if(new_parse)   //���µĽ�����Ϣ
		{
			/* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
			GMTconvert(&info.utc,&beiJingTime,8,1);
			
			/* �������õ�����Ϣ */
			printf("\r\n ʱ��%d-%02d-%02d,%d:%d:%d \r\n",beiJingTime.year,beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
			
			/* info.lat lon�еĸ�ʽΪ[degree][min].[sec/60] */
			deg_lat = nmea_ndeg2degree(info.lat);
			deg_lon = nmea_ndeg2degree(info.lon);
			
			printf("\r\n γ�ȣ�%f ,���ȣ�%f \r\n",deg_lat,deg_lon);
			printf("\r\n ���θ߶�: %f �� \r\n",info.elv);
			printf("\r\n �ٶ�: %f km/h \r\n",info.speed);
			printf("\r\n ����: %f �� \r\n", info.direction);
			
			printf("\r\n ����ʹ�õ�GPS����: %d,�ɼ�GPS����:%d \r\n",info.satinfo.inuse,info.satinfo.inview);
			
			printf("\r\n ����ʹ�õı�������: %d ,�ɼ��������ǣ� %d \r\n",info.BDsatinfo.inuse,info.BDsatinfo.inview);
			
			printf("\r\n PDOP: %f , HDOP : %f , VDOP : %f \r\n",info.PDOP,info.HDOP,info.VDOP);
			
			new_parse = 0;
		}
		
		
	}
	
}


#endif

