/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: info.h 10 2007-11-15 14:50:15Z xtimor $
 *
 */

/*! \file */

#ifndef __NMEA_INFO_H__
#define __NMEA_INFO_H__

#include "time.h"


// GPS质量指示器(0 =无效;1 =修复;2 =微分，3 =敏感)
#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

//操作模式，用于导航(1 = Fix不可用;2 = 2d;3 = 3d)
#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

//定义的可见卫星总数
#define NMEA_MAXSAT         (37)
//卫星数据 包含四种卫星编号 仰角 方位角 载噪比
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

//定义的纬度
#define NMEA_DEF_LAT        (5001.2621)
//定义的经度
#define NMEA_DEF_LON        (3613.0595)

#ifdef  __cplusplus
extern "C" {
#endif

/* 以分数度或弧度表示的位置数据 */
typedef struct _nmeaPOS
{
    double lat;         /**< 纬度 */
    double lon;         /**< 经度 */

} nmeaPOS;

/* 卫星信息 */
typedef struct _nmeaSATELLITE
{
    int     id;         /**< 卫星PRN */
    int     in_use;     /**< 用于位置固定 */
    int     elv;        /**< 仰角最高90度 */
    int     azimuth;    /**< 仰角，从0到359 */
    int     sig;        /**< 信号, 00-99 dB */

} nmeaSATELLITE;

/* 视野内所有卫星的资料 */
typedef struct _nmeaSATINFO
{
    int     inuse;      /**< 正在使用的卫星数目(不是所看到的数目) */
    int     inview;     /**< 所看到的卫星总数 */
    nmeaSATELLITE sat[NMEA_MAXSAT]; /**< 卫星信息 */

} nmeaSATINFO;

/* 所有 GPS 解码得到的结果都存储在这个结构中 在调用nmea_parse函数后 直接查询nmeaINFO结构的数据即可得到解码的结果*/
typedef struct _nmeaINFO
{
    int     smask;      /**< 接收到的GPS信息包类型 */

    nmeaTIME utc;       /**< 包含年、月、日、时、分、秒信息，格林威治时间 */

    int     sig;        /**< GPS质量指示器(0 =无效;1 =修复;2 =微分，3 =敏感) */
    int     fix;        /**< 操作模式，用于导航(1 = Fix不可用;2 = 2d;3 = 3d) */

    double  PDOP;       /**< 位置精度因子 */
    double  HDOP;       /**< 水平精度因子 */
    double  VDOP;       /**< 垂直精度因子 */

    double  lat;        /**< 纬度 - +/-[degree][min].[sec/60] */
    double  lon;        /**< 经度 - +/-[degree][min].[sec/60] */
    double  elv;        /**< 接收机天线相对于大地水准面的高度 */
    double  sog;        /**< 数值 对地速度，单位为节 */
    double  speed;      /**< 対地速度单位为千米每小时 */
    double  direction;  /**< 对地真航向 */
    double  declination; /**< 磁偏角单位为度，固定为空 */
    char    mode;       /**< 字符 定位模式标志 (A = 自主模式, D = 差分模式, E = 估算模式, N = 数据无效) */
    nmeaSATINFO satinfo; /**< 包含了 GSP 可见卫星，正在使用的卫星，卫星信号等信息 */
    nmeaSATINFO BDsatinfo; /**包含了北斗可见卫星，正在使用的卫星，卫星信号等信息*/
		
		int txt_level;
		char *txt;

} nmeaINFO;

void nmea_zero_INFO(nmeaINFO *info);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_INFO_H__ */
