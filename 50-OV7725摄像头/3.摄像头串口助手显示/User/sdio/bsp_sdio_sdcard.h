/**
  ******************************************************************************
  * @file    stm32_eval_sdio_sd.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains all the functions prototypes for the SD Card 
  *          stm32_eval_sdio_sd driver firmware library.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SDIO_SDCARD_H
#define __BSP_SDIO_SDCARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Exported types ------------------------------------------------------------*/

typedef enum
{
/** 
  * @brief  SD_Error���о��˿��������ܳ��ֵĴ���
	*			����CRCУ�����CRCУ�����ͨ�ŵȴ���ʱ��FIFO��������硢�����������ȵ�  
  */   
  SD_CMD_CRC_FAIL                    = (1), /*!< �յ�������Ӧ(��CRC���ʧ��) */
  SD_DATA_CRC_FAIL                   = (2), /*!< ����/�������ݿ�(CRCУ��ʧ��)  */
  SD_CMD_RSP_TIMEOUT                 = (3), /*!< ͨ�ŵȴ���ʱ */
  SD_DATA_TIMEOUT                    = (4), /*!< ���ݳ�ʱ */
  SD_TX_UNDERRUN                     = (5), /*!< ����FIFO ���� */
  SD_RX_OVERRUN                      = (6), /*!< ���� FIFO ���� */
  SD_START_BIT_ERR                   = (7), /*!< ��widE����ģʽ�£����������źŶ�û�м�⵽��ʼλ */
  SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD�Ĳ��������˷�Χ*/
  SD_ADDR_MISALIGNED                 = (9), /*!< ��ַƫ�� */
  SD_BLOCK_LEN_ERR                   = (10), /*!< ����Ŀ鳤�Ȳ�����������ֽ�����ƥ��鳤�� */
  SD_ERASE_SEQ_ERR                   = (11), /*!< ��������˳�����.*/
  SD_BAD_ERASE_PARAM                 = (12), /*!< �������ѡ����Ч */
  SD_WRITE_PROT_VIOLATION            = (13), /*!< ���Ա�дд��������� */
  SD_LOCK_UNLOCK_FAILED              = (14), /*!< �ڽ��������м�⵽���л�������󣬻��������ͼ���������Ŀ� */
  SD_COM_CRC_FAILED                  = (15), /*!< ǰһ�������CRC���ʧ�� */
  SD_ILLEGAL_CMD                     = (16), /*!< ������ڿ�״̬���Ϸ� */
  SD_CARD_ECC_FAILED                 = (17), /*!< ���ڲ�ECC��Ӧ�ã���δ�ܾ������� */
  SD_CC_ERROR                        = (18), /*!< �ڲ������������� */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< һ���δ֪���� */
  SD_STREAM_READ_UNDERRUN            = (20), /*!< ������ȡ�����У����޷�֧�����ݴ���. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< ���޷�֧����ģʽ�µ����ݱ�� */
  SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD���Ǵ��� */
  SD_WP_ERASE_SKIP                   = (23), /*!< �����ֵ�ַ�ռ䱻���� */
  SD_CARD_ECC_DISABLED               = (24), /*!< ������ִ�У�δʹ���ڲ�ECC  */
  SD_ERASE_RESET                     = (25), /*!< ��ִ��֮ǰ����������У���Ϊ�յ������������� */
  SD_AKE_SEQ_ERROR                   = (26), /*!< ��֤˳�����. */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDIO_DISABLED                   = (30),
  SD_SDIO_FUNCTION_BUSY              = (31),
  SD_SDIO_FUNCTION_FAILED            = (32),
  SD_SDIO_UNKNOWN_FUNCTION           = (33),

/** 
  * @brief  Standard error defines   
  */ 
  SD_INTERNAL_ERROR, 
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING, 
  SD_REQUEST_NOT_APPLICABLE, 
  SD_INVALID_PARAMETER,  
  SD_UNSUPPORTED_FEATURE,  
  SD_UNSUPPORTED_HW,  
  SD_ERROR,  
  SD_OK = 0 
} SD_Error;

/** 
  * @brief  SDTransferState������SDIO����״̬���д�������״̬������æ״̬�ʹ������״̬
  */   
typedef enum
{
  SD_TRANSFER_OK  = 0,
  SD_TRANSFER_BUSY = 1,
  SD_TRANSFER_ERROR
} SDTransferState;

/** 
  * @brief  SDCardState���忨�ĵ�ǰ״̬������׼��״̬��ʶ��״̬������״̬������״̬�ȵ� 
  */   
typedef enum
{
  SD_CARD_READY                  = ((uint32_t)0x00000001),
  SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
  SD_CARD_STANDBY                = ((uint32_t)0x00000003),
  SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
  SD_CARD_SENDING                = ((uint32_t)0x00000005),
  SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
  SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
  SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
  SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;


/** 
  * SD_CSD������SD�����ض�����(CSD)�Ĵ���λ�� һ���ṩR2���͵���Ӧ���Ի�ȡ�õ�CSD�Ĵ�������   
  */ 
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD�ṹ */
  __IO uint8_t  SysSpecVersion;       /*!< ϵͳ���汾 */
  __IO uint8_t  Reserved1;            /*!< ���� */
  __IO uint8_t  TAAC;                 /*!< ���ݶ�ȡ����ʱ��1  */
  __IO uint8_t  NSAC;                 /*!< CLK�����ڵ����ݶ�ȡ����ʱ��2 */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. ����ʱ��Ƶ�� */
  __IO uint16_t CardComdClasses;      /*!< ��Ƭ������ */
  __IO uint8_t  RdBlockLen;           /*!< Max. �����ݿ鳤��*/
  __IO uint8_t  PartBlockRead;        /*!< ������Ĳ��ֿ� */
  __IO uint8_t  WrBlockMisalign;      /*!< д���λ */
  __IO uint8_t  RdBlockMisalign;      /*!< ��ȡ�鲻���� */
  __IO uint8_t  DSRImpl;              /*!< DSRʵ��*/
  __IO uint8_t  Reserved2;            /*!< ���� */
  __IO uint32_t DeviceSize;           /*!< �豸��С */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. ��ȡ���� @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. ��ȡ���� @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. д���� @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. д���� @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< �豸��С���� */
  __IO uint8_t  EraseGrSize;          /*!< �������С */
  __IO uint8_t  EraseGrMul;           /*!< �������С���� */
  __IO uint8_t  WrProtectGrSize;      /*!< д�������С */
  __IO uint8_t  WrProtectGrEnable;    /*!< д������enable */
  __IO uint8_t  ManDeflECC;           /*!< ������Ĭ�ϵ�ECC */
  __IO uint8_t  WrSpeedFact;          /*!< д���ٶ����� */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. д���ݿ鳤�� */
  __IO uint8_t  WriteBlockPaPartial;  /*!< ����д�Ĳ��ֿ� */
  __IO uint8_t  Reserved3;            /*!< ���� */
  __IO uint8_t  ContentProtectAppli;  /*!< ���ݱ���Ӧ�� */
  __IO uint8_t  FileFormatGrouop;     /*!< �ļ���ʽ�� */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< ����д���� */
  __IO uint8_t  TempWrProtect;        /*!< ��ʱд���� */
  __IO uint8_t  FileFormat;           /*!< �ļ���ʽ */
  __IO uint8_t  ECC;                  /*!< ECC ���� */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< ��Ϊ 1*/
} SD_CSD;

/** 
  * SD_CID�ṹ�嶨��SD��CID�Ĵ������ݣ� Ҳ��ͨ��R2��Ӧ���ͻ�ȡ�õ� 
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
  __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
  __IO uint32_t ProdName1;            /*!< Product Name part1 */
  __IO uint8_t  ProdName2;            /*!< Product Name part2*/
  __IO uint8_t  ProdRev;              /*!< Product Revision */
  __IO uint32_t ProdSN;               /*!< Product Serial Number */
  __IO uint8_t  Reserved1;            /*!< Reserved1 */
  __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
  __IO uint8_t  CID_CRC;              /*!< CID CRC */
  __IO uint8_t  Reserved2;            /*!< ��Ϊ 1 */
} SD_CID;

/** 
  * SD_CardStatus�ṹ�嶨����SD��״̬�������ݿ�ȡ������͡��ٶȵȼ���������ȡ� ����ƫ�Ƶ�ַ�ȵ�SD��״̬
  */
typedef struct
{
  __IO uint8_t DAT_BUS_WIDTH;
  __IO uint8_t SECURED_MODE;
  __IO uint16_t SD_CARD_TYPE;
  __IO uint32_t SIZE_OF_PROTECTED_AREA;
  __IO uint8_t SPEED_CLASS;
  __IO uint8_t PERFORMANCE_MOVE;
  __IO uint8_t AU_SIZE;
  __IO uint16_t ERASE_SIZE;
  __IO uint8_t ERASE_TIMEOUT;
  __IO uint8_t ERASE_OFFSET;
} SD_CardStatus;


/** 
  * SD_CardInfo�ṹ�嶨����SD����Ϣ��������SD_CSD���ͺ�SD_CID���ͳ�Ա
	* ���ж����˿������������С������Ե�ַRCA�Ϳ����ͳ�Ա
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint32_t CardCapacity;  /*!< ������ */
  uint32_t CardBlockSize; /*!< �����С */
  uint16_t RCA;           /*!< ����Ե�ַRCA */
  uint8_t CardType;       /*!< �����ͳ�Ա */
} SD_CardInfo;

/* �궨��  ������FIOF��ַ����СΪ32�ֽ� ��0x40018080��0x400180fc */
#define SDIO_FIFO_ADDRESS                          ((uint32_t)0x40018080)  //SDIO_FIOF��ַ=SDIO��ַ+0x80��sdio��ַ+0xfc
/* SDIO��ʼ��ʱ��Ƶ�ʣ����400KHz�����ڿ�ʶ��ģʽ */
#define SDIO_INIT_CLK_DIV                          ((uint8_t)0xB2)
/* SDIO���ݴ���ʱ��Ƶ�ʣ����25MHz�� �������ݴ���ģʽ */
/*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
#define SDIO_TRANSFER_CLK_DIV                      ((uint8_t)0x01)


/** 
  * @brief SDIO Commands  Index 
  */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)  /*!< ��λCMD0 */
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((uint8_t)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define SD_CMD_SEND_CSD                            ((uint8_t)9)
#define SD_CMD_SEND_CID                            ((uint8_t)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD����֧�� */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /*!< SD����֧�� */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23) /*!< SD����֧�� */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26) /*!< ������ר�� */
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32) /*!< ����Ҫ�����ĵ�һ��д���ַ��(ֻ������SD��) */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33) /*!< ����Ҫ������������Χ�����һ��д��ĵ�ַ��(ֻ������SD��) */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< ����Ҫ�����ĵ�һ��д���ַ��(�������ڹ��3.31��MMC��) */

#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< ����Ҫ������������Χ�����һ��д���ַ��(�������ڹ��3.31��MMC��) */

#define SD_CMD_ERASE                               ((uint8_t)38)
#define SD_CMD_FAST_IO                             ((uint8_t)39) /*!< SD����֧�� */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD����֧�� */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42)
#define SD_CMD_APP_CMD                             ((uint8_t)55)
#define SD_CMD_GEN_CMD                             ((uint8_t)56)
#define SD_CMD_NO_CMD                              ((uint8_t)64)

/** 
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((uint8_t)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((uint8_t)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((uint8_t)53) /*!< For SD I/O Card only */

/** 
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48) /*!< For SD Card only */
  
/* ͨ��ע�ͣ�ѡ��SDIO����ʱʹ�õ�ģʽ����ѡDMAģʽ����ͨģʽ */  
#define SD_DMA_MODE                                ((uint32_t)0x00000000)
/*#define SD_POLLING_MODE                            ((uint32_t)0x00000002)*/

/**
  * @brief  ���SD���Ƿ���ȷ����ĺ�
  */
#define SD_PRESENT                                 ((uint8_t)0x01)
#define SD_NOT_PRESENT                             ((uint8_t)0x00)

/** 
  * @brief 
  */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)



/* Exported functions ------------------------------------------------------- */
void SD_DeInit(void);
SD_Error SD_Init(void);
SDTransferState SD_GetStatus(void);
SDCardState SD_GetState(void);
uint8_t SD_Detect(void);
SD_Error SD_PowerON(void);
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus);
SD_Error SD_EnableWideBusOperation(uint32_t WideMode);
SD_Error SD_SelectDeselect(uint32_t addr);
SD_Error SD_ReadBlock(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize);
SD_Error SD_ReadMultiBlocks(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SD_Error SD_WriteBlock(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize);
SD_Error SD_WriteMultiBlocks(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SDTransferState SD_GetTransferState(void);
SD_Error SD_StopTransfer(void);
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr);
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SD_Error SD_SendSDStatus(uint32_t *psdstatus);
SD_Error SD_ProcessIRQSrc(void);
SD_Error SD_WaitReadOperation(void);
SD_Error SD_WaitWriteOperation(void);

void SDIO_NVIC_Configuration(void);




#endif /* __BSP_SDIO_SDCARD_H */

