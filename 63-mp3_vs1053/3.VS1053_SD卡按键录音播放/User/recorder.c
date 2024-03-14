#include "recorder.h"
#include <string.h>
#include <stdint.h>
#include "bsp_vs1053.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_systick.h"
#include "ff.h"

/*------------------------------------------------------------*/
//演示从简使用DWT外设计时值作为随机录音文件名字
#define NAMESIZE     30
static char last_record_pathname[NAMESIZE] = "0:Test_0001.wav";

void enable_dwt(void)
{
	#define DWT_CR              *(__IO uint32_t *)0xE0001000
	#define DWT_CYCCNT          *(__IO uint32_t *)0xE0001004
	#define DEM_CR              *(__IO uint32_t *)0xE000EDFC
	#define DEM_CR_TRCENA                          (1 << 24)
	#define DWT_CR_CYCCNTENA                       (1 <<  0)
	
	/* 使能DWT外设 */
	DEM_CR |= (uint32_t)DEM_CR_TRCENA;
	
	/* DWT CYCCNT寄存器计数清0 */
	DWT_CYCCNT = (uint32_t)0u;
	
	/* 使能Cortex-M DWT CYCCNT寄存器 */
	DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

void make_new_pathname(char *pathstr)
{
	memset(last_record_pathname,0,NAMESIZE);
	sprintf(pathstr,"0:Test_%d.wav",((uint32_t)DWT_CYCCNT) % 9999);
}

#define TEMPDATA_SIZE    0x1000    //4096
#define RECORD_BUFFSIZE  512
#define PLAY_BUFFSIZE    1024*4

static FRESULT result;
static FIL     file;
static UINT    bw;


static __WaveHeader      rechead;
static _recoder_obj      recset;
static enumRecorder      recorder_statue = STATE_WAIT;

/* 复位 */
static void Rest_Statue(void)
{
	VS_HardReset();
	VS_SoftReset();
	recorder_statue = STATE_WAIT;
}

void Recorder_Run(void)
{
	uint32_t sectorsize = 0;           //录音使用的扇区数
	uint16_t recsec = 0;               //录音时间
	uint16_t idx = 0;
	uint16_t regval = 0;               //寄存器数值
	uint16_t i=0;
	uint8_t buffer[PLAY_BUFFSIZE] = {0};
	uint8_t tempdata[TEMPDATA_SIZE] = {0};      //录音时长
	
	enable_dwt();
	
	LED_RGBOFF;
	
	while(1)
	{
		switch(recorder_statue)
		{
			case STATE_WAIT:
				LED_YELLOW;
				
				if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
				{
					printf("开始录音\r\n");
					recorder_statue = STATE_RECORDING;
					Delay_us(10000);
				}
				
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
				{
					printf("播放最近一次录音\r\n");
					recorder_statue = STATE_PLAYING;
					Delay_us(10000);
				}
				
			break;
			
			case STATE_RECORDING:
				LED_RGBOFF;
			
				//将tempdata数组中的值设置为0
				memset(tempdata,0,TEMPDATA_SIZE);
			
				vsset.mvol = 240;           //音量大小
				vsset.bflimit = 6;          //低音限制
				vsset.bass = 15;            //低音效果
				vsset.effect = 0;           //无耳机效果
				vsset.tflimit = 10;         //高音限制
				vsset.treble= 15;           //高音效果
				vsset.speed = 1;            //正常速度
				VS_SetAll();
			
				
				recset.input = 0;           //MIC
				recset.age = 6;             //增益
				recset.channel = 0;         //左声道
				recset.samplerate = 1;      //8K
				Recoder_EnterRecMode(&recset);
			
				while(VS_ReadReg(SPI_HDAT1)>>8);          //等待buf空闲再开始
				
				rechead.riff.ChunkID = 0x46464952;        //"RIFF"
				rechead.riff.ChunkSize = 0;               //还未确定，需要最后计算
				rechead.riff.Format = 0x45564157;         //"WAVE"
				rechead.fmt.SubChunkID = 0x20746D66;      //"fmt "fmt后有一个空格
				rechead.fmt.SubChunkSize = 16;            //大小为16字节
				rechead.fmt.AudioFormat = 0x01;           //音频格式，0x01表示PCM 0x11表示IMA_ADPCM
				rechead.fmt.NumOfChannels = 1;            //单声道
				rechead.fmt.SampleRate = recset.samplerate*8000;    //采样速率
				rechead.fmt.ByteRate = rechead.fmt.SampleRate*2;    //16位,即2个字节
				rechead.fmt.BlockAlign = 2;               //块大小,2个字节为一个块
				rechead.fmt.BitsPerSample = 16;           //16位PCM
				rechead.data.SubChunkID = 0x61746164;     //"data"
				rechead.data.SubChunkSize = 0;            //数据大小,还需要计算  
				
				make_new_pathname(last_record_pathname);
				result = f_open(&file,last_record_pathname,FA_CREATE_ALWAYS | FA_WRITE);
				
				if(result != 0)
				{
					printf("f_open error: %d",result);
					Rest_Statue();
					return;
				}
				else
				{
					result = f_write(&file,&rechead,sizeof(__WaveHeader),&bw);//预写入头数据
				}
				
				while(1)
				{
					LED3_TOGGLE;
					regval = VS_ReadReg(SPI_HDAT1);
					if((regval>=256)&&(regval<896))
					{
						idx = 0;
						while(idx < RECORD_BUFFSIZE)   //一次读取BUFSIZE字节
						{
							regval = VS_ReadReg(SPI_HDAT0);
							tempdata[idx] = regval&0xFF;
							tempdata[idx+1] = regval>>8;
							idx+=2;
						}
						f_lseek(&file,44+sectorsize*RECORD_BUFFSIZE);
						result = f_write(&file,tempdata,RECORD_BUFFSIZE,&bw);  //写入文件
						
						if(result)
						{
							printf("err:%d bw:%d\r\n",result,bw);
							f_close(&file);
							Rest_Statue();
							return;   //写入错误
						}
						sectorsize++;      //扇区数增加1
					}
					
					if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
					{
						rechead.riff.ChunkSize = sectorsize*RECORD_BUFFSIZE+36;      //整个文件的大小-8
						rechead.data.SubChunkSize = sectorsize*RECORD_BUFFSIZE;      //数据大小
						f_lseek(&file,0);                 //偏移到文件头
						result = f_write(&file,&rechead,sizeof(__WaveHeader),&bw);   //覆写入头数据
						printf("%s->write sector:%d,result:%d,num:%d\r\n",last_record_pathname,sectorsize,result,bw);
						f_close(&file);
						VS_HardReset();
						VS_SoftReset();
						
						recsec = sectorsize*4/125/recset.samplerate;
						char tempstr[12] = {0};
						sprintf(tempstr,"%02d:%02d",recsec/60,recsec%60);
						tempstr[5] = '\0';
						sectorsize = 0;
						
						printf("录音结束\r\n录音时长：%s\r\n",tempstr);
						recorder_statue = STATE_WAIT;
						return;
					}
					
				}
				
			break;
				
			
			case STATE_PLAYING:
				LED_RGBOFF;
			
				vsset.bass = 15;           //低音效果
				vsset.bflimit = 6;         //低音限制
				vsset.effect = 0;          //无耳机效果
				vsset.mvol = 240;          //音量大小
				vsset.speed = 1;           //正常速度			
				vsset.tflimit = 10;        //高音限制
				vsset.treble = 15;         //高音效果
			
				VS_RestartPlay();
				VS_SetAll();
				VS_ResetDecodeTime();
				VS_SPI_SpeedHigh();
			
				result = f_open(&file,last_record_pathname,FA_READ);
				if(result != 0)
				{
					printf("f_open error :%d",result);
					Rest_Statue();
					return;
				}
				
				while(1)
				{
					LED2_TOGGLE;
					result = f_read(&file,buffer,PLAY_BUFFSIZE,(UINT*)bw);
					if(result != 0)
					{
						f_close(&file);
						printf("f_read error:%d",result);
						printf("停止播放\r\n");
						Rest_Statue();
						return;
					}
					
					i = 0;
					do
					{
						if(VS_SendMusicData(buffer+i) == 0)
						{
							i+=32;
						}
						
						if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
						{
							f_close(&file);
							printf("停止播放\r\n");
							recorder_statue = STATE_WAIT;
							return;
						}
					}while(i<bw);
					
					if(bw != PLAY_BUFFSIZE || result != 0)
					{
						f_close(&file);
						printf("播放结束\r\n");
						recorder_statue = STATE_WAIT;
						return;
					}
				}
				
			break;
		
			default:
			break;
		}
	}
}



