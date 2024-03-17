/**************************************************************************************
* 因为emWin显示只支持UTF-8编码格式的中文，如果希望直接显示在Keil直接输入的中文，      *
*            比如使用：GUI_DispStringHCenterAt("流水灯",110,120);                     *
* 该文件必须以UTF-8编码格式，不然中文无法正常显示。                                   *
*                                                                                     *
* 如果只是个别例程出现中文显示乱码（如果所有例程都无法显示中文可能是字库问题），      *
* 把对应的例程文件(比如LEDapp.c)用电脑的记事本软件打开，然后选择另存为，在弹出对      *
* 话框中“保存(S)"按钮的左边有个"编码(E)"选项，选择"UTF-8",然后同样保存为同名称的      *
* C语言文件(覆盖原来文件)，再编译。                                                   *
*                                                                                     *
* 如果编译工程时出现下面类似错误也是该文件编码格式问题,必须把文件保存为UTF-8格式      *
* 再编译                                                                              *
* ..\..\User\app\LEDapp.c(275): error:  #8: missing closing quote                     *
*        GUI_DispStringHCenterAt("娴?姘?鐏?",110,120);                                *
* ..\..\User\app\LEDapp.c(276): error:  #165: too few arguments in function call      *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
* ..\..\User\app\LEDapp.c(276): error:  #18: expected a ")"                           *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
*                                                                                     *
* 修改文件后编译就出错这是Keil5软件问题(Keil4没这问题)，推荐使用其他程序编辑工具，    *
* 只用Keil5完成编译和下载工作。                                                       *
***************************************************************************************
*                      实验平台: 野火STM32 ISO 开发板                                 *
*                      论    坛: http://www.chuxue123.com                             *
*                      淘    宝: http://firestm32.taobao.com                          *
*                      邮    箱: wildfireteam@163.com                                 *
***************************************************************************************
*/
/**************************************************************************************
*                                                                                     *
*                SEGGER Microcontroller GmbH & Co. KG                                 *
*        Solutions for real time microcontroller applications                         *
*                                                                                     *
***************************************************************************************
*                                                                                     *
* C-file generated by:                                                                *
*                                                                                     *
*        GUI_Builder for emWin version 5.22                                           *
*        Compiled Jul  4 2013, 15:16:01                                               *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG                                *
*                                                                                     *
***************************************************************************************
*                                                                                     *
*        Internet: www.segger.com  Support: support@segger.com                        *
*                                                                                     *
***************************************************************************************
*/
// USER START (Optionally insert additional includes)
#include "includes.h"
#include "app.h"
#include "..\User\bsp\vs1053\flac.h"
// USER END

/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/

// USER START (Optionally insert additional defines)
#define FILE_NAME_LEN 			100			//文件名长度，如果检测到文件名超过50 则丢弃这个文件 
#define LYRIC_MAX_SIZE			60					//歌词最大行数	
#define MAX_LINE_LEN 				90					//每行歌词最大字节数
#define _DF1S								0x81
#define SCANPATH						"1:"
#define BUFSIZE							2048
#define MAX_MUSIC_LIST			20
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
typedef struct 
{ 
	int32_t 	time_tbl[LYRIC_MAX_SIZE];		//LRC时间表								
	uint16_t	addr_tbl[LYRIC_MAX_SIZE];		//LRC地址表	
  uint8_t		length_tbl[LYRIC_MAX_SIZE]; //LRC每行歌词长度表
 	uint8_t 	indexsize; 									//LRC歌词行总数（包括重复的）		
 	int32_t		curtime;										//当前时间
  int32_t		oldtime;										//上个时间
}LYRIC;

extern GUI_CONST_STORAGE GUI_BITMAP bmback;
extern GUI_CONST_STORAGE GUI_BITMAP bmnext;
extern GUI_CONST_STORAGE GUI_BITMAP bmplay;
extern GUI_CONST_STORAGE GUI_BITMAP bmstop;
extern GUI_CONST_STORAGE GUI_BITMAP bmvoice;

static LYRIC lrc;
static uint8_t  file_num = 0;																	//存储扫描到的MP3+WAV音频文件数量 
static unsigned char playlist[MAX_MUSIC_LIST][FILE_NAME_LEN];
static uint8_t playflag=0;
static uint8_t chgsch=0;

/* MP3解码时用到的变量 */
static uint8_t  buffer[BUFSIZE];	 														// 文件缓冲区


/* 播放器状态标志   */
typedef enum {S_READY,S_PLAY,S_STOP,S_SWITCH}PLAYER_STATE;		//准备、播放、暂停、切歌
static PLAYER_STATE player_state;															//播放器状态标志位，指示切歌，正在播放，暂停等状态
static uint8_t play_index = 0;																//存储正在或准备播放的歌曲索引
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusic[] = {
  { FRAMEWIN_CreateIndirect, "Music", 0, 0, 0, 240, 320, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "PlayTime", GUI_ID_TEXT1, 8, 200, 30, 16, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "AllTime", GUI_ID_TEXT2, 192, 200, 30, 16, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "lyric", GUI_ID_TEXT3, 5, 3, 220, 20, 0, 0x0, 0 },
  { LISTBOX_CreateIndirect, "Musiclist", GUI_ID_LISTBOX0, 10, 30, 210, 160, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect, "schedule", GUI_ID_SLIDER1, 40, 195, 150, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Volume", GUI_ID_SLIDER0, 60, 270, 130, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Back", GUI_ID_BUTTON0, 40, 230, 32, 32, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Next", GUI_ID_BUTTON1, 155, 230, 32, 32, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Play", GUI_ID_BUTTON2, 100, 230, 32, 32, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
// USER START (Optionally insert additional static code)
/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files(char* path) 
{ 
	FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
	FILINFO fno; 
	DIR dir; 
	int i=0; 
	char *fn; 
	char file_name[FILE_NAME_LEN];	
	
	#if _USE_LFN //ffconf.h
	static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
	fno.lfname = lfn; 
	fno.lfsize = sizeof(lfn); 
	#endif 
	
	res = f_opendir(&dir, path); //打开目录
	if (res == FR_OK) 
	{ 
		i = strlen(path); 
		while(1)
		{ 
			res = f_readdir(&dir, &fno); 										//读取目录下的内容
			if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
			#if _USE_LFN 
			fn = *fno.lfname ? fno.lfname : fno.fname; 
			#else 
			fn = fno.fname; 
			#endif 
			if (*fn == '.') continue; 											//点表示当前目录，跳过			
			if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
				sprintf(&path[i], "/%s", fn); 							//合成完整目录名
				res = scan_files(path);											//递归遍历 
				if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
				path[i] = 0; 
			} 
			else 
			{
				//printf("%s/%s\r\n", path, fn);								//输出文件名
				if(strstr(fn,".mp3")||strstr(fn,".MP3")||
					 strstr(fn,".wav")||strstr(fn,".WAV")||
					 strstr(fn,".flac")||strstr(fn,".FLAC"))//判断是否mp3或wav文件
				{					
					if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
					{
						sprintf(file_name, "%s/%s",path, fn);						
						memcpy(&playlist[file_num][0],file_name,strlen(file_name));	
						file_num++;//记录文件个数
						if(file_num>=MAX_MUSIC_LIST)break;
					}
				}//if mp3||wav
       }//else
     } //for
   } 
   return res; 
} 


static uint16_t getonelinelrc(uint8_t *buff,uint8_t *str,int16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		*(str+i)=*(buff+i);
		if((*(buff+i)==0x0A)||(*(buff+i)==0x00))
		{
			*(buff+i)='\0';
			*(str+i)='\0';
			break;
		}
	}
	return (i+1);
}
static void lrc_chg_suffix(uint8_t *name,const char *sfx)
{		    	     
	while(*name!='\0')name++;
	while(*name!='.')name--;
	*(++name)=sfx[0];
	*(++name)=sfx[1];
	*(++name)=sfx[2];
	*(++name)='\0';
}
static void lrc_sequence(LYRIC	*lyric)
{
	uint16_t i=0,j=0;
	uint16_t temp=0;
	if (lyric->indexsize == 0)return;
	
	for(i = 0; i < lyric->indexsize - 1; i++)
	{
		for(j = i+1; j < lyric->indexsize; j++)
		{
			if(lyric->time_tbl[i] > lyric->time_tbl[j])
			{
				temp = lyric->time_tbl[i];
				lyric->time_tbl[i] = lyric->time_tbl[j];
				lyric->time_tbl[j] = temp;

				temp = lyric->addr_tbl[i];
				lyric->addr_tbl[i] = lyric->addr_tbl[j];
				lyric->addr_tbl[j] = temp;
			}
		}
	}	
}
static void lyric_analyze(LYRIC	*lyric,uint8_t *strbuf)
{
	uint8_t strtemp[MAX_LINE_LEN]={0};
	uint8_t *pos=0;
	uint8_t sta=0,strtemplen=0;
	uint16_t lrcoffset=0;
	uint16_t str_len=0,i=0;
	
	pos=strbuf;
	str_len=strlen((const char *)strbuf);
	if(str_len==0)return;
	i=str_len;
	while(--i)
	{
		if(*pos=='[')
			sta=1;
		else if((*pos==']')&&(sta==1))
			sta=2;
	  else if((sta==2)&&(*pos!=' '))
		{
			sta=3;
			break;
		}
		pos++; 
	}
	if(sta!=3)return;	
	lrcoffset=0;
	lyric->indexsize=0;
	while(lrcoffset<=str_len)
	{
		i=getonelinelrc(strbuf+lrcoffset,strtemp,MAX_LINE_LEN);
		lrcoffset+=i;
//		printf("lrcoffset:%d,i:%d\n",lrcoffset,i);
		strtemplen=strlen((const char *)strtemp);
		pos=strtemp;
		while(*pos!='[')
			pos++;
		pos++;
		if((*pos<='9')&&(*pos>='0'))
		{
			lyric->time_tbl[lyric->indexsize]=(((*pos-'0')*10+(*(pos + 1)-'0'))*60+((*(pos+3)-'0')*10+(*(pos+4)-'0')))*100+((*(pos+6)-'0')*10+(*(pos+7)-'0'));
			lyric->addr_tbl[lyric->indexsize]=(uint16_t)(lrcoffset-strtemplen+10); 
			lyric->length_tbl[lyric->indexsize]=strtemplen-10;
			lyric->indexsize++;
		}		
		else
				continue;		
	}
}

void vs1053_player(uint8_t *filepath,WM_HWIN hWin)
{
	uint16_t playtime=0,time=0;
	static uint8_t lyriccount=0;
	uint16_t musicrate=0,f_kbps=0xffff;
	uint16_t i=0;
	uint32_t val=0;
	char lyrictext[MAX_LINE_LEN]={0};
	OS_ERR      err;
	uint8_t type=0;
	uint8_t timestr[6]={0};
	
	printf("song:%s\n",filepath);
	VS_Restart_Play();  					//重启播放 
	VS_Set_All();        					//设置音量等信息 			 
	VS_Reset_DecodeTime();					//复位解码时间 	
	
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
	{
		VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
		bsp_DelayUS(1000);//1ms
		type=2;
	}
	else	if(strstr((const char*)filepath,".wav")||strstr((const char*)filepath,".WAV"))
	{
		type=3;
	}
	else//if(strstr((const char*)filepath,".mp3")||strstr((const char*)filepath,".mp3"))
	{
		type=1;
	}
	
	SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),0);
	lyriccount=0;
	lrc.oldtime=lrc.time_tbl[0];
	
	result=f_open(&file,(const TCHAR*)filepath,FA_READ);//打开文件	
	
	if(result!=FR_OK)
	{
		printf("res:%d\n",result);
		if(play_index<file_num-1)		//自动开始下一首歌曲
				play_index++;
			else
				play_index = 0;
		player_state = S_SWITCH;
		f_close(&file);
		return;
	}
	
	VS_SPI_SpeedHigh();	//高速						   
	while(playflag==1)
	{
		if (player_state==S_STOP)
		{			
			GUI_Delay(5);
			if(tpad_flag)WM_DeleteWindow(hWin);
			continue;											//暂停的时候跳出本循环
		}		
		if(chgsch&&((VS_RD_Reg(SPI_STATUS)&0x80)==0))//快进、快退
		{
			uint32_t sch=0;
			uint16_t numb=0;
			uint8_t 	dat[32]={0};
			VS_Set_Vol(10);
			numb=VS_Get_EndFillByte()&0xff;
			for(i=0;i<32;i++)dat[i]=numb;
			i=0;
			for(i=0;i<2048+32;)
			{
				if(VS_Send_MusicData(dat)==0)i+=32;
			}
			sch=chgsch*file.fsize/0xff;
//			printf("chgsch:%d,sch:%d\n",chgsch,sch);
			f_lseek(&file,sch);
			VS_Set_Vol(vsset.mvol);
			chgsch=0;
		}
		
		result=f_read(&file,buffer,BUFSIZE,(UINT*)&bw);//读出2048个字节	
		i=0;
		OSSchedLock(&err);
		while(i<bw)
		{
			if(VS_Send_MusicData(buffer+i)==0)//给VS10XX发送音频数据
			{
				i+=32;
			}
		}
		OSSchedUnlock(&err);		
		
		if(bw!=BUFSIZE||result!=FR_OK)
		{
			VS_Soft_Reset();
			GUI_Delay(10);
			if(play_index<file_num-1)		//自动开始下一首歌曲
				play_index++;
			else
				play_index = 0;
			player_state = S_SWITCH;
			f_close(&file);
			printf("song end!!\n");
			return;//读完了.		  
		}
		if(tpad_flag)WM_DeleteWindow(hWin);
		GUI_Delay(1);
		if(Flag_ICON000==0)return;
		if(player_state==S_SWITCH)break;

		if(f_kbps==0xffff)//只运行一次
		{
			uint8_t tmp[4]={0};
			uint32_t dattmp=0;
			uint16_t time=0;
			switch (type)
			{
				case 2://flac
					tmp[0]=buffer[22];//定位到flac文件总采样数存放的确实位置
					tmp[1]=buffer[23];//总采样数本身是以36位存储的，但我们只用了32位，一般文件不用这么大数据
					tmp[2]=buffer[24];//这里使用绝对定位到存储地址，可能部分文件会有偏差，为精确使用可使用查找定位
					tmp[3]=buffer[25];
					dattmp=(tmp[0]<<24)+(tmp[1]<<16)+(tmp[2]<<8)+tmp[3];//flac文件总采样数
					time=dattmp/44100;//flac文件总播放时间
					f_kbps=file.fsize/125/time;//flac文件比特率
					break;
				case 3://wav
					tmp[0]=buffer[24];//绝对定位到wav文件存储采样频率地址
					tmp[1]=buffer[25];				
					dattmp=(tmp[1]*256+tmp[0])*32/1000;				
					f_kbps=(uint16_t)dattmp;
					//printf("tmp:%02X %02X,dattmp:%d,f_kbps:%d\n",tmp[0],tmp[1],dattmp,f_kbps);
					break;
				default:
					f_kbps=VS_Get_HeadInfo();
					break;
			}				
		}
		
		time++;
		if((time>=(25*type+playtime))&&(chgsch==0))
		{
			uint32_t timetemp=0;
			OSSchedLock(&err);
			playtime=time;	
			if((type==1)&&((f_kbps>448)||(f_kbps<32)))f_kbps=VS_Get_HeadInfo();
			val=file.fsize/f_kbps/125;
			sprintf((char *)timestr,"%02d:%02d",val/60,val%60);
			TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT2),(char *)timestr);
			
			lrc.curtime=f_tell(&file)/f_kbps/125;
			
			sprintf((char *)timestr,"%02d:%02d",lrc.curtime/60,lrc.curtime%60);
			TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1),(char *)timestr);
			//printf("fkbps:%d,time:%d,val:%d\n",f_kbps,time,val);
			
			musicrate=lrc.curtime*255/val;
			SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER1),musicrate);

			if((lrc.oldtime<=lrc.curtime*100)&&(lrc.indexsize>10))
			{
				i=0;
				while(i<lrc.length_tbl[lyriccount])
				{
					lyrictext[i]=comdata[lrc.addr_tbl[lyriccount]+i-1];
					if(lyrictext[i]=='\0')break;
					++i;
				}
				lyrictext[i]='\0';
				if(i>2)TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT3),lyrictext);
				
				do{
					lyriccount++;					
					if(lyriccount>=lrc.indexsize)
					{
						lrc.indexsize=0;
						break;
					}
					lrc.oldtime=lrc.time_tbl[lyriccount];
				}while(lrc.oldtime<=(lrc.curtime*100));
			}
			OSSchedUnlock(&err);	
		}
	}
	lyriccount=0;
	f_close(&file);
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogMusic(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  uint8_t i=0,j=0;
  // USER START (Optionally insert additional variables)
	char music_name[FILE_NAME_LEN];
  // USER END
  switch (pMsg->MsgId) {
	case WM_DELETE:
		OS_INFO("Musicapp delete\n");	
		playflag=0;
		player_state = S_READY;
		VS_HD_Reset();
		//VS_Soft_Reset();
		play_index = 0;
		file_num=0;
//		PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
		UserApp_Flag = 0;
	  Flag_ICON000 = 0;		
		tpad_flag=0;
	break;
  case WM_INIT_DIALOG:
	//
	// Initialization of 'Music'
	//
	hItem = pMsg->hWin;
	FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
	FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
	FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
	FRAMEWIN_SetTitleHeight(hItem, 20);
	//
	// Initialization of 'PlayTime'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);	
	TEXT_SetFont(hItem, GUI_FONT_10_ASCII);
	TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
	TEXT_SetText(hItem,"00:00");
	//
	// Initialization of 'AllTime'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
	TEXT_SetFont(hItem, GUI_FONT_10_ASCII);
	TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	TEXT_SetText(hItem,"00:00");
	//
	// Initialization of 'lyric'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
	TEXT_SetFont(hItem, &XBF_Font);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetText(hItem,"歌词");
	TEXT_SetTextColor(hItem,GUI_MAGENTA);
	//
	// Initialization of 'Schedule'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
	SLIDER_SetRange(hItem,0,0xff);
	SLIDER_SetValue(hItem,0);
	SLIDER_SetSkinClassic(hItem);
	SLIDER_SetWidth(hItem,2);
	SLIDER_SetNumTicks(hItem,0);
	//
	// Initialization of 'Back'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmback,5,7);
	BUTTON_SetText(hItem,"");
	//
	// Initialization of 'Next'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmnext,6,7);
	BUTTON_SetText(hItem,"");
	//
	// Initialization of 'Play'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmplay,6,7);
	BUTTON_SetText(hItem,"");
	//
	// Initialization of 'Volume'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
	SLIDER_SetRange(hItem,140,254);
	SLIDER_SetValue(hItem,vsset.mvol);
	//
	// Initialization of 'Musiclist'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
	LISTBOX_SetMulti(hItem,0);
	LISTBOX_SetFont(hItem, &XBF_Font);
	LISTBOX_SetAutoScrollV(hItem,1);
	// USER START (Optionally insert additional code for further widget initialization)	
	for(j=0;j<MAX_MUSIC_LIST;j++)
		for(i=0;i<FILE_NAME_LEN;i++)
			playlist[j][i]='\0';
	sprintf((char *)comdata,"%s",SCANPATH);
	scan_files((char *)comdata);	
	if(file_num)
	{
		char *startadd;
			for(i=0;i<file_num;i++)
			{
				startadd=(char *)strrchr((char *)&playlist[i][0],'/');
				sprintf((char *)comdata,"%s",&startadd[1]);				
				com_gbk2utf8((char *)comdata,music_name);
				LISTBOX_AddString(hItem,(char *)music_name);
			}
	}
	else
		LISTBOX_AddString(hItem,"no music file");
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_LISTBOX0: // Notifications sent by 'Musiclist'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
				if(player_state == S_READY)
				{
					player_state = S_PLAY;
					BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmstop,7,7);
				}
				else
					player_state = S_SWITCH;
					play_index=LISTBOX_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0));
				//printf("paly_index=%d\n",play_index);
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)		
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER0: // Notifications sent by 'Volume'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
				vsset.mvol=SLIDER_GetValue(hItem);
				VS_Set_Vol(vsset.mvol);
				//printf("voice=%d\n",voice);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
		case GUI_ID_SLIDER1: // Notifications sent by 'Sehedule'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				if(playflag==0)break;
				chgsch=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1));
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'Front'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
		if(player_state != S_READY) 
		{
			player_state = S_SWITCH;
			f_close(&file);
			VS_Restart_Play();
//			bsp_DelayUS(1000);
		}
		play_index--;
		if(play_index>file_num)play_index=file_num-1;
		LISTBOX_SetSel(hItem,play_index);
		//printf("paly_index=%d\n",play_index);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'Back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
		if(player_state != S_READY) 
		{
			player_state = S_SWITCH;
			f_close(&file);
			VS_Restart_Play();
//			bsp_DelayUS(1000);
		}
		play_index++;
		if(play_index>=file_num)play_index=0;
		LISTBOX_SetSel(hItem,play_index);
		//printf("paly_index=%d\n",play_index);
		// USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON2: // Notifications sent by 'Play'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
		if(player_state != S_PLAY)
		{
			BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmstop,7,7);
			player_state = S_PLAY;
		}
		else if(player_state == S_PLAY)
		{
			BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmplay,7,7);
			player_state = S_STOP;
		}
		// USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
	case WM_PAINT:                                            //重绘背景	
		GUI_Clear();
		GUI_DrawGradientV(0, 0, 240 - 10, 150, GUI_LIGHTGREEN,GUI_LIGHTYELLOW );
		GUI_DrawGradientV(0,150,240-10,300,GUI_LIGHTYELLOW,GUI_LIGHTGREEN);
		GUI_DrawBitmap(&bmvoice,40,273);
    break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateMusic
*/
void FUN_ICON000Clicked(void)
{
	WM_HWIN hWin;
	uint16_t i=0;
	char music_name[FILE_NAME_LEN];
	OS_INFO("Musicapp create\n");
	
	vsset.mvol=220;
	vsset.bflimit=6;
	vsset.bass=15;	
	vsset.tflimit =10;
	vsset.treble =15;
	vsset.effect =0;
	vsset.speed =1;
	
	file_num = 0;	
	com_data2null(buffer,BUFSIZE);	
	com_data2null(comdata,COMDATA_SIZE);
	player_state = S_READY;				//初始化状态
	vsset.mvol=220;
	VS_Init();
	GUI_Delay(10);
	//printf("vs1053:%4X\n",VS_Ram_Test());
	//GUI_Delay(100);
	//VS_Sine_Test();
	VS_HD_Reset();
	VS_Soft_Reset();
	hWin = GUI_CreateDialogBox(_aDialogCreateMusic, GUI_COUNTOF(_aDialogCreateMusic), _cbDialogMusic, WM_HBKWIN, 0, 0);
	while(Flag_ICON000)
	{
		while(Flag_ICON000)//while(playflag==0)
		{
			if(player_state==S_PLAY)
			{
				lrc.indexsize=0;
				lrc.oldtime=0;
				lrc.curtime=0;
				
				LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0),play_index);
				//显示歌词名称
				LISTBOX_GetItemText(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0),play_index,music_name,FILE_NAME_LEN);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),music_name);
				//获取音频文件完整路径
				i=0;
				while(playlist[play_index][i]!='\0')
				{
					music_name[i]=playlist[play_index][i];
					i++;
				}				
				music_name[i]='\0';
//				printf("play:%s\n",music_name);
				lrc_chg_suffix((uint8_t *)music_name,"lrc");
				com_data2null(comdata,COMDATA_SIZE);
				i=0;
				while(i<LYRIC_MAX_SIZE)
				{
					lrc.addr_tbl[i]=0;
					lrc.length_tbl[i]=0;
					lrc.time_tbl[i]=0;
					i++;
				}
				result=f_open(&file,music_name,FA_READ);
			
				if((result==FR_OK)&&(file.fsize<COMDATA_SIZE))
				{					
					result=f_read(&file,comdata,file.fsize,(UINT*)&bw);		
					if(result==FR_OK) 
					{
						OS_ERR      err;
						OSSchedLock(&err);
						lyric_analyze(&lrc,comdata);
						lrc_sequence(&lrc);
						OSSchedUnlock(&err);
					}
				}
				f_close(&file);			
				playflag=1;
				break;
			}
			else if(player_state==S_SWITCH)
			{	
				player_state = S_PLAY;
				BUTTON_SetBitmapEx(WM_GetDialogItem(hWin, GUI_ID_BUTTON2),BUTTON_BI_UNPRESSED,&bmstop,7,7);
			}
			if(tpad_flag)WM_DeleteWindow(hWin);
			GUI_Delay(10);
		}
		while(Flag_ICON000)//while(playflag==1)
		{
			i=0;
			while(playlist[play_index][i]!='\0')
			{
				music_name[i]=playlist[play_index][i];
				i++;
			}
			music_name[i]='\0';
			vs1053_player((uint8_t *)music_name,hWin);
			playflag=0;
			break;
		}
		
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/