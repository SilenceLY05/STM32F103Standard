#include "bsp_hc05.h"
#include "bsp_usart_blt.h"
#include "bsp_ili9341_lcd.h"
#include <string.h>
#include <stdio.h>


BLTDev bltDevList;     //�����豸�б����ڱ��������豸����

/* ��ʼ��LED GPIO */
static void HC05_GPIO_Config(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	BLT_GPIO_APBxPeriph_FUN(BLT_GPIO_INT_CLK | BLT_GPIO_KEY_CLK ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = BLT_GPIO_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLT_GPIO_INT_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLT_GPIO_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLT_GPIO_KEY_PORT,&GPIO_InitStructure);
}


/* ��HC05д����������ģ�����Ӧ commandҪ���͵����� arg���������Ϊ0ʱ������������commandҲΪ0ʱ������"AT"���� */
void WriteCommand(const char *command,const char*arg)
{
	char str_buf[50];
	
	BLT_KEY_HIGHT;
	HC05_DELAY_MS(10);
	
	if(arg && arg[0] != 0)
		sprintf(str_buf,"AT+%s%s\r\n",command,arg);
	if(command && command[0] != 0)
	{
		sprintf(str_buf,"AT+%s\r\n",command);
	}
	else
		sprintf(str_buf,"AT\r\n");
	
	HC05_DEBUG("HC05 Send:%s",str_buf);
	
	USART_SendString(HC05_USART,(uint8_t*)str_buf);
}



/* ��HC05ģ�鷢��������OK��ֻ�����ھ���OKӦ��������ȴ�5sֱ���յ�OK
 * cmd:����������ַ�������Ҫ��\r\n��clean ����������Ƿ�������ջ�������1 �����0 �����
 * ��λ���	HC05_Send_CMD("AT+RESET\r\n",1)  ����ֵ0,���óɹ�;����,����ʧ��.	*/
uint8_t HC05_Send_CMD(char *cmd,uint8_t clean)
{
	uint8_t retry = 3;     //���Դ���
	uint32_t i;            //��ʱ
	uint16_t len;          
	char *redata;
	
	while(retry--)
	{
		BLT_KEY_HIGHT;
		USART_SendString(HC05_USART,(uint8_t *)cmd);
		i = 500;
		HC05_DELAY_MS(10);
		
		do
		{
			redata = get_rebuff(&len);
			
			if(len>>0)
			{
				if(strstr(redata,"OK"))
				{
					HC05_DEBUG("Send CMD:%s",cmd);           //��ӡ���͵�����ָ��ͷ�����Ϣ
					HC05_DEBUG("receive data : %s",redata);
					
					if(clean == 1)
						clean_rebuff();
					
					return 0;   //AT ָ��ɹ�
				}
			}
			HC05_DELAY_MS(10);
		}while(i--);//��ȴ�5��
		
		HC05_DEBUG("send CMD:%s",cmd);
		HC05_DEBUG("receive data :%s",redata);
		HC05_DEBUG("HC05 send CMD fail %d times",retry);//��ʾ�ظ�ʧ��
		
	}
	
	HC05_DEBUG("HC05 send CMD fail");
	
	if(clean == 1)
		clean_rebuff();
	
	return 1;
	
}




/* ��ʼ��GPIO�ͼ��HC05ģ�� 0��ʾ���� ������ʾ������ */
uint8_t HC05_Init(void)
{
	uint8_t i;
	
	HC05_GPIO_Config();
	
	BLT_USART_Config();
	
	for(i = 0 ; i< BLTDEV_MAX_NUM;i++)
	{
		sprintf(bltDevList.unpraseAddr[i]," ");
		sprintf(bltDevList.name[i]," ");
	}
	bltDevList.num = 0;
	
	return HC05_Send_CMD("AT\r\n",1);
}



/*  ��HC05ģ�鷢��������OK��ֻ�����ھ���OKӦ�������ȴ��̶�ʱ��
 *  cmd:����������ַ�������Ҫ��\r\n�� clean ����������Ƿ�������ջ�������1 �����0 �����
 * delayms �ȴ�ʱ�䣬��λms  ��ѯ���	HC05_Send_CMD_Wait("AT+INQ\r\n",0, 5000);	 ����ֵ0,���óɹ�;����,����ʧ��.	*/
uint8_t HC05_Send_CMD_Wait(char *cmd,uint8_t clean,uint32_t delayms)
{
	uint16_t len;
	char *redata;
	
	BLT_KEY_HIGHT;
	USART_SendString(BLT_USARTx,(uint8_t*)cmd);
	HC05_DELAY_MS(10);
	
	redata = get_rebuff(&len);
	
	if(len>0)
	{
		redata[len] = '\0';
		if(redata[len] != 0)
		{
			HC05_DEBUG("send CMD:%s\r\n",cmd);
			HC05_DEBUG("receive data :%s",redata);
		}
		
		if(strstr(redata,"OK"))
		{
			if(clean == 1)
				clean_rebuff();
			
			BLT_KEY_LOW;
			return 0;         //ATָ��ɹ�
		}
	}
	BLT_KEY_LOW;
	HC05_DEBUG("HC05 Send CMD fail");
	
	if(clean == 1)
		clean_rebuff();
	
	return 1;
	
	
}

/*  ʹ��HC05͸���ַ������� str,Ҫ������ַ��� */
void HC05_SendString(char *str)
{
	BLT_KEY_LOW;
	USART_SendString(BLT_USARTx,(uint8_t *)str);
}



/* ��str�У�������ǰ���prefix�ַ���,��strΪ"abcdefg",prefixΪ"abc"������ñ������󷵻�ָ��"defg"��ָ��
 * str,ԭ�ַ��� str_length���ַ�������  prefix��Ҫ�������ַ��� ����ֵ������prefix����ַ���ָ�� */
char *skipPrefix(char *str,size_t str_length,const char*prefix)
{
	uint16_t prefix_length = strlen(prefix);
	
	if(!str || str_length != 0 || !prefix)
		return 0;
	
	if(str_length >= prefix_length && strncmp(str,prefix,prefix_length) == 0)
		return str += prefix_length;
	
	return 0;
}


/* ��stream�л�ȡһ���ַ�����line��  line,�洢����е��ַ�������  stream��ԭ�ַ���������  max_size��stream�Ĵ�С 
 * ����ֵ��line�ĳ��ȣ���stream��û�С�\0����'\r'��'\n'���򷵻�0 */
int get_line(char *line,char *stream,int max_size)
{
	char *p;
	int len = 0;
	p = stream;
	
	while(*p != '\0'&&len<max_size)
	{
		line[len++] = *p;
		p++;
		
		if(*p=='\0'&& *p == '\r')
			break;
	}
	
	if(*p != '\0' && *p != '\r' && *p != '\n')
		return 0;
	
	line[len] = '\0';
	
	return len;
	
}

/* �滻 rawstr �ַ����е����� ':'�ַ�  rawstr,Դ�ַ���  delimiter��Ҫ�滻�ɵ��ַ�  */

void rawReplaceChar(char *rawstr,char delimiter)
{
	do
	{
		rawstr = strchr(rawstr,':');
		
		if(rawstr) *rawstr = delimiter;
	}while(rawstr != NULL);
}

/* ɨ���ܱߵ������豸�����洢���豸�б��С�����ֵ �Ƿ�ɨ�赽�豸��0��ʾɨ�赽����0��ʾû��ɨ�赽 */
uint8_t parseBltAddr(void)
{
	char *redata;
	uint16_t len;
	
	//��������豸�б�
	bltDevList.num = 0;
	
	HC05_DEBUG("���ڲ�ѯ�豸�б�");
	HC05_Send_CMD("AT+INQ\r\n",0);
	
	redata = get_rebuff(&len);
	
	if(redata[0] != 0 && strstr(redata,"+INQ:") != 0 && strstr(redata,"OK") != 0)
	{
		char *pstr = strstr(redata,"+INQ:");   //��ʼ��pstr
		char *ptmp;
		int num,i;
		
		for(num = 0; num<BLTDEV_MAX_NUM;)     //������������ɨ�赽�������豸����
		{
			pstr = strstr(pstr,"+INQ");
			if(pstr != NULL)
			{
				pstr += 5;
				ptmp = strchr(pstr,',');//������ַ��ĩβ
				*ptmp = '\0';           //�ַ���������־���滻','
				HC05_DEBUG("���ڽ���Str = %s",pstr);
				
				num++;
				HC05_DEBUG("����ĵ�ַ��Ч��num = %d",num);
				
				if(num == 1)
				{
					strcpy(bltDevList.unpraseAddr[num-1],pstr);    //���Ƶ�һ��������ַ�ַ���
					HC05_DEBUG("����Ч��ַ num = %d,Addr :%s\r\n",num,bltDevList.unpraseAddr[num-1]);
				}
				for(i = 0;i<num-1;i++)//�Ƚ�ǰ��ĵ�ַ�Ƿ��ظ�
				{
					if(strcmp(pstr,bltDevList.unpraseAddr[i]) == 0)
					{
						num--;               //��������ַ�ַ������ʱ����ʾ�õ�ַ��������ĵ�ַ��ͬ���������ظ���ַ
						HC05_DEBUG("���ظ���ַ��num= %d\r\n",num);
						break;
						
					}
					
					if(i ==num-2)
					{
						strcpy(bltDevList.unpraseAddr[num-1],pstr);
						HC05_DEBUG("����Ч��ַ,num = %d,Addr = %s\r\n",num,bltDevList.unpraseAddr[num-1]);
					}
				}
				pstr = ptmp +1;  //pstr����'\0'
			}
			else
			{
				break;          //û�м�⵽"+INQ:"˵��ɨ������˳�
			}
		}
		bltDevList.num = num;//����ʵ��������ַ��Ŀ
		
		//��ӡ����������ַ
		HC05_DEBUG("Addr List(%d)",bltDevList.num);
		for(i = 0;i<num;i++)
		{
			HC05_INFO("Addr %d:%s",i,bltDevList.unpraseAddr[i]);
		}
		HC05_INFO("ɨ�����\r\n");
		
		clean_rebuff();
		HC05_Send_CMD("AT+INQC\r\n",1);   //�˳�����ѯ�ж�
		
		return 0;
	}
	HC05_INFO("û���ҵ������豸\r\n");
	
	clean_rebuff();
	HC05_Send_CMD("AT+INQC\r\n",1);  //�˳�ǰ�жϲ�ѯ
	
	return 1;
	
}

/* ��ȡԶ�������豸������  bltDev �������豸�б�ָ�� ����ֵ��0��ȡ�ɹ�����0���ɹ�  */
uint8_t getRomoteDeviceName(void)
{
	char *redata;
	uint16_t len;
	uint8_t i;
	
	char linebuff[50];
	uint16_t linelen;
	char *p;
	
	char cmdbuff[50];
	
	HC05_DEBUG("device num = %d",bltDevList.num);
	
	for(i=0;i<bltDevList.num;i++)
	{
		rawReplaceChar(bltDevList.unpraseAddr[i],',');  //��ַ��ʽ�滻Ϊ','
		sprintf(cmdbuff,"AT+RNAME?%s\r\n",bltDevList.unpraseAddr[i]);
		
		HC05_Send_CMD(cmdbuff,0);   //���ͻ�ȡ������������
		
		redata = get_rebuff(&len);
		
		if(redata[0] != 0 && strstr(redata,"OK") != 0)
		{
			linelen = get_line(linebuff,redata,len);
			
			if(linelen>50 &&linelen !=0)    linebuff[linelen] = '\0';    //�����ض�
			
			p = skipPrefix(linebuff,linelen,"+RNAME");
			if( p != 0)
			{
				strcpy(bltDevList.name[i],p);
			}
		}
		else
		{
			strcpy(bltDevList.name[i],"<���ֻ�ȡʧ��>");
			HC05_INFO("�������ֻ�ȡʧ�ܣ���\r\n");
		}
		clean_rebuff();
		
	}
	return 0 ;
}

/* ��������豸�б� */
void printfBLTInfo(void)
{
	uint8_t i;
	
	#ifdef  ENABLE_LCD_DISPLAY
	char disp_buff[100];
	
	sprintf(disp_buff,"%d device found",bltDevList.num);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,120,240,200);
	ILI9341_DispString_EN(50,120,disp_buff);
	#endif
	
	if(bltDevList.num == 0)
	{
		HC05_INFO("/*******No remote BLT Device or in SLAVE mode********/");
	}
	else
	{
		HC05_INFO("ɨ�赽%d�������豸",bltDevList.num);
		
		for(i=0;i<NumOfTask;i++)
		{
			HC05_INFO("/*******Device[%d]********/",i);
			HC05_INFO("Device Addr: %s",bltDevList.unpraseAddr[i]);
			HC05_INFO("Device Name: %s",bltDevList.name[i]);
			
			#ifdef ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			sprintf(disp_buff,"/*******Device[%d]********/",i);
			ILI9341_DispString_EN(5,140+i*60,disp_buff);
			
			sprintf(disp_buff,"Device Addr: %s",bltDevList.unpraseAddr[i]);
			ILI9341_DispString_EN(5,160+i*60,disp_buff);
			
			sprintf(disp_buff,"Device Name: %s",bltDevList.name[i]);
			ILI9341_DispString_EN(5,180+i*60,disp_buff);
			
			
			#endif
		}
	}
}


/* ɨ�������豸 ��������������HC05����� ����ֵ 0��ʾ�ɹ�����ֵ��ʾʧ�� */
uint8_t linkHC05(void)
{
	uint8_t i=0;
	char cmdbuff[100];
	
	//��ѯ�����豸��������ַ
	parseBltAddr();
	//��ȡ�����豸����
	getRomoteDeviceName();
	
	//��������豸�б�
	printfBLTInfo();
	
	for(i=0;i<bltDevList.num;i++)
	{
		if(strstr(bltDevList.name[i],"HC05") != NULL)  //��NULL��ʾ�ҵ������Ʋ���ΪHC05���豸
		{
			HC05_INFO("������Զ��HC05ģ�飬���ڽ������ӡ�����");
			
			#ifdef  ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			ILI9341_Clear(0,80,240,20);
			ILI9341_DispString_EN(5,80,"Found a HC05 ,connecting...");
			#endif
			
			sprintf(cmdbuff,"AT+PAIR = %s,20\r\n",bltDevList.unpraseAddr[i]);
			HC05_Send_CMD(cmdbuff,0);
			
			sprintf(cmdbuff,"AT+LINK = %s\r\n",bltDevList.unpraseAddr[i]);
			
			return HC05_Send_CMD(cmdbuff,0);
		}
		
		
	}
	HC05_INFO("δ�����������к���HC05���豸");
	return 1;
	
}

