/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: sentence.h 17 2008-03-11 11:56:11Z xtimor $
 *
 */

/*! \file */

#ifndef __NMEA_SENTENCE_H__
#define __NMEA_SENTENCE_H__

#include "info.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* 由库解析和生成的NMEA包类型 */
enum nmeaPACKTYPE
{
    GPNON   = 0x0000,   /**< 未知包类型. */
    GPGGA   = 0x0001,   /**< GGA - 基本固定数据，提供3D位置和精度数据. */
    GPGSA   = 0x0002,   /**< GSA - GPS接收机工作模式、导航使用的sv、DOP值。 */
    GPGSV   = 0x0004,   /**< GSV - 视野中的sv数，PRN数，仰角，方位角和信噪比值. */
    GPRMC   = 0x0008,   /**< RMC - 推荐的最小特定GPS/TRANSIT数据. */
    GPVTG   = 0x0010,   /**< VTG - 对地速度和航向. */
	
		GPTXT   = 0x0020,   /**< TXT - 产品信息、天线状态、闰秒等 */

    BDGSV   = 0x0040,   /**< GSV - 可见卫星 */
    BDGSA   = 0x0080,   /**< GGA - 精度因子和有效卫星 */

    GNZDA   = 0x0100,   /**< ZDA - 时间和日期 */
    GNGLL   = 0x0200,   /**< GLL - 地理位置 经纬度 */
    GNVTG   = 0x0400,    /**< VTG - 对地速度和航向 */
    GNRMC   = 0x0800,   /**< RMC - 推荐的最少专用航向数据 */
    GNGGA   = 0x1000,   /**< GGA - 接收机定位数据，详细*/
};

/* GGA分组信息结构(全球定位系统固定数据) */
typedef struct _nmeaGPGGA
{
    nmeaTIME utc;       /* 当前定位的UTC时间 */
	double  lat;        /**< 纬度 - [degree][min].[sec/60] */
    char    ns;         /**< 维度方向[N]orth or [S]outh */
	double  lon;        /**< 经度 - [degree][min].[sec/60] */
    char    ew;         /**< 经度方向[E]ast or [W]est */
    int     sig;        /* 指示当前定位质量 0=定位不可用或者无效 1=SPS定位模式定位有效 4=估算模式（航位推算） */
	int     satinuse;   /* 用于定位的卫星数目，00-24 */
    double  HDOP;       /**< 水平经度因子n */
    double  elv;        /**< 海拔高度，即接收机天线相对于大地水准面的高度 */
    char    elv_units;  /**< 高度单位，米 */
    double  diff;       /**< 参考椭圆面与大地水准面之间的距离‘-’表示大地水准面低于参考椭圆面 */
    char    diff_units; /**< 高度单位，米 */
    double  dgps_age;   /**< 高分修正的数据龄期，未使用DGPS时该域为空 */
    int     dgps_sid;   /**< 差分参考站的ID */

} nmeaGPGGA;

/* GSA分组信息结构(卫星状态) */
typedef struct _nmeaGPGSA
{
    char    fix_mode;   /**< 模式切换方式，M手动切换，强制为2D或者3D工作模式，A自动切换模式 */
    int     fix_type;   /**< 定位状态标志 (1 = 定位无效; 2 = 2D; 3 = 3D) */
    int     sat_prn[NMEA_MAXSAT]; /**< 用于定位的卫星编号，最多显示12颗，不足的空白补齐 */
    double  PDOP;       /**< 位置经度因子 */
    double  HDOP;       /**< 水平经度因子 */
    double  VDOP;       /**< 垂直经度因子 */

} nmeaGPGSA;

/* GSV分组信息结构(可视卫星) */
typedef struct _nmeaGPGSV
{
    int     pack_count; /**< 语句总数 */
    int     pack_index; /**< 当前语句编号，每条GSV语句最多输出4颗可见卫星信息 */
    int     sat_count;  /**< 可见卫星总数 */
    nmeaSATELLITE sat_data[NMEA_SATINPACK];       /* 依次为卫星编号，仰角，方位角，载噪比 */

} nmeaGPGSV;

/* RMC包信息结构(建议最小句C) */
typedef struct _nmeaGPRMC
{
    nmeaTIME utc;       /**< 当前定位的UTC时间 */
    char    status;     /**< 位置有效标志 (A = 数据有效 V = 接收机警告，数据无效) */
	double  lat;        /**< 纬度 - [degree][min].[sec/60] */
    char    ns;         /**< 纬度方向[N]orth or [S]outh */
	double  lon;        /**< 经度 - [degree][min].[sec/60] */
    char    ew;         /**< 经度方向[E]ast or [W]est */
    double  speed;      /**< 対地速度 */
    double  direction;  /**< 对地真航向 */
    double  declination; /**< 磁偏角单位为度，固定为空 */
    char    declin_ew;  /**< 磁偏角方向[E]ast or [W]est */
    char    mode;       /**< 定位模式标志 (A = 自动模式, D = 差分模式, E = 估算模, N = 数据无效, S = 模拟器) */

} nmeaGPRMC;

/**
 * VTG分组信息结构 (对地速度和对地航向信息)
 */
typedef struct _nmeaGPVTG
{
    double  dir;        /**< 对地真北航向，单位为度 */
    char    dir_t;      /**< 真北指示，固定为‘T’ */
    double  dec;        /**< 对地磁北航向，单位为度 */
    char    dec_m;      /**< 磁北指示，固定为‘M’ */
    double  spn;        /**< 对地速度，单位为节 */
    char    spn_n;      /**< 速度单位节，固定为‘N’ */
    double  spk;        /**< 对地速度，单位为千米每小时 */
    char    spk_k;      /**< 速度单位，千米每小时，固定为‘K’ */

} nmeaGPVTG;


/* GNGGA分组信息结构(全球定位系统固定数据) */
typedef struct _nmeaGNGGA
{
    nmeaTIME utc;       /**< hhmmss.sss， 当前定位的UTC时间 */
    double  Lat;        /**< ddmm.mmmm， 纬度 前 2 字符表示度，后面的字符表示分  */
    char    uLat;       /**< 字符， 纬度方向： N-北， S-南 */
    double  Lon;        /**< dddmm.mmmm， 经度，前 3 字符表示度，后面的字符表示分 */
    char    uLon;       /**< 字符， 经度方向： E-东， W-西 */
    int     FS;         /**< 数值， 指示当前定位质量，该字段不应为空 (0 = 定位不可用或无效; 1 = SPS 定位模式，定位有效; 6 = 估算模式（航位推算）) */
    int     numSv;      /**< 数值， 用于定位的卫星数目， 00~24 */
    double  HDOP;       /**< 数值， 水平精度因子（ HDOP） */
    double  Msl;        /**< 数值，海拔高度，即接收机天线相对于大地水准面的高度 */
    char    uMsl;       /**< 字符，高度单位，米， 固定字符 M */
    double  Sep;        /**< 数值，参考椭球面与大地水准面之间的距离，"-"表示大地水准面低于参考椭球面 */
    char    uSep;        /**< 字符，高度单位，米， 固定字符 M */
    double  DiffAge;    /**< 数值，差分修正的数据龄期，未使用 DGPS 时该域为空 */
    int     DiffSta;    /**< 数值，差分参考站的 ID */

} nmeaGNGGA;


/**
 * GNRMC packet information structure (Recommended Minimum sentence C)
 */
typedef struct _nmeaGNRMC
{
    nmeaTIME utc;        /**< hhmmss.sss， 当前定位的UTC时间 */
    char    status;     /**< 字符，位置有效标志(A = 数据有效 或 V = 接收机警告，数据无效) */
    double  Lat;        /**< ddmm.mmmm， 纬度 前 2 字符表示度，后面的字符表示分  */
    char    uLat;       /**< 字符， 纬度方向： N-北， S-南 */
    double  Lon;        /**< dddmm.mmmm， 经度，前 3 字符表示度，后面的字符表示分 */
    char    uLon;       /**< 字符， 经度方向： E-东， W-西 */
    double  Spd;        /**< 数值 对地速度，单位为节 */
    double  Cog;        /**< 数值 对地真航向，单位为度 */
    double  mv;         /**< 数值 磁偏角，单位为度。 固定为空*/
    char    mvE;        /**< 字符 磁偏角方向： E-东， W-西。 固定为空 */
    char    mode;       /**< 字符 定位模式标志 (A = 自主模式, D = 差分模式, E = 估算模式, N = 数据无效) */

} nmeaGNRMC;

/**
 * VTG packet information structure (Track made good and ground speed)
 */
typedef struct _nmeaGNVTG
{
    double  Cogt;       /**< 数值 对地真航向，单位为度 */
    char    T;          /**< 字符 真北指示， 固定为 T*/
    double  CogM;        /**< 数值 对地磁北航向，单位为度*/
    char    M;          /**< 字符 磁北指示， 固定为 M*/
    double  Sog;        /**< 数值 对地速度，单位为节 */
    char    N;          /**< 字符 速度单位节， 固定为 N */
    double  kph;        /**< 数值 对地速度，单位为千米每小时 */
    char    K;          /**< 字符 速度单位，千米每小时， 固定为 K*/
    char    mode;       /**< 字符 定位模式标志 (A = 自主模式, D = 差分模式, E = 估算模式, N = 数据无效) */
} nmeaGNVTG;


/**
 * ZDA分组信息结构 (时间与日期信息)
 */
typedef struct _nmeaGNZDA
{
    nmeaTIME utc;       /**< hhmmss.sss， 当前定位的UTC时间 */
} nmeaGNZDA;

/**
 * GNGLL packet information structure (Recommended Minimum sentence C)
 */
typedef struct _nmeaGNGLL
{
    double  Lat;        /**< ddmm.mmmm， 纬度 前 2 字符表示度，后面的字符表示分  */
    char    uLat;       /**< 字符， 纬度方向： N-北， S-南 */
    double  Lon;        /**< dddmm.mmmm， 经度，前 3 字符表示度，后面的字符表示分 */
    char    uLon;       /**< 字符， 经度方向： E-东， W-西 */
    nmeaTIME utc;       /**< hhmmss.sss， 当前定位的UTC时间 */
    char    Value;      /**< 字符 数据有效性 (A-数据有效，V数据无效) */
    char    mode;       /**< 字符 定位模式标志 (A = 自主模式, D = 差分模式, E = 估算模式, N = 数据无效) */

} nmeaGNGLL;

/**
 * GSV 可见卫星的卫星编号及其仰角、方位角、载噪比等信息。每条 GSV 语句中的{卫星编
号,仰角,方位角,载噪比}参数组的数量可变，最多为 4 组，最少为 0 组
 */
typedef struct _nmeaBDGSV
{
    int     NumMsg; /**< 字符 语句总数。每条 GSV 语句最多输出 4 颗可见卫星信息，因此，当该系统可见卫星多于 4 颗时，将需要多条 GSV 语句。 */
    int     MsgNo; /**< 数字 当前语句编号 */
    int     numSv;  /**< 数值 可见卫星总数*/
    /*{,SVID,ele,az,cn0} 
    卫星编号；
    仰角0-90 度
    方位角 0-359 度
    载噪比 0-99 dB-Hz如果没有跟踪到当前卫星，补空*/
    nmeaSATELLITE sat_data[NMEA_SATINPACK];
//    int signalld;   /** 数值 NMEA 所定义的 GNSS 信号 ID（ 0 代表全部信号）*/

} nmeaBDGSV;

/**
 * GSA packet information structure (Satellite status)
 */
typedef struct _nmeaBDGSA
{
    char    Smode;      /**< 字符 模式切换方式指示 (M = 手动切换,强制为2D或者3D工作模式  A = 自动切换,接收机自动切换 3D/2D模式) */
    int     FS;         /**< 数字, 定位状态标志 (1 = 定位无效; 2 = 2D; 3 = 3D) */
    int     SVID[NMEA_MAXSAT]; /**< 用于定位的卫星编号，该字段共显示 12 颗可用卫星编号，多于 12 颗时只输出前 12 颗，不足 12 颗时不足的区域补空 */
    double  PDOP;       /**< 数值 位置精度因子（ PDOP）*/
    double  HDOP;       /**< 数值 水平精度因子（ HDOP） */
    double  VDOP;       /**< 数值 垂直精度因子（ VDOP）*/
//    int     systemId;   /**NMEA 所定义的 GNSS 系统 ID 号（备注[3]）仅 NMEA 4.1 及以上版本有效*/

} nmeaBDGSA;

/**
 * TXT 产品信息
 */
typedef struct _nmeaGPTXT
{
    char    xx;         /**< 数值，当前消息的语句总数 01~99， 如果某个消息过长，需要分为多条信息显示。 */
    int     yy;         /**< 数值, 语句编号 01~99。*/
    int     zz;         /**< 数值,文本识别符(00=错误信息；01=警告信息；02=通知信息；07=用户信息。) */
} nmeaGPTXT;

void nmea_zero_GPGGA(nmeaGPGGA *pack);
void nmea_zero_GPGSV(nmeaGPGSV *pack);
void nmea_zero_GPRMC(nmeaGPRMC *pack);
void nmea_zero_GPVTG(nmeaGPVTG *pack);

void nmea_zero_GNGGA(nmeaGNGGA *pack);
void nmea_zero_GNRMC(nmeaGNRMC *pack);
void nmea_zero_GNVTG(nmeaGNVTG *pack);
void nmea_zero_GNZDA(nmeaGNZDA *pack);
void nmea_zero_GNGLL(nmeaGNGLL *pack);
void nmea_zero_GPGSA(nmeaGPGSA *pack);
void nmea_zero_BDGSA(nmeaBDGSA *pack);
void nmea_zero_GPTXT(nmeaGPTXT *pack);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_SENTENCE_H__ */
