#include "bsp_sdfs_app.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart.h"
#include "ff.h"
#include <stdio.h>



FATFS myfs[2];                        //�߼��������Ĺ�����(�ļ�ϵͳ����)
FIL myfsrc,myfdst;                    //�ļ�����
FRESULT myres;                        //FatFs ����ͨ�ý������
BYTE mybuffer[512];                   //�ļ����ƻ�����
BYTE my_latest_buffer[512];
uint8_t mystring[512] = "this is a MicrsSD base on fatfs";
UINT mybr,mybw;                       // File R/W count
int mya = 0;
char mypath[512] = "0:";              // һ��Ҫ��ʼ��Ϊ0:

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
	disk_initialize(0);           /* SD ��Ӳ����ʼ�� */
	
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
	
	myres = f_open(&myfsrc,"0:/my_Demo.TXT",FA_OPEN_EXISTING|FA_READ);    /* ���ļ� */	  //ok    
	mybr =1;
	mya = 0;
	for(;;)
	{
		for(mya = 0;mya<512;mya++)         /* �建���� */
		mybuffer[mya] =0;
		myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);            /* ���ļ���������ݶ��������� */
		sprintf((char*)&my_latest_buffer[count*512],"%s",mybuffer);         //��ӡ�������� 
		count++;
		if(myres||mybr ==0) break;   // error or eof  
	}
	f_close(&myfsrc);   /* �رմ򿪵��ļ� */ 
}


/* Sdfs_new ��һ���ļ���д������ new_file_name�˽��ļ������� write_bufferд���ļ������ݻ������ĵ�ַ 
 * buffer_size��������С  ����ֵ0�ɹ�1ʧ�� */
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
	else if(myres == FR_EXIST)         //����ļ��Ѿ�����
	{
		return FR_EXIST;
	}
	else
	{
		return -1;
	}
}

/* Sdfs_write���ļ�д������ new_file_nameд�������ļ������� write_bufferд���ļ������ݻ������ĵ�ַ 
 * buffer_size��������С  ����ֵ0�ɹ�1ʧ�� */
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
	else if(myres == FR_NO_FILE)   //���û�и��ļ�
	{
		return FR_NO_FILE;
	}
	else
	{
		return -1;
	}
	
}

/* Sdfs_read ��һ���ļ��������ݵ������� read_file_name�ļ������� save_buffer������Ҫ����ĵ�ַ ����ֵ0��ʾ�ɹ�1��ʾʧ�� */
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
			for(mya =0;mya<512;mya++)    /* �建���� */
				mybuffer[mya] = 0;
			myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);/* ���ļ������������512�ֽ�Ϊ��λ�������ػ����� */
			sprintf((char*)&save_buffer[count*512],"%s",mybuffer);  //��ӡ���û�ָ���Ļ�����������
			
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

/* GetGBKCode_from_sd  ��SD���ֿ��ж�ȡ��ģ���ݵ�ָ���Ļ����� pBuffer---���ݱ����ַ c--�����ַ����ֽ��� */
int GetGBKCode_from_sd(uint8_t *pBuffer,uint16_t c)
{
	unsigned char High8bit,Low8bit;
	unsigned int pos;
	
	High8bit = c>>8;      /* ȡ��8λ���� */
	Low8bit = c&0x00FF;   /* ȡ��8λ���� */
	
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


