#include <string.h>
#include <stdio.h>
#include "weibo.h"
#include "bsp_w5500_conf.h"
#include "bsp_w5500.h"
#include "utility.h"
#include "socket.h"


#define weibo_server   "121.194.0.143"                //΢��������IP��ַ
#define http_path      "/wiznet/"                     //HTTP·��
#define weibo_id       "3196855541@qq.com"            //����΢���˺�
#define weibo_pwd      "wildfire123"                  //����΢������
#define BUFF_SIZE      512                            //΢�����ݳ��ȣ��������ݶ�ʱ��Ҫ�޸�

unsigned char weibo_server_ip[4] = {121,194,0,143};
static unsigned int any_local_port = 1004;            //TCPsocketԶ�̶˿�
char weibo_buffer[] = {"����һ���µ�W5500�����壬�ɹ��������������!"};


/* ΢���ϴ����� weibo:Ҫ�ϴ���΢������ */
unsigned char post_weibo_update(char *weibo)
{
	char post_data[385] = {0x00,};
	char temp_buff[BUFF_SIZE] = {0x00,};
	unsigned char ret = 0;
	unsigned int len = 0;
	char *p;
	
	if(socket(SOCK_WEIBO,Sn_MR_TCP,any_local_port++,0)!= 1)           //��ʼ��TCPsocket
	{
		printf("Socket initialization failed.\r\n");
		return 0;
	}
	else
	{
		printf("Connect with weibo server.\r\n");
		ret = Connect(SOCK_WEIBO,weibo_server_ip,80);
		if(ret !=1)
		{
			printf("Connect weibo server failed\r\n");
			return 0;
		}
		else
		{
			while(getSn_SR(SOCK_WEIBO) != Sn_SR_SOCK_ESTABLISHED);          //�ȴ����ӽ����ɹ�
			printf("Connected with weibo server\r\n");
			sprintf(post_data,"id=%s&pw=%s&cmd=update&status=%s",(char*)weibo_id,(char *)weibo_pwd,weibo);
			sprintf(temp_buff,"POST %s HTTP/1.1\r\nHost:%s\r\nUser-Agent:w5500\r\nContent-Type:application\
			/x-www-form-urlencoded;charset=gb2312\r\nConnection:close\r\nContent-Length:%d\r\n\r\n%s",(char*)http_path,(char*)weibo_server,strlen(post_data),post_data);
			len = Send(SOCK_WEIBO,(unsigned char*)temp_buff,strlen(temp_buff)); //�ϴ�΢������
			while(1)
			{
				len = getSn_RX_FSR(SOCK_WEIBO);
				if(len>0)
				{
					memset(temp_buff,0x00,MAX_BUFF_SIZE);
					len = Receive(SOCK_WEIBO,(unsigned char*)temp_buff,len);       //��������΢���������ķ��ؽ��
					p = strstr(temp_buff,(char*)"\r\n\r\n")+4;                     //������Ч����
					printf("receive:%s\r\n",p);
					printf("���ͳɹ�\r\n");
					Close(SOCK_WEIBO);
					return 1;
				}
			}
		}
	}
}


void WEIBO_Test(void)
{
	post_weibo_update(weibo_buffer);
}




