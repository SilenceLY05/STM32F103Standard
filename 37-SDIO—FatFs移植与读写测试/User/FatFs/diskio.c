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
#include "bsp_sdio_sdcard.h"
#include "stm32f10x.h"
#include <string.h>
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 Ԥ��SD��ʹ��*/
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define SPI_FLASH 1   //�ⲿSPI FLASH
#define SD_BLOCKSIZE   512

extern    SD_CardInfo    SDCardInfo;


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
		status &= ~STA_NOINIT;
		break;

	case SPI_FLASH :
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
	DSTATUS status = STA_NOINIT;
	switch(pdrv){
		case ATA:
			if(SD_Init() == SD_OK)
			{
				status &= ~STA_NOINIT;
			}
			else
			{
				status = STA_NOINIT;
			}
			break;
		
		case SPI_FLASH:
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
	SD_Error SD_state = SD_OK;
	
	switch (pdrv) {
	case ATA :
		if((DWORD)buff&3)
		{//��ַ������
			DRESULT res = RES_OK;
			DWORD scratch[SD_BLOCKSIZE / 4];
			
			while(count--)
			{
				res = disk_read(ATA,(void*)scratch,sector++,1);
					if(res != RES_OK)
					{
						break;
					}
					memcpy(buff,scratch,SD_BLOCKSIZE);//�ڴ濽������
					buff += SD_BLOCKSIZE;
			}
			return res;
		}
		
		SD_state = SD_ReadMultiBlocks(buff,(uint64_t)sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
		if(SD_state == SD_OK)
		{
			/* ��鴫���Ƿ���� */
			SD_state = SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		if(SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case SPI_FLASH :
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
	
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	if(!count){
		return RES_PARERR;
	}

	switch (pdrv) {
	case ATA :
		if((DWORD)buff&3)
		{
			DRESULT res = RES_OK;
			DWORD scratch[SD_BLOCKSIZE / 4];
			
			while(count--)
			{
				memcpy(scratch,buff,SD_BLOCKSIZE);
				res = disk_read(ATA,(void*)scratch,sector++,1);
					if(res != RES_OK)
					{
						break;
					}
					buff += SD_BLOCKSIZE;
			}
			return res;
		}
		
		SD_state = SD_WriteMultiBlocks((uint8_t *)buff,(uint64_t)sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
		if(SD_state == SD_OK)
		{
			/* ��鴫���Ƿ���� */
			SD_state = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		if(SD_state != SD_OK)
			status = RES_PARERR;
		else
			status = RES_OK;
		break;

	case SPI_FLASH :
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
		switch (cmd)
		{
			// Get R/W sector size (WORD) 
			case GET_SECTOR_SIZE:
				*(WORD*)buff = SD_BLOCKSIZE;
			break;
			// Get erase block size in unit of sector (DWOR
			case GET_BLOCK_SIZE:
				*(DWORD*)buff = 1;
			break;
			
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
				break;
			case CTRL_SYNC:
				break;
			
		}
		status = RES_OK;
		break;
		

	case SPI_FLASH :

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


