#ifndef ISO_14443B_H
#define ISO_14443B_H

#include "stm32f10x.h"


//#include "../sys_msp430/sys_def.h"
/////////////////////////////////////////////////////////////////////
//ISO14443B COMMAND
/////////////////////////////////////////////////////////////////////
#define ISO14443B_ANTICOLLISION                  0x05
#define ISO14443B_ATTRIB                         0x1D
#define ISO14443B_HLTB                           0x50

typedef struct
{
    uint8_t start_code;  // 1d
    uint8_t flag[4];   // PUDI
    uint8_t param[3];   //00 08|58|a8|f8   01
    uint8_t cid;  // 04
} S_ATTRIB;



/////////////////////////////////////////////////////////////////////
//º¯ÊýÔ­ÐÍ
/////////////////////////////////////////////////////////////////////
int8_t pcd_request_b(uint8_t req_code, uint8_t AFI, uint8_t N, uint8_t *ATQB);
int8_t pcd_slot_marker(uint8_t N, uint8_t *ATQB);
int8_t pcd_attri_b(uint8_t *PUPI, uint8_t dsi_dri, uint8_t pro_type,uint8_t CID, uint8_t *answer);
int8_t get_idcard_num(uint8_t *ATQB);
int8_t pcd_halt_b(uint8_t *PUPI);
int8_t select_sr(uint8_t *chip_id);
int8_t read_sr176(uint8_t addr, uint8_t *readdata);
int8_t write_sr176(uint8_t addr, uint8_t *writedata);
int8_t get_prot_sr176(uint8_t lockreg);
int8_t protect_sr176(uint8_t lockreg);
int8_t completion_sr(void);
int8_t pcd_change_rate_b(uint8_t CID, uint8_t *ATQB);



#endif

