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

/* �ɿ���������ɵ�NMEA������ */
enum nmeaPACKTYPE
{
    GPNON   = 0x0000,   /**< δ֪������. */
    GPGGA   = 0x0001,   /**< GGA - �����̶����ݣ��ṩ3Dλ�ú;�������. */
    GPGSA   = 0x0002,   /**< GSA - GPS���ջ�����ģʽ������ʹ�õ�sv��DOPֵ�� */
    GPGSV   = 0x0004,   /**< GSV - ��Ұ�е�sv����PRN�������ǣ���λ�Ǻ������ֵ. */
    GPRMC   = 0x0008,   /**< RMC - �Ƽ�����С�ض�GPS/TRANSIT����. */
    GPVTG   = 0x0010,   /**< VTG - �Ե��ٶȺͺ���. */
	
		GPTXT   = 0x0020,   /**< TXT - ��Ʒ��Ϣ������״̬������� */

    BDGSV   = 0x0040,   /**< GSV - �ɼ����� */
    BDGSA   = 0x0080,   /**< GGA - �������Ӻ���Ч���� */

    GNZDA   = 0x0100,   /**< ZDA - ʱ������� */
    GNGLL   = 0x0200,   /**< GLL - ����λ�� ��γ�� */
    GNVTG   = 0x0400,    /**< VTG - �Ե��ٶȺͺ��� */
    GNRMC   = 0x0800,   /**< RMC - �Ƽ�������ר�ú������� */
    GNGGA   = 0x1000,   /**< GGA - ���ջ���λ���ݣ���ϸ*/
};

/* GGA������Ϣ�ṹ(ȫ��λϵͳ�̶�����) */
typedef struct _nmeaGPGGA
{
    nmeaTIME utc;       /* ��ǰ��λ��UTCʱ�� */
	double  lat;        /**< γ�� - [degree][min].[sec/60] */
    char    ns;         /**< ά�ȷ���[N]orth or [S]outh */
	double  lon;        /**< ���� - [degree][min].[sec/60] */
    char    ew;         /**< ���ȷ���[E]ast or [W]est */
    int     sig;        /* ָʾ��ǰ��λ���� 0=��λ�����û�����Ч 1=SPS��λģʽ��λ��Ч 4=����ģʽ����λ���㣩 */
	int     satinuse;   /* ���ڶ�λ��������Ŀ��00-24 */
    double  HDOP;       /**< ˮƽ��������n */
    double  elv;        /**< ���θ߶ȣ������ջ���������ڴ��ˮ׼��ĸ߶� */
    char    elv_units;  /**< �߶ȵ�λ���� */
    double  diff;       /**< �ο���Բ������ˮ׼��֮��ľ��롮-����ʾ���ˮ׼����ڲο���Բ�� */
    char    diff_units; /**< �߶ȵ�λ���� */
    double  dgps_age;   /**< �߷��������������ڣ�δʹ��DGPSʱ����Ϊ�� */
    int     dgps_sid;   /**< ��ֲο�վ��ID */

} nmeaGPGGA;

/* GSA������Ϣ�ṹ(����״̬) */
typedef struct _nmeaGPGSA
{
    char    fix_mode;   /**< ģʽ�л���ʽ��M�ֶ��л���ǿ��Ϊ2D����3D����ģʽ��A�Զ��л�ģʽ */
    int     fix_type;   /**< ��λ״̬��־ (1 = ��λ��Ч; 2 = 2D; 3 = 3D) */
    int     sat_prn[NMEA_MAXSAT]; /**< ���ڶ�λ�����Ǳ�ţ������ʾ12�ţ�����Ŀհײ��� */
    double  PDOP;       /**< λ�þ������� */
    double  HDOP;       /**< ˮƽ�������� */
    double  VDOP;       /**< ��ֱ�������� */

} nmeaGPGSA;

/* GSV������Ϣ�ṹ(��������) */
typedef struct _nmeaGPGSV
{
    int     pack_count; /**< ������� */
    int     pack_index; /**< ��ǰ����ţ�ÿ��GSV���������4�ſɼ�������Ϣ */
    int     sat_count;  /**< �ɼ��������� */
    nmeaSATELLITE sat_data[NMEA_SATINPACK];       /* ����Ϊ���Ǳ�ţ����ǣ���λ�ǣ������ */

} nmeaGPGSV;

/* RMC����Ϣ�ṹ(������С��C) */
typedef struct _nmeaGPRMC
{
    nmeaTIME utc;       /**< ��ǰ��λ��UTCʱ�� */
    char    status;     /**< λ����Ч��־ (A = ������Ч V = ���ջ����棬������Ч) */
	double  lat;        /**< γ�� - [degree][min].[sec/60] */
    char    ns;         /**< γ�ȷ���[N]orth or [S]outh */
	double  lon;        /**< ���� - [degree][min].[sec/60] */
    char    ew;         /**< ���ȷ���[E]ast or [W]est */
    double  speed;      /**< �����ٶ� */
    double  direction;  /**< �Ե��溽�� */
    double  declination; /**< ��ƫ�ǵ�λΪ�ȣ��̶�Ϊ�� */
    char    declin_ew;  /**< ��ƫ�Ƿ���[E]ast or [W]est */
    char    mode;       /**< ��λģʽ��־ (A = �Զ�ģʽ, D = ���ģʽ, E = ����ģ, N = ������Ч, S = ģ����) */

} nmeaGPRMC;

/**
 * VTG������Ϣ�ṹ (�Ե��ٶȺͶԵغ�����Ϣ)
 */
typedef struct _nmeaGPVTG
{
    double  dir;        /**< �Ե��汱���򣬵�λΪ�� */
    char    dir_t;      /**< �汱ָʾ���̶�Ϊ��T�� */
    double  dec;        /**< �Եشű����򣬵�λΪ�� */
    char    dec_m;      /**< �ű�ָʾ���̶�Ϊ��M�� */
    double  spn;        /**< �Ե��ٶȣ���λΪ�� */
    char    spn_n;      /**< �ٶȵ�λ�ڣ��̶�Ϊ��N�� */
    double  spk;        /**< �Ե��ٶȣ���λΪǧ��ÿСʱ */
    char    spk_k;      /**< �ٶȵ�λ��ǧ��ÿСʱ���̶�Ϊ��K�� */

} nmeaGPVTG;


/* GNGGA������Ϣ�ṹ(ȫ��λϵͳ�̶�����) */
typedef struct _nmeaGNGGA
{
    nmeaTIME utc;       /**< hhmmss.sss�� ��ǰ��λ��UTCʱ�� */
    double  Lat;        /**< ddmm.mmmm�� γ�� ǰ 2 �ַ���ʾ�ȣ�������ַ���ʾ��  */
    char    uLat;       /**< �ַ��� γ�ȷ��� N-���� S-�� */
    double  Lon;        /**< dddmm.mmmm�� ���ȣ�ǰ 3 �ַ���ʾ�ȣ�������ַ���ʾ�� */
    char    uLon;       /**< �ַ��� ���ȷ��� E-���� W-�� */
    int     FS;         /**< ��ֵ�� ָʾ��ǰ��λ���������ֶβ�ӦΪ�� (0 = ��λ�����û���Ч; 1 = SPS ��λģʽ����λ��Ч; 6 = ����ģʽ����λ���㣩) */
    int     numSv;      /**< ��ֵ�� ���ڶ�λ��������Ŀ�� 00~24 */
    double  HDOP;       /**< ��ֵ�� ˮƽ�������ӣ� HDOP�� */
    double  Msl;        /**< ��ֵ�����θ߶ȣ������ջ���������ڴ��ˮ׼��ĸ߶� */
    char    uMsl;       /**< �ַ����߶ȵ�λ���ף� �̶��ַ� M */
    double  Sep;        /**< ��ֵ���ο�����������ˮ׼��֮��ľ��룬"-"��ʾ���ˮ׼����ڲο������� */
    char    uSep;        /**< �ַ����߶ȵ�λ���ף� �̶��ַ� M */
    double  DiffAge;    /**< ��ֵ������������������ڣ�δʹ�� DGPS ʱ����Ϊ�� */
    int     DiffSta;    /**< ��ֵ����ֲο�վ�� ID */

} nmeaGNGGA;


/**
 * GNRMC packet information structure (Recommended Minimum sentence C)
 */
typedef struct _nmeaGNRMC
{
    nmeaTIME utc;        /**< hhmmss.sss�� ��ǰ��λ��UTCʱ�� */
    char    status;     /**< �ַ���λ����Ч��־(A = ������Ч �� V = ���ջ����棬������Ч) */
    double  Lat;        /**< ddmm.mmmm�� γ�� ǰ 2 �ַ���ʾ�ȣ�������ַ���ʾ��  */
    char    uLat;       /**< �ַ��� γ�ȷ��� N-���� S-�� */
    double  Lon;        /**< dddmm.mmmm�� ���ȣ�ǰ 3 �ַ���ʾ�ȣ�������ַ���ʾ�� */
    char    uLon;       /**< �ַ��� ���ȷ��� E-���� W-�� */
    double  Spd;        /**< ��ֵ �Ե��ٶȣ���λΪ�� */
    double  Cog;        /**< ��ֵ �Ե��溽�򣬵�λΪ�� */
    double  mv;         /**< ��ֵ ��ƫ�ǣ���λΪ�ȡ� �̶�Ϊ��*/
    char    mvE;        /**< �ַ� ��ƫ�Ƿ��� E-���� W-���� �̶�Ϊ�� */
    char    mode;       /**< �ַ� ��λģʽ��־ (A = ����ģʽ, D = ���ģʽ, E = ����ģʽ, N = ������Ч) */

} nmeaGNRMC;

/**
 * VTG packet information structure (Track made good and ground speed)
 */
typedef struct _nmeaGNVTG
{
    double  Cogt;       /**< ��ֵ �Ե��溽�򣬵�λΪ�� */
    char    T;          /**< �ַ� �汱ָʾ�� �̶�Ϊ T*/
    double  CogM;        /**< ��ֵ �Եشű����򣬵�λΪ��*/
    char    M;          /**< �ַ� �ű�ָʾ�� �̶�Ϊ M*/
    double  Sog;        /**< ��ֵ �Ե��ٶȣ���λΪ�� */
    char    N;          /**< �ַ� �ٶȵ�λ�ڣ� �̶�Ϊ N */
    double  kph;        /**< ��ֵ �Ե��ٶȣ���λΪǧ��ÿСʱ */
    char    K;          /**< �ַ� �ٶȵ�λ��ǧ��ÿСʱ�� �̶�Ϊ K*/
    char    mode;       /**< �ַ� ��λģʽ��־ (A = ����ģʽ, D = ���ģʽ, E = ����ģʽ, N = ������Ч) */
} nmeaGNVTG;


/**
 * ZDA������Ϣ�ṹ (ʱ����������Ϣ)
 */
typedef struct _nmeaGNZDA
{
    nmeaTIME utc;       /**< hhmmss.sss�� ��ǰ��λ��UTCʱ�� */
} nmeaGNZDA;

/**
 * GNGLL packet information structure (Recommended Minimum sentence C)
 */
typedef struct _nmeaGNGLL
{
    double  Lat;        /**< ddmm.mmmm�� γ�� ǰ 2 �ַ���ʾ�ȣ�������ַ���ʾ��  */
    char    uLat;       /**< �ַ��� γ�ȷ��� N-���� S-�� */
    double  Lon;        /**< dddmm.mmmm�� ���ȣ�ǰ 3 �ַ���ʾ�ȣ�������ַ���ʾ�� */
    char    uLon;       /**< �ַ��� ���ȷ��� E-���� W-�� */
    nmeaTIME utc;       /**< hhmmss.sss�� ��ǰ��λ��UTCʱ�� */
    char    Value;      /**< �ַ� ������Ч�� (A-������Ч��V������Ч) */
    char    mode;       /**< �ַ� ��λģʽ��־ (A = ����ģʽ, D = ���ģʽ, E = ����ģʽ, N = ������Ч) */

} nmeaGNGLL;

/**
 * GSV �ɼ����ǵ����Ǳ�ż������ǡ���λ�ǡ�����ȵ���Ϣ��ÿ�� GSV ����е�{���Ǳ�
��,����,��λ��,�����}������������ɱ䣬���Ϊ 4 �飬����Ϊ 0 ��
 */
typedef struct _nmeaBDGSV
{
    int     NumMsg; /**< �ַ� ���������ÿ�� GSV ��������� 4 �ſɼ�������Ϣ����ˣ�����ϵͳ�ɼ����Ƕ��� 4 ��ʱ������Ҫ���� GSV ��䡣 */
    int     MsgNo; /**< ���� ��ǰ����� */
    int     numSv;  /**< ��ֵ �ɼ���������*/
    /*{,SVID,ele,az,cn0} 
    ���Ǳ�ţ�
    ����0-90 ��
    ��λ�� 0-359 ��
    ����� 0-99 dB-Hz���û�и��ٵ���ǰ���ǣ�����*/
    nmeaSATELLITE sat_data[NMEA_SATINPACK];
//    int signalld;   /** ��ֵ NMEA ������� GNSS �ź� ID�� 0 ����ȫ���źţ�*/

} nmeaBDGSV;

/**
 * GSA packet information structure (Satellite status)
 */
typedef struct _nmeaBDGSA
{
    char    Smode;      /**< �ַ� ģʽ�л���ʽָʾ (M = �ֶ��л�,ǿ��Ϊ2D����3D����ģʽ  A = �Զ��л�,���ջ��Զ��л� 3D/2Dģʽ) */
    int     FS;         /**< ����, ��λ״̬��־ (1 = ��λ��Ч; 2 = 2D; 3 = 3D) */
    int     SVID[NMEA_MAXSAT]; /**< ���ڶ�λ�����Ǳ�ţ����ֶι���ʾ 12 �ſ������Ǳ�ţ����� 12 ��ʱֻ���ǰ 12 �ţ����� 12 ��ʱ��������򲹿� */
    double  PDOP;       /**< ��ֵ λ�þ������ӣ� PDOP��*/
    double  HDOP;       /**< ��ֵ ˮƽ�������ӣ� HDOP�� */
    double  VDOP;       /**< ��ֵ ��ֱ�������ӣ� VDOP��*/
//    int     systemId;   /**NMEA ������� GNSS ϵͳ ID �ţ���ע[3]���� NMEA 4.1 �����ϰ汾��Ч*/

} nmeaBDGSA;

/**
 * TXT ��Ʒ��Ϣ
 */
typedef struct _nmeaGPTXT
{
    char    xx;         /**< ��ֵ����ǰ��Ϣ��������� 01~99�� ���ĳ����Ϣ��������Ҫ��Ϊ������Ϣ��ʾ�� */
    int     yy;         /**< ��ֵ, ����� 01~99��*/
    int     zz;         /**< ��ֵ,�ı�ʶ���(00=������Ϣ��01=������Ϣ��02=֪ͨ��Ϣ��07=�û���Ϣ��) */
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
