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
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 Ԥ��SD��ʹ��*/
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define SPI_FLASH 1   //�ⲿSPI FLASH


/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	DSTATUS status = STA_NOINIT;

	switch (pdrv) {
	case ATA :/* SD�� */
		break;

	case SPI_FLASH :
		/* SPI Flash״̬��⣺��ȡSPI FLASH �豸ID */
		if(sFLASH_ID == SPI_FLASH_ReadID())
		{
			/* �豸ID��ȡ��ȷ */
			status &= ~STA_NOINIT;
		}
		else
		{
			/* �豸ID��ȡ���� */
			status = STA_NOINIT;
		}
		break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
)
{
	uint16_t i;
	DSTATUS status = STA_NOINIT;
	switch(pdrv){
		case ATA:
			break;
		
		case SPI_FLASH:
			/* ��ʼ��SPI FLASH */
			SPI_FLASH_Init();
			/* ��ʱһ��ʱ�� */
			i = 500;
			while(--i);
			/* ����SPI FLASH */
			SPI_Flash_WAKEUP();
			/* ��ȡSPI FLASHоƬ״̬ */
			status = disk_status(SPI_FLASH);
			break;
		
		default:
			status = STA_NOINIT;
		
	}
		
	
	return status;
}



/*-----------------------------------------------------------------------*/
/* �����������������ݵ�ָ���洢��                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* �豸�����ַ */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	UINT count		/* ����������1..128�� */
)
{
	DRESULT status = RES_PARERR;
	
	switch (pdrv) {
	case ATA :
		break;

	case SPI_FLASH :
		// ����ƫ��2MB���ⲿFLASH�ļ�ϵͳ�ռ����SPI FLASH����6MB
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
/* д������������д��ָ�������ռ���                                                       */
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
		// ����ƫ��2MB���ⲿFLASH�ļ�ϵͳ�ռ����SPI FLASH����6MB
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
/* ��������                                                              */
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
			/* ����������1536*4096/1024/1024 = 6��MB�� */
			case GET_SECTOR_COUNT:
				*(DWORD * )buff = 1536;
			break;
			/* ������С  */
			case GET_SECTOR_SIZE:
				*(WORD *)buff = 4096;
			break;
			/* ͬʱ������������ */
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
	/* ���ص�ǰʱ��� */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}


