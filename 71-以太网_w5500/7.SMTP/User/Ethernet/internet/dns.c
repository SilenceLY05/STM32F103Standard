#include "bsp_w5500.h"
#include "bsp_w5500_conf.h"
#include "socket.h"
#include "dns.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

uint8 domain_name[] = "www.baidu.com";
uint8 dns_get_ip[4];
uint16 MSG_ID = 0x1122;
uint8 BUFFPUB[1024];




/* 生成DNS查询报文 op:操作类型 name:要查询的域名 buff:用于存放构建好的DNS缓冲区 len:缓冲区的长度 返回值：DNS查询消息的长度 */
int DNS_MakeQuery(uint16 op,uint8 *name,uint8 *buff,uint16 len)
{
	uint8 *cp;                 //用来记录报文长度
	uint8 *cp1;                //用来记录域名中两个.之间的长度
	uint8 *dname;              //用来记录域名中两个.之间的名称
	uint16 p;                  //记录操作DNS报文首部内容
	uint16 dlen;               //记录域名长度
	
	cp = buff;
	
	MSG_ID++;                  //让每次发送的事务ID不同
	
	*(uint16 *)&cp[0] = htons(MSG_ID);         //事务ID
	p = (op<<11) | 0x0100;                     //查询类型+递归查询
	*(uint16 *)&cp[2] = htons(p);              //标志字段
	*(uint16 *)&cp[4] = htons(1);              //问题计数字段
	*(uint16 *)&cp[6] = htons(0);              //回答资源计数
	*(uint16 *)&cp[8] = htons(0);              //权威服务器计数
	*(uint16 *)&cp[10] = htons(0);             //附加资源计数
	
	/* 使指针跳过头部信息 */
	cp += sizeof(uint16)*6;
	dname = name;
	
	dlen = strlen((char*)dname);
	
	for(;;)
	{
		/* strchr在一个字符串中查找给定字符的第一个匹配之处 */
		cp1 = (unsigned char*)strchr((char *)dname,'.');
		
		/* 给出第一个.前面字符的长度 */
		if(cp1)
			len = cp1 - dname;
		else
			len = dlen;
		/* 将len写入cp指向的位置并前移一位 */
		*cp++ = len;
		
		if(len == 0)
			break;
		
		/* 表示把dname开始的前len个字节复制到cp中 */
		strncpy((char *)cp,(char *)dname,len);
		cp+=len;
		/* 如果cp1为0 */
		if(!cp1)
		{
			*cp++ = 0;
			break;
			
		}
		dname += len+1;
		dlen -= len +1;
	}
	/* 跳过当前已经处理过的域名组件和. */
	*(uint16 *)&cp[0] = htons(0x0001);
	/* 更新了dlen的值 */
	*(uint16 *)&cp[2] = htons(0x0001);
	
	cp += sizeof(uint16)*2;
	
	return ((int)((uint32)cp - (uint32)buff));
	
}



/* 解析DNS报文中的压缩或非压缩域名字符串 msg:指向DNS报文的起始位置指针,compressed:指向需要解析的域名字符串指针，len:可以解析的最大长度 */
int Parse_Name(uint8 *msg,uint8 *compressed,uint16 len)
{
	uint16 slen;                 //当前段长度
	uint8 *cp;                   //指针在报文中的位置
	int16 clen = 0;              //压缩名称的总长度
	int16 indirect = 0;          //间接指引表示
	int16 nseg = 0;              //段计数
	int8 name[MAX_DNS_BUFF_SIZE];
	int8 *buff;
	
	
	buff = name;
	cp = compressed;
	
	for(;;)
	{
		/* 读取当前段的长度 */
		slen = *cp++;
		
		/* 如果没有遇到间接指引 增加压缩名长度计数 */
		if(!indirect)
			clen++;
		
		/* 检测当前段是否是一个压缩指针 前两位为11 */
		if((slen & 0xC0) == 0xC0)
		{
			if(!indirect)
				clen++;
			indirect = 1;
			
			/* 重新定位cp以跟随间接引用 */
			cp = &msg[((slen&0x3F)<<8) + *cp];
			slen = *cp++;   //读取新的字段
		}
		
		/* 如果段长为0，表示域名结束 */
		if(slen == 0)
			break;
		
		/* 更新剩余可解析的长度 */
		len -= slen +1;
		
		if(len <= 0)
			return -1;
		
		/* 如果没有遇到间接引用，增加压缩名长度计数 */
		if(!indirect)
			clen += slen;
		
		/* 读取并存储当前段 */
		while(slen-- != 0)
			*buff++ = (int8)*cp++;
		
		*buff++ = '.';
		nseg++;
		
	}
	if(nseg == 0)
	{
		*buff++ = '.';
		len--;
	}
	*buff++ = '\0';
	len--;
	
	return clen;
	
}


/* 解析回复消息的问询记录 msg:指向DNS报文的起始位置指针 cp: 指向需要解析的域名字符串指针 返回下一个指针位置*/
uint8 *DNS_Question(uint8 *msg,uint8 *cp)
{
	int16 len;
	
	len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
	
	if(len == -1)
		return 0;
	
	cp += len;
	
	cp += 2;  //加上TYPE长度
	
	cp += 2;  //加上class长度
	
	return cp;
}


/* 解析回复消息的应答记录 msg:指向DNS报文的起始位置指针 cp: 指向需要解析的域名字符串指针 返回下一个指针位置*/
uint8 *DNS_Answer(uint8 *msg,uint8 *cp)
{
	int16 len,type;
	
	len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
	
	if(len == -1)
		return 0;
	
	cp += len;
	type = ntohs(*((uint16 *)&cp[0]));
	cp += 2;  //加上TYPE长度
	cp += 2;  //加上class长度
	cp += 4;  //加上ttl长度
	cp += 2;  //加上len长度
	
	switch(type)
	{
		case TYPE_A:
			dns_get_ip[0] = *cp++;
			dns_get_ip[1] = *cp++;
			dns_get_ip[2] = *cp++;
			dns_get_ip[3] = *cp++;
			break;
		
		case TYPE_CNAME:
		case TYPE_MB:
		case TYPE_MG:
		case TYPE_MR:
		case TYPE_NS:
		case TYPE_PTR:
			/* 这些类型都是由一个域名组成 转换为ASCII格式 */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			
			cp += len;
			break;
			
		case TYPE_HINFO:
			len = *cp++;
			cp += len;
			len = *cp++;
			cp += len;
			break;
		
		case TYPE_MX:
			cp += 2;
			/* 获取交换机域名 */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			cp += len;
			break;
			
		case TYPE_SOA:
			/* 获取名称服务器域名 */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			cp+=len;
			
			/* 获取责任人域名 */
			len = Parse_Name(msg,cp,MAX_DNS_BUFF_SIZE);
			if(len == -1)
				return 0;
			
			cp += len;
			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			cp += 4;
			break;
			
		case TYPE_TXT:
			break;
		
		default:
			break;
		
	}
	return cp;
}


/* 解析来自DNS服务器的回复报文 DNSHMG:指向DNS信息头的指针 buff:接收的回复信息  */
uint8 parseMSG(struct DNSHMG *pdhdr,uint8 *pbuff)
{
	uint16 tmp;
	uint16 i;
	uint8 *msg;
	uint8 *cp;
	
	msg = pbuff;
	memset(pdhdr,0,sizeof(pdhdr));
	
	/* 获取ID */
	pdhdr->id = ntohs(*((uint16 *)&msg[0]));
	
	/* 获取FLAG部分 */
	tmp = ntohs(*((uint16 *)&msg[2]));
	if(tmp & 0x8000)
		pdhdr->qr = 1;
	
	pdhdr->opcode = (tmp >> 11) & 0xF;
	
	if(tmp & 0x0400) 
		pdhdr->aa =1;
	
	if(tmp & 0x0200) 
		pdhdr->tc =1;
	
	if(tmp & 0x0100) 
		pdhdr->rd =1;
	
	if(tmp & 0x0080) 
		pdhdr->ra =1;
	
	pdhdr->rcode = tmp & 0xF;
	pdhdr->qcount = ntohs(*((uint16 *)&msg[4]));
	pdhdr->acount = ntohs(*((uint16 *)&msg[6]));
	pdhdr->nscount = ntohs(*((uint16 *)&msg[8]));
	pdhdr->arcount = ntohs(*((uint16 *)&msg[10]));
	
	/* 解析可变长度 */
	cp = &msg[12];
	
	/* 问题部分 */
	for( i = 0; i<pdhdr->qcount;i++)
	{
		cp = DNS_Question(msg,cp);
	}
	
	/* 响应部分 */
	for(i = 0 ; i<pdhdr->acount; i++)
	{
		cp = DNS_Answer(msg,cp);
	}
	
	/* 权威名称服务器部分 */
	for(i = 0 ; i < pdhdr->nscount ;i++)
	{
		;
	}
	
	/* 额外的记录数目 */
	for(i = 0 ;i<pdhdr->arcount;i++)
	{
		;
	}
	
	if(pdhdr->rcode == 0)
		return 1;
	else
		return 0;
	
	
}


/* 查询DNS报文信息，解析来自DNS服务器的回复 s:DNS服务器socket name:要解析的信息 返回1成功 */
uint8 DNS_Query(uint8 s,uint8*name)
{
	static uint32 dns_wait_time = 0;
	struct DNSHMG dhp;                           //定义一个结构体用来包含报头文信息
	uint8 ip[4];
	uint16 len,port;
	
	/* 获取socket信息 */
	switch(getSn_SR(s))
	{
		case Sn_SR_SOCK_CLOSE:
			dns_wait_time = 0;
			/* 打开W5500 的socket 3000端口，并设置为模式 */
			socket(s,Sn_MR_UDP,3000,0);
			break;
		
		/* socket已经打开 */
		case Sn_SR_SOCK_UDP:
			len = DNS_MakeQuery(0,name,BUFFPUB,MAX_DNS_BUFF_SIZE);  //接收DNS请求报文并存入BUFFPUB
			SendTo(s,BUFFPUB,len,EXTERN_DNS_SERVERIP,IPPORT_DOMAIN);   //发送DNS请求报文给DNS服务器
		
			if((len = getSn_RX_FSR(s)) > 0)
			{
				if(len > MAX_DNS_BUFF_SIZE)
					len = MAX_DNS_BUFF_SIZE;
				
				len = RecvFrom(s,BUFFPUB,len,ip,&port);       //解析UDP传输的数据并存入BUFFPUB
				if(parseMSG(&dhp,BUFFPUB))                    //解析DNS响应信息
				{
					Close(s);
					return DNS_RET_SUCCESS;                     //关闭socket并返回DNS解析成功域名信息
				}
				else
					dns_wait_time = DNS_RESPONSE_TIMEOUT;       //等待响应时间设置为超时
			}
			else
			{
				Delay_Ms(1000);                               //没有收到DNS服务器的UDP回复，避免太频繁，延时1s
				dns_wait_time++;                              //DNS响应时间加1
			}
			
			if(dns_wait_time >= DNS_RESPONSE_TIMEOUT)       //如果DNS等待时间超过3s
			{
				Close(s);
				return DNS_RET_FAIL;
			}
			break;
	}
	return DNS_RET_PROGRESS;
}


void DNS_Test(void)
{
	uint8 dns_retry_cnt = 0;
	uint8 dns_ok = 0;
	if((dns_ok == 1) || (dns_retry_cnt > DNS_RETRY))
	{
		return;
	}
	
	/* 判断DNS服务器的IP地址是否匹配 memcmp按字节比较*/
	else if(memcmp(ConfigMsg.dns,"\x00\x00\x00\x00",4))
	{
		/* 发出DNS请求报文和解析DNS响应报文 */
		switch(DNS_Query(SOCK_DNS,domain_name))
		{
			/* DNS解析域名成功 */
			case DNS_RET_SUCCESS:
				dns_ok = 1;                              //DNS 运行标志位置1
				memcpy(ConfigMsg.rip,dns_get_ip,4);      //把解析到的IP地址复制给ConfigMsg.rip
				dns_retry_cnt = 0;                       //DNS请求报文次数置0
				printf("Get [%s]' s IP address [%d.%d.%d.%d] from %d.%d.%d.%d",domain_name,
								ConfigMsg.rip[0],ConfigMsg.rip[1],ConfigMsg.rip[2],ConfigMsg.rip[3],
								ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
				break;
			
			case DNS_RET_FAIL:
				dns_ok = 0;
				dns_retry_cnt++;
				printf("Fail! Please check your network configuration or DNS server \r\n");
				break;
			
			default:
				break;
			
		}
	}
	else
		printf("Invalid DNS server [%d.%d.%d.%d]\r\n",ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
}






