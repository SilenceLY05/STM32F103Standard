#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "udp_demo.h"
#include <string.h>
#include <stdio.h>
#include "utility.h"



/* UDP���Ժ��� */
void Do_UDP(void)
{
	uint16 len = 0;
	uint8 buff[2048];                      //����һ��2KB�Ļ���
	switch(getSn_SR(SOCK_UDPS))          /* ��ȡsocket��״̬ */
	{
		/* socket���ڹر�״̬ */
		case Sn_SR_SOCK_CLOSE:   
			 /* ��socket ����ΪUDPģʽ�����ӳ�Ӧ�� */	
			socket(SOCK_UDPS,Sn_MR_TCP,local_port,0);    
		break;

		/* socket����UDP����ģʽ�� */
		case Sn_SR_SOCK_UDP:
			/* ��ȡ�˿ڵĽ����жϱ�־λ */
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				/* ��������жϱ�־λ */
				setSn_IR(SOCK_UDPS,Sn_IR_RECV);
			}
			
			/* ����lenΪ�ѽ������ݵĳ��� */
			len = getSn_RX_FSR(SOCK_UDPS);
			
			/* ���յ����� */
			if(len > 0)
			{
				/* �������Է�TCP������ */
				RecvFrom(SOCK_UDPS,buff,len,remote_ip,&remote_port);
				/* ����ַ��������� */
				buff[len-8] = 0x00;
				printf("%s\r\n",buff);
			
				/* ���ͷ�TCPģʽ������ */
				SendTo(SOCK_UDPS,buff,len-8,remote_ip,remote_port);
			}
			break;

	}
}




