#include "bsp_as608.h"
#include "as608_test.h"

extern uint8_t TouchOut_int_flag;

/* 指令解析 */
void Show_Message(void)
{
	AS608_INFO("   请输入指令，输入指令后请在输入窗口按回车键后再发送指令！！！\r\n");
  AS608_INFO("   指令   ------      功能 \r\n");
  AS608_INFO("    1     ------    添加指纹 \r\n");
  AS608_INFO("    2     ------    对比指纹 \r\n");
  AS608_INFO("    3     ------    删除指定用户指纹\r\n");
  AS608_INFO("    4     ------    清空指纹库\r\n"); 
}

/* 检测与指纹模块的通信 */
void Connect_Test(void)
{
	USART_SendString(DEBUG_USARTx,"这是一个指纹模块实验\r\n");
	AS608_INFO("开始测试模块是否链接\r\n");
	
	if(PS_Connect(&AS608_Addr))
	{
		AS608_INFO("未检测到指纹模块，请检查连接！！！\r\n");
	}
	else
	{
		AS608_INFO("通讯成功\r\n");
		Show_Message();
	}
		
}

/* 录入指纹 */
void Add_FR(void)
{
	uint16_t i,j,sure,ID;
	i=j=0;
	
	while(1)
	{
		switch(j)
		{
			case 0:   /* 执行第1步 */
				i++;
				AS608_INFO("******命令：请按手指******\r\n");
				sure = PS_GetImage();   /* 录入图形 */
				if(sure == 0x00)
				{
					sure = PS_GenChar(CHAR_BUFFER1);  /* 生成特征1 */
					
					if(sure == 0x00)
					{
					  AS608_INFO("输入指纹1正常！！！\r\n");
						
						sure = PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
						if(sure == 0x00)
						{
							AS608_INFO("指纹已存在，指纹ID：%d！！！\r\n\r\n",ID);
							return;
						}
						else
						{
							i = 0;
							j = 1;     /* 跳转到第二步 */        
						}
					}
					else
					{
						ShowErrMessage(sure);
					}
				}
				else
				{
					ShowErrMessage(sure);
				}
				break;
				
			case 1:
				i++;
				AS608_INFO("******命令：请再按一次手指******\r\n");
				sure = PS_GetImage();   /* 录入图形 */
				if(sure == 0x00)
				{
					sure = PS_GenChar(CHAR_BUFFER2);  /* 生成特征2 */
					
					if(sure == 0x00)
					{
					  AS608_INFO("输入指纹2正常！！！\r\n");
						
						i=0;
						j=2;   /*跳转到第3步*/
						
					}
					else
					{
						ShowErrMessage(sure);
					}
				}
				else
				{
					ShowErrMessage(sure);
				}
				break;
				
			case 2:
				AS608_INFO("正在对比两次输入的指纹！\r\n");
			
				sure = PS_Match();     /*精确比对两枚指纹特征*/
				if(sure == 0x00)
				{
					AS608_INFO("输入指纹对比成功 ！\r\n");
					j = 3;     /* 跳转第4步 */
				}
				else
				{
					AS608_INFO("对比失败，请重新录入指纹\r\n");
					ShowErrMessage(sure);
					
					i=0;
					j=0;
				}
				break;
				
			case 3:
				AS608_INFO("正在生成指纹模块\r\n");
			
				sure = PS_RegModel();  /*合并特征（生成模板）*/
				if(sure == 0x00)
				{
					AS608_INFO("指纹模块生成成功！\r\n");
					j = 4;    /* 跳转第5步 */
				}
				else
				{
					j=0;
					ShowErrMessage(sure);
				}
				break;
				
			case 4:
				do
				{
					AS608_INFO("******命令：请输入存储ID，范围为0—239******\r\n");
					
					ID = GET_NUM();
				}while(!(ID<PS_MAXNUM));
				
				sure = PS_StoreChar(CHAR_BUFFER2,ID);   /* 储存模版 */
				if(sure == 0x00)
				{
					AS608_INFO("录入指纹模块成功！！！\r\n\r\n");
					return;
				}
				else
				{
					j = 0;
					ShowErrMessage(sure);
				}
				break;
		}
		
		SysTick_Delay_Ms(1000);
		
		if(i==4)         /*超过4次没有按手指则退出*/
		{
			AS608_INFO("录指纹失败！\r\n\r\n\r\n");
			
			break;
		}
	}
}

/* 对比指纹 */
void Compare_FR(void)
{
	uint16_t ID = 0;
	uint16_t sure;    /*初始化ID值*/  
	
	AS608_INFO("******命令：对比指纹，请按手指******\r\n");
	
	sure = PS_GetImage();
	
	if(sure == 0x00)
	{
		sure = PS_GenChar(CHAR_BUFFER1);
		if(sure == 0x00)
		{
			sure = PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
			if(sure == 0x00)
			{
				AS608_INFO ("对比指纹成功，指纹ID：%d！！！\r\n\r\n",ID);
			}
			else
			{
				ShowErrMessage(sure);
			}
		}
		else
		{
			ShowErrMessage(sure);
		}
	}
	else
	{
		ShowErrMessage(sure);
	}
}

/* 清空指纹库 */
void Clean_FR(void)
{
	uint16_t sure;
	
	AS608_INFO("清空指纹库\r\n");
	
	sure = PS_Empty();
	if(sure == 0x00)
	{
		AS608_INFO("清空指纹库成功！！！！\r\n\r\n");
		
	}
	else
	{
		ShowErrMessage(sure);
	}
}

/* 删除指定用户指纹 */
void Del_FR(void)
{
	uint32_t ID;
	uint16_t sure;
	
	AS608_INFO("删除指定用户\r\n\r\n");
	
	do
	{ 
		AS608_INFO ("******命令：请输入存储ID，范围为0—239******\r\n");
		ID=GET_NUM();
	}while(!(ID<PS_MAXNUM));         /*输入ID不能超过指纹模块最大容量*/
	
	sure = PS_DeletChar(ID,1);
	if(sure == 0x00)
	{
		AS608_INFO("删除指定用户成功！！！\r\n\r\n");
	}
	else
	{
		ShowErrMessage(sure);
	}
	
}

/* 指纹模块操作 */
void FR_Task(void)
{
	uint32_t ch;
	scanf("%d",&ch);
	
	AS608_INFO("接收到的字符为：%d\r\n",ch);
	
	switch(ch)
	{
		case 1:                                     /*添加指纹*/
			Add_FR();
			Show_Message();
		
		case 2:                                     /*比对指纹*/
			Compare_FR();
			Show_Message();
		
		case 3:                                     /*删除指定用户指纹*/
			Del_FR();
			Show_Message();
		
		case 4:                                     /*清空指纹库*/
			Clean_FR();
			Show_Message();
		break;
		
		default:
      /* 如果不是指定指令字符，打印提示信息 */
		  AS608_INFO("请输入合法指令！\r\n\r\n");
      Show_Message();
    break; 
	}
	return;		
}
