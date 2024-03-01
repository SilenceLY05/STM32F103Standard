#include "bsp_sdfs_app.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#include <stdio.h>


FATFS myfs[2];
FIL myfsrc,myfdst;
FRESULT myres;
BYTE mybuffer[512];
BYTE my_latest_buffer[512];
uint8_t mystring[512] = "this is a MicrsSD demo base on fatfs";
UINT mybr,mybw;
int mya = 0;
char mypath[512] = "0:";

/* Private functions ---------------------------------------------------------*/

void Sd_fs_init(void)
{
	/* SD���жϳ�ʼ�� */
	SDIO_NVIC_Configuration();
	
	/* SD��Ӳ����ʼ������ʼ���̷�Ϊ0 */
	f_mount(0,&myfs[0]);
}

void Sd_fs_test(void)
{
	int count = 0;
	SDIO_NVIC_Configuration();
	disk_initialize(0);
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,"0:/my_Demo.TXT",FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,mystring,sizeof(mystring),&mybr);
		f_close(&myfsrc);
	}
	else if(myres == FR_EXIST)//����ļ��Ѿ�����
	{
		
	}
	
	myres = f_open(&myfsrc,"0:/my_Demo.TXT",FA_OPEN_EXISTING|FA_READ);
	mybr =1;
	mya = 0;
	for(;;)
	{
		for(mya = 0;mya<512;mya++)
		mybuffer[mya] =0;
		myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);
		sprintf((char*)&my_latest_buffer[count*512],"%s",mybuffer);
		count++;
		if(myres||mybr ==0) break;
	}
	f_close(&myfsrc);
}


/* Sdfs_new ��һ���ļ���д������ */
int Sdfs_new(BYTE *new_file_name,BYTE *write_buffer,BYTE buffer_size)
{
	BYTE name_buffer[50];
	sprintf((char*)name_buffer,"0:%s",new_file_name);
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,(char*)name_buffer,FA_CREATE_NEW | FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,write_buffer,buffer_size,&mybr);//д������
		f_close(&myfsrc);
		return 0 ;
	}
	else if(myres == FR_EXIST)
	{
		return FR_EXIST;
	}
	else
	{
		return -1;
	}
}

/* Sdfs_write���ļ�д������ */
int Sdfs_write(BYTE *write_file_name,BYTE *write_buffer,BYTE buffer_size)
{
	BYTE name_buffer[50];
	sprintf((char*)name_buffer,"0:%s",write_file_name);
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,(char*)name_buffer,FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,write_buffer,buffer_size,&mybr);//д������
		f_close(&myfsrc);
		return 0 ;
	}
	else if(myres == FR_NO_FILE)
	{
		return FR_NO_FILE;
	}
	else
	{
		return -1;
	}
	
}

/* Sdfs_read ��һ���ļ��������ݵ������� */
int Sdfs_read(BYTE *read_file_name,BYTE *save_buffer)
{
	int count = 0;
	BYTE name_buffer[50];
	sprintf((char*)name_buffer,"0:%s",read_file_name);
	Sd_fs_init();
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,(char*)name_buffer,FA_OPEN_EXISTING |FA_WRITE);
	
	if(myres == FR_OK)
	{
		for(;;)
		{
			for(mya =0;mya<512;mya++)
				mybuffer[mya] = 0;
			myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);/* ���ļ������������512�ֽ�Ϊ��λ�������ػ����� */
			sprintf((char*)&save_buffer[count*512],"%s",mybuffer);
			
			count++;
			if(myres || mybr == 0) break;
			
		}
		
		return 0 ;
	}
	else
	{
		return -1;
	}
	
}

/* GetGBKCode_from_sd  ��SD���ֿ��ж�ȡ��ģ���ݵ�ָ���Ļ����� */
int GetGBKCode_from_sd(uint8_t *pBuffer,uint16_t c)
{
	unsigned char High8bit,Low8bit;
	unsigned int pos;
	
	High8bit = c>>8;
	Low8bit = c&0x00FF;
	
	pos = ((High8bit - 0xa0-16)*94+Low8bit-0xa0-1)*2*16;
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,"0:/HZLIB.bin",FA_OPEN_EXISTING |FA_WRITE);
	
	if(myres == FR_OK)
	{
		f_lseek(&myfsrc,pos);  //ָ��ƫ��
		myres = f_read(&myfsrc,pBuffer,32,&mybr);/* ���ļ������������512�ֽ�Ϊ��λ�������ػ����� */
		f_close(&myfsrc);
		
		return 0 ;
	}
	else
	{
		return -1;
	}
}

