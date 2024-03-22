#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "udp_demo.h"
#include <string.h>
#include <stdio.h>
#include "utility.h"



/* UDP测试函数 */
void Do_UDP(void)
{
	uint16 len = 0;
	uint8 buff[2048];                      //定义一个2KB的缓存
	switch(getSn_SR(SOCK_UDPS))          /* 获取socket的状态 */
	{
		/* socket处于关闭状态 */
		case Sn_SR_SOCK_CLOSE:   
			 /* 打开socket 配置为UDP模式，无延迟应答 */	
			socket(SOCK_UDPS,Sn_MR_TCP,local_port,0);    
		break;

		/* socket处于UDP工作模式下 */
		case Sn_SR_SOCK_UDP:
			/* 读取端口的接收中断标志位 */
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				/* 清除接收中断标志位 */
				setSn_IR(SOCK_UDPS,Sn_IR_RECV);
			}
			
			/* 定义len为已接收数据的长度 */
			len = getSn_RX_FSR(SOCK_UDPS);
			
			/* 接收到数据 */
			if(len > 0)
			{
				/* 接收来自非TCP的数据 */
				RecvFrom(SOCK_UDPS,buff,len,remote_ip,&remote_port);
				/* 添加字符串结束符 */
				buff[len-8] = 0x00;
				printf("%s\r\n",buff);
			
				/* 发送非TCP模式的数据 */
				SendTo(SOCK_UDPS,buff,len-8,remote_ip,remote_port);
			}
			break;

	}
}




