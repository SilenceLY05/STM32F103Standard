#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bsp_w5500.h"
#include "socket.h"
#include "utility.h"
#include "http_Client.h"
#include "bsp_w5500_conf.h"
#include "bsp_dht11.h"


uint8 temp_rh[2];                     //temp_rh[0]是湿度，temp_rh[1]是温度
DHT11_Data_Typedef dht11_data;


char postT[] = {
"POST /v1.0/device/351740/sensor/395542/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:1689c460bd674715079cecc4e244042b\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"{\"value\":xx}\r\n"	
};   /* xx用来填充温度数值 */


char postH[] = {
"POST /v1.0/device/351740/sensor/395544/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:1689c460bd674715079cecc4e244042b\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"{\"value\":xx}\r\n"	
};   /* xx用来填充湿度数值 */

char *post[] = {postH,postT};

uint8 yeelink_server_ip[4] = {118,190,25,51};                /* api,yeelink.net的IP地址 */
uint8 yeelink_port = 80;                                     /* api,yeelink.net的端口号 */



void HTTP_ClientTest(void)
{
	static uint8 i=0;
	uint8 ch = SOCK_HUMTEM;
	uint8 buffer[1024] = {0};
	uint16 anyport = 3000;
	
	switch(getSn_SR(ch))
	{
		case Sn_SR_SOCK_INIT:
			Connect(ch,yeelink_server_ip,yeelink_port);
		break;
		
		case Sn_SR_SOCK_ESTABLISHED:
			if(getSn_IR(ch) & Sn_IR_CON)
			{
				setSn_IR(ch,Sn_IR_CON);
			}
			
			if(DHT11_Read_TempAndHumidity(&dht11_data) == SUCCESS)
			{
				temp_rh[0] = dht11_data.humi_int;
				temp_rh[1] = dht11_data.temp_int;
				memcpy(buffer,post[i],strlen(post[i]));
				buffer[233] = temp_rh[i]/10+0x30;
				buffer[234] = temp_rh[i]%10+0x30;
				Send(ch,(const uint8 *)buffer,sizeof(buffer));           //W5500向yeelink发送数据
				
				if(i==0)
					i=1;
				else
					i=0;
				printf("i = %d,RH = %d,Temp = %d\r\n",i,temp_rh[0],temp_rh[1]);
			}
			
			Delay_Ms(500);
			
			break;
			
		case Sn_SR_SOCK_CLOSE_WAIT:
			Close(ch);
		break;
		
		case Sn_SR_SOCK_CLOSE:
			if(i==0)
				printf("Send to Yeelink:OK\r\n");
			socket(ch,Sn_MR_TCP,anyport++,0x00);
			
			break;
			
		default:
			break;
	}
}

