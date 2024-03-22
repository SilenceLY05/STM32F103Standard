#ifndef __SMTP_H__
#define __SMTP_H__

#include "bsp_w5500_conf.h"

extern char from[];
extern char to[];


#define waitfor220                     0           //�ȴ����ӳɹ�����ȷ��Ӧ״̬
#define waitforHELO250                 1           //�ȴ�hello�������ȷ��Ӧ״̬
#define waitforAUTH334                 2           //�ȴ�AUTH��֤�������ȷ��Ӧ
#define waitforUSER334                 3           //�ȴ������¼����½�ɹ�����ȷ��Ӧ
#define waitforPASSERWORD235           4           //�ȴ����������½�ɹ���ȷ��Ӧ
#define waitforSEND250                 5           //�ȴ���������Ϣ���ͳɹ���ȷ��Ӧ
#define waitforRCPT250                 6           //�ȴ��ռ�����Ϣ���ͳɹ���ȷ��Ӧ
#define waitforDATA354                 7           //�ȴ��������ݴ�����������ͳɹ���ȷ��Ӧ
#define waitforMIME250                 8           //�ȴ�mimeȷ�ϳɹ�


void Base64Encode(char *s,char *r);
void Str_Insert(char *s1,char *s2,int pos);
void Mail_Message(void);
void Send_Mail(void);
void SMTP_Test(void);


#endif /* __SMTP_H__ */



