/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: gmath.h 17 2008-03-11 11:56:11Z xtimor $
 *
 */

#ifndef __NMEA_GMATH_H__
#define __NMEA_GMATH_H__

#include "info.h"

#define NMEA_PI                     (3.141592653589793)             /**< π值 */
#define NMEA_PI180                  (NMEA_PI / 180)                 /**< π除以180度 */
#define NMEA_EARTHRADIUS_KM         (6378)                          /**< 地球的平均半径，单位是千米 */
#define NMEA_EARTHRADIUS_M          (NMEA_EARTHRADIUS_KM * 1000)    /**< 地球的平均半径，单位是米 */
#define NMEA_EARTH_SEMIMAJORAXIS_M  (6378137.0)                     /**< 根据WGS84，地球半长轴以m为单位 */
#define NMEA_EARTH_SEMIMAJORAXIS_KM (NMEA_EARTHMAJORAXIS_KM / 1000) /**< 根据WGS84，地球半长轴以KM为单位 */
#define NMEA_EARTH_FLATTENING       (1 / 298.257223563)             /**< 根据WGS 84，经纬度转换为平面坐标 */
#define NMEA_DOP_FACTOR             (5)                             /**< 将DOP转换成米的系数 */

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * degree VS radian
 */

double nmea_degree2radian(double val);
double nmea_radian2degree(double val);

/*
 * NDEG (NMEA degree)
 */

double nmea_ndeg2degree(double val);
double nmea_degree2ndeg(double val);

double nmea_ndeg2radian(double val);
double nmea_radian2ndeg(double val);

/*
 * DOP
 */

double nmea_calc_pdop(double hdop, double vdop);
double nmea_dop2meters(double dop);
double nmea_meters2dop(double meters);

/*
 * positions work
 */

void nmea_info2pos(const nmeaINFO *info, nmeaPOS *pos);
void nmea_pos2info(const nmeaPOS *pos, nmeaINFO *info);

double  nmea_distance(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos
        );

double  nmea_distance_ellipsoid(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos,
        double *from_azimuth,
        double *to_azimuth
        );

int     nmea_move_horz(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance
        );

int     nmea_move_horz_ellipsoid(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance,
        double *end_azimuth
        );

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_GMATH_H__ */
