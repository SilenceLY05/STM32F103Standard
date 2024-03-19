#ifndef ISO14443A_H
#define ISO14443A_H

//#include "define.h"
//#include "../sys_msp430/sys_def.h"
#include "sl2x23.h"

/**
 * DEFINES ISO14443A COMMAND
 * commands which are handled by the tag,Each tag command is written to the
 * reader IC and transfered via RF
 ****************************************************************
 */

#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_ANTICOLL3        0x97               //防冲撞
#define PICC_HLTA             0x50               //休眠

/*
 * FUNCTION DECLARATIONS
 ****************************************************************
 */

int8_t pcd_request(uint8_t req_code, uint8_t *ptagtype);
int8_t pcd_cascaded_anticoll(uint8_t select_code, uint8_t coll_position, uint8_t *psnr);
int8_t pcd_cascaded_select(uint8_t select_code, uint8_t *psnr,uint8_t *psak);
int8_t pcd_hlta(void);
int8_t pcd_rats_a(uint8_t CID, uint8_t *ats);

int8_t pcd_pps_rate(transceive_buffer *pi, uint8_t *ATS, uint8_t CID, uint8_t rate);
#endif


