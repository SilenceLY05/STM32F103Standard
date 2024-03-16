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

#ifndef FATFS_FLASH_SPI
	#define FATFS_FLASH_SPI				1
#endif

#ifndef FATFS_USE_SDIO
	#define FATFS_USE_SDIO			1
#endif

/* Set in defines.h file if you want it */
#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif


/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "bsp_sdio_sdcard.h"
#endif

#if FATFS_FLASH_SPI == 1
	#include "bsp_spi_flash.h"
#endif

//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */


/* Definitions of physical drive number for each drive */
#define ATA		    0	/* Example: Map ATA harddisk to physical drive 0 预留SD卡使用*/
#define SPI_FLASH 1   //外部SPI FLASH





/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* 物理编号 */
)
{
	DSTATUS status = STA_NOINIT;
	switch (pdrv) {
	case ATA :/* SD卡 */
		#if FATFS_USE_SDIO == 1
			status = TM_FATFS_SD_SDIO_disk_initialize();
		#endif
		break;

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
			status = TM_FATFS_FLASH_SPI_disk_initialize();
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
				status = TM_FATFS_SD_SDIO_disk_status();
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
	SD_Error SD_state = SD_OK;
	switch (pdrv) {
	case ATA :
		#if FATFS_USE_SDIO == 1
			status = TM_FATFS_SD_SDIO_disk_read(buff,sector,count);
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
			status = TM_FATFS_SD_SDIO_disk_write((BYTE*)buff,sector,count);
		#endif
		break;

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
			status = TM_FATFS_FLASH_SPI_disk_write((BYTE*)buff,sector,count);
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
			status = TM_FATFS_SD_SDIO_disk_ioctl(cmd,buff);
		#endif
		break;
		

	case SPI_FLASH :
		#if FATFS_FLASH_SPI == 1
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

