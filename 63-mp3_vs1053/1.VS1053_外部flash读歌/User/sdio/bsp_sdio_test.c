#include "bsp_sdio_test.h"
#include "bsp_led.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      10  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;


/* Private function prototypes -----------------------------------------------*/
void SD_EraseTest(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void SD_Test(void)
{
	LED_BLUE;
  
  /*------------------------------ SD Init ---------------------------------- */
	/* SD卡使用SDIO中断及DMA中断接收数据，中断服务程序位于bsp_sdio_sd.c文件尾*/
  if((Status = SD_Init()) != SD_OK)
  {
    LED_RED;
		printf("SD 卡初始化失败，请确保SD卡已正确接入开发板,或换一张SD卡测试！\n");
  }
	else
	{
		printf("SD卡初始化成功！\n");
	}
	if(Status == SD_OK)
	{
		LED_BLUE;
		
		/* 擦除测试 */
		SD_EraseTest();
		
		LED_BLUE;
		/* single block读写测试 */
		SD_SingleBlockTest();
		
		//暂时不支持直接多块读写，多块读写可用多个单块读写流程替代
		LED_BLUE;
		/* multi block读写测试 */
		SD_MultiBlockTest();
	}
        
  
}


/**
  * @brief  Tests the SD card erase operation.
  * @param  None
  * @retval None
  */
void SD_EraseTest(void)
{
  /*------------------- Block Erase ------------------------------------------*/
  if (Status == SD_OK)
  {
    /* 擦除 NumberOfBlocks 个块每个块长度为512字节 */
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));
  }

  if (Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);

    /* 等待传输完成 */
    Status = SD_WaitReadOperation();

    /* 检查传输是否正常 */
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* 校验数据 */
  if (Status == SD_OK)
  {
		/* 调用eBuffercmp函数判断擦除结果，它有两个形参，分别为数据指针和数据字节长度， 
			 它实际上是把数据存储器内所有数据都与0xff或0x00做比较，只有出现这两个数之外就报错退出 */
    EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(EraseStatus == PASSED)
  {
    LED_GREEN;
		printf("SD卡擦除测试成功！\n");
  }
  else
  {
    LED_BLUE;
		printf("SD卡擦除测试失败！ \n");
		printf("温馨提示：部分SD卡不支持擦除测试，若SD卡能通过下面的single读写测试，即表示SD卡能够正常使用\n");
  }
}

/**
  * @brief  Tests the SD card Single Blocks operations.
  * @param  None
  * @retval None
  */
void SD_SingleBlockTest(void)
{
  /*------------------- 块 读写 --------------------------*/
    /* 向数组填充要写入的数据*/
  Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);

  if (Status == SD_OK)
  {
    /* 把512个字节写入到SD卡的0地址 */
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
    /* 把512个字节写入到SD卡的0地址 */
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
    /* 检查传输 */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

   /* 校验读出的数据是否与写入的数据一致 */
  if (Status == SD_OK)
  {
		/* Buffercmp函数用于比较两个存储区内容是否完全相等，它有三个形参，分别为第一个存储区指针、
			 第二个存储区指针和存储器长度，该函数只是循环比较两个存储区对应位置的两个数据是否相等，
			 只有发现存在不相等就报错退出 */
    TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE);
  }
  
  if(TransferStatus1 == PASSED)
  {
    LED_GREEN;
		printf("Single block测试成功！\n");
  }
  else
  {
    LED_RED;
		printf("Single block测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试！\n");
  }
}

/**
  * @brief  Tests the SD card Multiple Blocks operations.
  * @param  None
  * @retval None
  */
void SD_MultiBlockTest(void)
{
  /*--------------- Multiple Block Read/Write ---------------------*/
  /* Fill the buffer to send */
  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

  if (Status == SD_OK)
  {
    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* Check the correctness of written data */
  if (Status == SD_OK)
  {
    TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(TransferStatus2 == PASSED)
  {
    LED_GREEN;
		printf("Multi block测试成功！");
  }
  else
  {
    LED_RED;
		printf("Multi block测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试!");
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

/* 简单实用for循环赋值方法给存储区填充数据，它有三个形参， 分别为存储区指针、填充字节数和起始数选择 */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = index + Offset;
  }
}

/**
* @brief检查一个缓冲区是否所有的值都等于0。
* @param pBuffer:要比较的缓冲区。
* @param BufferLength:缓冲区的长度
@retval PASSED: pBuffer值为零
* FAILED: pBuffer buffer中至少有一个值不为零。
  */
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
    if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))
    {
      return FAILED;
    }

    pBuffer++;
  }

  return PASSED;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
