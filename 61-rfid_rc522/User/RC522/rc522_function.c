#include "rc522_config.h"
#include "rc522_function.h"
#include "bsp_systick.h"
#include "bsp_usart.h"

#define RC522_DELAY()          Delay_us(2)

/* 向RC522发送1byte数据 byte要发送的数据 */
void SPI_RC522_SendByte(uint8_t byte)
{
	uint8_t counter;
	
	for(counter = 0; counter < 8;counter ++)
	{
		if(byte&0x80)
			RC522_MOSI_1();
		else
			RC522_MOSI_0();
		
		RC522_DELAY();
		RC522_SCK_0();
		
		RC522_DELAY();
		RC522_SCK_1();
		
		RC522_DELAY();
		byte<<=1;
	}
}

/* 从RC522读取 1byte数据 */
uint8_t SPI_RC522_ReadByte(void)
{
	uint8_t counter;
	uint8_t SPI_DATA;
	
	for(counter = 0; counter < 8;counter ++)
	{
		SPI_DATA <<= 1;
		RC522_SCK_0();
		
		RC522_DELAY();
		if(RC522_MISO_GET() ==1)
			SPI_DATA |=0x01;
		
		RC522_DELAY();
		RC522_SCK_1();
		
		RC522_DELAY();
	}
	return SPI_DATA;
}

/* 读RC522寄存器 usAddress寄存器地址 返回寄存器当前值 */
uint8_t ReadRawRC(uint8_t ucAddress)
{
	uint8_t ucAddr,ucReturn;
	
	//将寄存器地址左移一位，并将最高位置为1，其他位保持不变
	ucAddr = ((ucAddress << 1) & 0x7E ) | 0x80;
	RC522_CS_Enable();
	
	SPI_RC522_SendByte(ucAddr);
	ucReturn = SPI_RC522_ReadByte();
	
	RC522_CS_Disable();
	return ucReturn;
	
}

/* 写RC522寄存器 ucAddress寄存器地址 ucValue写入值 */
void WriteRawRC(uint8_t ucAddress , uint8_t ucValue)
{
	uint8_t ucAddr;
	//将寄存器地址左移一位，并将最高位置为0
	ucAddr = (ucAddress << 1) & 0x7E;
	RC522_CS_Enable();
	
	SPI_RC522_SendByte(ucAddr);
	SPI_RC522_SendByte(ucValue);
	
	RC522_CS_Disable();
	
}

/* 对寄存器地址置位 ucReg寄存器地址 ucMask置位值 */
void SetBitMask(uint8_t ucReg,uint8_t ucMask)
{
	uint8_t ucTemp;
	
	ucTemp = ReadRawRC(ucReg);
	WriteRawRC(ucReg,ucMask | ucTemp);
}

/* 对寄存器进行清位 ucReg寄存器地址 ucMask清位值 */
void ClearBitMask(uint8_t ucReg,uint8_t ucMask)
{
	uint8_t ucTemp;
	ucTemp = ReadRawRC(ucReg);
	WriteRawRC(ucReg,ucTemp & (~ucMask));
}

/* 开启天线 */
void PcdAntennaOn(void)
{
	uint8_t uc;
	
	uc = ReadRawRC(TxControlReg);
	if(!(uc&0x03))
		SetBitMask(TxControlReg,0x30);
}

/* 关闭天线 */
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg,0x30);
}

/* 复位RC522 */
void PcdReset(void)
{
	RC522_RST_Disable();
	Delay_us(1);
	
	RC522_RST_Enable();
	Delay_us(1);
	
	RC522_RST_Disable();
	Delay_us(1);
	
	WriteRawRC(CommandReg,0x0F);
	
	while(ReadRawRC(CommandReg)&0x10);
	
	Delay_us(1);
	
	//定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
	WriteRawRC(ModeReg,0x3D);
	WriteRawRC(TReloadRegL,30);          //16位定时器低位 
	WriteRawRC(TReloadRegH,0);           //16位定时器高位
	WriteRawRC(TModeReg,0x8D);           //定义内部定时器的设置
	WriteRawRC(TPrescalerReg,0x3E);      //设置定时器分频系数
	WriteRawRC(TxAutoReg,0x40);          //调制发送信号为100%ASK	
}

/* 设置RC522的工作方式 ucType工作方式 */
void M500PcdConfigISOType(uint8_t ucType)
{
	if(ucType == 'A')             //ISO14443_A
	{
		ClearBitMask(Status2Reg,0x08);
		
		WriteRawRC(ModeReg,0x3D);
		WriteRawRC(RxSelReg,0x86);
		WriteRawRC(RFCfgReg,0x7F);
		WriteRawRC(TReloadRegL,30);
		WriteRawRC(TReloadRegH,0);
		WriteRawRC(TModeReg,0x8D);
		WriteRawRC(TPrescalerReg,0x3E);
		Delay_us(2);
		PcdAntennaOn();//开天线
	}
}

/* 通过RC522和ISO14443卡通讯  ucCommand,RC522命令字 pInData，通过RC522发送到卡片的数据 ucInLenByte 发送数据的字节长度
 * pOutData，接收到的卡片返回数据 pOutLenBit，返回数据的位长度 返回状态值=MI_OK成功*/
char PcdComMF522(uint8_t ucCommand,uint8_t *pInData,uint8_t ucInLenByte,uint8_t *pOutData,uint32_t *pOutLenBit)
{
	char cStatus = MI_ERR;
	uint8_t ucIrqEn = 0x00;
	uint8_t ucWairFor = 0x00;
	uint8_t ucLastBits;
	uint8_t ucN;
	uint32_t ul;
	
	switch(ucCommand)
	{
		case PCD_AUTHENT:                      //Mifare认证
			ucIrqEn = 0x12;                      //允许错误中断请求ErrIEn  允许空闲中断IdleIEn
			ucWairFor = 0x10;                    //认证寻卡等待时候 查询空闲中断标志位
			break;
		
		case PCD_TRANSCEIVE:                   //接收发送 发送接收
			ucIrqEn = 0x77;                      //允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
			ucWairFor = 0x30;                    //寻卡等待时候 查询接收中断标志位与 空闲中断标志位
			break;
		
		default:
			break;
	}
	
	//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
	WriteRawRC(ComIEnReg,ucIrqEn | 0x80);
	//Set1该位清零时，CommIRqReg的屏蔽位清零
	ClearBitMask(ComIrqReg,0x80);
	//写空闲命令
	WriteRawRC(CommandReg,PCD_IDLE);
	//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
	SetBitMask(FIFOLevelReg,0x80);
	
	for(ul=0;ul<ucInLenByte;ul++)
		WriteRawRC(FIFODataReg,pInData[ul]);    //写数据进FIFOdata
	
	WriteRawRC(CommandReg,ucCommand);         //写命令
	
	if(ucCommand == PCD_TRANSCEIVE)
		//StartSend置位启动数据发送 该位与收发命令使用时才有效
		SetBitMask(BitFramingReg,0x80);
	
	ul=1000;                                  //根据时钟频率调整，操作M1卡最大等待时间25ms
	do                                        //认证 与寻卡等待时间	
	{
		ucN = ReadRawRC(ComIrqReg);             //查询事件中断
		ul--;
	}while((ul != 0) && (!(ucN &0x01)) && (!(ucN&ucWairFor)));
	
	ClearBitMask(BitFramingReg,0x80);         //清理允许StartSend位
	
	if(ul != 0)
	{
		//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
		if(!(ReadRawRC(ErrorReg) & 0x1B))
		{
			cStatus = MI_OK;
			
			if(ucN & ucIrqEn & 0x01)              //是否发生定时器中断
				cStatus = MI_NOTAGERR;
			
			if(ucCommand == PCD_TRANSCEIVE)
			{
				//读FIFO中保存的字节数
				ucN = ReadRawRC(FIFOLevelReg);
				
				//最后接收到得字节的有效位数
				ucLastBits = ReadRawRC(ControlReg) & 0x07;
				
				if(ucLastBits)
					//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
					* pOutLenBit = (ucN -1) *8 + ucLastBits;
				else
					* pOutLenBit = ucN * 8;//最后接收到的字节整个字节有效
				
				if(ucN==0)
					ucN = 1;
				if(ucN>MAXRLEN)
					ucN = MAXRLEN;
				
				for(ul = 0;ul < ucN;ul++)
					pOutData[ul] = ReadRawRC(FIFODataReg);
				
			}
		}
		else
			cStatus = MI_ERR;
	}
	SetBitMask(ControlReg,0x80);
	WriteRawRC(CommandReg,PCD_IDLE);
	
	return cStatus;
	
}

/* 寻卡 ucReq_code，寻卡方式 = 0x52，寻感应区内所有符合14443A标准的卡；寻卡方式= 0x26，寻未进入休眠状态的卡
 * pTagType，卡片类型代码 = 0x4400，Mifare_UltraLight or = 0x0400，Mifare_One(S50) or = 0x0200，Mifare_One(S70)
 * or = 0x0800，Mifare_Pro(X)) or  = 0x4403，Mifare_DESFire 返回值 状态值= MI_OK，成功*/
char PcdRequest(uint8_t ucReq_code,uint8_t *pTagType)
{
	char cStatus;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
	ClearBitMask(Status2Reg,0x08);
	//发送的最后一个字节的 七位
	WriteRawRC(BitFramingReg,0x07);
	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
	SetBitMask(TxControlReg,0x03);
	
	ucComMF522Buf[0] = ucReq_code;  //存入 卡片命令字
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&ulLen);   //寻卡  
	
	if((cStatus == MI_OK) && (ulLen == 0x10))   //寻卡成功返回卡类型 
	{
		*pTagType = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else 
		cStatus = MI_ERR;
	
	return cStatus;
	
}

/* 防冲撞 pSnr卡片序列号，4字节 返回状态值=MI_OK 成功 */
char PcdAnticoll(uint8_t *pSnr)
{
	char cStatus;
	uint8_t uc,ucSnr_check = 0;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
	ClearBitMask(Status2Reg,0x08);
	//清理寄存器 停止收发
	WriteRawRC(BitFramingReg,0x00);
	//清ValuesAfterColl所有接收的位在冲突后被清除
	ClearBitMask(CollReg,0x80);
	
	ucComMF522Buf[0] = 0x93;                      //卡片防冲突命令
	ucComMF522Buf[1] = 0x20;
	
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&ulLen);
	
	if(cStatus == MI_OK)
	{
		for(uc=0;uc<4;uc++)
		{
			*(pSnr + uc) = ucComMF522Buf[uc];    //读出UID
			//异或 相同取0 不同取1
			ucSnr_check ^= ucComMF522Buf[uc];
		}
		
		if(ucSnr_check != ucComMF522Buf[uc])
			cStatus = MI_ERR;
			
	}
	SetBitMask(CollReg,0x80);
	
	return cStatus;
	
}

/* 用RC522计算CRC16 pIndata,计算CRC16的数组，ucLen,计算CRC16的数组字节长度 pOutData，存放计算结果存放的首地址 */
void CalulateCRC(uint8_t *pIndata,uint8_t ucLen,uint8_t *pOutData)
{
	uint8_t uc,ucN;
	
	//当CRC命令有效且所有数据被处理时该位置位
	ClearBitMask(DivIrqReg,0x04);
	
	//写空闲命令
	WriteRawRC(CommandReg,PCD_IDLE);
	
	//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
	ClearBitMask(FIFOLevelReg,0x80);
	
	//内部64字节FIFO缓冲区的数据输入和输出口
	for(uc=0;uc<ucLen;uc++)
	WriteRawRC(FIFODataReg,*(pIndata+uc));
	
	//写CRC计算命令
	WriteRawRC(CommandReg,PCD_CALCCRC);
	
	uc = 0xFF;
	
	do
	{
		//读取中断请求标志寄存器数据
		ucN = ReadRawRC(DivIrqReg);
		uc--;
	}while((uc!=0) && !(ucN & 0x04));
	
	pOutData[0] = ReadRawRC(CRCResultRegL);
	pOutData[1] = ReadRawRC(CRCResultRegM);
}

/* 选定卡片 pSnr卡片序列号 4字节 返回状态值 */
char PcdSelect(uint8_t *pSnr)
{
	char ucN;
	uint8_t uc;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	
	for(uc = 0 ; uc< 4;uc++)
	{
		ucComMF522Buf[uc+2] = *(pSnr+uc);
		ucComMF522Buf[6] ^= *(pSnr +uc);
	}
	
	CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
	
	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
	ClearBitMask(Status2Reg,0x80);
	
	PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&ulLen);
	
	if((uc == MI_OK) && (ulLen == 0x18))
		ucN = MI_OK;
	else
		ucN = MI_ERR;
	
	return ucN;
}

/* 验证卡片密码 ucAuth_mode，密码验证模式= 0x60，验证A密钥，  密码验证模式= 0x61，验证B密钥
 * uint8_t ucAddr，块地址 pKey，密码 pSnr，卡片序列号，4字节  状态值= MI_OK，成功 */
char PcdAuthState(uint8_t ucAuth_mode,uint8_t ucAddr,uint8_t *pKey,uint8_t *pSnr)
{
	char cStatus;
	uint8_t uc,ucComMf522Buf[MAXRLEN];
	uint32_t ulLen;
	
	ucComMf522Buf[0] = ucAuth_mode;
	ucComMf522Buf[1] = ucAddr;
	
	for(uc=0;uc<6;uc++)
		ucComMf522Buf[uc+2] = *(pKey+uc);
	
	for(uc=0;uc<6;uc++)
		ucComMf522Buf[uc+8] = *(pSnr+uc);

	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMf522Buf,12,ucComMf522Buf,&ulLen);
	
	if((cStatus != MI_OK) || (ReadRawRC(Status2Reg)&0x08))
		cStatus = MI_ERR;
	
	return cStatus;
	
}

/* 写数据到M1卡一块 ucAddr，块地址，pData，写入的数据 返回状态值 */
char PcdWrite(uint8_t ucAddr,uint8_t *pData)
{
	char cStatus;
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = ucAddr;
	
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&ulLen);
	
	if((cStatus != MI_OK) || (ulLen!=4) || ((ucComMF522Buf[0]&0x0F) != 0x0A))
		cStatus = MI_ERR;
	
	if(cStatus == MI_OK)
	{
		for(uc=0;uc<16;uc++)
			ucComMF522Buf[uc] = *(pData+uc);
		
		CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
		
		cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&ulLen);
		
		if((cStatus != MI_OK) || (ulLen!=4) || ((ucComMF522Buf[0]&0x0F) != 0x0A))
		cStatus = MI_ERR;
	}		
		
	return cStatus;
}

/* 读数据从M1卡一块 ucAddr，块地址，pData，读出的数据 返回状态值 */
char PcdRead(uint8_t ucAddr,uint8_t *pData)
{
	char cStatus;
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = ucAddr;
	
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&ulLen);
	
	if((cStatus == MI_OK) && (ulLen == 0x90))
	{
		for(uc=0;uc<16;uc++)
			ucComMF522Buf[uc ] = *(pData+uc);
	}
	else
		cStatus = MI_ERR;
	
	return cStatus;
}

/* 判断ucAddr是否是数据块 ucAddr块地址， 返回值1为数据块，0 不是数据块 */
char IsDataBlock(uint8_t ucAddr)
{
	if(ucAddr == 0)
	{
		printf("第0扇区的块0不可更改，不应对其进行操作！\r\n");
		return 0;
	}
	
	if((ucAddr<64) &&(((ucAddr+1)%4) != 0))
	{
		return 1;
	}
	
	printf("块地址不是指向数据块\r\n");
	return 0;
}

/* 写 pData 字符串到M1卡中的数据块 ucAddr，数据块地址（不能写入控制块） pData，写入的数据，16字节 返回状态值 */
char PcdWriteString(uint8_t ucAddr,uint8_t *pData)
{
	if(IsDataBlock)
	{
		return PcdWrite(ucAddr,pData);
	}
	return MI_ERR;
}

/* 读取M1卡中的一块数据到pData ucAddr，数据块地址（不能读取控制块） pData，读取的数据，16字节 返回状态值 */
char PcdReadString(uint8_t ucAddr,uint8_t *pData)
{
	if(IsDataBlock)
	{
		return PcdRead(ucAddr,pData);
	}
	return MI_ERR;
}

/* 命令卡进入休眠状态 返回状态值 */
char PcdHalt(void)
{
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	
	PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,3,ucComMF522Buf,&ulLen);
	
	return MI_OK;
}

/* 写入钱包金额 ucAddr[IN]：块地址 pData：写入的金额 返回状态  */
char WriteAmount(uint8_t ucAddr,uint32_t pData)
{
	char cStatus;
	uint8_t ucComMF522Buf[MAXRLEN];
	
	ucComMF522Buf[0] = (pData&((uint32_t)0x000000FF));
	ucComMF522Buf[1] = (pData&((uint32_t)0x0000FF00))>>8;
	ucComMF522Buf[2] = (pData&((uint32_t)0x00FF0000))>>16;
	ucComMF522Buf[3] = (pData&((uint32_t)0xFF000000))>>24;
	
	ucComMF522Buf[4] = ~(pData&((uint32_t)0x000000FF));
	ucComMF522Buf[5] = ~(pData&((uint32_t)0x0000FF00))>>8;
	ucComMF522Buf[6] = ~(pData&((uint32_t)0x00FF0000))>>16;
	ucComMF522Buf[7] = ~(pData&((uint32_t)0xFF000000))>>24;
	
	ucComMF522Buf[8] = (pData&((uint32_t)0x000000FF));
	ucComMF522Buf[9] = (pData&((uint32_t)0x0000FF00))>>8;
	ucComMF522Buf[10] = (pData&((uint32_t)0x00FF0000))>>16;
	ucComMF522Buf[11] = (pData&((uint32_t)0xFF000000))>>24;
	
	ucComMF522Buf[12] = ucAddr;
	ucComMF522Buf[13] = ~ucAddr;
	ucComMF522Buf[14] = ucAddr;
	ucComMF522Buf[15] = ~ucAddr;
	
	cStatus = PcdWrite(ucAddr,ucComMF522Buf);
	return cStatus;
}

/* 读取钱包金额 ucAddr[IN]：块地址 pData：读出的金额 返回状态  */
char ReadAmount(uint8_t ucAddr,uint32_t *pData)
{
	char cStatus;
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	
	cStatus = PcdRead(ucAddr,ucComMF522Buf);
	
	if(cStatus != MI_OK)
		return cStatus;
	
	for(uc=0;uc<4;uc++)
	{
		if((ucComMF522Buf[uc] != ucComMF522Buf[uc+8]) && (ucComMF522Buf[uc] != ~ucComMF522Buf[uc+4]))
			break;
	}
	if(uc==4)
	{
		cStatus = MI_OK;
		*pData = ucComMF522Buf[0] + (ucComMF522Buf[1]<<8) + (ucComMF522Buf[2] <<16 ) +(ucComMF522Buf[3]<<24);
		
	}
	else
	{
		cStatus = MI_ERR;
		*pData = 0;
	}
	return cStatus;
}

/* 修改控制块 ucAddr 的密码A。注意 ucAddr 指的是控制块的地址。必须要校验密码B，密码B默认为6个0xFF，如果密码B也忘记了，那就改不了密码A了
 * 注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 * ucAddr：[控制块]所在的地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块  pKeyA：指向新的密码A字符串，六个字符，比如 "123456"  */
char ChangeKeyA(uint8_t ucAddr,uint8_t *pKeyA)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B密钥
	uint8_t ucArry_ID[4];/*先后存放IC卡的类型和UID(IC卡序列号)*/
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	
	/*寻卡*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("寻卡失败\r\n");
		Delay_us(1000000);
	}
	printf("寻卡成功\r\n");
	
	/* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* 选中卡 */
		PcdSelect(ucArry_ID);
		
		/* 校验 B 密码 */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("检验密码B失败\r\n");
		}
		
		// 读取控制块里原本的数据（只要修改密码A，其他数据不改）
		if(PcdRead(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("读取控制块数据失败\r\n");
			return MI_ERR;
		}
		
		/* 修改密码A */
		for(uc=0;uc<6;uc++)
			ucComMF522Buf[uc] = pKeyA[uc];
		
		if(PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("写入数据到控制块失败\r\n");
			return MI_ERR;
		}
		
		printf("密码A修改成功！\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

/**
 * @brief 按照RC522操作流程写入16字节数据到块 ucAddr
 *        函数里校验的是密码B，密码B默认为6个0xFF，也可以校验密码A
 *        用法：WriteDataBlock( 1, "123456789\n", 10); //字符串不够16个字节的后面补零写入
 * @note  注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 *        注意：使用该函数要注意 ucAddr 是块0、数据块还是控制块，该函数内部不对此做判断
 * @param ucAddr：任意块地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param pData：指向要写入的数据，最大16个字符
 * @param Len：要写入数据的字节数
 * @retval 成功返回 MI_OK
 */
char WriteDataBlock(uint8_t ucAddr,uint8_t *pData,uint8_t Len)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B密钥
	uint8_t ucArry_ID[4];/*先后存放IC卡的类型和UID(IC卡序列号)*/
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	
	/*寻卡*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("寻卡失败\r\n");
		Delay_us(1000000);
	}
	printf("寻卡成功\r\n");
	
	/* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* 选中卡 */
		PcdSelect(ucArry_ID);
		
		/* 校验 B 密码 */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("检验密码B失败\r\n");
		}
		
		
		/* 修改密码A */
		for(uc=0;uc<16;uc++)
		{
			if(uc<Len)  ucComMF522Buf[uc] = pData[uc];
			else ucComMF522Buf[uc] = 0;  //16个字节若是未填满的字节置0
		}
		
		if(PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("写入数据到控制块失败\r\n");
			return MI_ERR;
		}
		
		printf("写入数据成功！\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

/**
 * @brief 按照RC522操作流程读取块 ucAddr
 *        函数里校验的是密码B，密码B默认为6个0xFF，也可以校验密码A
 *        用法：ReadDataBlock( 1, databuf);  // databuf 至少为16字节：uint8_t databuf[16];
 * @note  注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 *        注意：使用该函数要注意 ucAddr 是块0、数据块还是控制块，该函数内部不对此做判断
 * @param ucAddr：任意块地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param pData：指向读取到的数据，包含16个字符
 * @retval 成功返回 MI_OK
 */
char ReadDataBlock(uint8_t ucAddr,uint8_t *pData)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B密钥
	uint8_t ucArry_ID[4];/*先后存放IC卡的类型和UID(IC卡序列号)*/
	
	/*寻卡*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("寻卡失败\r\n");
		Delay_us(1000000);
	}
	printf("寻卡成功\r\n");
	
	/* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* 选中卡 */
		PcdSelect(ucArry_ID);
		
		/* 校验 B 密码 */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("检验密码B失败\r\n");
		}
		
		
		if(PcdWrite(ucAddr,pData) != MI_OK)
		{
			printf("写入数据到控制块失败\r\n");
			return MI_ERR;
		}
		
		printf("读取数据成功！\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

