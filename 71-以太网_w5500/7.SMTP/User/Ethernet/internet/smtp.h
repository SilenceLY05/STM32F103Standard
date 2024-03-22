#ifndef __SMTP_H__
#define __SMTP_H__

#include "bsp_w5500_conf.h"

extern char from[];
extern char to[];


#define waitfor220                     0           //等待连接成功的正确响应状态
#define waitforHELO250                 1           //等待hello命令的正确响应状态
#define waitforAUTH334                 2           //等待AUTH认证请求的正确响应
#define waitforUSER334                 3           //等待邮箱登录名登陆成功的正确响应
#define waitforPASSERWORD235           4           //等待邮箱密码登陆成功正确响应
#define waitforSEND250                 5           //等待发件人信息发送成功正确响应
#define waitforRCPT250                 6           //等待收件人信息发送成功正确响应
#define waitforDATA354                 7           //等待发送数据传输请求命令发送成功正确响应
#define waitforMIME250                 8           //等待mime确认成功


void Base64Encode(char *s,char *r);
void Str_Insert(char *s1,char *s2,int pos);
void Mail_Message(void);
void Send_Mail(void);
void SMTP_Test(void);


#endif /* __SMTP_H__ */



