#include "bsp_sdfs_app.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart.h"
#include "ff.h"
#include <stdio.h>



FATFS myfs[2];                        //逻辑驱动器的工作区(文件系统对象)
FIL myfsrc,myfdst;                    //文件对象
FRESULT myres;                        //FatFs 函数通用结果代码
BYTE mybuffer[512];                   //文件复制缓冲区
BYTE my_latest_buffer[512];
uint8_t mystring[512] = "this is a MicrsSD base on fatfs";
UINT mybr,mybw;                       // File R/W count
int mya = 0;
char mypath[512] = "0:";              // 一定要初始化为0:

/* Private functions ---------------------------------------------------------*/

void Sd_fs_init(void)
{
	/* SD卡中断初始化 */
	SDIO_NVIC_Configuration();
	
	/* SD卡硬件初始化，初始化盘符为0 */
	f_mount(0,&myfs[0]);
}

void Sd_fs_test(void)
{
	int count = 0;
	SDIO_NVIC_Configuration();
	disk_initialize(0);           /* SD 卡硬件初始化 */
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,"0:/my_Demo.TXT",FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,mystring,sizeof(mystring),&mybr);
		f_close(&myfsrc);
	}
	else if(myres == FR_EXIST)//如果文件已经存在
	{
		
	}
	
	myres = f_open(&myfsrc,"0:/my_Demo.TXT",FA_OPEN_EXISTING|FA_READ);    /* 打开文件 */	  //ok    
	mybr =1;
	mya = 0;
	for(;;)
	{
		for(mya = 0;mya<512;mya++)         /* 清缓冲区 */
		mybuffer[mya] =0;
		myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);            /* 将文件里面的内容读到缓冲区 */
		sprintf((char*)&my_latest_buffer[count*512],"%s",mybuffer);         //打印到缓冲区 
		count++;
		if(myres||mybr ==0) break;   // error or eof  
	}
	f_close(&myfsrc);   /* 关闭打开的文件 */ 
}


/* Sdfs_new 建一个文件并写入数据 new_file_name兴建文件的名称 write_buffer写入文件的数据缓冲区的地址 
 * buffer_size缓冲区大小  返回值0成功1失败 */
int Sdfs_new(BYTE *new_file_name,BYTE *write_buffer,BYTE buffer_size)
{
	BYTE name_buffer[50];
	sprintf((char*)name_buffer,"0:%s",new_file_name);
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,(char*)name_buffer,FA_CREATE_NEW | FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,write_buffer,buffer_size,&mybr);//写入数据
		f_close(&myfsrc);
		return 0 ;
	}
	else if(myres == FR_EXIST)         //如果文件已经存在
	{
		return FR_EXIST;
	}
	else
	{
		return -1;
	}
}

/* Sdfs_write对文件写入数据 new_file_name写入数据文件的名称 write_buffer写入文件的数据缓冲区的地址 
 * buffer_size缓冲区大小  返回值0成功1失败 */
int Sdfs_write(BYTE *write_file_name,BYTE *write_buffer,BYTE buffer_size)
{
	BYTE name_buffer[50];
	sprintf((char*)name_buffer,"0:%s",write_file_name);
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,(char*)name_buffer,FA_WRITE);
	
	if(myres == FR_OK)
	{
		myres = f_write(&myfsrc,write_buffer,buffer_size,&mybr);//写入数据
		f_close(&myfsrc);
		return 0 ;
	}
	else if(myres == FR_NO_FILE)   //如果没有该文件
	{
		return FR_NO_FILE;
	}
	else
	{
		return -1;
	}
	
}

/* Sdfs_read 从一个文件读出数据到缓冲区 read_file_name文件的名称 save_buffer数据需要保存的地址 返回值0表示成功1表示失败 */
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
			for(mya =0;mya<512;mya++)    /* 清缓冲区 */
				mybuffer[mya] = 0;
			myres = f_read(&myfsrc,mybuffer,sizeof(mybuffer),&mybr);/* 将文件里面的内容以512字节为单位读到本地缓冲区 */
			sprintf((char*)&save_buffer[count*512],"%s",mybuffer);  //打印到用户指定的缓冲区缓冲区
			
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

/* GetGBKCode_from_sd  从SD卡字库中读取字模数据到指定的缓冲区 pBuffer---数据保存地址 c--汉字字符低字节码 */
int GetGBKCode_from_sd(uint8_t *pBuffer,uint16_t c)
{
	unsigned char High8bit,Low8bit;
	unsigned int pos;
	
	High8bit = c>>8;      /* 取高8位数据 */
	Low8bit = c&0x00FF;   /* 取低8位数据 */
	
	pos = ((High8bit - 0xa0-16)*94+Low8bit-0xa0-1)*2*16;
	
	f_mount(0,&myfs[0]);
	myres = f_open(&myfsrc,"0:/HZLIB.bin",FA_OPEN_EXISTING |FA_WRITE);
	
	if(myres == FR_OK)
	{
		f_lseek(&myfsrc,pos);  //指针偏移
		myres = f_read(&myfsrc,pBuffer,32,&mybr);/* 将文件里面的内容以512字节为单位读到本地缓冲区 */
		f_close(&myfsrc);
		
		return 0 ;
	}
	else
	{
		return -1;
	}
}


