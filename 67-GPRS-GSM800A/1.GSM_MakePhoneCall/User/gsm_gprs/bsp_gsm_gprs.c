#include "bsp_usart.h"
#include "bsp_gsm_gprs.h"
#include "bsp_gsm_usart.h"
#include "ff.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


uint8_t currentAddIndex = 0;
uint8_t address[MAXADDSIZE] = {0};


/* 检查命令返回值 0表示成功 1表示失败 */
uint8_t gsm_cmd_check(char *reply)
{
	uint16_t len;
	uint16_t n;
	uint16_t off;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	redata = GSM_RX(len);      //接收数据
	
	*(redata + len) = '\0';
	
	GSM_DEBUG("Reply:%s",redata);
	
	n= 0;
	off=0;
	
	while((n+off)<len)
	{
		if(reply[n] == 0)               //数据为空或者比较完毕
		{
			return GSM_TRUE;
		}
		if(redata[n+off] == reply[n])
		{
			n++;                         //移动到下一个接收数据
		}
		else
		{
			off++;                       //进行下一轮匹配
			n=0;                         //重来
		}
	}
	if(reply[n] == 0)                //刚好匹配完毕
	{
		return GSM_TRUE;
	}
	return GSM_FALSE;                //跳出循环表示比较完毕后都没有相同的数据，因此跳出
	
	
}

/* 发送GSM指令 返回值0表示发送成功1表示失败 */
uint8_t gsm_cmd(char *cmd,char *reply,uint32_t waittime)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();                   //清空了接收缓冲区数据
	
	GSM_TX(cmd);                      //发送命令
	
	GSM_DEBUG("Send cmd:%s",cmd);
	
	if(reply == 0)                    //不需要接收数据
	{
		return GSM_TRUE;
	}
	
	GSM_DELAY(waittime);             //延时
	
	return gsm_cmd_check(reply);     //对接收数据进行处理
}	


/* 等待有数据应答  */
char *gsm_waitask(uint8_t waitask_hook(void))
{
	uint16_t len;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	do
	{
		redata = GSM_RX(len);                  //接收数据
		
		if(waitask_hook() != 0)
		{
			if(waitask_hook() == GSM_TRUE)       //返回 GSM_TRUE 表示检测到事件，需要退出
			{
				redata  = 0;
				return redata;
			}
		}
	}while(len ==0);                         //接收数据为0时一直等待
	
	GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
	GSM_DELAY(20);                           //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
	
	return redata;
}


/* 本机号码指令 返回0表示成功1失败 */
uint8_t gsm_cnum(char *num)
{
	uint16_t len;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	if(gsm_cmd("AT+CNUM\r","OK",100) != GSM_TRUE)
	{
		return GSM_FALSE;
	}
	
	redata = GSM_RX(len);
	
	GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
	if(len == 0)
	{
		return GSM_FALSE;
	}
	
	//第一个逗号后面的数据是本机号码
	while(*redata != ',')
	{
		len--;
		if(len == 0)
		{
			return GSM_FALSE;
		}
		redata++;
	}
	
	while(*redata != '"')
	{
		*num++=*redata++;
			
	}
	*num = 0;           //字符串结尾需要清0
	return GSM_TRUE;
}

/* 初始化并检测模块 返回值0表成功1表示失败 */
uint8_t gsm_init(void)
{
	uint16_t len;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();        //清空了接收缓冲区数据
	
	GSM_USART_Config();    //初始化串口
	
	if(gsm_cmd("AT+CGMN\r","OK",100) != GSM_TRUE)
	{
		return GSM_FALSE;
	}
	
	redata = GSM_RX(len);
	
	if(len == 0)
	{
		return GSM_FALSE;
	}
	
	//本程序兼容GSM900A、GSM800A、GSM800C  找"SIMCOM_GSM900A"第一次出现在redata中的位置
	if(strstr(redata,"SIMCOM_GSM900A") != 0)
	{
		return GSM_TRUE;
	}
	else if(strstr(redata,"SIMCOM_GSM800C") != 0)
	{
		return GSM_TRUE;
	}
	else
		return GSM_FALSE;
	
}


/* 发起拨打电话（无论是否接通） */
void gsm_call(char *num)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	GSM_DEBUG("GSM printf : ATD%s;\r",num);
	
	GSM_USARTx_printf("ATD%s;\r",num);
	
	//拨打后是不返回数据的
  //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
  //对方挂掉电话（没接通），返回：BUSY
  //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
  //对方接听了电话,然后挂掉：NO CARRIER
  //对方超时没接电话：NO ANSWER
}

/* 来电电话 返回值0表示成功1表示失败 */
uint8_t IsRing(char *num)
{
	uint16_t len;
	char *redata;
	char *redata_off;
	
	GSM_DEBUG_FUNC();
	
	if(gsm_cmd_check("RING"))
	{
		return GSM_FALSE;
	}
	
	redata = GSM_RX(len);
	
	//偏移至响应的字符串地址
	redata_off = strstr(redata,"RING");
	
	if(redata_off == NULL)
	{
		return GSM_FALSE;
	}
	
	//计算偏移后剩余的长度
	len = len - (redata_off-redata);
	
	if(len == 0)
	{
		return GSM_FALSE;
	}
	
	//第一个冒号后面的数据为:”号码“
	while(*redata != ':')
	{
		len--;
		if(len == 0)
		{
			return GSM_FALSE;
		}
		redata++;
		
	}
	redata+=3;
	
	
	while(*redata != '"')
	{
		*num++=*redata++;
			
	}
	*num = 0;           //字符串结尾需要清0
	GSM_CLEAN_RX();
		return GSM_TRUE;
}

/* 检测是否有卡 返回值0表示成功 1表示失败 */
uint8_t IsInserCard(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	return gsm_cmd("AT+CNUM\r","OK",200);
}




/* 判断是否是纯ASSIC编码 str字符串指针 纯ASSIC编码返回TRUE 反之返回FALSE */
uint8_t IsASSIC(char *str)
{
	GSM_DEBUG_FUNC();
	
	while(*str)
	{
		if(*str > 0x7F)
		{
			return GSM_FALSE;
		}
		str++;
	}
	return GSM_TRUE;
}


/* 把GBK编码转换成UCS2编码 ucs2:ucs2字符串指针 gbk:GBK字符串指针 */
void gsm_gbk2usc2(char *ucs2,char *gbk)
{
	WCHAR  tmp;
	GSM_DEBUG_FUNC();
	
	while(*gbk)
	{
		if((*gbk & 0xFF) < 0x7F)//英文
		{
			*ucs2 ++ = 0;
			*ucs2 ++ = *gbk++;
		}
		else                   //中文
		{
			tmp = GSM_SWAP16(*(WCHAR*)gbk);
			
			*(WCHAR*)ucs2 = GSM_SWAP16(ff_convert(tmp,1));
			
			gbk +=2;
			ucs2 +=2;
		}
	}
}


/* 把字符转换成16进制字符 hex:16进制字符存储的位置指针，ch:字符 */
void gsm_char2hex(char *hex,char ch)
{
	const char numhex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	GSM_DEBUG_FUNC();
	
	*hex++=numhex[(ch & 0xF0) >> 4];
	*hex  = numhex[ch & 0x0F];
}

/* GBK编码转ucs2编码再转成16进制字符 */
void gsm_gbk2ucs2hex(char *ucs2hex,char *gbk)
{
	WCHAR tmp;
	GSM_DEBUG_FUNC();
	
	while(*gbk)
	{
		if((*gbk & 0xFF) < 0x7F)//英文
		{
			*ucs2hex ++ = '0';
			*ucs2hex ++ = '0';
			gsm_char2hex(ucs2hex,*gbk);
			ucs2hex += 2;
			gbk++;
		}
		else                   //中文
		{
			tmp = GSM_SWAP16(*(WCHAR*)gbk);
			
			tmp = ff_convert(tmp,1);
			gsm_char2hex(ucs2hex,(char)(tmp>>8));
			ucs2hex += 2;
			gsm_char2hex(ucs2hex,(char)tmp);
			ucs2hex += 2;
			gbk+=2;
		}
	}
	*ucs2hex =0;
}

/* 发送短信（支持中文，中文为GBK码） num:电话号码 smstext：短信内容 返回0成功1失败 */
uint8_t gsm_sms(char *num,char *smstext)
{
	char converbuff[160];
	char cmdbuff[80];
	char end = 0x1A;
	uint8_t testsend = 0;
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	if(IsASSIC(smstext) == GSM_TRUE)
	{
		//英文
		//"GSM"字符集
		if(gsm_cmd("AT+CSCS=\"GSM\"\r","OK",100) != GSM_TRUE)  goto sms_failure;
		
		//文本模式
		if(gsm_cmd("AT+CMGF=1\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		//生成电话号码命令
		sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
		//复制短信内容
		strcpy(converbuff,smstext);
	}
	else
	{
		//中文
		//"UCS2"字符集
		if(gsm_cmd("AT+CSCS=\"UCS2\"\r","OK",100) != GSM_TRUE)  goto sms_failure;
		
		//文本模式
		if(gsm_cmd("AT+CMGF=1\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		if(gsm_cmd("AT+CMGF=17,167,0,8\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		//生成电话号码命令，UCS2的电话号码(需要转成 ucs2码)
		gsm_gbk2ucs2hex(converbuff,num);//转换成UCS2
		sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);
		
		//转换短信内容
		gsm_gbk2ucs2hex(converbuff,smstext);
	}
	
	/* 发送电话号码 */
	if(gsm_cmd(cmdbuff,">",200) == 0)
	{
		GSM_USARTx_printf("%s",converbuff);   //发送短信内容
		GSM_DEBUG("Send String :%s",converbuff);
		
		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);       //0x1A结束符
		
		//检测是否发送完成
		while(gsm_cmd_check("OK") != GSM_TRUE)
		{
			if(++testsend>100)//最长等待10秒
			{
				goto sms_failure;
			}
			GSM_DELAY(100);
		}
		return GSM_TRUE;
	}
	else
	{
sms_failure:
		end = 0x1B;
		gsm_cmd(&end,0,0);//ESC,取消发送
		return GSM_FALSE;
	}
}


/* 查询是否接收到新短信 0表示无新短信 1表示新短信地址 */
uint8_t IsReceiveMS(void)
{
	uint8_t addressnum = 0;
	char *redata;
	char *redata_off;
	uint16_t len;
	
	GSM_DEBUG_FUNC();
	
	/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
	if(gsm_cmd_check("+CMTI:"))
	{
		return 0;
	}
	
	redata = GSM_RX(len);
	
	//偏移至响应的字符串地址
	redata_off = strstr(redata,"+CMTI:\"SD\",");
	
	if(redata_off == NULL)
		return 0;
	
	//计算偏移后剩余的长度
	len = len - (redata_off - redata);
	
	if(len == 0)
	{
		return 0;
	}
	
	//分割字符串   redata_off为要分解的字符串 ，为分隔符
	strtok(redata_off,",");
	
	//strtok分割，第二段作为atoi输入，若转换成功则为短信地址，否则atoi返回0
	addressnum = atoi(strtok(NULL,","));
	
	return addressnum;
	
}


/* 查询是否接收到新短信 0表示无新短信 1表示新短信地址 */
uint8_t IsReceiveMS_Test(void)
{
	char *redata;
	uint16_t len;
	
	/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
	if(gsm_cmd_check("+CMTI:"))
	{
		return 0;
	}
	
	redata = GSM_RX(len);       //接收数据
	
	//重置记录接收短信存储编号
	currentAddIndex = 0;
	
	for(int i = 0;i<MAXADDSIZE;i++)
	{
		address[i] = 0;
	}
	char *pos = redata;
	
	while((pos = strstr(pos,"\",")) != NULL) // +CMTI:"XX",YY   XX可能不同，YY为短信存储编号，以 ", 判断
	{
		pos += sizeof("\",") -1;
		
		char *numend;
		
		//strtoul(const char *nptr,char **endptr,int base)   nptr:要转换为无符号长整型的字符串   
		//endptr:对类型char*的对象的引用，起值由函数设置为nptr中数值后的下一个字符  base:进制方式，必须介于2和36之间，或者是特殊值0
		address[currentAddIndex] = strtoul(pos,&numend,10);//转换字符串数字直到不是数字字符
		currentAddIndex++;
		pos = numend;//转换字符串数字直到不是数字字符后，重新起点找是否还有下一个+CMTI
		
		if(currentAddIndex == MAXADDSIZE)
		{
			break;
		}
		
	}
	return 1;
}


/* 读取短信内容 messadd短信地址 num保存发件人号码(unicode编码格式的字符串) str保存短信内容(unicode编码格式的字符串)
 * 	0表示失败 1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）2表示成功读取到短信，该短信已读 */
uint8_t readmessage(uint8_t messadd,char *num,char *str)
{
	uint16_t len;
	char *redata;
	char cmd[20] = {0};
	char result = 0;
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	if(messadd>MAXADDSIZE)  return 0;
	
	/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);
	
	if(gsm_cmd(cmd,"+CMGR:",500) != GSM_TRUE)
	{
		printf("GSM_FALSE");
		return 0;
	}
	
	redata = GSM_RX(len);
	
	if(len == 0)
	{
		return 0;
	}
	
	if(strstr(redata,"UNREAD") == 0) result = 2;
	else result =1;
	
	//第一个逗号后面的数据为:”发件人号码"
	while(*redata != ',')
	{
		len--;
		if(len == 0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	
	
	while(*redata != '"')
	{
		*num++=*redata++;
			len --;
	}
	*num = 0;         //字符串结尾需要清0
	
	while(*redata != '+')
	{
		len--;
		if(len == 0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=6;//去掉'+32"\r'
	
	
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;         //字符串结尾需要清0
	
	return result;
	
}


uint8_t hexuni2gbk(char *hexuni,char *chgbk)
{
	uint16_t len=0,i=0;
	WCHAR wgbk = 0;
	WCHAR tmp = 0;
	uint8_t unitmp[4] = {0};
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	//strlen计算字符串hexuni的长度 返回无符号整型
	len = strlen(hexuni);
	
	if(!len) return GSM_FALSE;
	
	for(i=0;i<len/4;i++)
	{
		if(hexuni[4*i]>0x41) unitmp[0] = hexuni[4*i] - 0x41 +10;
		else unitmp[0] = hexuni[4*i] - 0x30;
		if(hexuni[4*i+1]>0x41) unitmp[1] = hexuni[4*i+1] - 0x41 +10;
		else unitmp[1] = hexuni[4*i+1] - 0x30;
		if(hexuni[4*i+2]>0x41) unitmp[2] = hexuni[4*i+2] - 0x41 +10;
		else unitmp[2] = hexuni[4*i+2] - 0x30;
		if(hexuni[4*i+3]>0x41) unitmp[3] = hexuni[4*i+3] - 0x41 +10;
		else unitmp[3] = hexuni[4*i+3] - 0x30;
		
		tmp = unitmp[0] *0x1000 +unitmp[1] *0x100 + unitmp[2]*16+unitmp[3];
		wgbk = ff_convert(tmp,0);
		
		if(wgbk < 0x80)
		{
			*chgbk = (char)wgbk;
			chgbk++;
		}
		else
		{
			*chgbk = (char)(wgbk>>8);
				chgbk++;
			*chgbk = (char)wgbk;
			chgbk++;
		}
	}
	*chgbk = 0;
	return GSM_TRUE;
	
}


/* 初始化GPRS网络 */
uint8_t gsm_gprs_init(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	//设置移动台类别为B
	if(gsm_cmd("AT+CGCLASS=\"B\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//PDP上下文标识1，互联网协议，接入点名称：CMNET
	if(gsm_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//附着 GPRS 网络
	if(gsm_cmd("AT+CGATT=1\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//设置为 GPRS 连接模式 ，接入点为CMNET
	if(gsm_cmd("AT+CIPCSGP=1,\"CMNET\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	return GSM_TRUE;
	
}

/*  建立TCP连接，最长等待时间20秒 localport：本地端口 serverip：服务器IP serverport:服务器端口*/
uint8_t gsm_gprs_tcp_link(char *localport,char *serverip,char *serverport)
{
	char cmd_buff[100];
	uint8_t testConnect = 0;
	
	GSM_DEBUG_FUNC();
	
	sprintf(cmd_buff,"AT+CLPORT=\"TCP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buff,"OK",100) != GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	sprintf(cmd_buff,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
	gsm_cmd(cmd_buff,0,100);
	
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE)
	{
		if(++testConnect>200)
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100);
	}
	return GSM_TRUE;
}


/*  建立UDP连接，最长等待时间20秒 localport：本地端口 serverip：服务器IP serverport:服务器端口*/
uint8_t gsm_gprs_udp_link(char *localport,char *serverip,char *serverport)
{
	char cmd_buff[100];
	uint8_t testConnect = 0;
	
	GSM_DEBUG_FUNC();
	
	sprintf(cmd_buff,"AT+CLPORT=\"UDP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buff,"OK",100) != GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	sprintf(cmd_buff,"AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);
	gsm_cmd(cmd_buff,0,100);
	
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE)
	{
		if(++testConnect>200)
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100);
	}
	return GSM_TRUE;
}


/* 使用GPRS发送数据 发送前需要先建立TPC或者UDP连接 str要发送的数据 */
uint8_t gsp_gprs_send(const char *str)
{
	char end = 0x1A;
	
	uint8_t testSend = 0;
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	if(gsm_cmd("AT+CIPSEND\r",">",500) == 0)
	{
		GSM_USARTx_printf("%s",str);
		GSM_DEBUG("Send String:%s",str);
		
		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);
		
		//检测是否发送完成
		while(gsm_cmd_check("SENDOK") != GSM_TRUE)
		{
			if(++testSend>200)
			{
				goto gprs_send_failure;
			}
			GSM_DELAY(100);
		}
		return GSM_TRUE;
	}
	else
	{
gprs_send_failure:
			end = 0x1B;
		gsm_cmd(&end,0,100);
		return GSM_FALSE;
	}
}


/* 断开网络连接 */
uint8_t gsm_gprs_link_close(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	if(gsm_cmd("AT+CIPCLOSE=1\r","OK",200) != GSM_TRUE)
	{
		return GSM_FALSE;
	}
	return GSM_TRUE;
}


/* 关闭场景 */
uint8_t gsm_gprs_shut_close(void)
{
	uint8_t check_time = 0;
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	gsm_cmd("AT+CIPSHUT\r",0,0);
	while(gsm_cmd_check("OK")!= GSM_TRUE)
	{
		if(++check_time>50)
			return GSM_FALSE;
		GSM_DELAY(200);
	
	}
	return GSM_TRUE;
}


/* 返回从GSM模块接收到的网络数据，打印到串口 */
uint8_t PostGPRS(void)
{
	char *redata;
	uint16_t len;
	
	GSM_DEBUG_FUNC();
	
	redata = GSM_RX(len);
	
	if(len ==0)
	{
		return GSM_FALSE;
	}
	
	printf("PostGPRS:%s\n",redata);
	GSM_CLEAN_RX();
	
	return GSM_TRUE;
}	

