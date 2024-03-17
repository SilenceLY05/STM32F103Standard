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


// GPS����ָʾ��(0 =��Ч;1 =�޸�;2 =΢�֣�3 =����)
#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

//����ģʽ�����ڵ���(1 = Fix������;2 = 2d;3 = 3d)
#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

//����Ŀɼ���������
#define NMEA_MAXSAT         (37)
//�������� �����������Ǳ�� ���� ��λ�� �����
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

//�����γ��
#define NMEA_DEF_LAT        (5001.2621)
//����ľ���
#define NMEA_DEF_LON        (3613.0595)

#ifdef  __cplusplus
extern "C" {
#endif

/* �Է����Ȼ򻡶ȱ�ʾ��λ������ */
typedef struct _nmeaPOS
{
    double lat;         /**< γ�� */
    double lon;         /**< ���� */

} nmeaPOS;

/* ������Ϣ */
typedef struct _nmeaSATELLITE
{
    int     id;         /**< ����PRN */
    int     in_use;     /**< ����λ�ù̶� */
    int     elv;        /**< �������90�� */
    int     azimuth;    /**< ���ǣ���0��359 */
    int     sig;        /**< �ź�, 00-99 dB */

} nmeaSATELLITE;

/* ��Ұ���������ǵ����� */
typedef struct _nmeaSATINFO
{
    int     inuse;      /**< ����ʹ�õ�������Ŀ(��������������Ŀ) */
    int     inview;     /**< ���������������� */
    nmeaSATELLITE sat[NMEA_MAXSAT]; /**< ������Ϣ */

} nmeaSATINFO;

/* ���� GPS ����õ��Ľ�����洢������ṹ�� �ڵ���nmea_parse������ ֱ�Ӳ�ѯnmeaINFO�ṹ�����ݼ��ɵõ�����Ľ��*/
typedef struct _nmeaINFO
{
    int     smask;      /**< ���յ���GPS��Ϣ������ */

    nmeaTIME utc;       /**< �����ꡢ�¡��ա�ʱ���֡�����Ϣ����������ʱ�� */

    int     sig;        /**< GPS����ָʾ��(0 =��Ч;1 =�޸�;2 =΢�֣�3 =����) */
    int     fix;        /**< ����ģʽ�����ڵ���(1 = Fix������;2 = 2d;3 = 3d) */

    double  PDOP;       /**< λ�þ������� */
    double  HDOP;       /**< ˮƽ�������� */
    double  VDOP;       /**< ��ֱ�������� */

    double  lat;        /**< γ�� - +/-[degree][min].[sec/60] */
    double  lon;        /**< ���� - +/-[degree][min].[sec/60] */
    double  elv;        /**< ���ջ���������ڴ��ˮ׼��ĸ߶� */
    double  sog;        /**< ��ֵ �Ե��ٶȣ���λΪ�� */
    double  speed;      /**< �����ٶȵ�λΪǧ��ÿСʱ */
    double  direction;  /**< �Ե��溽�� */
    double  declination; /**< ��ƫ�ǵ�λΪ�ȣ��̶�Ϊ�� */
    char    mode;       /**< �ַ� ��λģʽ��־ (A = ����ģʽ, D = ���ģʽ, E = ����ģʽ, N = ������Ч) */
    nmeaSATINFO satinfo; /**< ������ GSP �ɼ����ǣ�����ʹ�õ����ǣ������źŵ���Ϣ */
    nmeaSATINFO BDsatinfo; /**�����˱����ɼ����ǣ�����ʹ�õ����ǣ������źŵ���Ϣ*/
		
		int txt_level;
		char *txt;

} nmeaINFO;

void nmea_zero_INFO(nmeaINFO *info);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_INFO_H__ */
