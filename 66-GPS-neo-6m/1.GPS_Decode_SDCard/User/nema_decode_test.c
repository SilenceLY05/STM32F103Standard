#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_gps.h"
#include "nmea.h"
#include "ff.h"


#ifdef  __GPS_LOG_FILE       //对SD卡上的gpslog.txt文件进行解码；（需要在sd卡上存放gpslog.txt文件）

FATFS       fs;
FIL         log_file;
FRESULT     res;
UINT        br,bw;           //file R/W count

/* 解码GPS文件信息 */
void nmea_decode_test(void)
{
	double deg_lat;        //转换成[degree].[degree]格式的纬度
	double deg_lon;        //转换成[degree].[degree]格式的经度
	
	nmeaINFO info;         //GPS解码后得到的信息
	nmeaPARSER parser;     //解码时使用的数据结构
	
	nmeaTIME  beiJingTime; //北京时间
	
	char buff[2048];
	
	/* 注册盘符 */
	res = f_mount(&fs,"0:",1);
	
	if(res != FR_OK)
	{
		printf("\r\n ！！SD卡挂载文件系统失败。(%d)，请将SD卡插入开发板 \r\n",res);
		while(1);
	}
	
	/* 打开记录有GPS信息的文件 */
	res = f_open(&log_file,"0:gpslog.txt",FA_OPEN_EXISTING | FA_READ);
	
	if(!(res == FR_OK))
	{
		printf("\r\n 打开gpslog.txt文件失败，请检测SD卡的根目录是否存放了gpslog.txt文件！ \r\n");
		return;
	}
	
	/* 设置用于输出调试信息的函数 */
	nmea_property()->trace_func = &trace;
	nmea_property()->error_func = &error;
	nmea_property()->info_func = &gps_info;
	
	/* 初始化GPS数据结构 */
	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);
	
	while(!f_eof(&log_file))
	{
		f_read(&log_file,&buff[0],100,&br);
		
		/* 进行nmea格式解码 */
		nmea_parse(&parser,&buff[0],br,&info);
		
		/* 对解码后的时间进行转换，转换成北京时间 */
		GMTconvert(&info.utc,&beiJingTime,8,1);
		
		/* 输出解码得到的信息 */
		printf("\r\n 时间%d-%02d-%02d,%d:%d:%d \r\n",beiJingTime.year,beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
		
		/* info.lat lon中的格式为[degree][min].[sec/60] */
		deg_lat = nmea_ndeg2degree(info.lat);
		deg_lon = nmea_ndeg2degree(info.lon);
		
		printf("\r\n 纬度：%f ,经度：%f \r\n",deg_lat,deg_lon);
		printf("\r\n 海拔高度: %f 米 \r\n",info.elv);
		printf("\r\n 速度: %f km/h \r\n",info.speed);
		printf("\r\n 航向: %f 度 \r\n", info.direction);
		
		printf("\r\n 正在使用的GPS卫星: %d,可见GPS卫星:%d \r\n",info.satinfo.inuse,info.satinfo.inview);
		
		printf("\r\n 正在使用的北斗卫星: %d ,可见北斗卫星： %d \r\n",info.BDsatinfo.inuse,info.BDsatinfo.inview);
		
		printf("\r\n PDOP: %f , HDOP : %f , VDOP : %f \r\n",info.PDOP,info.HDOP,info.VDOP);
	}
	
	f_lseek(&log_file,f_size(&log_file));
	
	/* 释放GPS数据结构 */
	nmea_parser_destroy(&parser);
	
	/* 关闭文件 */
	f_close(&log_file);
}

#else

/* 解码GPS文件信息 */
int nmea_decode_test(void)
{
	double deg_lat;        //转换成[degree].[degree]格式的纬度
	double deg_lon;        //转换成[degree].[degree]格式的经度
	
	nmeaINFO info;         //GPS解码后得到的信息
	nmeaPARSER parser;     //解码时使用的数据结构
	uint8_t new_parse = 0; //是否有新的解码数据标志
	
	nmeaTIME  beiJingTime; //北京时间
	
	
	/* 设置用于输出调试信息的函数 */
	nmea_property()->trace_func = &trace;
	nmea_property()->error_func = &error;
	nmea_property()->info_func = &gps_info;
	
	/* 初始化GPS数据结构 */
	nmea_zero_INFO(&info);
	nmea_parser_init(&parser);
	
	while(1)
	{
		if(GPS_HalfTransferEnd)
		{
			/* 进行nmea格式解码 */
			nmea_parse(&parser,(const char*)&gps_buff[0],HALF_GPS_RBUFF_SIZE,&info);
			
			GPS_HalfTransferEnd = 0;   //清空标志位
			new_parse = 1;             //设置解码消息标志
		}
		else if(GPS_TransferEnd)
		{
			nmea_parse(&parser,(const char*)&gps_buff[HALF_GPS_RBUFF_SIZE],HALF_GPS_RBUFF_SIZE,&info);
			
			GPS_TransferEnd = 0;
			new_parse = 1;
		}
		
		if(new_parse)   //有新的解码消息
		{
			/* 对解码后的时间进行转换，转换成北京时间 */
			GMTconvert(&info.utc,&beiJingTime,8,1);
			
			/* 输出解码得到的信息 */
			printf("\r\n 时间%d-%02d-%02d,%d:%d:%d \r\n",beiJingTime.year,beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
			
			/* info.lat lon中的格式为[degree][min].[sec/60] */
			deg_lat = nmea_ndeg2degree(info.lat);
			deg_lon = nmea_ndeg2degree(info.lon);
			
			printf("\r\n 纬度：%f ,经度：%f \r\n",deg_lat,deg_lon);
			printf("\r\n 海拔高度: %f 米 \r\n",info.elv);
			printf("\r\n 速度: %f km/h \r\n",info.speed);
			printf("\r\n 航向: %f 度 \r\n", info.direction);
			
			printf("\r\n 正在使用的GPS卫星: %d,可见GPS卫星:%d \r\n",info.satinfo.inuse,info.satinfo.inview);
			
			printf("\r\n 正在使用的北斗卫星: %d ,可见北斗卫星： %d \r\n",info.BDsatinfo.inuse,info.BDsatinfo.inview);
			
			printf("\r\n PDOP: %f , HDOP : %f , VDOP : %f \r\n",info.PDOP,info.HDOP,info.VDOP);
			
			new_parse = 0;
		}
		
		
	}
	
}


#endif

