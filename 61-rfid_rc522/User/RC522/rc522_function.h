#ifndef __RC533_FUNCTION_H
#define __RC533_FUNCTION_H

#include "stm32f10x.h"
#include "stm32f10x_it.h"

#define Dummy_Data          0x00


void SPI_RC522_SendByte(uint8_t byte);
uint8_t SPI_RC522_ReadByte(void);
uint8_t ReadRawRC(uint8_t ucAddress);
void WriteRawRC(uint8_t ucAddress , uint8_t ucValue);
void SetBitMask(uint8_t ucReg,uint8_t ucMask);
void ClearBitMask(uint8_t ucReg,uint8_t ucMask);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
void PcdReset(void);
void M500PcdConfigISOType(uint8_t ucType);
char PcdComMF522(uint8_t ucCommand,uint8_t *pInData,uint8_t ucInLenByte,uint8_t *pOutData,uint32_t *pOutLenBit);
char PcdRequest(uint8_t ucReq_code,uint8_t *pTagType);
char PcdAnticoll(uint8_t *pSnr);
void CalulateCRC(uint8_t *pIndata,uint8_t ucLen,uint8_t *pOutData);
char PcdSelect(uint8_t *pSnr);
char PcdAuthState(uint8_t ucAuth_mode,uint8_t ucAddr,uint8_t *pKey,uint8_t *pSnr);
char PcdWrite(uint8_t ucAddr,uint8_t *pData);
char PcdRead(uint8_t ucAddr,uint8_t *pData);
char IsDataBlock(uint8_t ucAddr);
char PcdWriteString(uint8_t ucAddr,uint8_t *pData);
char PcdReadString(uint8_t ucAddr,uint8_t *pData);
char PcdHalt(void);
char WriteAmount(uint8_t ucAddr,uint32_t pData);
char ReadAmount(uint8_t ucAddr,uint32_t *pData);
char ChangeKeyA(uint8_t ucAddr,uint8_t *pKeyA);
char WriteDataBlock(uint8_t ucAddr,uint8_t *pData,uint8_t Len);
char ReadDataBlock(uint8_t ucAddr,uint8_t *pData);


#endif /* __RC533_FUNCTION_H */
