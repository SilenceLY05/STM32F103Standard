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
  * @brief  SD_Error是列举了控制器可能出现的错误
	*			比如CRC校验错误、CRC校验错误、通信等待超时、FIFO上溢或下溢、擦除命令错误等等  
  */   
  SD_CMD_CRC_FAIL                    = (1), /*!< 收到命令响应(但CRC检查失败) */
  SD_DATA_CRC_FAIL                   = (2), /*!< 发送/接收数据块(CRC校验失败)  */
  SD_CMD_RSP_TIMEOUT                 = (3), /*!< 通信等待超时 */
  SD_DATA_TIMEOUT                    = (4), /*!< 数据超时 */
  SD_TX_UNDERRUN                     = (5), /*!< 发送FIFO 下溢 */
  SD_RX_OVERRUN                      = (6), /*!< 接收 FIFO 上溢 */
  SD_START_BIT_ERR                   = (7), /*!< 在widE总线模式下，所有数据信号都没有检测到起始位 */
  SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD的参数超出了范围*/
  SD_ADDR_MISALIGNED                 = (9), /*!< 地址偏差 */
  SD_BLOCK_LEN_ERR                   = (10), /*!< 传输的块长度不允许卡或传输的字节数不匹配块长度 */
  SD_ERASE_SEQ_ERR                   = (11), /*!< 擦除命令顺序错误.*/
  SD_BAD_ERASE_PARAM                 = (12), /*!< 擦除组的选择无效 */
  SD_WRITE_PROT_VIOLATION            = (13), /*!< 尝试编写写保护块程序 */
  SD_LOCK_UNLOCK_FAILED              = (14), /*!< 在解锁命令中检测到序列或密码错误，或者如果试图访问锁定的卡 */
  SD_COM_CRC_FAILED                  = (15), /*!< 前一个命令的CRC检查失败 */
  SD_ILLEGAL_CMD                     = (16), /*!< 命令对于卡状态不合法 */
  SD_CARD_ECC_FAILED                 = (17), /*!< 卡内部ECC已应用，但未能纠正数据 */
  SD_CC_ERROR                        = (18), /*!< 内部卡控制器错误 */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< 一般或未知错误 */
  SD_STREAM_READ_UNDERRUN            = (20), /*!< 在流读取操作中，卡无法支持数据传输. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< 卡无法支持流模式下的数据编程 */
  SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD覆盖错误 */
  SD_WP_ERASE_SKIP                   = (23), /*!< 仅部分地址空间被擦除 */
  SD_CARD_ECC_DISABLED               = (24), /*!< 命令已执行，未使用内部ECC  */
  SD_ERASE_RESET                     = (25), /*!< 在执行之前清除擦除序列，因为收到擦除序列命令 */
  SD_AKE_SEQ_ERROR                   = (26), /*!< 验证顺序错误. */
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
  * @brief  SDTransferState定义了SDIO传输状态，有传输正常状态、传输忙状态和传输错误状态
  */   
typedef enum
{
  SD_TRANSFER_OK  = 0,
  SD_TRANSFER_BUSY = 1,
  SD_TRANSFER_ERROR
} SDTransferState;

/** 
  * @brief  SDCardState定义卡的当前状态，比如准备状态、识别状态、待机状态、传输状态等等 
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
  * SD_CSD定义了SD卡的特定数据(CSD)寄存器位， 一般提供R2类型的响应可以获取得到CSD寄存器内容   
  */ 
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD结构 */
  __IO uint8_t  SysSpecVersion;       /*!< 系统规格版本 */
  __IO uint8_t  Reserved1;            /*!< 保留 */
  __IO uint8_t  TAAC;                 /*!< 数据读取访问时间1  */
  __IO uint8_t  NSAC;                 /*!< CLK周期内的数据读取访问时间2 */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. 总线时钟频率 */
  __IO uint16_t CardComdClasses;      /*!< 卡片命令类 */
  __IO uint8_t  RdBlockLen;           /*!< Max. 读数据块长度*/
  __IO uint8_t  PartBlockRead;        /*!< 允许读的部分块 */
  __IO uint8_t  WrBlockMisalign;      /*!< 写块错位 */
  __IO uint8_t  RdBlockMisalign;      /*!< 读取块不对齐 */
  __IO uint8_t  DSRImpl;              /*!< DSR实现*/
  __IO uint8_t  Reserved2;            /*!< 保留 */
  __IO uint32_t DeviceSize;           /*!< 设备大小 */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. 读取电流 @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. 读取电流 @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. 写电流 @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. 写电流 @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< 设备大小乘数 */
  __IO uint8_t  EraseGrSize;          /*!< 擦除组大小 */
  __IO uint8_t  EraseGrMul;           /*!< 擦除组大小乘数 */
  __IO uint8_t  WrProtectGrSize;      /*!< 写保护组大小 */
  __IO uint8_t  WrProtectGrEnable;    /*!< 写保护组enable */
  __IO uint8_t  ManDeflECC;           /*!< 制造商默认的ECC */
  __IO uint8_t  WrSpeedFact;          /*!< 写入速度因子 */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. 写数据块长度 */
  __IO uint8_t  WriteBlockPaPartial;  /*!< 允许写的部分块 */
  __IO uint8_t  Reserved3;            /*!< 保留 */
  __IO uint8_t  ContentProtectAppli;  /*!< 内容保护应用 */
  __IO uint8_t  FileFormatGrouop;     /*!< 文件格式组 */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< 永久写保护 */
  __IO uint8_t  TempWrProtect;        /*!< 临时写保护 */
  __IO uint8_t  FileFormat;           /*!< 文件格式 */
  __IO uint8_t  ECC;                  /*!< ECC 代码 */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< 总为 1*/
} SD_CSD;

/** 
  * SD_CID结构体定义SD卡CID寄存器内容， 也是通过R2响应类型获取得到 
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
  __IO uint8_t  Reserved2;            /*!< 总为 1 */
} SD_CID;

/** 
  * SD_CardStatus结构体定义了SD卡状态，有数据宽度、卡类型、速度等级、擦除宽度、 传输偏移地址等等SD卡状态
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
  * SD_CardInfo结构体定义了SD卡信息，包括了SD_CSD类型和SD_CID类型成员
	* 还有定义了卡容量、卡块大小、卡相对地址RCA和卡类型成员
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint32_t CardCapacity;  /*!< 卡容量 */
  uint32_t CardBlockSize; /*!< 卡块大小 */
  uint16_t RCA;           /*!< 卡相对地址RCA */
  uint8_t CardType;       /*!< 卡类型成员 */
} SD_CardInfo;

/* 宏定义  定义了FIOF地址，大小为32字节 从0x40018080至0x400180fc */
#define SDIO_FIFO_ADDRESS                          ((uint32_t)0x40018080)  //SDIO_FIOF地址=SDIO地址+0x80至sdio地址+0xfc
/* SDIO初始化时钟频率（最大400KHz）用于卡识别模式 */
#define SDIO_INIT_CLK_DIV                          ((uint8_t)0xB2)
/* SDIO数据传输时钟频率（最大25MHz） 用于数据传输模式 */
/*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
#define SDIO_TRANSFER_CLK_DIV                      ((uint8_t)0x01)


/** 
  * @brief SDIO Commands  Index 
  */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)  /*!< 复位CMD0 */
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
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD卡不支持 */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /*!< SD卡不支持 */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23) /*!< SD卡不支持 */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26) /*!< 制造商专用 */
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32) /*!< 设置要擦除的第一个写块地址。(只适用于SD卡) */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33) /*!< 设置要擦除的连续范围的最后一个写块的地址。(只适用于SD卡) */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< 设置要擦除的第一个写块地址。(仅适用于规格3.31的MMC卡) */

#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< 设置要擦除的连续范围的最后一个写块地址。(仅适用于规格3.31的MMC卡) */

#define SD_CMD_ERASE                               ((uint8_t)38)
#define SD_CMD_FAST_IO                             ((uint8_t)39) /*!< SD卡不支持 */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD卡不支持 */
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
  
/* 通过注释，选择SDIO传输时使用的模式，可选DMA模式及普通模式 */  
#define SD_DMA_MODE                                ((uint32_t)0x00000000)
/*#define SD_POLLING_MODE                            ((uint32_t)0x00000002)*/

/**
  * @brief  检测SD卡是否正确插入的宏
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

