#ifndef ISO14443_4_H
#define ISO14443_4_H


#include "stm32f10x.h"



typedef struct ST_PCDINFO
{
    int8_t     ucNad;
    int8_t     ucNadEn;/*the picc support NAD*/
    int8_t     ucCid;/*the identification channel*/
    int8_t     ucCidEn;/*the picc support CID*/
    int32_t      uiFsc;/*the maximum size of PICC frame*/
    int32_t      uiFwi;/*the maximum frame waiting time*/
    int32_t      uiSfgi;/*the frame guard time*/
    /*the PCD and PICC maintain last i block PCB*/
    int8_t     ucPcdPcb;
    int8_t     ucPiccPcb;
    /*rf data buffer*/
    int32_t      uiPcdTxRNum;
    int32_t      uiPcdTxRLastBits;
    int8_t   ucWTXM;

} ST_PCDINFO;


extern struct ST_PCDINFO      gtPcdModuleInfo;


enum EXC_STEP
{
    ORG_IBLOCK = 1,
    ORG_ACKBLOCK,
    ORG_NACKBLOCK,
    ORG_SBLOCK,
    ORG_TRARCV, /*transmitted and received*/
    RCV_IBLOCK,
    RCV_RBLOCK,
    RCV_SBLOCK,
    RCV_INVBLOCK,/*protocol error*/
    NON_EVENT /*end*/
};

/*the re-transmission maxium count*/
#define ISO14443_PROTOCOL_RETRANSMISSION_LIMITED  ( 2 )

/**
 * protocol PCB structure
 */
#define ISO14443_CL_PROTOCOL_CHAINED              ( 0x10 )
#define ISO14443_CL_PROTOCOL_CID                  ( 0x8 )
#define ISO14443_CL_PROTOCOL_NAD                  ( 0x4 )
#define ISO14443_CL_PROTOCOL_ISN                  ( 0x1 )


/**
 * implement the half duplex communication protocol with ISO14443-4
 *
 * parameters:
 *             ptPcdInfo  : PCD information structure pointer
 *             pucSrc     : the datas information will be transmitted by ptPcdInfo
 *             iTxNum     : the number of transmitted datas by ptPcdInfo
 *             pucDes     : the datas information will be transmitted by PICC
 *             piRxN      : the number of transmitted datas by PICC.
 * retval:
 *            0 - successfully
 *            others, error.
 */
int32_t ISO14443_4_HalfDuplexExchange(struct ST_PCDINFO *ptPcdInfo,
                                      uint8_t *pucSrc,
                                      uint16_t iTxN,
                                      uint8_t *pucDes,
                                      uint16_t *piRxN) ;

/**
 * send 'DESELECT' command to PICC
 *
 * param:
 *       ptPcdInfo  :  PCD information structure pointer
 *       ucSPcb     :  S-block PCB
 *                       0xC2 - DESELECT( no parameter )
 *                       0xF2 - WTX( one byte wtx parameter )
 *       ucParam    :  S-block parameters
 * reval:
 *       0 - successfully
 *       others, failure
 */
int32_t ISO14443_4_SRequest(struct ST_PCDINFO *ptPcdInfo,
                            uint8_t      ucSPcb,
                            uint8_t      ucParam);
void pcd_default_info(void);

int8_t iso14443_4_deselect(uint8_t CID);

#define ISO14443_4_ERR_PROTOCOL  ( -100 )

#ifdef __cplusplus
}
#endif

#endif


