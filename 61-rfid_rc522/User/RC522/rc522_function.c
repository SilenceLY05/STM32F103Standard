#include "rc522_config.h"
#include "rc522_function.h"
#include "bsp_systick.h"
#include "bsp_usart.h"

#define RC522_DELAY()          Delay_us(2)

/* ��RC522����1byte���� byteҪ���͵����� */
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

/* ��RC522��ȡ 1byte���� */
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

/* ��RC522�Ĵ��� usAddress�Ĵ�����ַ ���ؼĴ�����ǰֵ */
uint8_t ReadRawRC(uint8_t ucAddress)
{
	uint8_t ucAddr,ucReturn;
	
	//���Ĵ�����ַ����һλ���������λ��Ϊ1������λ���ֲ���
	ucAddr = ((ucAddress << 1) & 0x7E ) | 0x80;
	RC522_CS_Enable();
	
	SPI_RC522_SendByte(ucAddr);
	ucReturn = SPI_RC522_ReadByte();
	
	RC522_CS_Disable();
	return ucReturn;
	
}

/* дRC522�Ĵ��� ucAddress�Ĵ�����ַ ucValueд��ֵ */
void WriteRawRC(uint8_t ucAddress , uint8_t ucValue)
{
	uint8_t ucAddr;
	//���Ĵ�����ַ����һλ���������λ��Ϊ0
	ucAddr = (ucAddress << 1) & 0x7E;
	RC522_CS_Enable();
	
	SPI_RC522_SendByte(ucAddr);
	SPI_RC522_SendByte(ucValue);
	
	RC522_CS_Disable();
	
}

/* �ԼĴ�����ַ��λ ucReg�Ĵ�����ַ ucMask��λֵ */
void SetBitMask(uint8_t ucReg,uint8_t ucMask)
{
	uint8_t ucTemp;
	
	ucTemp = ReadRawRC(ucReg);
	WriteRawRC(ucReg,ucMask | ucTemp);
}

/* �ԼĴ���������λ ucReg�Ĵ�����ַ ucMask��λֵ */
void ClearBitMask(uint8_t ucReg,uint8_t ucMask)
{
	uint8_t ucTemp;
	ucTemp = ReadRawRC(ucReg);
	WriteRawRC(ucReg,ucTemp & (~ucMask));
}

/* �������� */
void PcdAntennaOn(void)
{
	uint8_t uc;
	
	uc = ReadRawRC(TxControlReg);
	if(!(uc&0x03))
		SetBitMask(TxControlReg,0x30);
}

/* �ر����� */
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg,0x30);
}

/* ��λRC522 */
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
	
	//���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
	WriteRawRC(ModeReg,0x3D);
	WriteRawRC(TReloadRegL,30);          //16λ��ʱ����λ 
	WriteRawRC(TReloadRegH,0);           //16λ��ʱ����λ
	WriteRawRC(TModeReg,0x8D);           //�����ڲ���ʱ��������
	WriteRawRC(TPrescalerReg,0x3E);      //���ö�ʱ����Ƶϵ��
	WriteRawRC(TxAutoReg,0x40);          //���Ʒ����ź�Ϊ100%ASK	
}

/* ����RC522�Ĺ�����ʽ ucType������ʽ */
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
		PcdAntennaOn();//������
	}
}

/* ͨ��RC522��ISO14443��ͨѶ  ucCommand,RC522������ pInData��ͨ��RC522���͵���Ƭ������ ucInLenByte �������ݵ��ֽڳ���
 * pOutData�����յ��Ŀ�Ƭ�������� pOutLenBit���������ݵ�λ���� ����״ֵ̬=MI_OK�ɹ�*/
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
		case PCD_AUTHENT:                      //Mifare��֤
			ucIrqEn = 0x12;                      //��������ж�����ErrIEn  ��������ж�IdleIEn
			ucWairFor = 0x10;                    //��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
			break;
		
		case PCD_TRANSCEIVE:                   //���շ��� ���ͽ���
			ucIrqEn = 0x77;                      //����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
			ucWairFor = 0x30;                    //Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
			break;
		
		default:
			break;
	}
	
	//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴ 
	WriteRawRC(ComIEnReg,ucIrqEn | 0x80);
	//Set1��λ����ʱ��CommIRqReg������λ����
	ClearBitMask(ComIrqReg,0x80);
	//д��������
	WriteRawRC(CommandReg,PCD_IDLE);
	//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
	SetBitMask(FIFOLevelReg,0x80);
	
	for(ul=0;ul<ucInLenByte;ul++)
		WriteRawRC(FIFODataReg,pInData[ul]);    //д���ݽ�FIFOdata
	
	WriteRawRC(CommandReg,ucCommand);         //д����
	
	if(ucCommand == PCD_TRANSCEIVE)
		//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч
		SetBitMask(BitFramingReg,0x80);
	
	ul=1000;                                  //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	do                                        //��֤ ��Ѱ���ȴ�ʱ��	
	{
		ucN = ReadRawRC(ComIrqReg);             //��ѯ�¼��ж�
		ul--;
	}while((ul != 0) && (!(ucN &0x01)) && (!(ucN&ucWairFor)));
	
	ClearBitMask(BitFramingReg,0x80);         //��������StartSendλ
	
	if(ul != 0)
	{
		//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
		if(!(ReadRawRC(ErrorReg) & 0x1B))
		{
			cStatus = MI_OK;
			
			if(ucN & ucIrqEn & 0x01)              //�Ƿ�����ʱ���ж�
				cStatus = MI_NOTAGERR;
			
			if(ucCommand == PCD_TRANSCEIVE)
			{
				//��FIFO�б�����ֽ���
				ucN = ReadRawRC(FIFOLevelReg);
				
				//�����յ����ֽڵ���Чλ��
				ucLastBits = ReadRawRC(ControlReg) & 0x07;
				
				if(ucLastBits)
					//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
					* pOutLenBit = (ucN -1) *8 + ucLastBits;
				else
					* pOutLenBit = ucN * 8;//�����յ����ֽ������ֽ���Ч
				
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

/* Ѱ�� ucReq_code��Ѱ����ʽ = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ���Ѱ����ʽ= 0x26��Ѱδ��������״̬�Ŀ�
 * pTagType����Ƭ���ʹ��� = 0x4400��Mifare_UltraLight or = 0x0400��Mifare_One(S50) or = 0x0200��Mifare_One(S70)
 * or = 0x0800��Mifare_Pro(X)) or  = 0x4403��Mifare_DESFire ����ֵ ״ֵ̬= MI_OK���ɹ�*/
char PcdRequest(uint8_t ucReq_code,uint8_t *pTagType)
{
	char cStatus;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
	ClearBitMask(Status2Reg,0x08);
	//���͵����һ���ֽڵ� ��λ
	WriteRawRC(BitFramingReg,0x07);
	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�
	SetBitMask(TxControlReg,0x03);
	
	ucComMF522Buf[0] = ucReq_code;  //���� ��Ƭ������
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&ulLen);   //Ѱ��  
	
	if((cStatus == MI_OK) && (ulLen == 0x10))   //Ѱ���ɹ����ؿ����� 
	{
		*pTagType = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else 
		cStatus = MI_ERR;
	
	return cStatus;
	
}

/* ����ײ pSnr��Ƭ���кţ�4�ֽ� ����״ֵ̬=MI_OK �ɹ� */
char PcdAnticoll(uint8_t *pSnr)
{
	char cStatus;
	uint8_t uc,ucSnr_check = 0;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	
	//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
	ClearBitMask(Status2Reg,0x08);
	//����Ĵ��� ֹͣ�շ�
	WriteRawRC(BitFramingReg,0x00);
	//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
	ClearBitMask(CollReg,0x80);
	
	ucComMF522Buf[0] = 0x93;                      //��Ƭ����ͻ����
	ucComMF522Buf[1] = 0x20;
	
	cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&ulLen);
	
	if(cStatus == MI_OK)
	{
		for(uc=0;uc<4;uc++)
		{
			*(pSnr + uc) = ucComMF522Buf[uc];    //����UID
			//��� ��ͬȡ0 ��ͬȡ1
			ucSnr_check ^= ucComMF522Buf[uc];
		}
		
		if(ucSnr_check != ucComMF522Buf[uc])
			cStatus = MI_ERR;
			
	}
	SetBitMask(CollReg,0x80);
	
	return cStatus;
	
}

/* ��RC522����CRC16 pIndata,����CRC16�����飬ucLen,����CRC16�������ֽڳ��� pOutData����ż�������ŵ��׵�ַ */
void CalulateCRC(uint8_t *pIndata,uint8_t ucLen,uint8_t *pOutData)
{
	uint8_t uc,ucN;
	
	//��CRC������Ч���������ݱ�����ʱ��λ��λ
	ClearBitMask(DivIrqReg,0x04);
	
	//д��������
	WriteRawRC(CommandReg,PCD_IDLE);
	
	//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
	ClearBitMask(FIFOLevelReg,0x80);
	
	//�ڲ�64�ֽ�FIFO����������������������
	for(uc=0;uc<ucLen;uc++)
	WriteRawRC(FIFODataReg,*(pIndata+uc));
	
	//дCRC��������
	WriteRawRC(CommandReg,PCD_CALCCRC);
	
	uc = 0xFF;
	
	do
	{
		//��ȡ�ж������־�Ĵ�������
		ucN = ReadRawRC(DivIrqReg);
		uc--;
	}while((uc!=0) && !(ucN & 0x04));
	
	pOutData[0] = ReadRawRC(CRCResultRegL);
	pOutData[1] = ReadRawRC(CRCResultRegM);
}

/* ѡ����Ƭ pSnr��Ƭ���к� 4�ֽ� ����״ֵ̬ */
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
	
	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
	ClearBitMask(Status2Reg,0x80);
	
	PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&ulLen);
	
	if((uc == MI_OK) && (ulLen == 0x18))
		ucN = MI_OK;
	else
		ucN = MI_ERR;
	
	return ucN;
}

/* ��֤��Ƭ���� ucAuth_mode��������֤ģʽ= 0x60����֤A��Կ��  ������֤ģʽ= 0x61����֤B��Կ
 * uint8_t ucAddr�����ַ pKey������ pSnr����Ƭ���кţ�4�ֽ�  ״ֵ̬= MI_OK���ɹ� */
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

/* д���ݵ�M1��һ�� ucAddr�����ַ��pData��д������� ����״ֵ̬ */
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

/* �����ݴ�M1��һ�� ucAddr�����ַ��pData������������ ����״ֵ̬ */
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

/* �ж�ucAddr�Ƿ������ݿ� ucAddr���ַ�� ����ֵ1Ϊ���ݿ飬0 �������ݿ� */
char IsDataBlock(uint8_t ucAddr)
{
	if(ucAddr == 0)
	{
		printf("��0�����Ŀ�0���ɸ��ģ���Ӧ������в�����\r\n");
		return 0;
	}
	
	if((ucAddr<64) &&(((ucAddr+1)%4) != 0))
	{
		return 1;
	}
	
	printf("���ַ����ָ�����ݿ�\r\n");
	return 0;
}

/* д pData �ַ�����M1���е����ݿ� ucAddr�����ݿ��ַ������д����ƿ飩 pData��д������ݣ�16�ֽ� ����״ֵ̬ */
char PcdWriteString(uint8_t ucAddr,uint8_t *pData)
{
	if(IsDataBlock)
	{
		return PcdWrite(ucAddr,pData);
	}
	return MI_ERR;
}

/* ��ȡM1���е�һ�����ݵ�pData ucAddr�����ݿ��ַ�����ܶ�ȡ���ƿ飩 pData����ȡ�����ݣ�16�ֽ� ����״ֵ̬ */
char PcdReadString(uint8_t ucAddr,uint8_t *pData)
{
	if(IsDataBlock)
	{
		return PcdRead(ucAddr,pData);
	}
	return MI_ERR;
}

/* �����������״̬ ����״ֵ̬ */
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

/* д��Ǯ����� ucAddr[IN]�����ַ pData��д��Ľ�� ����״̬  */
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

/* ��ȡǮ����� ucAddr[IN]�����ַ pData�������Ľ�� ����״̬  */
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

/* �޸Ŀ��ƿ� ucAddr ������A��ע�� ucAddr ָ���ǿ��ƿ�ĵ�ַ������ҪУ������B������BĬ��Ϊ6��0xFF���������BҲ�����ˣ��Ǿ͸Ĳ�������A��
 * ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 * ucAddr��[���ƿ�]���ڵĵ�ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����  pKeyA��ָ���µ�����A�ַ����������ַ������� "123456"  */
char ChangeKeyA(uint8_t ucAddr,uint8_t *pKeyA)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B��Կ
	uint8_t ucArry_ID[4];/*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	
	/*Ѱ��*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("Ѱ��ʧ��\r\n");
		Delay_us(1000000);
	}
	printf("Ѱ���ɹ�\r\n");
	
	/* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* ѡ�п� */
		PcdSelect(ucArry_ID);
		
		/* У�� B ���� */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("��������Bʧ��\r\n");
		}
		
		// ��ȡ���ƿ���ԭ�������ݣ�ֻҪ�޸�����A���������ݲ��ģ�
		if(PcdRead(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("��ȡ���ƿ�����ʧ��\r\n");
			return MI_ERR;
		}
		
		/* �޸�����A */
		for(uc=0;uc<6;uc++)
			ucComMF522Buf[uc] = pKeyA[uc];
		
		if(PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("д�����ݵ����ƿ�ʧ��\r\n");
			return MI_ERR;
		}
		
		printf("����A�޸ĳɹ���\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

/**
 * @brief ����RC522��������д��16�ֽ����ݵ��� ucAddr
 *        ������У���������B������BĬ��Ϊ6��0xFF��Ҳ����У������A
 *        �÷���WriteDataBlock( 1, "123456789\n", 10); //�ַ�������16���ֽڵĺ��油��д��
 * @note  ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 *        ע�⣺ʹ�øú���Ҫע�� ucAddr �ǿ�0�����ݿ黹�ǿ��ƿ飬�ú����ڲ����Դ����ж�
 * @param ucAddr��������ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
 * @param pData��ָ��Ҫд������ݣ����16���ַ�
 * @param Len��Ҫд�����ݵ��ֽ���
 * @retval �ɹ����� MI_OK
 */
char WriteDataBlock(uint8_t ucAddr,uint8_t *pData,uint8_t Len)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B��Կ
	uint8_t ucArry_ID[4];/*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/
	uint8_t uc,ucComMF522Buf[MAXRLEN];
	
	/*Ѱ��*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("Ѱ��ʧ��\r\n");
		Delay_us(1000000);
	}
	printf("Ѱ���ɹ�\r\n");
	
	/* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* ѡ�п� */
		PcdSelect(ucArry_ID);
		
		/* У�� B ���� */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("��������Bʧ��\r\n");
		}
		
		
		/* �޸�����A */
		for(uc=0;uc<16;uc++)
		{
			if(uc<Len)  ucComMF522Buf[uc] = pData[uc];
			else ucComMF522Buf[uc] = 0;  //16���ֽ�����δ�������ֽ���0
		}
		
		if(PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
		{
			printf("д�����ݵ����ƿ�ʧ��\r\n");
			return MI_ERR;
		}
		
		printf("д�����ݳɹ���\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

/**
 * @brief ����RC522�������̶�ȡ�� ucAddr
 *        ������У���������B������BĬ��Ϊ6��0xFF��Ҳ����У������A
 *        �÷���ReadDataBlock( 1, databuf);  // databuf ����Ϊ16�ֽڣ�uint8_t databuf[16];
 * @note  ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 *        ע�⣺ʹ�øú���Ҫע�� ucAddr �ǿ�0�����ݿ黹�ǿ��ƿ飬�ú����ڲ����Դ����ж�
 * @param ucAddr��������ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
 * @param pData��ָ���ȡ�������ݣ�����16���ַ�
 * @retval �ɹ����� MI_OK
 */
char ReadDataBlock(uint8_t ucAddr,uint8_t *pData)
{
	uint8_t KeyBValue[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // B��Կ
	uint8_t ucArry_ID[4];/*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/
	
	/*Ѱ��*/
	while(PcdRequest(PICC_REQALL,ucArry_ID) != MI_OK)
	{
		printf("Ѱ��ʧ��\r\n");
		Delay_us(1000000);
	}
	printf("Ѱ���ɹ�\r\n");
	
	/* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
	if(PcdAnticoll(ucArry_ID) == MI_OK)
	{
		/* ѡ�п� */
		PcdSelect(ucArry_ID);
		
		/* У�� B ���� */
		if(PcdAuthState(PICC_AUTHENT1B,ucAddr,KeyBValue,ucArry_ID) != MI_OK)
		{
			printf("��������Bʧ��\r\n");
		}
		
		
		if(PcdWrite(ucAddr,pData) != MI_OK)
		{
			printf("д�����ݵ����ƿ�ʧ��\r\n");
			return MI_ERR;
		}
		
		printf("��ȡ���ݳɹ���\r\n");
		PcdHalt();
		
		return MI_OK;
	}
	return MI_ERR;
		
}

