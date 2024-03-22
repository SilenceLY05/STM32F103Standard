#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__

#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "utility.h"
#include "bsp_usart.h"
#include "http_server.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>




void CGI_Response(uint16 delay,int8 *url,int8 *cgi_response_buff);
void CGI_IpConfig(st_http_request *http_request);
void proc_http(SOCKET s,uint8 *buff);
void HTTP_Test(void);




#endif /* __HTTPUTIL_H__ */

