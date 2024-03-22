#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "utility.h"
#include "http_server.h"


char tx_buff[MAX_URI_SIZE];
char rx_buff[MAX_URI_SIZE];

uint8 pub_buff[1024];


/* 转化转义字符为ASCII charater 输入ur1：需要转化网页地址 */
void unescape_http_ur1(char *ur1)
{
	int x,y;
	
	for(x= 0,y=0;ur1[y];++x,++y)
	{
		if((ur1[x] = ur1[y]) == '%')    //将y值赋给x，并检查是否为%
		{
			ur1[x] = c2d(ur1[y+1])*0x10+c2d(ur1[y+2]);   //将%后面的两个字符转换成一个字符
			y+=2;
		}
	}
	ur1[x] = '\0';
}



/* 执行一个答复，如html,gif,jpeg... 输入buff答复数据 type答复数据类型 len答复数据长度 */
void HTTP_RespsonseHead(unsigned char*buff,char type,uint32 len)
{
	char *head;
	char temp[10];
	memset(buff,0x00,MAX_URI_SIZE);
	
	/* 文件类型 */
	if(type == PTYPE_HTML) head = RES_HTMLHEAD_OK;
	else if(type == PTYPE_GIF) head = RES_GIFHEAD_OK;
	else if(type == PTYPE_TEXT) head = RES_TEXTHEAD_OK;
	else if(type == PTYPE_FLASH) head = RES_FLASHHEAD_OK;
	else if(type == PTYPE_JPEG) head = RES_JPEGHEAD_OK;
	else if(type == PTYPE_PDF) head = RES_PDFHEAD_OK;
	else if(type == PTYPE_MPEG)head = RES_MPEGHEAD_OK;
	
	sprintf(temp,"%ld",len);
	strcpy((char*)buff,head);
	strcat((char*)buff,temp);
	strcat((char*)buff,"\r\n\r\n");
}

/* 寻找一个MIME类型文件 type 数据类型 buffMIME型文件 */
void HTTP_FindURIType(u_char *type ,char *buff)
{
	/* 通过扩展名决定类型 */
	if(strstr(buff,".pl"))
		*type = PTYPE_PL;
	else if(strstr(buff,".html") || strstr(buff,".htm"))
		*type = PTYPE_HTML;
	else if(strstr(buff,".gif"))
		*type = PTYPE_GIF;
	else if(strstr(buff,".text") || strstr(buff,".txt"))
		*type = PTYPE_TEXT;
	else if(strstr(buff,".jpeg") || strstr(buff,".jpg"))
		*type = PTYPE_JPEG;
	else if(strstr(buff,".swf"))
		*type = PTYPE_FLASH;
	else if(strstr(buff,".mpeg") || strstr(buff,".mpg"))
		*type = PTYPE_MPEG;
	else if(strstr(buff,".pdf"))
		*type = PTYPE_PDF;
	else if(strstr(buff,".cgi") || strstr(buff,".CGI"))
		*type = PTYPE_CGI;
	else if(strstr(buff,".js") || strstr(buff,".XML"))
		*type = PTYPE_HTML;
	else
		*type = PTYPE_ERR;
	
}


/* 解析每一个http响应 输入request:定义一个指针 buff为响应数据*/
void HTTP_ParseRequest(st_http_request *requtest,u_char  *buff)
{
	char *nexttok;
	/* 将传入的数据用空格分隔 */
	nexttok = strtok((char *)buff," ");
	
	/* 如果nexttok为0 */
	if(!nexttok)
	{
		requtest->METHOD = METHOD_ERR;
		return;
	}
	/* 比较文件开头 相等为0 */
	if(!strcmp(nexttok,"GET") || !strcmp(nexttok,"get"))
	{
		requtest->METHOD = METHOD_GET;
		/* strtok在后续调用中如果NULL作为第一个参数，则将从上一次分割的位置继续 寻找下一个分割的标记 */
		nexttok = strtok(NULL," ");
	}
	else if(!strcmp(nexttok,"HEAD") || !strcmp(nexttok,"head"))
	{
		requtest->METHOD = METHOD_HEAD;
		nexttok = strtok(NULL," ");
	}
	else if(!strcmp(nexttok,"POST") || !strcmp(nexttok,"post"))
	{
		nexttok = strtok(NULL,"\0");
		requtest->METHOD = METHOD_POST;
	}
	else
	{
		requtest->METHOD = METHOD_ERR;
	}
	
	if(!nexttok)
	{
		requtest->METHOD = METHOD_ERR;
		return;
	}
	strcpy(requtest->URI,nexttok);
}


/* 得到响应过程中的下一个参数 url需要转化网页地址 param_name:想要获取的参数名 返回参数值 */
uint8 *HTTP_GetParamValue(char *url,char *param_name)
{
	uint16 len;
	uint8 *pos2;
	uint8 *name = 0;
	uint8 *ret = pub_buff;
	uint16 content_len = 0;
	int8 tmp_buff[10] = {0x00,};
	
	/* 如果没有得到响应 */
	if(!url || !param_name)
	{
		return 0;
	}
	
	/* 得到响应的正文总长度 将长度转为整型 */
	mid(url,"Content-Length:","\r\n",tmp_buff);
	content_len = atoi16(tmp_buff,10);
	
	/* 定位到正文开始的位置 */
	url = (int8 *)strstr(url,"\r\n\r\n");
	url +=4;
	/* 在正文结尾添加结束字符 */
	url[content_len] = 0;
	
	name = (uint8 *)strstr(url,param_name);
	/* 如果存在要获取的参数 */
	if(name)
	{
		name += strlen(param_name)+1;
		/* 查找参数值的结束位置 并计算长度 */
		pos2 = (uint8 *)strstr((char*)name,"&");
		if(!pos2)
		{
			pos2 = name + strlen((char*)name);
			
		}
		len = 0;
		len = pos2 - name;
		
		if(len)
		{
			ret[len] = 0;
			strncpy((char*)ret,(char *)name,len);
			unescape_http_ur1((char *)ret);
			replacetochar((char *)ret,'+',' ');
			
		}
		else
			ret[0] = 0;
	}
	else
		return 0;
	return ret;
}


