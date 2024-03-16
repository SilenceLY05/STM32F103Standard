#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_gps.h"
#include "nmea.h"
#include "ff.h"
#include "bsp_ili9341_lcd.h"




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
	
	char str_buff[100];
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	LCD_SetColors(RED,BLACK);
	
	ILI9341_DispStringLine_EN(LINE(1),"Wildfire STM32-F103");
	ILI9341_DispStringLine_EN(LINE(2),"GPS module");

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
		
		/* Һ����� */
        
    /* ����ǰ����ɫ��������ɫ��*/
		LCD_SetTextColor(BLUE);
		
		ILI9341_DispStringLine_EN(LINE(5),"GPS Info:");
		
		/* ����ǰ����ɫ��������ɫ��*/
		LCD_SetTextColor(WHITE);
		
		/* ��ʾʱ������ */
		sprintf(str_buff,"Data:%4d/%02d/%02d",beiJingTime.year+1900,beiJingTime.mon,beiJingTime.day);
		ILI9341_DispStringLine_EN(LINE(6),str_buff);
		
		sprintf(str_buff,"Data:%02d:%02d:%02d",beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
		ILI9341_DispStringLine_EN(LINE(7),str_buff);
		
		/* γ�� ����*/
		sprintf(str_buff,"latitude:%.6f",deg_lat);
		ILI9341_DispStringLine_EN(LINE(8),str_buff);
		
		sprintf(str_buff,"longitude:%.6f",deg_lon);
		ILI9341_DispStringLine_EN(LINE(9),str_buff);
		
		/* ����ʹ�õ����� �ɼ�������*/
		sprintf(str_buff,"GPS Satellite in use:%2d",info.satinfo.inuse);
		ILI9341_DispStringLine_EN(LINE(10),str_buff);
		
		sprintf(str_buff,"GPS Satellite in view:%2d",info.satinfo.inview);
		ILI9341_DispStringLine_EN(LINE(11),str_buff);
		
		/* ����ʹ�õ����� �ɼ�������*/
		sprintf(str_buff,"BD Satellite in use:%2d",info.BDsatinfo.inuse);
		ILI9341_DispStringLine_EN(LINE(12),str_buff);
		
		sprintf(str_buff,"BD Satellite in view:%2d",info.BDsatinfo.inview);
		ILI9341_DispStringLine_EN(LINE(13),str_buff);
		
		/* ���θ߶� */
		sprintf(str_buff,"Alititude:%4.2f m",info.elv);
		ILI9341_DispStringLine_EN(LINE(14),str_buff);
		
		/* �ٶ� */
		sprintf(str_buff,"Speed:%4.2f km/h",info.speed);
		ILI9341_DispStringLine_EN(LINE(15),str_buff);
		
		/* ���� */
		sprintf(str_buff,"Track angle:%3.2f deg",info.direction);
		ILI9341_DispStringLine_EN(LINE(16),str_buff);
		
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
	
	char str_buff[100];
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	LCD_SetColors(RED,BLACK);
	
	ILI9341_DispStringLine_EN(LINE(1),"Wildfire STM32-F103");
	ILI9341_DispStringLine_EN(LINE(2),"GPS module");
	
	
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
			
			/* Һ����� */
        
        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(BLUE);
        
        ILI9341_DispStringLine_EN(LINE(5)," GPS Info:");

        /* ����ǰ����ɫ��������ɫ��*/
        LCD_SetTextColor(WHITE);
        
        /* ��ʾʱ������ */
        sprintf(str_buff," Date:%4d/%02d/%02d ", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day);
        ILI9341_DispStringLine_EN(LINE(6),str_buff);
      
			  sprintf(str_buff," Time:%02d:%02d:%02d", beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        ILI9341_DispStringLine_EN(LINE(7),str_buff);

        /* γ�� ����*/
        sprintf(str_buff," latitude :%.6f ", deg_lat);
        ILI9341_DispStringLine_EN(LINE(8),str_buff);
        
        sprintf(str_buff," longitude :%.6f",deg_lon);
        ILI9341_DispStringLine_EN(LINE(9),str_buff);
        
        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," GPS  Satellite in use :%2d ", info.satinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(10),str_buff);    
        
       sprintf(str_buff," GPS Satellite in view :%2d", info.satinfo.inview);
        ILI9341_DispStringLine_EN(LINE(11),str_buff);    

        /* ����ʹ�õ����� �ɼ�������*/
        sprintf(str_buff," BDS  Satellite in use :%2d ", info.BDsatinfo.inuse);
        ILI9341_DispStringLine_EN(LINE(12),str_buff);    
        
       sprintf(str_buff," BDS Satellite in view :%2d", info.BDsatinfo.inview);
        ILI9341_DispStringLine_EN(LINE(13),str_buff);    
        
        /* ���θ߶� */
        sprintf(str_buff," Altitude:%4.2f m", info.elv);
        ILI9341_DispStringLine_EN(LINE(14),str_buff);
        
        /* �ٶ� */
        sprintf(str_buff," speed:%4.2f km/h", info.speed);
        ILI9341_DispStringLine_EN(LINE(15),str_buff);
        
        /* ���� */
        sprintf(str_buff," Track angle:%3.2f deg", info.direction);
        ILI9341_DispStringLine_EN(LINE(16),str_buff);
			
			new_parse = 0;
		}
		
		
	}
	
}


#endif

