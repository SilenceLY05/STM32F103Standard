#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "types.h"


#define HTTP_SERVER_PORT         80        //httpЭ��Ĭ��ʹ��80�˿�  https�˿ں���443

/* HTTP method */
#define METHOD_ERR               0         //����
#define METHOD_GET               1         //��ȡ��Դ
#define METHOD_HEAD              2         //��ñ����ײ�
#define METHOD_POST              3         //����ʵ������
//POST�������bodyһ�㲻Ϊ�գ�body�ľ������ݸ�ʽ��header�е�Content-Type��������body�ľ������ݳ�����header�е�Content-Length������


/* HTTP GET Method */
#define PTYPE_ERR                0         //Error file
#define PTYPE_HTML               1         //HTML file
#define PTYPE_GIF                2         //GIF file
#define PTYPE_TEXT               3         //TEXT file
#define PTYPE_JPEG               4         //JPEG file
#define PTYPE_FLASH              5         //FLASH file
#define PTYPE_MPEG               6         //MPEG file
#define PTYPE_PDF                7         //PDF file
#define PTYPE_CGI                8         //CGI file
#define PTYPE_PL                 9         //PL file



/* �����ĸ�ʽ��
	 ���󷽷�����GET/HEAD/PUT/POST����ʾ����Դ�Ĳ���
	 ����Ŀ�꣺ͨ����һ��URI����������󷽷�Ҫ��������Դ
	 �汾�� 
	 ��Ӧ���ĸ�ʽ��
	 �汾��
	 ״̬�룺һ����λ�����ô������ʽ��ʾ����Ľ��
	 ԭ����Ϊ����״̬�벹�䣬�Ǹ���ϸ�Ľ������� */
	 
	 



/* HTML Doc. for ERROR */
#define ERROR_HTML_PAGE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 78\r\n\r\n<HTML>\r\n<BODY>\r\nSorry, the page you requested was not found.\r\n</BODY>\r\n</HTML>\r\n0"

#define ERROR_REQUEST_PAGE "HTTP/1.1 400 OK\r\nContent-Type: test/html\r\nContent-Length: 50\r\n\r\n<HTML>\r\n<BODY>\r\nInvalid request.\r\n</BODY>\r\n</HTML>\r\n0"

#define ERROR_CGI_PAGE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 59\r\n\r\n<html><head><title>iWeb - Configuration</title></head><BODY>CGI command was executed.</BODY></HTML>\0"


/* HTML��Ӧͷ��ʽ */
#define RES_HTMLHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:"

/* TEXT��Ӧͷ��ʽ */
#define RES_TEXTHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-length:"

/* GIF��Ӧͷ��ʽ */
#define RES_GIFHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:image/gif\r\nContent-length:"

/* JPEG��Ӧͷ��ʽ */
#define RES_JPEGHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:image/jpeg\r\nContent-length:"

/* FLASH��Ӧͷ��ʽ */
#define RES_FLASHHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:application/x-shockwave-flash\r\nContent-length:"

/* MPEG��Ӧͷ��ʽ */
#define RES_MPEGHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:vedio/mpeg\r\nContent-length:"

/* PDF��Ӧͷ��ʽ */
#define RES_PDFHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:application/pdf\r\nContent-length:"

/* ����IO������ƽ����Ӧ */
#define DOUT_RES_1 "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nContent-Length: 1\r\n\r\n1"
#define DOUT_RES_0 "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nContent-Length: 1\r\n\r\n0"


#define MAX_URI_SIZE         1460         //(TX_RX_MAX_BUFF_SIZE /2 - sizeof(char)*2)

/* HTTP����ṹ */
typedef struct _st_http_request
{
	uint8 METHOD;                  //����ģʽ
	uint8 TYPE;                    //��������
	int8  URI[MAX_URI_SIZE];       //�����ļ���
}st_http_request;



void unescape_http_ur1(char *ur1);
void HTTP_RespsonseHead(unsigned char*buff,char type,uint32 len);
void HTTP_FindURIType(u_char *type ,char *buff);
void HTTP_ParseRequest(st_http_request *requtest,u_char  *buff);
uint8 *HTTP_GetParamValue(char *url,char *param_name);





#endif /* __HTTP_SERVER_H__ */

