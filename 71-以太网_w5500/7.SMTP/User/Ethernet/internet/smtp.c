#include "bsp_w5500.h"
#include "smtp.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>

extern CONFIG_MSG ConfigMsg;
uint8 mail_send_ok = 0;
char RX_BUFF[2048];
char hello[50] = "HELO local_host";                                //���ʶ������
char hello_reply[] = "250 OK";                                     //���ʶ��ɹ���Ӧ
char AUTH[50] = "AUTH LOGIN";                                      //��֤����
char AUTH_reply[] = "334 dXN1cm5hbWU6";                            //��֤�����ͳɹ�
char name_163[100] = "13015757858@163.com";                        //163��¼������
char base64name_163[200];                                          //163��¼��������base64����
char name_reply[] = "334UGFzc3dvcmQ6";                             //���͵�¼���ɹ���Ӧ
char password_163[50] = "Shihaoming512";                           //163��¼��������
char base64password_163[100];                                      //163��¼���������base64����
char password_reply[] = "235 Authentication successful";           //��¼�ɹ���Ӧ
char from[] = "13015757858@163.com";                               //����������
char from_reply[] = "250 Mail OK";                                 
char to[] = "435191950@qq.com";                                    //�ռ�������
char to_reply[] = "250 Mail OK";
char data_init[10] = "data";                                       //�������ݴ���
char data_reply[] = "354";                                         //����ɹ���ӦHEAD
char Cc[] = "221127179@fzu.edu.cn";                                //����������
char subject[] = "Hello World!";                                   //����
char content[] = "Hello World!";                                   //����
char mime_reply[] = "250 Mail OK queued as";                       //�ʼ����ͳɹ���Ӧ
char mailfrom[50] = "MAIL FROM:<>"; 
char rcptto[50] = "RCPT TO:<>";
char mime[200] = "From:\r\n";
char mime1[50] = "To:\r\n";
char mime2[50] = "Cc:\r\n";
char mime3[50] = "Subject:\r\n";
char mime4[50] = "MIME-Version:1.0\r\nContent-Type:text/plain\r\n\r\n";
char mime5[50] = "\r\n.\r\n";



/* base64����ת������ *s:��Ҫת�����ַ� *r:���ת������ַ� */
void Base64Encode(char *s,char *r)
{
	char padstr[4];  //����ַ�
	char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	uint16 i = 0,c;
	uint32 n;
	c = strlen(s)%3;
	
	if(c>0)
	{
		for(i = 0; c<3;c++)
		{
			padstr[i++] = '=';
		}
	}
	
	padstr[i] = 0;
	i = 0;
	
	for(c= 0 ;c <strlen(s);c+=3)   //ÿ�δ��������ַ�
	{
		n = s[c];
		n<<=8;
		n+=s[c+1];
		if(c+2>strlen(s))
		{
			n &=0xFF00;
		}
		n<<=8;
		n+=s[c+2];
		if(c+1>strlen(s))
		{
			n&= 0xFFFF00;
		}
		
		r[i++] = base64chars[(n>>18)&63];
		r[i++] = base64chars[(n>>12)&63];
		r[i++] = base64chars[(n>>6)&63];
		r[i++] = base64chars[n&63];
	}
	i-=strlen(padstr);
	
	for(c = 0 ;c<strlen(padstr);c++)
	{
		r[i++] = padstr[c];
	}
	r[i] = 0;
}


/* ���ַ���s2����s1��posλ��ʼ�ĵط� */
void Str_Insert(char *s1,char *s2,int pos)
{
	int i;
	int len = strlen(s2);
	for(i = 0 ;i<len;i++)
	{
		*(s1 + pos +len + i) = s1[pos + i];
		*(s1 + pos + i) = s2[i];
	}
}


/* �ʼ�������Ϣ���� */
void Mail_Message(void)
{
	uint16 len_from = strlen(from);
	uint16 len_to = strlen(to);
	uint16 len_Cc = strlen(Cc);
	uint16 len_sub = strlen(subject);
	
	/* strcat ������char�������ӣ�strcat(a,b),a.b��ָ�ڴ����򲻿��ص�����a�������㹻�Ŀռ�������b���ַ���������ָ��a��ָ�� */
	strcat(hello,"\r\n");
	strcat(AUTH,"\r\n");
	Base64Encode(name_163,base64name_163);
	Base64Encode(password_163,base64password_163);
	strcat(base64name_163,"\r\n");
	strcat(base64password_163,"\r\n");
	Str_Insert(mailfrom,from,11);
	strcat(mailfrom,"\r\n");
	Str_Insert(rcptto,to,9);
	strcat(rcptto,"\r\n");
	strcat(data_init,"\r\n");
	
	Str_Insert(mime,from,5);
	Str_Insert(mime1,to,3);
	Str_Insert(mime2,Cc,3);
	Str_Insert(mime3,subject,8);
	Str_Insert(mime5,content,0);
	strcat(mime,mime1);
	strcat(mime,mime2);
	strcat(mime,mime3);
	strcat(mime,mime4);
	strcat(mime,mime5);
	
}



/* �����ʼ� */
uint8 SMTP_STATE = waitfor220;
void Send_Mail(void)
{
	switch(SMTP_STATE)
	{
		case waitfor220:
			if(strstr((const char *)RX_BUFF,"220") != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)hello,strlen(hello));   //����helloָ��
				SMTP_STATE = waitforHELO250;
			}
			break;
			
		case waitforHELO250:
			if(strstr((const char*)RX_BUFF,hello_reply) != NULL && strstr((const char*)RX_BUFF,"Mail") == NULL)
			{
				Send(SOCK_SMTP,(const uint8*)AUTH,strlen(AUTH));
				SMTP_STATE = waitforAUTH334;
			}
			break;
			
		case waitforAUTH334:
			if(strstr((const char*)RX_BUFF,AUTH_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)base64name_163,strlen(base64name_163));
				SMTP_STATE = waitforUSER334;
			}
			break;

		case waitforUSER334:
			if(strstr((const char*)RX_BUFF,name_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)base64password_163,strlen(base64password_163));
				SMTP_STATE = waitforPASSERWORD235;
			}
			break;
			
			
		case waitforPASSERWORD235:
			if(strstr((const char*)RX_BUFF,password_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)mailfrom,strlen(mailfrom));
				SMTP_STATE = waitforSEND250;
			}
			break;
			
		case waitforSEND250:
			if(strstr((const char*)RX_BUFF,from_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)rcptto,strlen(rcptto));
				SMTP_STATE = waitforRCPT250;
			}
			break;
			
		case waitforRCPT250:
			if(strstr((const char*)RX_BUFF,to_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)data_init,strlen(data_init));
				SMTP_STATE = waitforDATA354;
			}
			break;
			
		case waitforDATA354:
			if(strstr((const char*)RX_BUFF,data_reply) != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)mime,strlen(mime));
				SMTP_STATE = waitforMIME250;
			}
			break;
			
		case waitforMIME250:
			if(strstr((const char*)RX_BUFF,mime_reply) != NULL)
			{
				mail_send_ok = 1;
				printf("mail send OK\r\n");
			}
			break;
			
		default:
			break;
				
	}
}



/* SMTPʵ�麯�� */
void SMTP_Test(void)
{
	uint8 ch = SOCK_SMTP;
	uint16 len;
	uint16 anyport = 5000;                          //����SMTP Client��ͨ�Ŷ˿ں�
	uint8 SMTP_PORT = 25;                           //SMTP Server�Ķ˿ںţ�Ĭ��Ϊ25
	memset(RX_BUFF,0,sizeof(RX_BUFF));
	switch(getSn_SR(ch))                            //��ȡW5500 socket��״̬
	{
		case Sn_SR_SOCK_INIT:
			Connect(ch,ConfigMsg.rip,SMTP_PORT);        //����SMTP Server
		break;
		
		case Sn_SR_SOCK_ESTABLISHED:                  //socket�����ɹ�
			if(getSn_IR(ch)&Sn_IR_CON)
			{
				setSn_IR(ch,Sn_IR_CON);                   //����жϱ�־λ
			}
			if((len = getSn_RX_FSR(ch)) > 0)
			{
				while(!mail_send_ok)                      //����ʼ�û�з��ͳɹ�
				{
					memset(RX_BUFF,0,sizeof(RX_BUFF));      //���ջ�����ڴ�ռ�����
					len = Receive(ch,(uint8 *)RX_BUFF,len); //W5500�������ݲ�����RX_BUFF
					Send_Mail();                            //�����ʼ�
				}
				Disconnect(ch);                           //�Ͽ�socket����
			}
			break;
			
		case Sn_SR_SOCK_CLOSE_WAIT:                   //�ȴ�socket�ر�
			if((len = getSn_RX_FSR(ch))>0)
			{
				while(!mail_send_ok)
				{
					len = Receive(ch,(uint8 *)RX_BUFF,len);
					Send_Mail();
				}
				Disconnect(ch);
			}
			break;
			
		case Sn_SR_SOCK_CLOSE:
			socket(ch,Sn_MR_TCP,anyport++,0x00);        //��socket
		break;
		
		default:
			break;
	}
	
	
}



