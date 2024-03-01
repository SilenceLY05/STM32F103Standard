#include "bsp_bmp.h"
#include "ff.h"
#include "bsp_ili9341_lcd.h"

#define RGB24TORGB16(R,G,B) ((unsigned short int)((((R)>>3)<<11) | (((G)>>2)<<5)	| ((B)>>3)))
	
BYTE pColorData[960];   /* 一行真彩色数据缓存 320 * 3 = 960 */
FIL bmpfsrc,bmpfdst;
FRESULT bmpres;

/* 如果不需要打印bmp相关的提示信息，将printf注释掉 如果需要使用printf()需要将串口驱动文件包含进来 */
#define BMP_DEBUG_PRINTF(FORMAT,...)  printf(FORMAT,##__VA_ARGS__)

/* 打印BMP文件的头信息，用于调试 */
static void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
	BMP_DEBUG_PRINTF("位图文件头：\r\n");
	BMP_DEBUG_PRINTF("文件大小：%ld\r\n",(*pBmpHead).bfSize);
	BMP_DEBUG_PRINTF("保留字：%d\r\n",(*pBmpHead).bfReserved1);
	BMP_DEBUG_PRINTF("保留字：%d\r\n",(*pBmpHead).bfReserved2);
	BMP_DEBUG_PRINTF("实际位图数据的偏移字节数：%ld\r\n",(*pBmpHead).bfOffBits);
	BMP_DEBUG_PRINTF("\r\n");
}


/* 打印BMP文件的头信息，用于调试 */
static void showBmpInforHead(tagBITMAPINFOHEADER* pBmpInforHead)
{
    BMP_DEBUG_PRINTF("位图信息头:\r\n");
    BMP_DEBUG_PRINTF("结构体的长度:%ld\r\n",(*pBmpInforHead).biSize);
    BMP_DEBUG_PRINTF("位图宽:%ld\r\n",(*pBmpInforHead).biWidth);
    BMP_DEBUG_PRINTF("位图高:%ld\r\n",(*pBmpInforHead).biHeight);
    BMP_DEBUG_PRINTF("biPlanes平面数:%d\r\n",(*pBmpInforHead).biPlanes);
    BMP_DEBUG_PRINTF("biBitCount采用颜色位数:%d\r\n",(*pBmpInforHead).biBitCount);
    BMP_DEBUG_PRINTF("压缩方式:%ld\r\n",(*pBmpInforHead).biCompression);
    BMP_DEBUG_PRINTF("biSizeImage实际位图数据占用的字节数:%ld\r\n",(*pBmpInforHead).biSizeImage);
    BMP_DEBUG_PRINTF("X方向分辨率:%ld\r\n",(*pBmpInforHead).biXPelsPerMeter);
    BMP_DEBUG_PRINTF("Y方向分辨率:%ld\r\n",(*pBmpInforHead).biYPelsPerMeter);
    BMP_DEBUG_PRINTF("使用的颜色数:%ld\r\n",(*pBmpInforHead).biClrUsed);
    BMP_DEBUG_PRINTF("重要颜色数:%ld\r\n",(*pBmpInforHead).biClrImportant);
		BMP_DEBUG_PRINTF("\r\n");
}



/* 设置ILI9341的截取BMP图片 x:在扫描模式1下截取区域的起点x坐标 y:在扫描模式1下截取区域的起点Y坐标*/
void Lcd_show_bmp(uint16_t x,uint16_t y,char *pic_name)
{
	int i,j,k;
	int width,height,l_width;
	
	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead;
	BITMAPINFOHEADER bitInfoHead;
	WORD fileType;
	
	unsigned int read_num;
	
	bmpres = f_open(&bmpfsrc,(char *)pic_name,FA_OPEN_EXISTING | FA_READ);
	
	if(bmpres == FR_OK)
	{
		BMP_DEBUG_PRINTF("文件打开成功\r\n");
		
		/* 读取文件头信息 两个字节 */
		f_read(&bmpfsrc,&fileType,sizeof(WORD),&read_num);
		
		/* 判断是不是BMP文件 */
		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("这不是一个 .bmp 文件\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("这是一个 .bmp 文件 \r\n");
		}
		
		/* 读取BMP文件头文件 */
		f_read(&bmpfsrc,&bitHead,sizeof(tagBITMAPFILEHEADER),&read_num);
		showBmpHead(&bitHead);
		
		f_read(&bmpfsrc,&bitInfoHead,sizeof(tagBITMAPINFOHEADER),&read_num);
		showBmpInforHead(&bitInfoHead);
	}
	
	else
	{
		BMP_DEBUG_PRINTF("打开文件失败！错误代码：bmpres = %d \r\n",bmpres);
		return;
	}
	
	width = bitInfoHead.biWidth;
	height = bitInfoHead.biHeight;
	
	/* 计算位图的实际宽度并保存它为32的倍数 */
	l_width = WIDTHBYTES(width*bitInfoHead.biBitCount);
	
	if(l_width>960)
	{
		BMP_DEBUG_PRINTF("\n 本图片太大，无法再液晶屏上显示 (<=320) \n");
		return;
	}
	
	/* 开一个图片大小的窗口 */
	ILI9341_OpenWindow(x,y,width,height);
	ILI9341_Write_Cmd(CMD_SetPixel);
	
	/* 判断是否是24bit真彩色图 */
	if(bitInfoHead.biBitCount>=24)
	{
		for(i=0;i<height;i++)
		{
			f_lseek(&bmpfsrc,bitHead.bfOffBits+(height-i-1)*l_width);
			
			/* 读取一行bmp的数据到数组pColorData里面 */
			f_read(&bmpfsrc,pColorData,l_width,&read_num);
			
			for(j=0;j<width;j++)                                       //一行有效信息
			{
				k = j*3;                                                  //一行中第K个像素的起点
				red = pColorData[k+2];
				green = pColorData[k+1];
				blue = pColorData[k];
				
				ILI9341_Write_Data(RGB24TORGB16(red,green,blue));         //写入LCD-GRAM
			}			
				
		}
	}
	else
	{
		BMP_DEBUG_PRINTF("这不是一个24位真彩色BMP文件！");
		return;
	}
	f_close(&bmpfsrc);
}	

/* 设置ILI9341的截取BMP图片 x ：在扫描模式1下截取区域的起点X坐标  y ：在扫描模式1下截取区域的起点Y坐标  Width ：区域宽度  Height ：区域高度  */
int Screen_shot(uint16_t x,uint16_t y,uint16_t Width,uint16_t Height,char *filename)
{
	/* bmp文件头54个字节 */
	unsigned char header[54]=
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	int i;
	int j;
	long file_size;
	long width;
	long height;
	unsigned char r,g,b;
	unsigned int mybw;
	unsigned int read_data;
	char kk[4] = {0,0,0,0};
	
	uint8_t ucAlign;
	
	/* 宽*高 + 补充字节 + 头部信息 */
	file_size = (long)Width * (long)Height*3+Height*(Width%4)+54;
	
	/* 文件大小 4个字节 */
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size>>8)&0x000000ff;
	header[4] = (file_size>>16)&0x000000ff;
	header[5] = (file_size<<24)&0x000000ff;
	
	/* 位图宽 4个字节 */
	width = Width;
	header[18] = width & 0x000000ff;
	header[19] = (width>>8)&0x000000ff;
	header[20] = (width>>16)&0x000000ff;
	header[21] = (width<<24)&0x000000ff;
	
	/* 位图高 4个字节 */
	height = Height;
	header[22] = height & 0x000000ff;
	header[23] = (height>>8)&0x000000ff;
	header[24] = (height>>16)&0x000000ff;
	header[25] = (height<<24)&0x000000ff;

	
	/* 新建一个文件 */
	bmpres = f_open(&bmpfsrc,(char*)filename,FA_CREATE_ALWAYS |FA_WRITE);
	
	f_close(&bmpfsrc);
	
	bmpres = f_open(&bmpfsrc,(char*)filename,FA_OPEN_EXISTING|FA_WRITE);
	
	if(bmpres == FR_OK)
	{
		/* 将预先定义好的bmp头部信息写进文件 */
		bmpres = f_write(&bmpfsrc,header,sizeof(unsigned char)*54,&mybw);
		
		/* 下面是将指定窗口的数据读出写到文件里面 */
		
		ucAlign = Width %4;
		
		for(i=0;i<Height;i++)
		{
			for(j=0;j<Width;j++)
			{
				read_data = ILI9341_GetPointPixel(x+j,y+Height-1-i);
				
				r = GETR_FROM_RGB16(read_data);
				g = GETG_FROM_RGB16(read_data);
				b = GETB_FROM_RGB16(read_data);
				
				bmpres = f_write(&bmpfsrc,&b,sizeof(unsigned char),&mybw);
				bmpres = f_write(&bmpfsrc,&g,sizeof(unsigned char),&mybw);
				bmpres = f_write(&bmpfsrc,&r,sizeof(unsigned char),&mybw);
			}
			
			if(ucAlign)  /* 如果不是4字节对齐 */
			{
				bmpres = f_write(&bmpfsrc,kk,sizeof(unsigned char)*(ucAlign),&mybw);
			}
		}//截屏完毕
		f_close(&bmpfsrc);
		return 0;
	}
	else if(bmpres == FR_EXIST)  // 如果文件已经存在
		return FR_EXIST;
	
	else  /* 截屏失败 */
		return -1;
	
}


