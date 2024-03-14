/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "string.h"
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

#ifndef FATFS_FLASH_SPI
	#define FATFS_FLASH_SPI				1
#endif

#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif

#ifndef FATFS_USE_SDIO
	#define FATFS_USE_SDIO			0
#endif

/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "bsp_sdio_sdcard.h"
	extern SD_CardInfo SDCardInfo;
#endif

#if FATFS_FLASH_SPI == 1
	#include "bsp_spi_flash.h"
#endif

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 预留SD卡使用*/
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define SPI_FLASH 1   //外部SPI FLASH
#define SD_BLOCK_SIZE   512

__align(4) uint8_t align_buffer[SD_BLOCK_SIZE];


/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* 物理编号 */
)
{
	DSTATUS status = STA_NOINIT;
	FLASH_DEBUG_FUNC();
	switch (pdrv) {
	case ATA :/* SD卡 */
		#if FATFS_USE_SDIO == 1
				FLASH_DEBUG("SD Init");
				status = SD_Init();
	
				if(status != SD_OK)
				{
					status = STA_NOINIT;
				}
				else
				{
					status = RES_OK;
				}
			#endif	
		break;

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
				FLASH_DEBUG("FLASH Init");
				status = SPI_FLASH_Init();
		#endif
		break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* 设备初始化                                                            */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* 物理编号 */
)
{
	DSTATUS status = STA_NOINIT;
	switch(pdrv){
		case ATA:
			#if FATFS_USE_SDIO == 1
					status = RES_OK;
			#endif
			break;
		
		case SPI_FLASH:
			#if FATFS_FLASH_SPI == 1
					status = TM_FATFS_FLASH_SPI_disk_status();
			#endif
			break;
		
		default:
			status = STA_NOINIT;
		
	}
		
	
	return status;
}



/*-----------------------------------------------------------------------*/
/* 读扇区：读扇区内容到指定存储区                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* 设备物理地址 */
	BYTE *buff,		/* 数据缓存区 */
	DWORD sector,	/* 扇区首地址 */
	UINT count		/* 扇区个数（1..128） */
)
{
	DRESULT status = RES_PARERR;
	
	switch (pdrv) {
	case ATA :
		#if FATFS_USE_SDIO == 1
	//若传入的buff地址不是4字节对齐，需要额外处理
				if((uint32_t)buff%4 != 0)
				{
					uint8_t i;
					for(i=0;i<count;i++)
					{
						SD_ReadBlock(align_buffer,sector*SD_BLOCK_SIZE+SD_BLOCK_SIZE*i,SD_BLOCK_SIZE);
						/* 循环查询dma传输是否结束 */
						SD_WaitReadOperation();
						
						/* 等待dma传输结束 */
						while(SD_GetStatus() != SD_TRANSFER_OK);
						
						memcpy(buff,align_buffer,SD_BLOCK_SIZE);
						buff += SD_BLOCK_SIZE;
					}
				}
				/* 传入的buff数据地址四字节对齐，直接读取 */
				else
				{
					if(count>1)
					{
						SD_ReadMultBlocks(buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE,count);
						
						/* 循环查询dma传输是否结束 */
						SD_WaitReadOperation();
						while(SD_GetStatus() != SD_TRANSFER_OK);
					}
					else
					{
						SD_ReadBlock(buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE);
						
						/* 循环查询dma传输是否结束 */
						SD_WaitReadOperation();
						while(SD_GetStatus() != SD_TRANSFER_OK);
					}
				}
				static = RES_OK;
			#endif
		break;

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
				status = TM_FATFS_FLASH_SPI_disk_read(buff,sector,count);
		#endif
		break;
	
	default:
		status = RES_PARERR;
	}

	return status;
}



/*-----------------------------------------------------------------------*/
/* 写扇区：将数据写入指定扇区空间上                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	
	DRESULT status = RES_PARERR;
	if(!count){
		return RES_PARERR;
	}

	switch (pdrv) {
	case ATA :
		#if FATFS_USE_SDIO == 1
	//若传入的buff地址不是4字节对齐，需要额外处理
				if((uint32_t)buff%4 != 0)
				{
					uint8_t i;
					for(i=0;i<count;i++)
					{
						memcpy(align_buffer,buff,SD_BLOCK_SIZE);
						SD_WriteBlock(align_buffer,sector*SD_BLOCK_SIZE+SD_BLOCK_SIZE*i,SD_BLOCK_SIZE);
						/* 循环查询dma传输是否结束 */
						SD_WaitWriteOperation();
						while(SD_GetStatus() != SD_TRANSFER_OK);
						
						memcpy(buff,align_buffer,SD_BLOCK_SIZE);
						buff += SD_BLOCK_SIZE;
					}
				}
				/* 传入的buff数据地址四字节对齐，直接读取 */
				else
				{
					if(count>1)
					{
						SD_WriteMultBlocks((uint8_t*)buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE,count);
						
						/* 循环查询dma传输是否结束 */
						SD_WaitWriteOperation();
						while(SD_GetStatus() != SD_TRANSFER_OK);
					}
					else
					{
						SD_WriteBlock((uint8_t *)buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE);
						
						/* 循环查询dma传输是否结束 */
						SD_WaitWriteOperation();
						while(SD_GetStatus() != SD_TRANSFER_OK);
					}
				}
				static = RES_OK;
			#endif
		break;

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
				status = TM_FATFS_FLASH_SPI_disk_write((BYTE *)buff,sector,count);
		#endif
		break;
	
	default:
		status = RES_PARERR;
	}

	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* 其他控制                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;

	switch (pdrv) {
		case ATA :
		#if FATFS_USE_SDIO == 1
		switch (cmd)
		{
			// Get R/W sector size (WORD) 
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
			break;
			// Get erase block size in unit of sector (DWOR
			case GET_BLOCK_SIZE:
				*(DWORD*)buff = SDCardInfo.CardBlockSize;
			break;
			
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
				break;
			case CTRL_SYNC:
				break;
			
		}
		status = RES_OK;
		#endif
		break;
		

	case SPI_FLASH :
		#if FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_ioctl(cmd,buff);
		#endif
		break;
		
		default:
			status = RES_PARERR;
	}

	return status;
}
#endif

__weak DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

