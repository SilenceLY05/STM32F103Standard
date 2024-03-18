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
/* ��ȡ�豸״̬                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
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
/* �����������������ݵ�ָ���洢��                                         */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* �豸�����ַ */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	BYTE count		/* ����������1..128�� */
)
{
	if(count>1)
	{
		SD_ReadMultiBlocks(buff,sector*BLOCK_SIZE,BLOCK_SIZE,count);
		
		/* ѭ����ѯdma�����Ƿ���� */
		SD_WaitReadOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//�ȴ�sdio��sd���������
	}
	else
	{
		SD_ReadBlock(buff,sector*BLOCK_SIZE,BLOCK_SIZE);
		/* ѭ����ѯdma�����Ƿ���� */
		SD_WaitReadOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//�ȴ�sdio��sd���������
		
		
	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* д������������д��ָ�������ռ���                                                       */
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
		
		/* ѭ����ѯdma�����Ƿ���� */
		SD_WaitWriteOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//�ȴ�sdio��sd���������
	}
	else
	{
		SD_WriteBlock((uint8_t *)buff,sector*BLOCK_SIZE,BLOCK_SIZE);
		/* ѭ����ѯdma�����Ƿ���� */
		SD_WaitWriteOperation();
		
		while(SD_GetStatus() != SD_TRANSFER_OK);//�ȴ�sdio��sd���������
		
		
	}
	return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* ��������                                                              */
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
	/* ���ص�ǰʱ��� */
	return	  0;
}


