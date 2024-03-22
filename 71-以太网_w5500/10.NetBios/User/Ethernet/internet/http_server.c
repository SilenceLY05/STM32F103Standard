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


/* ת��ת���ַ�ΪASCII charater ����ur1����Ҫת����ҳ��ַ */
void unescape_http_ur1(char *ur1)
{
	int x,y;
	
	for(x= 0,y=0;ur1[y];++x,++y)
	{
		if((ur1[x] = ur1[y]) == '%')    //��yֵ����x��������Ƿ�Ϊ%
		{
			ur1[x] = c2d(ur1[y+1])*0x10+c2d(ur1[y+2]);   //��%����������ַ�ת����һ���ַ�
			y+=2;
		}
	}
	ur1[x] = '\0';
}



/* ִ��һ���𸴣���html,gif,jpeg... ����buff������ type���������� len�����ݳ��� */
void HTTP_RespsonseHead(unsigned char*buff,char type,uint32 len)
{
	char *head;
	char temp[10];
	memset(buff,0x00,MAX_URI_SIZE);
	
	/* �ļ����� */
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

/* Ѱ��һ��MIME�����ļ� type �������� buffMIME���ļ� */
void HTTP_FindURIType(u_char *type ,char *buff)
{
	/* ͨ����չ���������� */
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


/* ����ÿһ��http��Ӧ ����request:����һ��ָ�� buffΪ��Ӧ����*/
void HTTP_ParseRequest(st_http_request *requtest,u_char  *buff)
{
	char *nexttok;
	/* ������������ÿո�ָ� */
	nexttok = strtok((char *)buff," ");
	
	/* ���nexttokΪ0 */
	if(!nexttok)
	{
		requtest->METHOD = METHOD_ERR;
		return;
	}
	/* �Ƚ��ļ���ͷ ���Ϊ0 */
	if(!strcmp(nexttok,"GET") || !strcmp(nexttok,"get"))
	{
		requtest->METHOD = METHOD_GET;
		/* strtok�ں������������NULL��Ϊ��һ���������򽫴���һ�ηָ��λ�ü��� Ѱ����һ���ָ�ı�� */
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


/* �õ���Ӧ�����е���һ������ url��Ҫת����ҳ��ַ param_name:��Ҫ��ȡ�Ĳ����� ���ز���ֵ */
uint8 *HTTP_GetParamValue(char *url,char *param_name)
{
	uint16 len;
	uint8 *pos2;
	uint8 *name = 0;
	uint8 *ret = pub_buff;
	uint16 content_len = 0;
	int8 tmp_buff[10] = {0x00,};
	
	/* ���û�еõ���Ӧ */
	if(!url || !param_name)
	{
		return 0;
	}
	
	/* �õ���Ӧ�������ܳ��� ������תΪ���� */
	mid(url,"Content-Length:","\r\n",tmp_buff);
	content_len = atoi16(tmp_buff,10);
	
	/* ��λ�����Ŀ�ʼ��λ�� */
	url = (int8 *)strstr(url,"\r\n\r\n");
	url +=4;
	/* �����Ľ�β��ӽ����ַ� */
	url[content_len] = 0;
	
	name = (uint8 *)strstr(url,param_name);
	/* �������Ҫ��ȡ�Ĳ��� */
	if(name)
	{
		name += strlen(param_name)+1;
		/* ���Ҳ���ֵ�Ľ���λ�� �����㳤�� */
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


