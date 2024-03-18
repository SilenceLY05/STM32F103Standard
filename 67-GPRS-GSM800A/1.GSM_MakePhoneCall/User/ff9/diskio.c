/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"

#define BLOCK_SIZE            512 /* Block Size in Bytes */


/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* 物理编号 */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* 设备初始化                                                            */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* 物理编号 */
)
{
	SD_Error Status;
	
	if(pdrv)
	{
		return STA_NOINIT;
	}
	
	Status = SD_Init();
	if(Status != SD_OK)
	{
		return STA_NOINIT;
	}
	else
	{
		return RES_OK;
	}
}



/*-----------------------------------------------------------------------*/
/* 读扇区：读扇区内容到指定存储区                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* 设备物理地址 */
	BYTE *buff,		/* 数据缓存区 */
	DWORD sector,	/* 扇区首地址 */
	BYTE count		/* 扇区个数（1..128） */
)
{
	if(count>1)
	{
		SD_ReadMultiBlocks(buff,sector*BLOCK_SIZE,BLOCK_SIZE,count);
		
		/* 循环查询dma传输是否结束 */
		SD_WaitReadOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//等待sdio到sd卡传输结束
	}
	else
	{
		SD_ReadBlock(buff,sector*BLOCK_SIZE,BLOCK_SIZE);
		/* 循环查询dma传输是否结束 */
		SD_WaitReadOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//等待sdio到sd卡传输结束
		
		
	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* 写扇区：将数据写入指定扇区空间上                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	BYTE count			/* Number of sectors to write */
)
{
	
	if(count>1)
	{
		SD_WriteMultiBlocks((uint8_t *)buff,sector*BLOCK_SIZE,BLOCK_SIZE,count);
		
		/* 循环查询dma传输是否结束 */
		SD_WaitWriteOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//等待sdio到sd卡传输结束
	}
	else
	{
		SD_WriteBlock((uint8_t *)buff,sector*BLOCK_SIZE,BLOCK_SIZE);
		/* 循环查询dma传输是否结束 */
		SD_WaitWriteOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//等待sdio到sd卡传输结束
		
		
	}
	return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* 其他控制                                                              */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	return RES_OK;
}


__weak DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  0;
}


