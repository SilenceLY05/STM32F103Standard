/**
  ******************************************************************************
  * @file    stm32_eval_sdio_sd.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file provides a set of functions needed to manage the SDIO SD 
  *          Card memory mounted on STM32xx-EVAL board (refer to stm32_eval.h
  *          to know about the boards supporting this memory). 
  *          
  *            
  *  @verbatim
  *
  *          ===================================================================
  *                                   How to use this driver
  *          ===================================================================
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32 hardware resources (SDIO and 
  *          GPIO) are defined in stm32xx_eval.h file, and the initialization is 
  *          performed in SD_LowLevel_Init() function declared in stm32xx_eval.c 
  *          file.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          SD_LowLevel_Init() function.
  *            
  *          A - SD Card Initialization and configuration
  *          ============================================    
  *            - To initialize the SD Card, use the SD_Init() function.  It 
  *              Initializes the SD Card and put it into StandBy State (Ready 
  *              for data transfer). This function provide the following operations:
  *           
  *              1 - Apply the SD Card initialization process at 400KHz and check
  *                  the SD Card type (Standard Capacity or High Capacity). You 
  *                  can change or adapt this frequency by adjusting the 
  *                  "SDIO_INIT_CLK_DIV" define inside the stm32xx_eval.h file. 
  *                  The SD Card frequency (SDIO_CK) is computed as follows:
  *                    
  *                     +---------------------------------------------+    
  *                     | SDIO_CK = SDIOCLK / (SDIO_INIT_CLK_DIV + 2) |
  *                     +---------------------------------------------+  
  *                          
  *                  In initialization mode and according to the SD Card standard, 
  *                  make sure that the SDIO_CK frequency don't exceed 400KHz.        
  *         
  *              2 - Get the SD CID and CSD data. All these information are
  *                  managed by the SDCardInfo structure. This structure provide
  *                  also ready computed SD Card capacity and Block size.   
  *
  *              3 - Configure the SD Card Data transfer frequency. By Default,
  *                  the card transfer frequency is set to 24MHz. You can change
  *                  or adapt this frequency by adjusting the "SDIO_TRANSFER_CLK_DIV" 
  *                  define inside the stm32xx_eval.h file.
  *                  The SD Card frequency (SDIO_CK) is computed as follows:
  *                    
  *                     +---------------------------------------------+    
  *                     | SDIO_CK = SDIOCLK / (SDIO_INIT_CLK_DIV + 2) |
  *                     +---------------------------------------------+     
  *                                    
  *                  In transfer mode and according to the SD Card standard, 
  *                  make sure that the SDIO_CK frequency don't exceed 25MHz
  *                  and 50MHz in High-speed mode switch.
  *                  To be able to use a frequency higher than 24MHz, you should
  *                  use the SDIO peripheral in bypass mode. Refer to the 
  *                  corresponding reference manual for more details.
  *                    
  *              4 -  Select the corresponding SD Card according to the address
  *                   read with the step 2.
  *                                   
  *              5 -  Configure the SD Card in wide bus mode: 4-bits data.                
  *
  *          B - SD Card Read operation
  *          ========================== 
  *           - You can read SD card by using two function: SD_ReadBlock() and
  *             SD_ReadMultiBlocks() functions. These functions support only
  *             512-byte block length.
  *           - The SD_ReadBlock() function read only one block (512-byte). This
  *             function can transfer the data using DMA controller or using 
  *             polling mode. To select between DMA or polling mode refer to 
  *             "SD_DMA_MODE" or "SD_POLLING_MODE" inside the stm32_eval_sdio_sd.h
  *             file and uncomment the corresponding line. By default the SD DMA
  *             mode is selected          
  *           - The SD_ReadMultiBlocks() function read only mutli blocks (multiple 
  *             of 512-byte). 
  *           - Any read operation should be followed by two functions to check
  *             if the DMA Controller and SD Card status.
  *              - SD_ReadWaitOperation(): this function insure that the DMA
  *                controller has finished all data transfer.
  *              - SD_GetStatus(): to check that the SD Card has finished the 
  *                data transfer and it is ready for data.
  *                  
  *           - The DMA transfer is finished by the SDIO Data End interrupt. User
  *             has to call the SD_ProcessIRQ() function inside the SDIO_IRQHandler().
  *             Don't forget to enable the SDIO_IRQn interrupt using the NVIC controller.      
  *                
  *          C - SD Card Write operation
  *          =========================== 
  *           - You can write SD card by using two function: SD_WriteBlock() and
  *             SD_WriteMultiBlocks() functions. These functions support only
  *             512-byte block length.   
  *           - The SD_WriteBlock() function write only one block (512-byte). This
  *             function can transfer the data using DMA controller or using 
  *             polling mode. To select between DMA or polling mode refer to 
  *             "SD_DMA_MODE" or "SD_POLLING_MODE" inside the stm32_eval_sdio_sd.h
  *             file and uncomment the corresponding line. By default the SD DMA
  *             mode is selected          
  *           - The SD_WriteMultiBlocks() function write only mutli blocks (multiple 
  *             of 512-byte). 
  *           - Any write operation should be followed by two functions to check
  *             if the DMA Controller and SD Card status.
  *              - SD_ReadWaitOperation(): this function insure that the DMA
  *                controller has finished all data transfer.
  *              - SD_GetStatus(): to check that the SD Card has finished the 
  *                data transfer and it is ready for data.         
  *                
  *           - The DMA transfer is finished by the SDIO Data End interrupt. User
  *             has to call the SD_ProcessIRQ() function inside the SDIO_IRQHandler().
  *             Don't forget to enable the SDIO_IRQn interrupt using the NVIC controller.      
               
  *             
  *          D - SD card status
  *          ================== 
  *           - At any time, you can check the SD Card status and get the SD card
  *             state by using the SD_GetStatus() function. This function checks
  *             first if the SD card is still connected and then get the internal
  *             SD Card transfer state.     
  *           - You can also get the SD card SD Status register by using the 
  *             SD_SendSDStatus() function.       
  *               
  *          E - Programming Model
  *          ===================== 
  *             Status = SD_Init(); // Initialization Step as described in section A
  *               
  *             // SDIO Interrupt ENABLE
  *             NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  *             NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  *             NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  *             NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  *             NVIC_Init(&NVIC_InitStructure);
  *             
  *             // Write operation as described in Section C
  *             Status = SD_WriteBlock(buffer, address, 512);
  *             Status = SD_WaitWriteOperation();
  *             while(SD_GetStatus() != SD_TRANSFER_OK); 
  *             
  *             Status = SD_WriteMultiBlocks(buffer, address, 512, NUMBEROFBLOCKS);
  *             Status = SD_WaitWriteOperation();
  *             while(SD_GetStatus() != SD_TRANSFER_OK);     
  *             
  *             // Read operation as described in Section B
  *             Status = SD_ReadBlock(buffer, address, 512);
  *             Status = SD_WaitReadOperation();
  *             while(SD_GetStatus() != SD_TRANSFER_OK);
  *             
  *             Status = SD_ReadMultiBlocks(buffer, address, 512, NUMBEROFBLOCKS);
  *             Status = SD_WaitReadOperation();
  *             while(SD_GetStatus() != SD_TRANSFER_OK);            
  *               
  *                                     
  *          STM32 SDIO Pin assignment
  *          =========================    
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SDIO Pins            |     SD        |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          |      SDIO D2                |   D2          |    1        |
  *          |      SDIO D3                |   D3          |    2        |
  *          |      SDIO CMD               |   CMD         |    3        |
  *          |                             |   VCC         |    4 (3.3 V)|
  *          |      SDIO CLK               |   CLK         |    5        |
  *          |                             |   GND         |    6 (0 V)  |
  *          |      SDIO D0                |   D0          |    7        |
  *          |      SDIO D1                |   D1          |    8        |  
  *          +-----------------------------+---------------+-------------+  
  *              
  *  @endverbatim                
  *             
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
	
#include "bsp_sdio_sdcard.h"
#include "bsp_usart.h"

/* Private macro -------------------------------------------------------------*/
/** 
  * @brief  SDIO Static flags, TimeOut, FIFO Address  
  */
#define NULL 0
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000)

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

/** 
  * @brief  Masks for R6 Response 
  */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)

#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

/** 
  * @brief  Command Class Supported 
  */
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020)

/** 
  * @brief  以下命令为SD卡相关命令。应该在发送这些命令之前发送SDIO_APP_CMD。 
  */
#define SDIO_SEND_IF_COND               ((uint32_t)0x00000008)


/* Private variables ---------------------------------------------------------*/
static uint32_t CardType =  SDIO_STD_CAPACITY_SD_CARD_V1_1;  //存储卡的类型，先把它初始化为1.1协议的卡
static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;  //存储CSD，DID，寄存器和卡相对地址
static uint8_t SDSTATUS_Tab[16];  //存储卡状态，是CSR的一部分
__IO uint32_t StopCondition = 0;  //用于停止卡操作的标志
__IO SD_Error TransferError = SD_OK;  //用于存储卡错误，初始化为正常状态
__IO uint32_t TransferEnd = 0;   //用于标志传输是否结束，在中断服务函数中调用
SD_CardInfo SDCardInfo;    //用于存储卡的信息，DSR的一部分？
 

/*用于sdio初始化的结构体*/
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;   


/* Private function prototypes -----------------------------------------------*/
static SD_Error CmdError(void);
static SD_Error CmdResp1Error(uint8_t cmd);
static SD_Error CmdResp7Error(void);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(uint8_t *pstatus);
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr);

static void GPIO_Configuration(void);
static uint32_t SD_DMAEndOfTransferStatus(void);
static void SD_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
static void SD_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);

uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);
  
	
/* Private functions ---------------------------------------------------------*/

/*
 * 函数名：SD_DeInit
 * 描述  ：复位SDIO端口
 * 输入  ：无
 * 输出  ：无
 */
void SD_DeInit(void)
{ 
  GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 关闭SDIO时钟 */
	SDIO_ClockCmd(DISABLE);
	
	/* 将电源状态设置为关闭 */
	SDIO_SetPowerState(SDIO_PowerState_OFF);
	
	/* 取消初始化SDIO外设 */
	SDIO_DeInit();
	
	/* 关闭SDIO AHB时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO,DISABLE);
	
	/* 配置pc08、pc09、pc10、pc11、pc12引脚:D0、D1、D2、D3、CLK引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/* 配置PD02 CMD引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

/* NVIC_Configuration SD优先级配置为最高优先级 */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStructure);
}

/* 返回DMA传输结束状态 */
uint32_t SD_DMAEndOfTransferStatus(void)
{
	return (uint32_t)DMA_GetFlagStatus(DMA2_FLAG_TC4);   //Channel4 传输完成标志 

}

/* SD_DMA_RXConfig(),为SDIO接受数据配置DMA2的通信4的请求，
*BufferDST发送时用于指定要对外发送的数据的内存地址 BufferSize表示缓冲区大小 */
void SD_DMA_RxConfig(uint32_t *BufferDST,uint32_t BufferSize)
{
	DMA_InitTypeDef    DMA_InitStructure;
	
	DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE2 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4 );  //清除DMA标志位
	
	/* 配置前先禁止DMA */
	DMA_Cmd(DMA2_Channel4,DISABLE);
	
	/* DMA2传输配置 */
	/* 外设地址，fifo */
	//除以4，把字转成字节单位
	DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
	//外设为源地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//非 存储器至存储器模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//目标地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferDST;
	//外设数据大小为字，32位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	//存储目标地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//不循环
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//外设地址，fifo
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )SDIO_FIFO_ADDRESS;
	//外设数据大小为字，32位
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	//外设地址不自增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//通道优先级高
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(DMA2_Channel4,&DMA_InitStructure);
	
	/* 使能DMA通道 */
	DMA_Cmd(DMA2_Channel4,ENABLE);
	
}

/* SD_DMA_TXConfig(),为SDIO发送数据配置DMA2的通信4的请求， 
*BufferSRC在接收时用于指定接收到的数据存储的内存地址 BufferSize表示缓冲区大小 */
void SD_DMA_TxConfig(uint32_t *BufferSRC,uint32_t BufferSize)
{
	DMA_InitTypeDef    DMA_InitStructure;
	
	DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE2 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4 );  //清除DMA标志位
	
	/* 配置前先禁止DMA */
	DMA_Cmd(DMA2_Channel4,DISABLE);
	
	/* DMA2传输配置 */
	/* 外设地址，fifo */
	//除以4，把字转成字节单位
	DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
	//外设为写入目标
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//非 存储器至存储器模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//目标地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferSRC;
	//外设数据大小为字，32位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	//存储目标地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//不循环
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//外设地址，fifo
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )SDIO_FIFO_ADDRESS;
	//外设数据大小为字，32位
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	//外设地址不自增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//通道优先级高
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(DMA2_Channel4,&DMA_InitStructure);
	
	/* 使能DMA通道 */
	DMA_Cmd(DMA2_Channel4,ENABLE);
	
}



/* GPIO_Configuration初始化SDIO用到的引脚，开启时钟 */
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	
	 /*  使能端口时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD,ENABLE);
	
	/* 配置 PC.08, PC.09, PC.10, PC.11,PC.12 引脚: D0, D1, D2, D3,CLK 引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/* 配置 PD.02 CMD 引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	/* 使能 SDIO AHB 时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO,ENABLE);
	
	/* 使能 DMA2 时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
}

/**
 * 函数名：SD_Init
 * 描述  ：初始化SD卡，使卡处于就绪状态(准备传输数据)
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_Init(void)
{
	/* 重置SD_Error状态 */
  SD_Error errorstatus = SD_OK;
  
  /* 复位卡到idle状态 */
	/* 配置SDIO优先级 */
	NVIC_Configuration();
	/* 对SDIO的外设底层引脚初始化 */
	GPIO_Configuration();

	/*对SDIO的所有寄存器进行复位*/
  SDIO_DeInit();

	/*上电并进行卡识别流程，确认卡的操作电压  */
  errorstatus = SD_PowerON();
	/*如果上电，识别不成功，返回“响应超时”错误 */
  if (errorstatus != SD_OK)
  {
    /*!< CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);
  }
	/* 判断执行SD_PowerON函数无错误后，执行下面的SD_InitializeCards函数进行与SD卡相关的初始化，
		 使得卡进入数据传输模式下的待机模式    */
  errorstatus = SD_InitializeCards();

	
  if (errorstatus != SD_OK)
  { //失败返回
    /*!< CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);
  }

  /* 配置SDIO外设 上电识别，卡初始化都完成后，进入数据传输模式，提高读写速度 
		 之前的卡识别模式都设定CMD线时钟为小于400KHz， 进入数据传输模式可以把时钟设置为小于25MHz，以便提高数据传输速率*/
  /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
  /*!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz */  
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
	/*上升沿采集数据 */
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	/* Bypass模式使能的话，SDIO_CK不经过SDIO_ClockDiv分频 */
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	/* 若开启此功能，在总线空闲时关闭sd_clk时钟 */
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	/* 暂时配置成1bit模式 */
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	/* 硬件流，若开启，在FIFO不能进行发送和接收数据时，数据传输暂停 */
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);
  
  if (errorstatus == SD_OK)
  {
     /* 调用SD_GetCardInfo函数获取SD卡信息， 
			  它需要一个指向SD_CardInfo类型变量地址的指针形参，这里赋值为SDCardInfo变量的地址 */
    errorstatus = SD_GetCardInfo(&SDCardInfo);
  }

  if (errorstatus == SD_OK)
  {
     /* 调用SD_SelectDeselect函数用于选择特定RCA的SD卡，
				它实际是向SD卡发送CMD7。执行之后， 卡就从待机状态转变为传输模式 */
    errorstatus = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
  }

  if (errorstatus == SD_OK)
  {
		/* 最后为了提高读写，扩展数据线宽度,开启4bits模式 */
    errorstatus = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }  

  return(errorstatus);
}

/**
  * @brief  获取当前sd卡数据传输状态。
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetStatus(void)
{
  SDCardState cardstate =  SD_CARD_TRANSFER;

  cardstate = SD_GetState();
  
  if (cardstate == SD_CARD_TRANSFER)
  {
    return(SD_TRANSFER_OK);
  }
  else if(cardstate == SD_CARD_ERROR)
  {
    return (SD_TRANSFER_ERROR);
  }
  else
  {
    return(SD_TRANSFER_BUSY);
  }
}

/**
  * @brief  返回当前卡的状态.
  * @param  None
  * @retval SDCardState: SD Card Error or SD Card Current State.
  */
SDCardState SD_GetState(void)
{
  uint32_t resp1 = 0;
  
 
  if (SD_SendStatus(&resp1) != SD_OK)
  {
    return SD_CARD_ERROR;
  }
  else
  {
    return (SDCardState)((resp1 >> 9) & 0x0F);
  }
  
}


/*
 * 函数名：SD_PowerON
 * 描述  ：确保SD卡的工作电压和配置控制时钟
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：在 SD_Init() 调用
 */
SD_Error SD_PowerON(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t response = 0, count = 0, validvoltage = 0;
  uint32_t SDType = SD_STD_CAPACITY;

/********************************************************************************************************/
  /* 上电初始化 
   * 配置SDIO的外设
   * SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV)   
   * 初始化时的时钟不能大于400KHz
   */
	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	/* 不使用bypass模式，直接用HCLK进行分频得到SDIO_CK */
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	 /* 空闲时不关闭时钟电源 */
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	 /* 初始化的时候暂时先把数据线配置成1根 */
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	/* 禁止能硬件流控制 */
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);

  /* 开启SDIO外设的电源 */
  SDIO_SetPowerState(SDIO_PowerState_ON);

  /* 使能 SDIO 时钟 */
  SDIO_ClockCmd(ENABLE);

/********************************************************************************************************/   
  /* 下面发送一系列命令,开始卡识别流程
   * CMD0: GO_IDLE_STATE(复位所以SD卡进入空闲状态) 
   * 没有响应  
	 */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;
	 /* 没有响应 */
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
	/* 关闭等待中断 */
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	/* 关闭等待中断 */
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
	/* 检测是否正确接收到cmd0  CmdError函数用于无需响应的命令发送检测，带有等待超时检测功能
		 它通过不断检测SDIO_STA寄存器的CMDSENT位即可知道命令发送成功与否。 */
  errorstatus = CmdError();
	/* 命令发送出错，返回 如果遇到超时错误则直接退出SD_PowerON函数。如果无错误则执行下面程序 */
  if (errorstatus != SD_OK)
  {
    /* CMD 响应超时  */
    return(errorstatus);
  }

   /* CMD8: SEND_IF_COND
    * 发送 CMD8 检查SD卡的电压操作条件
    *
    * 参数: - [31:12]: 保留 (要被设置为 '0')
    *       - [11:8] : 支持的电压 (VHS) 0x1 (范围: 2.7-3.6 V)
    *       - [7:0]  : 校验模式 (推荐 0xAA)
    * 响应类型: R7
    */
    /* 接收到命令sd会返回这个参数 */
  SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
	 /*检查是否接收到命令  使用CmdResp7Error函数可获取得到R7响应结果*/
  errorstatus = CmdResp7Error();
	/* 如果CmdResp7Error函数返回值为SD_OK，即CMD8有响应
		 可以判定SD卡为V2.0及以上的高容量SD卡，如果没有响应可能是V1.1版本卡或者是不可用卡 */
  if (errorstatus == SD_OK)
  {
		/* SD Card 2.0 ，先把它定义会sdsc类型的卡 */
    CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; /*!< SD Card 2.0 */
		/* SD Card 2.0 ，先把它定义会sdsc类型的卡 */
    SDType = SD_HIGH_CAPACITY;
  }
  else
  {
		/* 无响应，说明是1.x的或mmc的卡 */
    /* 发命令 CMD55 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  }
  /* CMD55 		
   * 发送cmd55，用于检测是sd卡还是mmc卡，或是不支持的卡
	 * CMD 响应: R1
   */ 
  SDIO_CmdInitStructure.SDIO_Argument = 0x00;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	/* 是否响应，没响应的是mmc或不支持的卡 */
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

/********************************************************************************************************/
  /* 若 errorstatus 为 Command TimeOut, 说明是MMC 卡
    * 若 errorstatus 为 SD_OK ，说明是SD card: SD 卡 2.0 (电压范围不匹配)
    * 或 SD 卡 1.x
    */
  if (errorstatus == SD_OK)   //响应了cmd55，是sd卡，可能为1.x,可能为2.0
  {
		/*下面开始循环地发送sdio支持的电压范围，循环一定次数*/
		
    /* SD CARD  发送 ACMD41 SD_APP_OP_COND ，带参数 0x80100000*/
    while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {

      /* 在发送ACMD命令前都要先向卡发送CMD55 发送 CMD55 APP_CMD ， RCA 为 0
			如果CMD55命令都没有响应说明是MMC卡或不可用卡*/
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
			
			/* ACMD41
			 * 命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSC还是SDHC
			 * 0:SDSC
			 * 1:SDHC
       * 响应：R3,对应的是OCR寄存器			
			 */	
      SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

			/* CmdResp3Error函数用于检测命令正确发送并带有超时检测功能
				 但并不具备响应内容接收功能，需要在判定命令正确发送之后调用SDIO_GetResponse函数才能获取响应的内容 */
      errorstatus = CmdResp3Error();
      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

			/* 若卡需求电压在SDIO的供电电压范围内，会自动上电并标志pwr_up位读取卡寄存器，卡状态
				 在有响应时，SDIO外设会自动把响应存放在SDIO_RESPx寄存器中，
				 SDIO_GetResponse函数只是根据形参返回对应响应寄存器的值。通过判定响应内容值即可确定SD卡类型
			 */
      response = SDIO_GetResponse(SDIO_RESP1);
			
			/* 读取卡的ocr寄存器的pwr_up位，看是否已工作在正常电压 */
      validvoltage = (((response >> 31) == 1) ? 1 : 0);
      count++;    /* 计算循环次数 */
    }
    if (count >= SD_MAX_VOLT_TRIAL)        /* 循环检测超过一定次数还没上电 */
    {
      errorstatus = SD_INVALID_VOLTRANGE;   /* SDIO不支持card的供电电压 */
      return(errorstatus);
    }

		/*检查卡返回信息中的HCS位*/
		/* 判断ocr中的ccs位 ，如果是sdsc卡则不执行下面的语句 */
    if (response &= SD_HIGH_CAPACITY)
    {
      CardType = SDIO_HIGH_CAPACITY_SD_CARD;  /* 把卡类型从初始化的sdsc型改为sdhc型 */
    }

  }/*!< else MMC Card */

	/* 执行SD_PowerON函数无错误后就已经确定了SD卡类型，并说明卡和主机电压是匹配的，
		SD卡处于卡识别模式下的准备状态。 退出SD_PowerON函数返回SD_Init函数，执行接下来代码 */
  return(errorstatus);
}

/*
 * 函数名：SD_PowerOFF
 * 描述  ：关掉SDIO的输出信号
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_PowerOFF(void)
{
  SD_Error errorstatus = SD_OK;

  /*!< Set Power State to OFF */
  SDIO_SetPowerState(SDIO_PowerState_OFF);

  return(errorstatus);
}

  /*
 * 函数名：SD_InitializeCards
 * 描述  ：初始化所有的卡或者单个卡进入就绪状态
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：在 SD_Init() 调用，在调用power_on（）上电卡识别完毕后，调用此函数进行卡初始化
 */
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  uint16_t rca = 0x01;

  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

	/* 判断卡的类型 */
  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
  {
    /* 发送CMD2，CMD2是用于通知所有卡通过CMD线返回CID值 
		 * 响应：R2，对应CID寄存器
		 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

		/* 执行CMD2发送之后就可以使用CmdResp2Error函数获取CMD2命令发送情况 */
    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

		/*  发送无错误后即可以使用SDIO_GetResponse函数获取响应内容，
			  它是个长响应，我们把CMD2响应内容存放在CID_Tab数组内 */
    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
	
	/********************************************************************************************************/
  if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) 
			||  (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) 
			||  (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType)
      ||  (SDIO_HIGH_CAPACITY_SD_CARD == CardType))   /* 使用的是2.0的卡 */
  {
     /*发送CMD2之后紧接着就发送CMD3，用于指示SD卡自行推荐RCA地址
     * 响应：R6，对应RCA寄存器		
		 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

		/* 把接收到的卡相对地址存起来 */
		/* CmdResp6Error函数用于检查R6响应错误， 它有两个形参，一个是命令号，这里为CMD3，另外一个是RCA数据指针，
			 这里使用rca变量的地址赋值给它，使得在CMD3正确响应之后rca变量即存放SD卡的RCA */
    errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);

		/* 如果判断无错误说明此刻SD卡已经处于数据传输模式 */
    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
  }

	/********************************************************************************************************/
  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
  {
    RCA = rca;

    /* 发送CMD9给指定RCA的SD卡使其发送返回其CSD寄存器内容， 
		这里的RCA就是在CmdResp6Error函数获取得到的rca。最后把响应内容存放在CSD_Tab数组中 */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }

	/********************************************************************************************************/	
	/*全部卡初始化成功 */
  errorstatus = SD_OK; /*!< All cards get intialized */

	/* 执行SD_InitializeCards函数无错误后SD卡就已经处于数据传输模式下的待机状态，
		 退出SD_InitializeCards后会返回前面的SD_Init函数， 执行接下来代码 */
  return(errorstatus);
}

/*
 * 函数名：SD_GetCardInfo
 * 描述  ：获取SD卡的具体信息 把CID_Tab数组和CSD_Tab数组中的内容整合复制到SDCardInfo变量对应成员内
 * 输入  ：-cardinfo 指向SD_CardInfo结构体的指针
 *         这个结构里面包含了SD卡的具体信息
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error errorstatus = SD_OK;
  uint8_t tmp = 0;

  cardinfo->CardType = (uint8_t)CardType;
  cardinfo->RCA = (uint16_t)RCA;

  /*!< Byte 0 */
  tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);  //取CSD_Tab[0]数据的前8个字节
  cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;    //取temp数据的前2个字节
  cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;   //取temp数据的前中间4个字节
  cardinfo->SD_csd.Reserved1 = tmp & 0x03;   //取temp数据的后2个字节

  /*!< Byte 1 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.TAAC = tmp;

  /*!< Byte 2 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.NSAC = tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);
  cardinfo->SD_csd.MaxBusClkFrec = tmp;

  /*!< Byte 4 */
  tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);  //取CSD_Tab[1]数据的前8个字节
  cardinfo->SD_csd.CardComdClasses = tmp << 4;

  /*!< Byte 5 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);  //取CSD_Tab[1]数据的前8-16位字节
  cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;  //取CSD_Tab[1]数据的前8-12位字节
  cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;   //取CSD_Tab[1]数据的前12-16位字节

  /*!< Byte 6 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.Reserved2 = 0; /*!< Reserved */

  if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
  {
    cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;

    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize |= (tmp) << 2;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;

    cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    
    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
    cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
    cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
    cardinfo->CardCapacity *= cardinfo->CardBlockSize;
  }
  else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);

    cardinfo->SD_csd.DeviceSize |= (tmp << 8);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);

    cardinfo->SD_csd.DeviceSize |= (tmp);

    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
    
    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
    cardinfo->CardBlockSize = 512;    
  }


  cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;

  /*!< Byte 11 */
  tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);
  cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);

  /*!< Byte 12 */
  tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
  cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
  cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

  /*!< Byte 13 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.Reserved3 = 0;
  cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);

  /*!< Byte 14 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
  cardinfo->SD_csd.ECC = (tmp & 0x03);

  /*!< Byte 15 */
  tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);
  cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_csd.Reserved4 = 1;


  /*!< Byte 0 */
  tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ManufacturerID = tmp;

  /*!< Byte 1 */
  tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.OEM_AppliID = tmp << 8;

  /*!< Byte 2 */
  tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
  cardinfo->SD_cid.OEM_AppliID |= tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);
  cardinfo->SD_cid.ProdName1 = tmp << 24;

  /*!< Byte 4 */
  tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdName1 |= tmp << 16;

  /*!< Byte 5 */
  tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdName1 |= tmp << 8;

  /*!< Byte 6 */
  tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdName1 |= tmp;

  /*!< Byte 7 */
  tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);
  cardinfo->SD_cid.ProdName2 = tmp;

  /*!< Byte 8 */
  tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdRev = tmp;

  /*!< Byte 9 */
  tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdSN = tmp << 24;

  /*!< Byte 10 */
  tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdSN |= tmp << 16;

  /*!< Byte 11 */
  tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);
  cardinfo->SD_cid.ProdSN |= tmp << 8;

  /*!< Byte 12 */
  tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdSN |= tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
  cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;

  /*!< Byte 14 */
  tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ManufactDate |= tmp;

  /*!< Byte 15 */
  tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);
  cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_cid.Reserved2 = 1;
  
  return(errorstatus);
}

/**
  * @brief  如果卡支持，为所请求的卡启用宽总线操作
  * @param  WideMode: Specifies the SD card wide bus mode. 
  *   This parameter can be one of the following values:
  *     @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
  *     @arg SDIO_BusWide_4b: 4-bit data transfer
  *     @arg SDIO_BusWide_1b: 1-bit data transfer
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus)
{
  SD_Error errorstatus = SD_OK;
  uint8_t tmp = 0;

  errorstatus = SD_SendSDStatus((uint32_t *)SDSTATUS_Tab);

  if (errorstatus  != SD_OK)
  {
    return(errorstatus);
  }

  /*!< Byte 0 */
  tmp = (uint8_t)((SDSTATUS_Tab[0] & 0xC0) >> 6);
  cardstatus->DAT_BUS_WIDTH = tmp;

  /*!< Byte 0 */
  tmp = (uint8_t)((SDSTATUS_Tab[0] & 0x20) >> 5);
  cardstatus->SECURED_MODE = tmp;

  /*!< Byte 2 */
  tmp = (uint8_t)((SDSTATUS_Tab[2] & 0xFF));
  cardstatus->SD_CARD_TYPE = tmp << 8;

  /*!< Byte 3 */
  tmp = (uint8_t)((SDSTATUS_Tab[3] & 0xFF));
  cardstatus->SD_CARD_TYPE |= tmp;

  /*!< Byte 4 */
  tmp = (uint8_t)(SDSTATUS_Tab[4] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA = tmp << 24;

  /*!< Byte 5 */
  tmp = (uint8_t)(SDSTATUS_Tab[5] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 16;

  /*!< Byte 6 */
  tmp = (uint8_t)(SDSTATUS_Tab[6] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 8;

  /*!< Byte 7 */
  tmp = (uint8_t)(SDSTATUS_Tab[7] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp;

  /*!< Byte 8 */
  tmp = (uint8_t)((SDSTATUS_Tab[8] & 0xFF));
  cardstatus->SPEED_CLASS = tmp;

  /*!< Byte 9 */
  tmp = (uint8_t)((SDSTATUS_Tab[9] & 0xFF));
  cardstatus->PERFORMANCE_MOVE = tmp;

  /*!< Byte 10 */
  tmp = (uint8_t)((SDSTATUS_Tab[10] & 0xF0) >> 4);
  cardstatus->AU_SIZE = tmp;

  /*!< Byte 11 */
  tmp = (uint8_t)(SDSTATUS_Tab[11] & 0xFF);
  cardstatus->ERASE_SIZE = tmp << 8;

  /*!< Byte 12 */
  tmp = (uint8_t)(SDSTATUS_Tab[12] & 0xFF);
  cardstatus->ERASE_SIZE |= tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((SDSTATUS_Tab[13] & 0xFC) >> 2);
  cardstatus->ERASE_TIMEOUT = tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((SDSTATUS_Tab[13] & 0x3));
  cardstatus->ERASE_OFFSET = tmp;
 
  return(errorstatus);
}

/*
 * 函数名：SD_EnableWideBusOperation
 * 描述  ：配置卡的数据宽度(但得看卡是否支持)
 * 输入  ：-WideMode 指定SD卡的数据线宽
 *         具体可配置如下
 *         @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
 *         @arg SDIO_BusWide_4b: 4-bit data transfer
 *         @arg SDIO_BusWide_1b: 1-bit data transfer (默认)
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_EnableWideBusOperation(uint32_t WideMode)
{
  SD_Error errorstatus = SD_OK;

  /*!< MMC Card doesn't support this feature */
  if (SDIO_MULTIMEDIA_CARD == CardType)
  {
    errorstatus = SD_UNSUPPORTED_FEATURE;
    return(errorstatus);
  }
  else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    if (SDIO_BusWide_8b == WideMode)    //2.0 sd不支持8bits
    {
      errorstatus = SD_UNSUPPORTED_FEATURE;
      return(errorstatus);
    }
    else if (SDIO_BusWide_4b == WideMode)   //4数据线模式
    {
			/* 调用了SDEnWideBus函数使能使用宽数据线， 
				 然后传输SDIO_InitTypeDef类型变量并使用SDIO_Init函数完成使用4根数据线配置 */
      errorstatus = SDEnWideBus(ENABLE);    //使用acmd6设置总线宽度，设置卡的传输方式

      if (SD_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;   //这个是设置stm32的sdio的传输方式 ，切换模式必须从卡和sdio都对应好
        SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&SDIO_InitStructure);
      }
    }
    else   //单数据线模式
    {
      errorstatus = SDEnWideBus(DISABLE);

      if (SD_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
        SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&SDIO_InitStructure);
      }
    }
  }

  return(errorstatus);
}

/*
 * 函数名：SD_SelectDeselect
 * 描述  ：利用cmd7，选择卡相对地址为addr的卡，取消选择其它卡
 *   		如果addr = 0,则取消选择所有的卡
 * 输入  ：-addr 选择卡的地址
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_SelectDeselect(uint32_t addr)
{
  SD_Error errorstatus = SD_OK;

  /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
  SDIO_CmdInitStructure.SDIO_Argument =  addr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SEL_DESEL_CARD);

  return(errorstatus);
}

/**
* @brief  从sd卡读取一个BLOCK的数据（512字节）
* @note   本函数使用后需要调用如下两个函数来等待数据传输完成
*          - SD_ReadWaitOperation(): 确认DMA已从SDIO传输到数据到内存
*          - SD_GetStatus(): 确认SD卡传输完成
* @param  writebuff: 指向要接收数据的缓冲区
* @param  WriteAddr: 要把数据写入到sd卡的地址
* @param  BlockSize: 块大小，sdhc卡为512字节
* @retval SD_Error: 返回的sd错误代码
*/
SD_Error SD_ReadBlock(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;
#if defined (SD_POLLING_MODE) 
  uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
#endif

  TransferError = SD_OK;
  TransferEnd = 0;    //传输结束标置位，在中断服务置1
  StopCondition = 0;
  
	/* 将SDIO外设的数据控制寄存器 (SDIO_DCTRL)清零， 复位之前的传输设置。 */
  SDIO->DCTRL = 0x0;

  /* 对SD卡进行数据读写之前，都必须发送CMD16指定块的大小， 
		 对于标准卡，读取BlockSize长度字节的块；对于SDHC卡，固定读取512字节的块 */
  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

	/*******************add，没有这一段容易卡死在DMA检测中*************************************/
  /* Set Block Size for Card，cmd16,
	 * 若是sdsc卡，可以用来设置块大小，
	 * 若是sdhc卡，块大小为512字节，不受cmd16影响 
	 */
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

	/*********************************************************************************/
	
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /* 向SD卡发送单块读数据命令CMD17， SD卡在接收到命令后就会通过数据线把数据传输到SDIO的数据FIFO内 */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
	

#if defined (SD_POLLING_MODE)  
  /*!< In case of single block transfer, no need of stop transfer at all.*/
  /*!< Polling mode */
  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(tempbuff + count) = SDIO_ReadData();
      }
      tempbuff += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
  while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
  {
    *tempbuff = SDIO_ReadData();
    tempbuff++;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

#elif defined (SD_DMA_MODE)
	  /* 调用SDIO_ITConfig函数使能相关中断数据结束中断， 当数据传输完成时会进入SDIO的中断服务函数 */
    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);
    SDIO_DMACmd(ENABLE);
    SD_DMA_RxConfig((uint32_t *)readbuff, BlockSize);
#endif

  return(errorstatus);
}

/**
*  允许从卡片中的指定地址读取块。数据传输可以采用DMA模式或轮询模式进行管理。
* @注意此操作后需要有两个功能来修改DMA控制器和SD卡状态。
—SD_ReadWaitOperation():此函数确保DMA控制器已完成所有数据传输。
—SD_GetStatus():检查SD卡是否完成数据传输，是否可以进行数据读取。
* @param readbuff:指向将包含接收数据的缓冲区的指针。
* @param ReadAddr:读取数据的地址。
* @param BlockSize: SD卡数据块大小。块大小应该是512。
* @param NumberOfBlocks:要读取的块数量。
* @retval SD_Error: SD卡错误码。
  */
SD_Error SD_ReadMultiBlocks(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 1;
	
  SDIO->DCTRL = 0x0;    //复位数据控制寄存器

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)    //sdhc卡的地址以块为单位，每块512字节
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

  /*!< Set Block Size for Card，cmd16,若是sdsc卡，可以用来设置块大小，若是sdhc卡，块大小为512字节，不受cmd16影响 */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }
    
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;   //等待超时限制
  SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;  //对于块数据传输，数据长度寄存器中的数值必须是数据块长度(见SDIO_DCTRL)的倍数
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;   //直接用参数多好。。。SDIO_DataBlockSize_512b
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;  //传输方向
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;  //传输模式
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;//开启数据状态机
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /*!< Send CMD18 READ_MULT_BLOCK with argument data address */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;     //起始地址
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;  
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;    //r1 
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);   //开启数据传输结束中断 ，Data end (data counter, SDIDCOUNT, is zero) interrupt 
  SDIO_DMACmd(ENABLE);  //使能dma方式
  SD_DMA_RxConfig((uint32_t *)readbuff, (NumberOfBlocks * BlockSize));   //配置DMA接收

  return(errorstatus);
}

/**
* @brief  本函数会一直等待到DMA传输结束
*          SDIO_ReadMultiBlocks() 函数后
    必须被调用以确保DMA数据传输完成
* @param  None.
* @retval SD_Error: 返回的sd错误代码.
*/
SD_Error SD_WaitReadOperation(void)
{
  SD_Error errorstatus = SD_OK;

	//等待dma传输结束
	/* 利用SD_DMAEndOfTransferStatus函数及TransferEnd和TransferError全局变量确认是否传输完成， 
		 并检查传输是否正常结束，若不正常则直接返回错误代码 */
  while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
  {}

  if (TransferError != SD_OK)
  {
    return(TransferError);
  }

  return(errorstatus);
}

/**
* @brief  SD_WriteBlock函数用于向指定的目标地址写入一个块的数据
* @note   本函数使用后需要调用如下两个函数来等待数据传输完成
*          - SD_WaitWriteOperation(): 确认DMA已把数据传输到SDIO接口
*          - SD_GetStatus(): 确认SD卡内部已经把数据写入完毕
* @param  writebuff: 指向要写入的数据
* @param  WriteAddr: 要把数据写入到sd卡的地址
* @param  BlockSize: 块大小，sdhc卡为512字节
* @retval SD_Error: 返回的sd错误代码
*/
SD_Error SD_WriteBlock(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;

#if defined (SD_POLLING_MODE)
  uint32_t bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)writebuff;
#endif

  TransferError = SD_OK;
  TransferEnd = 0;      
  StopCondition = 0;
  
	/* 将SDIO 数据控制寄存器 (SDIO_DCTRL)清零， 复位之前的传输设置 */
  SDIO->DCTRL = 0x0;


  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }
  
	/* 对SD卡进行数据读写之前，都必须发送CMD16指定块的大小，对于标准卡，
		 要写入BlockSize长度字节的块；对于SDHC卡， 写入固定为512字节的块 */
  /*-------------- add , 没有这一段容易卡死在DMA检测中 -------------------*/
	/* Set Block Size for Card，cmd16,
	 * 若是sdsc卡，可以用来设置块大小，
	 * 若是sdhc卡，块大小为512字节，不受cmd16影响 
	 */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
	
	/********************************************************/

  /* 发送块写入命令CMD24通知SD卡要进行数据写入操作，并指定待写入数据的目标地址 */
	SDIO_CmdInitStructure.SDIO_Argument = WriteAddr;   //写入地址
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

	//配置sdio的写数据寄存器
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;  //可用此参数代替SDIO_DataBlockSize_512b
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;//写数据，
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable; //开启数据通道状态机
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /*!< In case of single data block transfer no need of stop command at all */
#if defined (SD_POLLING_MODE)   //普通模式
  while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
    {
      if ((512 - bytestransferred) < 32)
      {
        restwords = ((512 - bytestransferred) % 4 == 0) ? ((512 - bytestransferred) / 4) : (( 512 -  bytestransferred) / 4 + 1);
        for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
        {
          SDIO_WriteData(*tempbuff);
        }
      }
      else
      {
        for (count = 0; count < 8; count++)
        {
          SDIO_WriteData(*(tempbuff + count));
        }
        tempbuff += 8;
        bytestransferred += 32;
      }
    }
  }
  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
    errorstatus = SD_TX_UNDERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
#elif defined (SD_DMA_MODE)  //dma模式
	/* 调用SDIO_ITConfig函数使能SDIO数据结束传输结束中断，传输结束时， 会跳转到SDIO的中断服务函数运行 */
  SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);  //数据传输结束中断
  SD_DMA_TxConfig((uint32_t *)writebuff, BlockSize);   //配置dma，跟rx类似
  SDIO_DMACmd(ENABLE);   //	使能sdio的dma请求
#endif

  return(errorstatus);
}

  /*
 * 函数名：SD_WriteMultiBlocks
 * 描述  ：从输入的起始地址开始，向卡写入多个数据块，
 		  只能在DMA模式下使用这个函数
	注意：调用这个函数后一定要调用
			SD_WaitWriteOperation（）来等待DMA传输结束
			和	SD_GetStatus() 检测卡与SDIO的FIFO间是否已经完成传输
 * 输入  ： 
		  * @param  WriteAddr: Address from where data are to be read.
		  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
		  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
		  * @param  NumberOfBlocks: number of blocks to be written.
 * 输出  ：SD错误类型
 */
SD_Error SD_WriteMultiBlocks(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  __IO uint32_t count = 0;

  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 1;
  
  SDIO->DCTRL = 0x0;

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }
	
	/*******************add，没有这一段容易卡死在DMA检测中*************************************/
    /*!< Set Block Size for Card，cmd16,若是sdsc卡，可以用来设置块大小，若是sdhc卡，块大小为512字节，不受cmd16影响 */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);


  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }
	/*********************************************************************************/

  /*!< To improve performance */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) (RCA << 16);
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;  // cmd55
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);


  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
  /*!< To improve performance *///  pre-erased，在多块写入时可发送此命令进行预擦除
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)NumberOfBlocks;   //参数为将要写入的块数目
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;    //cmd23
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }


  /*!< Send CMD25 WRITE_MULT_BLOCK with argument data address */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)WriteAddr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);
  SDIO_DMACmd(ENABLE);    
  SD_DMA_TxConfig((uint32_t *)writebuff, (NumberOfBlocks * BlockSize));

  return(errorstatus);
}

/**
* @brief  本函数会一直等待到DMA传输结束
*         在 SDIO_WriteBlock() 和 SDIO_WriteMultiBlocks() 函数后必须被调用以确保DMA数据传输完成
* @param  None.
* @retval SD_Error: 返回的sd错误代码.
*/
SD_Error SD_WaitWriteOperation(void)
{
  SD_Error errorstatus = SD_OK;
/* 等待dma是否传输结束  调用库函数SD_DMAEndOfTransferStatus一直检测DMA的传输完成标志，
	 当DMA传输结束时，该函数会返回SET值 */
  while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
  {}

  if (TransferError != SD_OK)
  {
    return(TransferError);
  }

  /* 清除相关标志位并返回错误 */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/**
  * @brief  Gets the cuurent data transfer state.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetTransferState(void)
{
  if (SDIO->STA & (SDIO_FLAG_TXACT | SDIO_FLAG_RXACT))
  {
    return(SD_TRANSFER_BUSY);
  }
  else
  {
    return(SD_TRANSFER_OK);
  }
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_StopTransfer(void)
{
  SD_Error errorstatus = SD_OK;

  /*!< Send CMD12 STOP_TRANSMISSION  */
  SDIO->ARG = 0x0;
  SDIO->CMD = 0x44C;
  errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);

  return(errorstatus);
}

/**
* @brief  控制SD卡擦除指定的数据区域
* @param  startaddr: 擦除的开始地址
* @param  endaddr: 擦除的结束地址
* @retval SD_Error: SD返回的错误代码.
*/
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr)
{
  SD_Error errorstatus = SD_OK;
  uint32_t delay = 0;
  __IO uint32_t maxdelay = 0;
  uint8_t cardstate = 0;

  /* 检查card命令类是否支持erase命令 */
  if (((CSD_Tab[1] >> 20) & SD_CCCC_ERASE) == 0)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

  maxdelay = 120000 / ((SDIO->CLKCR & 0xFF) + 2);  //延时，根据时钟分频设置来计算

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)  //卡已上锁
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)  //sdhc卡，为什么要 /512？详见2.0协议page52
  {//在sdhc卡，地址参数为块地址，每块512字节，sdsc卡地址为字节地址
		//所以若是sdhc卡要对地址/512进行转换
    startaddr /= 512;
    endaddr /= 512;
  }
  
	/* 为保证擦除指令正常进行， 要求主机一个遵循下面的命令序列发送指令：CMD32->CMD33->CMD38。
		 如果发送顺序不对，SD卡会设置ERASE_SEQ_ERROR位到状态寄存器 */
  /*!<  ERASE_GROUP_START (CMD32)设置擦除的起始地址， erase_group_end(CMD33)设置擦除的结束地址， */
  if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) 
			|| (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) 
			|| (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    /* 发送命令 CMD32 SD_ERASE_GRP_START ，带参数startaddr  */
    SDIO_CmdInitStructure.SDIO_Argument = startaddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_START;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //R1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_START);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }

    /* 发送命令 CMD33 SD_ERASE_GRP_END ，带参数endaddr  */
    SDIO_CmdInitStructure.SDIO_Argument = endaddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_END;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_END);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
  }

  /* 发送 CMD38 ERASE 命令，开始擦除 */
  SDIO_CmdInitStructure.SDIO_Argument = 0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ERASE;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_ERASE);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

	/*  擦除操作需要花费一定时间，这段时间不能对SD卡进行其他操作 */
  for (delay = 0; delay < maxdelay; delay++)
  {}

  /* 等待SD卡的内部时序操作完成 */
	/* 通过IsCardProgramming函数可以检测SD卡是否处于编程状态(即卡内部的擦写状态)，
			需要确保SD卡擦除完成才退出SD_Erase函数 */
  errorstatus = IsCardProgramming(&cardstate);

  while ((errorstatus == SD_OK) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return(errorstatus);
}

/**
  * @brief  Returns the current card's status.
  * @param  pcardstatus: 指向包含SD卡状态(卡状态寄存器)的缓冲区的指针。
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
  SD_Error errorstatus = SD_OK;

  SDIO->ARG = (uint32_t) RCA << 16;
  SDIO->CMD = 0x44D;
  
  errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  *pcardstatus = SDIO->RESP1;
  return(errorstatus);
}

/**
  * @brief  Returns the current SD card's status.
  * @param  psdstatus: pointer to the buffer that will contain the SD card status 
  *         (SD Status register).
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendSDStatus(uint32_t *psdstatus)
{
  SD_Error errorstatus = SD_OK;
  uint32_t count = 0;

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  /*!< 如果卡的块大小不等于卡的当前块大小，则设置卡的块大小. */
  SDIO_CmdInitStructure.SDIO_Argument = 64;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /*!< CMD55 */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 64;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_64b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /*!< Send ACMD13 SD_APP_STAUS  with argument as card's RCA.*/
  SDIO_CmdInitStructure.SDIO_Argument = 0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_STAUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_SD_APP_STAUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(psdstatus + count) = SDIO_ReadData();
      }
      psdstatus += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
  {
    *psdstatus = SDIO_ReadData();
    psdstatus++;
  }

  /*!< Clear all the static status flags*/
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/*
 * 函数名：SD_ProcessIRQSrc
 * 描述  ：数据传输结束中断
 * 输入  ：无		 
 * 输出  ：SD错误类型
 */
SD_Error SD_ProcessIRQSrc(void)
{
	/* 判断全局变量StopCondition变量是否为1， 该全局变量在SDIO的多块读写函数中被置1 */
  if (StopCondition == 1)
  {
    SDIO->ARG = 0x0;   //命令参数寄存器
    SDIO->CMD = 0x44C;  // 命令寄存器： 0100 	01 	 	001100
						//						[7:6]  	[5:0]
						//				CPSMEN  WAITRESP CMDINDEX
						//		开启命令状态机	短响应   多块读写命令由CMD12结束
    TransferError = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
  }
	/* 根据传输情况设置全局变量TransferError和TransferEnd */
  else
  {
    TransferError = SD_OK;
  }
  SDIO_ClearITPendingBit(SDIO_IT_DATAEND);  //清中断
  SDIO_ITConfig(SDIO_IT_DATAEND, DISABLE);  //关闭sdio中断使能
  TransferEnd = 1;
  return(TransferError);
}

 /*
 * 函数名：CmdError
 * 描述  ：对CMD0命令的检查。
 * 输入  ：无
 * 输出  ：SD错误类型
 */
static SD_Error CmdError(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t timeout;

  timeout = SDIO_CMD0TIMEOUT; /*!< 10000 */

	/*检查命令是否已发送*/
  while ((timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
  {
    timeout--;
  }

  if (timeout == 0)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清除静态标志位

  return(errorstatus);
}

 /*
 * 函数名：CmdResp7Error
 * 描述  ：对响应类型为R7的命令进行检查
 * 输入  ：无
 * 输出  ：SD错误类型
 */
static SD_Error CmdResp7Error(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t status;
  uint32_t timeout = SDIO_CMD0TIMEOUT;

  status = SDIO->STA;  //读取SDIO状态寄存器 ，此状态寄存器是stm32的寄存器

	/* Command response received (CRC check failed) ：Command response received (CRC check passed)：Command response timeout */
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
  {
    timeout--;
    status = SDIO->STA;
  }

	//卡不响应cmd8
  if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
  {
    /*!< 卡不是V2.0兼容或卡不支持设置的电压范围 */
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }

  if (status & SDIO_FLAG_CMDREND)
  {
    /*!< Card is SD V2.0 compliant */
    errorstatus = SD_OK;
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return(errorstatus);
  }
  return(errorstatus);
}

 /*
 * 函数名：CmdResp1Error
 * 描述  ：对响应类型为R1的命令进行检查
 * 输入  ：无
 * 输出  ：SD错误类型
 */
static SD_Error CmdResp1Error(uint8_t cmd)//传入的参数有什么用？
{
	/*不是这些状态就等待	*/
  while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;//清中断标志

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);  //判断是否在供电范围
}

 /*
 * 函数名：CmdResp3Error
 * 描述  ：对响应类型为R3的命令进行检查
 * 输入  ：无
 * 输出  ：SD错误类型
 */
static SD_Error CmdResp3Error(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return(errorstatus);
}

 /*
 * 函数名：CmdResp2Error
 * 描述  ：对响应类型为R2的命令进行检查
 * 输入  ：无
 * 输出  ：SD错误类型
 */
static SD_Error CmdResp2Error(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

 /*
 * 函数名：CmdResp6Error
 * 描述  ：对响应类型为R6的命令进行检查
 * 输入  ：cmd 命令索引号，
 			prca 用来存储接收到的卡相对地址
 * 输出  ：SD错误类型
 */
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca)
{
  SD_Error errorstatus = SD_OK;
  uint32_t status;
  uint32_t response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)   //检测是否接收到正常命令
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /*!< We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

	/*以下状态全为0表明成功接收到card返回的rca */
  if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
  {
    *prca = (uint16_t) (response_r1 >> 16);  //右移16位，就是接收到的返回rca
    return(errorstatus);
  }

  if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  return(errorstatus);
}

  /*
 * 函数名：SDEnWideBus
 * 描述  ：使能或关闭SDIO的4bit模式
 * 输入  ：新状态	ENABLE 或DISABLE
 * 输出  ：SD错误类型
 */
static SD_Error SDEnWideBus(FunctionalState NewState)
{
  SD_Error errorstatus = SD_OK;

  uint32_t scr[2] = {0, 0};

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)  //检测卡是否已上锁
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;   
    return(errorstatus);
  }

  /*!< Get SCR Register */
  errorstatus = FindSCR(RCA, scr);   //获取scr寄存器内容到scr数组中

  if (errorstatus != SD_OK)   //degug,crc错误，scr读取不了数值
  {
    return(errorstatus);
  }

  /*!< If wide bus operation to be enabled */
  if (NewState == ENABLE)
  {
    /*!< If requested card supports wide bus operation */
    if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)  //判断卡是否支持4位方式
    {
      /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
      SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
			/*开启4bit模式的命令acmd6*/
      SDIO_CmdInitStructure.SDIO_Argument = 0x2;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }   /*!< If wide bus operation to be disabled */
  else
  {
    /*!< If requested card supports 1 bit mode operation */
    if ((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
    {
      /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
      SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);


      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }
}

  /*
 * 函数名：IsCardProgramming
 * 描述  ：检测SD卡是不是正在进行内部读写操作
 * 输入  ：用来装载SD state状态的指针
 * 输出  ：SD错误类型
 */
static SD_Error IsCardProgramming(uint8_t *pstatus)
{
  SD_Error errorstatus = SD_OK;
  __IO uint32_t respR1 = 0, status = 0;

	/*通过发送CMD13命令SD卡发送它的状态寄存器内容，并对响应内容进行分析得出当前SD卡的状态以及可能发送的错误，
		存储到m3的位置为sdio_sta寄存器*/
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;  //卡相对地址参数
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

	/*一系列的状态判断*/
  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  status = (uint32_t)SDIO_GetCommandResponse();

  /*!< Check response received is of desired command */
  if (status != SD_CMD_SEND_STATUS)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);


  /*!< We have received response, retrieve it for analysis  */
  respR1 = SDIO_GetResponse(SDIO_RESP1);

  /*!< Find out card status */
  *pstatus = (uint8_t) ((respR1 >> 9) & 0x0000000F);   //status[12:9] :cardstate 

  if ((respR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return(errorstatus);
  }

  if (respR1 & SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }

  if (respR1 & SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }

  if (respR1 & SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }

  if (respR1 & SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }

  if (respR1 & SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }

  if (respR1 & SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }

  if (respR1 & SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }

  if (respR1 & SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  if (respR1 & SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (respR1 & SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }

  if (respR1 & SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }

  if (respR1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (respR1 & SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }

  if (respR1 & SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }

  if (respR1 & SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }

  if (respR1 & SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }

  if (respR1 & SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }

  if (respR1 & SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }

  if (respR1 & SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }

  return(errorstatus);
}

  /*
 * 函数名：FindSCR
 * 描述  ：读取SD卡的SCR寄存器的内容
 * 输入  ：RCA卡相对地址
 		   pscr	用来装载SCR内容的指针
 * 输出  ：SD错误类型
 */
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr)
{
  uint32_t index = 0;
  SD_Error errorstatus = SD_OK;
  uint32_t tempscr[2] = {0, 0};

  /*!< Set Block Size To 8 Bytes */
  /*!< Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;    //块大小如果是sdhc卡是无法改变块大小的	//原参数8
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;   //	 cmd16
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /*!< Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
	/*设置数据接收寄存器*/
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 8;  //8byte,64位
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;   //块大小8byte 
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);


  /*!< Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
	
	/*等待接收数据 */
  /*不是这些情况就循环*/																						  
 /*上溢出错	  //数据crc失败		//数据超时	  //已接收数据块，crc检测成功	//没有在所有数据线上检测到起始信号*/

  while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) //接收到的数据是否可用
    {
      *(tempscr + index) = SDIO_ReadData();
      index++;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);

  *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);

  return(errorstatus);
}

/**
  * @brief  转换以2为幂的字节数并返回该幂。.
  * @param  NumberOfBytes: number of bytes.
  * @retval None
  */
uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
  uint8_t count = 0;

  while (NumberOfBytes != 1)
  {
    NumberOfBytes >>= 1;
    count++;
  }
  return(count);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
