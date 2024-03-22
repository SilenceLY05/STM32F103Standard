#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "tcp_demo.h"
#include <string.h>
#include <stdio.h>

uint8 buff[2048];                      //����һ��2KB�Ļ���


/* TCP Server�ػ���ʾ���� */
void Do_TCPServer(void)
{
	uint16 len = 0;
	
	switch(getSn_SR(SOCK_TCPS))          /* ��ȡsocket��״̬ */
	{
		/* socket���ڹر�״̬ */
		case Sn_SR_SOCK_CLOSE:   
			 /* ��socket ����ΪTCPģʽ�����ӳ�Ӧ�� */	
			socket(SOCK_TCPS,Sn_MR_TCP,local_port,Sn_MR_ND);    
		break;
		
		/* socket�ѳ�ʼ��״̬ */
		case Sn_SR_SOCK_INIT:
			/* socket�������� */
			Listen(SOCK_TCPS);
		break;
		
		/* socket�������ӽ���״̬ */
		case Sn_SR_SOCK_ESTABLISHED:
			/* ��ȡ�˿ڵ������жϱ�־λ */
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				/* ��������жϱ�־λ */
				setSn_IR(SOCK_TCPS,Sn_IR_CON);
			}
			
			/* ����lenΪ�ѽ������ݵĳ��� */
			len = getSn_RX_FSR(SOCK_TCPS);
			
			/* ���յ����� */
			if(len > 0)
			{
				/* ��������Client������ */
				Receive(SOCK_TCPS,buff,len);
				/* ����ַ��������� */
				buff[len] = 0x00;
				printf("%s\r\n",buff);
			
				/* ��Client�������� */
				Send(SOCK_TCPS,buff,len);
			}
		break;
		
		/* socket���ڵȴ��ر�״̬ */
		case Sn_SR_SOCK_CLOSE_WAIT:
			Close(SOCK_TCPS);
		break;
			
	}
}


/* TCP Client�ػ���ʾ���� */
void Do_TCPClient(void)
{
	uint16 len = 0;
	
	switch(getSn_SR(SOCK_TCPC))          /* ��ȡsocket��״̬ */
	{
		/* socket���ڹر�״̬ */
		case Sn_SR_SOCK_CLOSE:   
			 /* ��socket ����ΪTCPģʽ�����ӳ�Ӧ�� */	
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);    
		break;
		
		/* socket�ѳ�ʼ��״̬ */
		case Sn_SR_SOCK_INIT:
			/* socket���ӷ����� */
			Connect(SOCK_TCPC,remote_ip,remote_port);
		break;
		
		/* socket�������ӽ���״̬ */
		case Sn_SR_SOCK_ESTABLISHED:
			/* ��ȡ�˿ڵ������жϱ�־λ */
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				/* ��������жϱ�־λ */
				setSn_IR(SOCK_TCPC,Sn_IR_CON);
			}
			
			/* ����lenΪ�ѽ������ݵĳ��� */
			len = getSn_RX_FSR(SOCK_TCPC);
			
			/* ���յ����� */
			if(len > 0)
			{
				/* ��������Client������ */
				Receive(SOCK_TCPC,buff,len);
				/* ����ַ��������� */
				buff[len] = 0x00;
				printf("%s\r\n",buff);
			
				/* ��Client�������� */
				Send(SOCK_TCPC,buff,len);
			}
		break;
		
		/* socket���ڵȴ��ر�״̬ */
		case Sn_SR_SOCK_CLOSE_WAIT:
			Close(SOCK_TCPC);
		break;

	}
}


