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
#include "bsp_spi_flash.h"
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 预留SD卡使用*/
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
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
		break;

	case SPI_FLASH :
		/* SPI Flash状态检测：读取SPI FLASH 设备ID */
		if(sFLASH_ID == SPI_FLASH_ReadID())
		{
			/* 设备ID读取正确 */
			status &= ~STA_NOINIT;
		}
		else
		{
			/* 设备ID读取错误 */
			status = STA_NOINIT;
		}
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
	uint16_t i;
	DSTATUS status = STA_NOINIT;
	switch(pdrv){
		case ATA:
			break;
		
		case SPI_FLASH:
			/* 初始化SPI FLASH */
			SPI_FLASH_Init();
			/* 延时一段时间 */
			i = 500;
			while(--i);
			/* 唤醒SPI FLASH */
			SPI_Flash_WAKEUP();
			/* 获取SPI FLASH芯片状态 */
			status = disk_status(SPI_FLASH);
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
		break;

	case SPI_FLASH :
		// 扇区偏移2MB，外部FLASH文件系统空间放在SPI FLASH后面6MB
		sector += 512;
		SPI_FLASH_BufferRead(buff, sector <<12 ,count <<12);
		status =RES_OK;
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
	uint32_t write_addr;
	DRESULT status = RES_PARERR;
	if(!count){
		return RES_PARERR;
	}

	switch (pdrv) {
	case ATA :
		break;

	case SPI_FLASH :
		// 扇区偏移2MB，外部FLASH文件系统空间放在SPI FLASH后面6MB
		sector += 512;
		write_addr = sector<<12;
		SPI_FLASH_SectorErase(write_addr);
		SPI_FLASH_BufferWrite((uint8_t*)buff,write_addr,count<<12);
		status = RES_OK;
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
		break;
		

	case SPI_FLASH :
		switch (cmd){
			/* 扇区数量：1536*4096/1024/1024 = 6（MB） */
			case GET_SECTOR_COUNT:
				*(DWORD * )buff = 1536;
			break;
			/* 扇区大小  */
			case GET_SECTOR_SIZE:
				*(WORD *)buff = 4096;
			break;
			/* 同时擦除扇区个数 */
			case GET_BLOCK_SIZE:
				*(DWORD * )buff = 1;
			break;
		}
		status = RES_OK;
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


