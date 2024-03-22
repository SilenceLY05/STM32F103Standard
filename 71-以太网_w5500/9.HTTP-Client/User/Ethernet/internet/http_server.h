#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "types.h"


#define HTTP_SERVER_PORT         80        //http协议默认使用80端口  https端口号是443

/* HTTP method */
#define METHOD_ERR               0         //错误
#define METHOD_GET               1         //获取资源
#define METHOD_HEAD              2         //获得报文首部
#define METHOD_POST              3         //传输实体主体
//POST请求里的body一般不为空，body的具体数据格式由header中的Content-Type来描述，body的具体数据长度由header中的Content-Length来描述


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



/* 请求报文格式：
	 请求方法：如GET/HEAD/PUT/POST，表示对资源的操作
	 请求目标：通常是一个URI，标记了请求方法要操作的资源
	 版本号 
	 响应报文格式：
	 版本号
	 状态码：一个三位数，用代码的形式表示处理的结果
	 原因：作为数字状态码补充，是更详细的解释文字 */
	 
	 



/* HTML Doc. for ERROR */
#define ERROR_HTML_PAGE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 78\r\n\r\n<HTML>\r\n<BODY>\r\nSorry, the page you requested was not found.\r\n</BODY>\r\n</HTML>\r\n0"

#define ERROR_REQUEST_PAGE "HTTP/1.1 400 OK\r\nContent-Type: test/html\r\nContent-Length: 50\r\n\r\n<HTML>\r\n<BODY>\r\nInvalid request.\r\n</BODY>\r\n</HTML>\r\n0"

#define ERROR_CGI_PAGE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 59\r\n\r\n<html><head><title>iWeb - Configuration</title></head><BODY>CGI command was executed.</BODY></HTML>\0"


/* HTML响应头格式 */
#define RES_HTMLHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:"

/* TEXT响应头格式 */
#define RES_TEXTHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-length:"

/* GIF响应头格式 */
#define RES_GIFHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:image/gif\r\nContent-length:"

/* JPEG响应头格式 */
#define RES_JPEGHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:image/jpeg\r\nContent-length:"

/* FLASH响应头格式 */
#define RES_FLASHHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:application/x-shockwave-flash\r\nContent-length:"

/* MPEG响应头格式 */
#define RES_MPEGHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:vedio/mpeg\r\nContent-length:"

/* PDF响应头格式 */
#define RES_PDFHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type:application/pdf\r\nContent-length:"

/* 数字IO输出控制结果响应 */
#define DOUT_RES_1 "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nContent-Length: 1\r\n\r\n1"
#define DOUT_RES_0 "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nContent-Length: 1\r\n\r\n0"


#define MAX_URI_SIZE         1460         //(TX_RX_MAX_BUFF_SIZE /2 - sizeof(char)*2)

/* HTTP请求结构 */
typedef struct _st_http_request
{
	uint8 METHOD;                  //请求模式
	uint8 TYPE;                    //请求类型
	int8  URI[MAX_URI_SIZE];       //请求文件名
}st_http_request;



void unescape_http_ur1(char *ur1);
void HTTP_RespsonseHead(unsigned char*buff,char type,uint32 len);
void HTTP_FindURIType(u_char *type ,char *buff);
void HTTP_ParseRequest(st_http_request *requtest,u_char  *buff);
uint8 *HTTP_GetParamValue(char *url,char *param_name);





#endif /* __HTTP_SERVER_H__ */

