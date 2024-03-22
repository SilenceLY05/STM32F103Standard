#include "bsp_w5500.h"
#include "smtp.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>

extern CONFIG_MSG ConfigMsg;
uint8 mail_send_ok = 0;
char RX_BUFF[2048];
char hello[50] = "HELO local_host";                                //身份识别命令
char hello_reply[] = "250 OK";                                     //身份识别成功响应
char AUTH[50] = "AUTH LOGIN";                                      //认证请求
char AUTH_reply[] = "334 dXN1cm5hbWU6";                            //认证请求发送成功
char name_163[100] = "13015757858@163.com";                        //163登录邮箱名
char base64name_163[200];                                          //163登录邮箱名的base64编码
char name_reply[] = "334UGFzc3dvcmQ6";                             //发送登录名成功响应
char password_163[50] = "Shihaoming512";                           //163登录邮箱密码
char base64password_163[100];                                      //163登录邮箱密码的base64编码
char password_reply[] = "235 Authentication successful";           //登录成功响应
char from[] = "13015757858@163.com";                               //发件人邮箱
char from_reply[] = "250 Mail OK";                                 
char to[] = "435191950@qq.com";                                    //收件人邮箱
char to_reply[] = "250 Mail OK";
char data_init[10] = "data";                                       //请求数据传输
char data_reply[] = "354";                                         //请求成功响应HEAD
char Cc[] = "221127179@fzu.edu.cn";                                //抄送人邮箱
char subject[] = "Hello World!";                                   //主题
char content[] = "Hello World!";                                   //正文
char mime_reply[] = "250 Mail OK queued as";                       //邮件发送成功响应
char mailfrom[50] = "MAIL FROM:<>"; 
char rcptto[50] = "RCPT TO:<>";
char mime[200] = "From:\r\n";
char mime1[50] = "To:\r\n";
char mime2[50] = "Cc:\r\n";
char mime3[50] = "Subject:\r\n";
char mime4[50] = "MIME-Version:1.0\r\nContent-Type:text/plain\r\n\r\n";
char mime5[50] = "\r\n.\r\n";



/* base64编码转换函数 *s:需要转化的字符 *r:存放转化后的字符 */
void Base64Encode(char *s,char *r)
{
	char padstr[4];  //填充字符
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
	
	for(c= 0 ;c <strlen(s);c+=3)   //每次处理三个字符
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


/* 把字符串s2插在s1的pos位开始的地方 */
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


/* 邮件命令信息处理 */
void Mail_Message(void)
{
	uint16 len_from = strlen(from);
	uint16 len_to = strlen(to);
	uint16 len_Cc = strlen(Cc);
	uint16 len_sub = strlen(subject);
	
	/* strcat 将两个char类型链接，strcat(a,b),a.b所指内存区域不可重叠，且a必须有足够的空间来容纳b的字符串，返回指向a的指针 */
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



/* 发送邮件 */
uint8 SMTP_STATE = waitfor220;
void Send_Mail(void)
{
	switch(SMTP_STATE)
	{
		case waitfor220:
			if(strstr((const char *)RX_BUFF,"220") != NULL)
			{
				Send(SOCK_SMTP,(const uint8*)hello,strlen(hello));   //发送hello指令
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



/* SMTP实验函数 */
void SMTP_Test(void)
{
	uint8 ch = SOCK_SMTP;
	uint16 len;
	uint16 anyport = 5000;                          //定义SMTP Client的通信端口号
	uint8 SMTP_PORT = 25;                           //SMTP Server的端口号，默认为25
	memset(RX_BUFF,0,sizeof(RX_BUFF));
	switch(getSn_SR(ch))                            //读取W5500 socket的状态
	{
		case Sn_SR_SOCK_INIT:
			Connect(ch,ConfigMsg.rip,SMTP_PORT);        //连接SMTP Server
		break;
		
		case Sn_SR_SOCK_ESTABLISHED:                  //socket建立成功
			if(getSn_IR(ch)&Sn_IR_CON)
			{
				setSn_IR(ch,Sn_IR_CON);                   //清除中断标志位
			}
			if((len = getSn_RX_FSR(ch)) > 0)
			{
				while(!mail_send_ok)                      //如果邮件没有发送成功
				{
					memset(RX_BUFF,0,sizeof(RX_BUFF));      //接收缓存的内存空间清零
					len = Receive(ch,(uint8 *)RX_BUFF,len); //W5500接收数据并存入RX_BUFF
					Send_Mail();                            //发送邮件
				}
				Disconnect(ch);                           //断开socket连接
			}
			break;
			
		case Sn_SR_SOCK_CLOSE_WAIT:                   //等待socket关闭
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
			socket(ch,Sn_MR_TCP,anyport++,0x00);        //打开socket
		break;
		
		default:
			break;
	}
	
	
}



