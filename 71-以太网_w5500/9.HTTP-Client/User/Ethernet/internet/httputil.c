#include "httputil.h"
#include "webpge.h"
#include "utility.h"



#define DEBUG_HTTP

extern CONFIG_MSG ConfigMsg;
extern char tx_buff[MAX_URI_SIZE];
extern char rx_buff[MAX_URI_SIZE];
extern uint8 reboot_flag;


uint8 boundary[64];
uint8 temp_buff[460] = {0xFF,};
extern uint8 pub_buff[1024];



/* ��������������Ϣ���õ�json_callback */
static void make_basic_config_setting_json_callback(int8 *buff,CONFIG_MSG config_msg)
{
	sprintf(buff,"settingsCallBack({\"ver\":\"%d.%d\",\
	\"mac\":\"%02x:%02x:%02x:%02x:%02x:%02x\",\
	\"ip\":\"%d.%d.%d.%d\",\
	\"gw\":\"%d.%d.%d.%d\",\
	\"sub\":\"%d.%d.%d.%d\",\
	}):",config_msg.sw_ver[0],config_msg.sw_ver[1],
	config_msg.mac[0],config_msg.mac[1],config_msg.mac[2],config_msg.mac[3],config_msg.mac[4],config_msg.mac[5],
	config_msg.lip[0],config_msg.lip[1],config_msg.lip[2],config_msg.lip[3],
	config_msg.gw[0],config_msg.gw[1],config_msg.gw[2],config_msg.gw[3],
	config_msg.sub[0],config_msg.sub[1],config_msg.sub[2],config_msg.sub[3]);
}


/* ִ��HTTP��Ӧ */
void CGI_Response(uint16 delay,int8 *url,int8 *cgi_response_buff)
{
	sprintf(cgi_response_buff,"<html><head><title>iWeb - Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j + ' ';j--;setTimeout('func()',1000);if(j==0)location.href='http://%d.%d.%d.%d/';}</script></head><body onload='func()'>please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>",delay,url[0],url[1],url[2],url[3]);
	return;
}


/* ��������Ϣд��EEPROM ����http_request������һ��http����Ľṹ��ָ�� */
void CGI_IpConfig(st_http_request *http_request)
{
	uint8 *param;
	/* ��ȡ�޸ĺ��IP��ַ */
	param = HTTP_GetParamValue(http_request->URI,"ip");
	if(param)
	{
		internet_addr((uint8 *)param,ConfigMsg.lip);
	}
	
	param = HTTP_GetParamValue(http_request->URI,"gw");
	if(param)
	{
		internet_addr((uint8 *)param,ConfigMsg.gw);
	}
	
	param = HTTP_GetParamValue(http_request->URI,"sub");
	if(param)
	{
		internet_addr((uint8 *)param,ConfigMsg.sub);
	}
	
	WriteConfigToEeprom();
	
}



/* ����HTTP�����Ĳ�����HTTP��Ӧ s:http������socket buff�������������� */
void proc_http(SOCKET s,uint8 *buff)
{
	int8 *name;
	int8 req_name[32] = {0x00,};
	unsigned long file_len = 0;
	uint16 send_len = 0;
	uint8 *http_response;                  //����һ��http��Ӧ����ָ��
	st_http_request *http_request;         //����http������ͷ�Ľṹ��ָ��
	memset(tx_buff,0x00,MAX_URI_SIZE);
	
	http_response = (uint8 *)rx_buff;
	http_request = (st_http_request *)tx_buff;
	HTTP_ParseRequest(http_request,buff);              //����HTTP������ͷ
	
	switch(http_request->METHOD)
	{
		case METHOD_ERR:
			memcpy(http_response,ERROR_REQUEST_PAGE,sizeof(ERROR_REQUEST_PAGE));
			Send(s,(uint8 *)http_response,strlen((int8 const*)http_response));
		break;
		
		case METHOD_HEAD:
		
		case METHOD_GET:
			name = http_request->URI;
			if(strcmp(name,"/index.htm") == 0 || strcmp(name,"/") == 0 || strcmp(name,"/index.html") == 0)
			{
				file_len = strlen(INDEX_HTML);
				HTTP_RespsonseHead((uint8 *)http_response,PTYPE_HTML,file_len);
				Send(s,http_response,strlen((char const*)http_response));
				send_len = 0;
				while(file_len)
				{
					if(file_len >1024)
					{
						if(getSn_SR(s) != Sn_SR_SOCK_ESTABLISHED)
						{
							return;
						}
						Send(s,(uint8 *)INDEX_HTML+send_len,1024);
						send_len += 1024;
						file_len -= 1024;
					}
					else
					{
						Send(s,(uint8 *)INDEX_HTML+send_len,file_len);
						send_len += file_len;
						file_len -= file_len;
					}
					
				}
			}
			else if(strcmp(name,"/w5500.js") == 0)
			{
				memset(tx_buff,0,MAX_URI_SIZE);
				make_basic_config_setting_json_callback(tx_buff,ConfigMsg);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buff),tx_buff);
				Send(s,(u_char*)http_response,strlen((char const*)http_response));
			}
			break;
			
		case METHOD_POST:
			mid(http_request->URI,"/"," ",req_name);          //��ȡ�������ļ���
			if(strcmp(req_name,"config.cgi") == 0)
			{
				CGI_IpConfig(http_request);                     //��������Ϣд����Ƭ��
				CGI_Response(5,(int8 *)ConfigMsg.lip,tx_buff);  //������Ӧ���ı�����
				
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buff),tx_buff);
				
				/* ����HTTP��Ӧ */
				Send(s,(u_char*)http_response,strlen((char*)http_response));
				Disconnect(s);
				reboot_flag =1;
				return;
				
			}
			break;
			
		default:
			break;
		
			
	}
}


/* ���HTTP��Ӧ */
void HTTP_Test(void)
{
	uint8 ch = SOCK_HTTPS;
	uint16 len;
	
	/* ����һ���ṹ��ָ�� */
	st_http_request *http_request;
	memset(rx_buff,0x00,MAX_URI_SIZE);
	
	http_request = (st_http_request*)rx_buff;
	
	switch(getSn_SR(ch))             //��ȡsocket״̬
	{
		case Sn_SR_SOCK_INIT:          //���ڳ�ʼ��״̬
			Listen(ch);
		break;
		
		case Sn_SR_SOCK_LISTEN:
			break;
		
		case Sn_SR_SOCK_ESTABLISHED:
			if(getSn_IR(ch) & Sn_IR_CON)
			{
				setSn_IR(ch,Sn_IR_CON);           //����жϱ�־λ
			}
			
			if((len = getSn_RX_FSR(ch)) > 0)
			{
				len = Receive(ch,(uint8 *)http_request,len);        //����HTTP����
				*(((uint8 *)http_request) + len) = 0;
				proc_http(ch,(uint8 *)http_request);                //����HTTP���󲢷���HTTP��Ӧ
				Disconnect(ch);
			}
			break;
			
		case Sn_SR_SOCK_CLOSE_WAIT:                  //���ڵȴ��ر�״̬
			if((len = getSn_RX_FSR(ch))>0)
			{
				len = Receive(ch,(uint8 *)http_request,len);
				*(((uint8 *)http_request)+len) = 0;
				proc_http(ch,(uint8 *)http_request);
			}
			Disconnect(ch);
		break;

		case Sn_SR_SOCK_CLOSE:
			socket(ch,Sn_MR_TCP,80,0x00);
		break;
		
		default:
		break;
			
	}
}


