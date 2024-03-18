#include "bsp_usart.h"
#include "bsp_gsm_gprs.h"
#include "bsp_gsm_usart.h"
#include "ff.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


uint8_t currentAddIndex = 0;
uint8_t address[MAXADDSIZE] = {0};


/* ��������ֵ 0��ʾ�ɹ� 1��ʾʧ�� */
uint8_t gsm_cmd_check(char *reply)
{
	uint16_t len;
	uint16_t n;
	uint16_t off;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	redata = GSM_RX(len);      //��������
	
	*(redata + len) = '\0';
	
	GSM_DEBUG("Reply:%s",redata);
	
	n= 0;
	off=0;
	
	while((n+off)<len)
	{
		if(reply[n] == 0)               //����Ϊ�ջ��߱Ƚ����
		{
			return GSM_TRUE;
		}
		if(redata[n+off] == reply[n])
		{
			n++;                         //�ƶ�����һ����������
		}
		else
		{
			off++;                       //������һ��ƥ��
			n=0;                         //����
		}
	}
	if(reply[n] == 0)                //�պ�ƥ�����
	{
		return GSM_TRUE;
	}
	return GSM_FALSE;                //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
	
	
}

/* ����GSMָ�� ����ֵ0��ʾ���ͳɹ�1��ʾʧ�� */
uint8_t gsm_cmd(char *cmd,char *reply,uint32_t waittime)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();                   //����˽��ջ���������
	
	GSM_TX(cmd);                      //��������
	
	GSM_DEBUG("Send cmd:%s",cmd);
	
	if(reply == 0)                    //����Ҫ��������
	{
		return GSM_TRUE;
	}
	
	GSM_DELAY(waittime);             //��ʱ
	
	return gsm_cmd_check(reply);     //�Խ������ݽ��д���
}	


/* �ȴ�������Ӧ��  */
char *gsm_waitask(uint8_t waitask_hook(void))
{
	uint16_t len;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	do
	{
		redata = GSM_RX(len);                  //��������
		
		if(waitask_hook() != 0)
		{
			if(waitask_hook() == GSM_TRUE)       //���� GSM_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
			{
				redata  = 0;
				return redata;
			}
		}
	}while(len ==0);                         //��������Ϊ0ʱһֱ�ȴ�
	
	GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
	GSM_DELAY(20);                           //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
	
	return redata;
}


/* ��������ָ�� ����0��ʾ�ɹ�1ʧ�� */
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
	
	//��һ�����ź���������Ǳ�������
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
	*num = 0;           //�ַ�����β��Ҫ��0
	return GSM_TRUE;
}

/* ��ʼ�������ģ�� ����ֵ0��ɹ�1��ʾʧ�� */
uint8_t gsm_init(void)
{
	uint16_t len;
	char *redata;
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();        //����˽��ջ���������
	
	GSM_USART_Config();    //��ʼ������
	
	if(gsm_cmd("AT+CGMN\r","OK",100) != GSM_TRUE)
	{
		return GSM_FALSE;
	}
	
	redata = GSM_RX(len);
	
	if(len == 0)
	{
		return GSM_FALSE;
	}
	
	//���������GSM900A��GSM800A��GSM800C  ��"SIMCOM_GSM900A"��һ�γ�����redata�е�λ��
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


/* ���𲦴�绰�������Ƿ��ͨ�� */
void gsm_call(char *num)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	GSM_DEBUG("GSM printf : ATD%s;\r",num);
	
	GSM_USARTx_printf("ATD%s;\r",num);
	
	//������ǲ��������ݵ�
  //�����κ�Ӧ�𣬿�ͷ����"\r\n",��������Ч�������Ǵӵ�3���ֽڿ�ʼ
  //�Է��ҵ��绰��û��ͨ�������أ�BUSY
  //�Է������˵绰��+COLP: "555",129,"",0,"9648674F98DE"   OK
  //�Է������˵绰,Ȼ��ҵ���NO CARRIER
  //�Է���ʱû�ӵ绰��NO ANSWER
}

/* ����绰 ����ֵ0��ʾ�ɹ�1��ʾʧ�� */
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
	
	//ƫ������Ӧ���ַ�����ַ
	redata_off = strstr(redata,"RING");
	
	if(redata_off == NULL)
	{
		return GSM_FALSE;
	}
	
	//����ƫ�ƺ�ʣ��ĳ���
	len = len - (redata_off-redata);
	
	if(len == 0)
	{
		return GSM_FALSE;
	}
	
	//��һ��ð�ź��������Ϊ:�����롰
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
	*num = 0;           //�ַ�����β��Ҫ��0
	GSM_CLEAN_RX();
		return GSM_TRUE;
}

/* ����Ƿ��п� ����ֵ0��ʾ�ɹ� 1��ʾʧ�� */
uint8_t IsInserCard(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	return gsm_cmd("AT+CNUM\r","OK",200);
}




/* �ж��Ƿ��Ǵ�ASSIC���� str�ַ���ָ�� ��ASSIC���뷵��TRUE ��֮����FALSE */
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


/* ��GBK����ת����UCS2���� ucs2:ucs2�ַ���ָ�� gbk:GBK�ַ���ָ�� */
void gsm_gbk2usc2(char *ucs2,char *gbk)
{
	WCHAR  tmp;
	GSM_DEBUG_FUNC();
	
	while(*gbk)
	{
		if((*gbk & 0xFF) < 0x7F)//Ӣ��
		{
			*ucs2 ++ = 0;
			*ucs2 ++ = *gbk++;
		}
		else                   //����
		{
			tmp = GSM_SWAP16(*(WCHAR*)gbk);
			
			*(WCHAR*)ucs2 = GSM_SWAP16(ff_convert(tmp,1));
			
			gbk +=2;
			ucs2 +=2;
		}
	}
}


/* ���ַ�ת����16�����ַ� hex:16�����ַ��洢��λ��ָ�룬ch:�ַ� */
void gsm_char2hex(char *hex,char ch)
{
	const char numhex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	GSM_DEBUG_FUNC();
	
	*hex++=numhex[(ch & 0xF0) >> 4];
	*hex  = numhex[ch & 0x0F];
}

/* GBK����תucs2������ת��16�����ַ� */
void gsm_gbk2ucs2hex(char *ucs2hex,char *gbk)
{
	WCHAR tmp;
	GSM_DEBUG_FUNC();
	
	while(*gbk)
	{
		if((*gbk & 0xFF) < 0x7F)//Ӣ��
		{
			*ucs2hex ++ = '0';
			*ucs2hex ++ = '0';
			gsm_char2hex(ucs2hex,*gbk);
			ucs2hex += 2;
			gbk++;
		}
		else                   //����
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

/* ���Ͷ��ţ�֧�����ģ�����ΪGBK�룩 num:�绰���� smstext���������� ����0�ɹ�1ʧ�� */
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
		//Ӣ��
		//"GSM"�ַ���
		if(gsm_cmd("AT+CSCS=\"GSM\"\r","OK",100) != GSM_TRUE)  goto sms_failure;
		
		//�ı�ģʽ
		if(gsm_cmd("AT+CMGF=1\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		//���ɵ绰��������
		sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
		//���ƶ�������
		strcpy(converbuff,smstext);
	}
	else
	{
		//����
		//"UCS2"�ַ���
		if(gsm_cmd("AT+CSCS=\"UCS2\"\r","OK",100) != GSM_TRUE)  goto sms_failure;
		
		//�ı�ģʽ
		if(gsm_cmd("AT+CMGF=1\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		if(gsm_cmd("AT+CMGF=17,167,0,8\r","OK",100) != GSM_TRUE) goto sms_failure;
		
		//���ɵ绰�������UCS2�ĵ绰����(��Ҫת�� ucs2��)
		gsm_gbk2ucs2hex(converbuff,num);//ת����UCS2
		sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);
		
		//ת����������
		gsm_gbk2ucs2hex(converbuff,smstext);
	}
	
	/* ���͵绰���� */
	if(gsm_cmd(cmdbuff,">",200) == 0)
	{
		GSM_USARTx_printf("%s",converbuff);   //���Ͷ�������
		GSM_DEBUG("Send String :%s",converbuff);
		
		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);       //0x1A������
		
		//����Ƿ������
		while(gsm_cmd_check("OK") != GSM_TRUE)
		{
			if(++testsend>100)//��ȴ�10��
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
		gsm_cmd(&end,0,0);//ESC,ȡ������
		return GSM_FALSE;
	}
}


/* ��ѯ�Ƿ���յ��¶��� 0��ʾ���¶��� 1��ʾ�¶��ŵ�ַ */
uint8_t IsReceiveMS(void)
{
	uint8_t addressnum = 0;
	char *redata;
	char *redata_off;
	uint16_t len;
	
	GSM_DEBUG_FUNC();
	
	/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
	if(gsm_cmd_check("+CMTI:"))
	{
		return 0;
	}
	
	redata = GSM_RX(len);
	
	//ƫ������Ӧ���ַ�����ַ
	redata_off = strstr(redata,"+CMTI:\"SD\",");
	
	if(redata_off == NULL)
		return 0;
	
	//����ƫ�ƺ�ʣ��ĳ���
	len = len - (redata_off - redata);
	
	if(len == 0)
	{
		return 0;
	}
	
	//�ָ��ַ���   redata_offΪҪ�ֽ���ַ��� ��Ϊ�ָ���
	strtok(redata_off,",");
	
	//strtok�ָ�ڶ�����Ϊatoi���룬��ת���ɹ���Ϊ���ŵ�ַ������atoi����0
	addressnum = atoi(strtok(NULL,","));
	
	return addressnum;
	
}


/* ��ѯ�Ƿ���յ��¶��� 0��ʾ���¶��� 1��ʾ�¶��ŵ�ַ */
uint8_t IsReceiveMS_Test(void)
{
	char *redata;
	uint16_t len;
	
	/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
	if(gsm_cmd_check("+CMTI:"))
	{
		return 0;
	}
	
	redata = GSM_RX(len);       //��������
	
	//���ü�¼���ն��Ŵ洢���
	currentAddIndex = 0;
	
	for(int i = 0;i<MAXADDSIZE;i++)
	{
		address[i] = 0;
	}
	char *pos = redata;
	
	while((pos = strstr(pos,"\",")) != NULL) // +CMTI:"XX",YY   XX���ܲ�ͬ��YYΪ���Ŵ洢��ţ��� ", �ж�
	{
		pos += sizeof("\",") -1;
		
		char *numend;
		
		//strtoul(const char *nptr,char **endptr,int base)   nptr:Ҫת��Ϊ�޷��ų����͵��ַ���   
		//endptr:������char*�Ķ�������ã���ֵ�ɺ�������Ϊnptr����ֵ�����һ���ַ�  base:���Ʒ�ʽ���������2��36֮�䣬����������ֵ0
		address[currentAddIndex] = strtoul(pos,&numend,10);//ת���ַ�������ֱ�����������ַ�
		currentAddIndex++;
		pos = numend;//ת���ַ�������ֱ�����������ַ�������������Ƿ�����һ��+CMTI
		
		if(currentAddIndex == MAXADDSIZE)
		{
			break;
		}
		
	}
	return 1;
}


/* ��ȡ�������� messadd���ŵ�ַ num���淢���˺���(unicode�����ʽ���ַ���) str�����������(unicode�����ʽ���ַ���)
 * 	0��ʾʧ�� 1��ʾ�ɹ���ȡ�����ţ��ö���δ�����˴��ǵ�һ�ζ����������Զ�����׼Ϊ�Ѷ���2��ʾ�ɹ���ȡ�����ţ��ö����Ѷ� */
uint8_t readmessage(uint8_t messadd,char *num,char *str)
{
	uint16_t len;
	char *redata;
	char cmd[20] = {0};
	char result = 0;
	
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	if(messadd>MAXADDSIZE)  return 0;
	
	/*------------- ��ȡ�������� ----------------------------*/
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
	
	//��һ�����ź��������Ϊ:�������˺���"
	while(*redata != ',')
	{
		len--;
		if(len == 0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//ȥ��',"'
	
	
	while(*redata != '"')
	{
		*num++=*redata++;
			len --;
	}
	*num = 0;         //�ַ�����β��Ҫ��0
	
	while(*redata != '+')
	{
		len--;
		if(len == 0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=6;//ȥ��'+32"\r'
	
	
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;         //�ַ�����β��Ҫ��0
	
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
	
	//strlen�����ַ���hexuni�ĳ��� �����޷�������
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


/* ��ʼ��GPRS���� */
uint8_t gsm_gprs_init(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	
	//�����ƶ�̨���ΪB
	if(gsm_cmd("AT+CGCLASS=\"B\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//PDP�����ı�ʶ1��������Э�飬��������ƣ�CMNET
	if(gsm_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//���� GPRS ����
	if(gsm_cmd("AT+CGATT=1\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	GSM_CLEAN_RX();
	
	//����Ϊ GPRS ����ģʽ �������ΪCMNET
	if(gsm_cmd("AT+CIPCSGP=1,\"CMNET\"\r","OK",200) != GSM_TRUE) return GSM_FALSE;
	
	return GSM_TRUE;
	
}

/*  ����TCP���ӣ���ȴ�ʱ��20�� localport�����ض˿� serverip��������IP serverport:�������˿�*/
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


/*  ����UDP���ӣ���ȴ�ʱ��20�� localport�����ض˿� serverip��������IP serverport:�������˿�*/
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


/* ʹ��GPRS�������� ����ǰ��Ҫ�Ƚ���TPC����UDP���� strҪ���͵����� */
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
		
		//����Ƿ������
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


/* �Ͽ��������� */
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


/* �رճ��� */
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


/* ���ش�GSMģ����յ����������ݣ���ӡ������ */
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

