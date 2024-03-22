#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "tcp_demo.h"
#include <string.h>
#include <stdio.h>

uint8 buff[2048];                      //定义一个2KB的缓存


/* TCP Server回环演示函数 */
void Do_TCPServer(void)
{
	uint16 len = 0;
	
	switch(getSn_SR(SOCK_TCPS))          /* 获取socket的状态 */
	{
		/* socket处于关闭状态 */
		case Sn_SR_SOCK_CLOSE:   
			 /* 打开socket 配置为TCP模式，无延迟应答 */	
			socket(SOCK_TCPS,Sn_MR_TCP,local_port,Sn_MR_ND);    
		break;
		
		/* socket已初始化状态 */
		case Sn_SR_SOCK_INIT:
			/* socket建立监听 */
			Listen(SOCK_TCPS);
		break;
		
		/* socket处于连接建立状态 */
		case Sn_SR_SOCK_ESTABLISHED:
			/* 读取端口的连接中断标志位 */
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				/* 清除接收中断标志位 */
				setSn_IR(SOCK_TCPS,Sn_IR_CON);
			}
			
			/* 定义len为已接收数据的长度 */
			len = getSn_RX_FSR(SOCK_TCPS);
			
			/* 接收到数据 */
			if(len > 0)
			{
				/* 接收来自Client的数据 */
				Receive(SOCK_TCPS,buff,len);
				/* 添加字符串结束符 */
				buff[len] = 0x00;
				printf("%s\r\n",buff);
			
				/* 向Client发送数据 */
				Send(SOCK_TCPS,buff,len);
			}
		break;
		
		/* socket处于等待关闭状态 */
		case Sn_SR_SOCK_CLOSE_WAIT:
			Close(SOCK_TCPS);
		break;
			
	}
}


/* TCP Client回环演示函数 */
void Do_TCPClient(void)
{
	uint16 len = 0;
	
	switch(getSn_SR(SOCK_TCPC))          /* 获取socket的状态 */
	{
		/* socket处于关闭状态 */
		case Sn_SR_SOCK_CLOSE:   
			 /* 打开socket 配置为TCP模式，无延迟应答 */	
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);    
		break;
		
		/* socket已初始化状态 */
		case Sn_SR_SOCK_INIT:
			/* socket连接服务器 */
			Connect(SOCK_TCPC,remote_ip,remote_port);
		break;
		
		/* socket处于连接建立状态 */
		case Sn_SR_SOCK_ESTABLISHED:
			/* 读取端口的连接中断标志位 */
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				/* 清除接收中断标志位 */
				setSn_IR(SOCK_TCPC,Sn_IR_CON);
			}
			
			/* 定义len为已接收数据的长度 */
			len = getSn_RX_FSR(SOCK_TCPC);
			
			/* 接收到数据 */
			if(len > 0)
			{
				/* 接收来自Client的数据 */
				Receive(SOCK_TCPC,buff,len);
				/* 添加字符串结束符 */
				buff[len] = 0x00;
				printf("%s\r\n",buff);
			
				/* 向Client发送数据 */
				Send(SOCK_TCPC,buff,len);
			}
		break;
		
		/* socket处于等待关闭状态 */
		case Sn_SR_SOCK_CLOSE_WAIT:
			Close(SOCK_TCPC);
		break;

	}
}


