#include "bsp_ili9341_lcd.h"
#include "ascii.h"



//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9341_GramScan�������÷���ʱ���Զ�����
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
//������ѡֵΪ0-7
//����ILI9341_GramScan�������÷�����Զ�����
//LCD��ʼ�����ʱ��ʹ�ñ�Ĭ��ֵ
uint8_t LCD_SCAN_MODE = 6;

//����Һ��������ic�� ID
uint16_t lcdid = LCDID_UNKNOWN;

static uint16_t CurrentTextColor = BLACK;  //ǰ��ɫ
static uint16_t CurrentBackColor = WHITE;  //����ɫ


static void                   ILI9341_Delay               ( __IO uint32_t nCount );
static void                   ILI9341_GPIO_Config         ( void );
static void                   ILI9341_FSMC_Config         ( void );
static void                   ILI9341_REG_Config          ( void );
static void                   ILI9341_SetCursor           ( uint16_t usX, uint16_t usY );
static __inline void          ILI9341_FillColor           ( uint32_t ulAmout_Point, uint16_t usColor );
static uint16_t               ILI9341_Read_PixelData      ( void );


/* ��ILI9341д������ usCmdΪҪд������� */
__inline void ILI9341_Write_Cmd(uint16_t usCmd)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_CMD) = usCmd;
}


/* ��ILI9341д������ usDataΪҪд������� */
__inline void ILI9341_Write_Data(uint16_t usData)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA) = usData;
}

/* ��ILI9341��ȡ����  */
__inline uint16_t ILI9341_Read_Data(void)
{
	return (*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA));
}

/* ����ILI9341����ʱ���� nCount��ʱ����ֵ */
static void ILI9341_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}


//��ʼ��ILI9341��IO����
static void ILI9341_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ʹ�ܸ���IOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	/* ʹ��FSMC��ص�GPIOʱ�� */
												
	RCC_APB2PeriphClockCmd(
												 /* �����ź��� */	
												 ILI9341_CS_CLK | ILI9341_DC_CLK |ILI9341_WR_CLK |
												 ILI9341_RD_CLK | ILI9341_RST_CLK |ILI9341_BK_CLK |												 
												 /* �����ź��� */
	                       ILI9341_D0_GPIO_CLK | ILI9341_D1_GPIO_CLK | ILI9341_D2_GPIO_CLK |
												 ILI9341_D3_GPIO_CLK | ILI9341_D4_GPIO_CLK | ILI9341_D5_GPIO_CLK |
												 ILI9341_D6_GPIO_CLK | ILI9341_D7_GPIO_CLK | ILI9341_D8_GPIO_CLK |
												 ILI9341_D9_GPIO_CLK | ILI9341_D10_GPIO_CLK | ILI9341_D11_GPIO_CLK |
												 ILI9341_D12_GPIO_CLK | ILI9341_D13_GPIO_CLK | ILI9341_D14_GPIO_CLK | 
												 ILI9341_D15_GPIO_CLK ,ENABLE);
	
	/* --GPIO����----------------------------------------------------- */
	
	/* ����FSMC���Ӧ��������,FSMC-D0~D15 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //����Ϊ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ILI9341_D0_GPIO_PIN;
	GPIO_Init(ILI9341_D0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D1_GPIO_PIN;
	GPIO_Init(ILI9341_D1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D2_GPIO_PIN;
	GPIO_Init(ILI9341_D2_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D3_GPIO_PIN;
	GPIO_Init(ILI9341_D3_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D4_GPIO_PIN;
	GPIO_Init(ILI9341_D4_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D5_GPIO_PIN;
	GPIO_Init(ILI9341_D5_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D6_GPIO_PIN;
	GPIO_Init(ILI9341_D6_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D7_GPIO_PIN;
	GPIO_Init(ILI9341_D7_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D8_GPIO_PIN;
	GPIO_Init(ILI9341_D8_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D9_GPIO_PIN;
	GPIO_Init(ILI9341_D9_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D10_GPIO_PIN;
	GPIO_Init(ILI9341_D10_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D11_GPIO_PIN;
	GPIO_Init(ILI9341_D11_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D12_GPIO_PIN;
	GPIO_Init(ILI9341_D12_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D13_GPIO_PIN;
	GPIO_Init(ILI9341_D13_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D14_GPIO_PIN;
	GPIO_Init(ILI9341_D14_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D15_GPIO_PIN;
	GPIO_Init(ILI9341_D15_GPIO_PORT, &GPIO_InitStructure);
	
	/* ����FSMC���Ӧ�Ŀ�����
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16   :LCD-DC
	 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //����Ϊ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
	GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
	GPIO_Init(ILI9341_WR_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
	GPIO_Init(ILI9341_RD_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
	GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);
	
	//����LCD��λRST���ƹܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
	GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);
	
	//����LCD����BK���ƹܽ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
	GPIO_Init(ILI9341_BK_PORT, &GPIO_InitStructure);

}

/* ��ʼ��LCD_FSMCģʽ���� */
static void ILI9341_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef   FSMC_NORSRAMInitStructure = {0};
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming = {0};
	
	/* ʹ��FSMC����ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	
	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 2/72M=28ns
	readWriteTiming.FSMC_AddressSetupTime = 0x02;   //��ַ����ʱ��
	
	//���ݱ���ʱ�䣨DATAST��+1��HCLK = 5/72M = 70ns(��EM��STAMоƬ)
	readWriteTiming.FSMC_DataSetupTime = 0x05;  //���ݽ���ʱ��
	
	//ѡ����Ƶ�ģʽ
	//ģʽB���첽NOR FLASHģʽ����ILI9341��8080ʱ��ƥ��
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;
	
	//����������ģʽB�޹�
	//��ַ����ʱ�䣨ADDHOLD��ģʽAδ�õ�
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	
	//��������ת�����ڣ������ڸ���ģʽ��NOR����
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	//����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
	readWriteTiming.FSMC_CLKDivision = 0x00;
	
	//���ݱ���ʱ�䣬������ͬ���͵�NOR
	readWriteTiming.FSMC_DataLatency = 0x00;
	
	
	//ѡ��FSMCӳ��Ĵ洢����Bank1 sram3
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAMx;
	
	//ѡ���ַ���������������Ƿ��ã�������NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	
	//����Ҫ���ƵĴ洢������
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	
	//�洢�����ݿ�ȣ�16λ
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	
	//�����Ƿ�ʹ��ͻ������ģʽ��������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	
	//���õȴ��źŵ���Ч���ԣ�������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	
	//�����Ƿ�֧�ְѷǶ����ͻ������ ������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	
	//���õȴ��źŵĲ���ʱ�䣬������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	
	//�洢дʹ��
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	
	//��ʹ�õȴ�ʹ��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	
	//��ʹ����չģʽ����дʹ����ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	
	//ͻ��д����
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	
	//��дʱ������
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	
	//��дͬ��ʱ��ʹ����չģʽʱ������ò���Ч
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); // ��ʼ��FSMC����  ������д�뵽�Ĵ���
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  // ʹ��BANK
}

/* ��ʼ��ILI9341�Ĵ��� */
static void ILI9341_REG_Config(void)
{
	lcdid = ILI9341_ReadID();
	
	if(lcdid == LCDID_ILI9341)
	{
		/* Power control B(CFh) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xCF);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x81);
		ILI9341_Write_Data(0x30);
		
		/* Power on sequence control (EDh) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xED);
		ILI9341_Write_Data(0x64);
		ILI9341_Write_Data(0x03);
		ILI9341_Write_Data(0x12);
		ILI9341_Write_Data(0x81);
		
		/* Driver timing control A(E8h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xE8);
		ILI9341_Write_Data(0x85);
		ILI9341_Write_Data(0x10);
		ILI9341_Write_Data(0x78);
		
		/* Power control A(CBh) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xCB);
		ILI9341_Write_Data(0x39);
		ILI9341_Write_Data(0x2C);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x34);
		
		/* ILI9341_Write_Data(0x02) */
		ILI9341_Write_Data(0x06);//ԭ����02���Ըĳ�06�ɷ�ֹҺ����ʾ������ʱ�����������
		
		/* Pump ratio control (F7h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xF7);
		ILI9341_Write_Data(0x20);
		
		/* Driver timing control B(EAh) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xEA);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x00);
		
		/* Frame Rate Control (In Normal Mode / Full Colors)(B1h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xB1);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x1B);
		
		/* Display Function Control (B6h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xB6);
		ILI9341_Write_Data(0x0A);
		ILI9341_Write_Data(0xA2);
		
		/* Power control 1(C0h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xC0);
		ILI9341_Write_Data(0x35);

		/* Power control 2(C1h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xC1);
		ILI9341_Write_Data(0x11);
		
		/* VCOM Control 1(C5h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xC5);
		ILI9341_Write_Data(0x45);
		ILI9341_Write_Data(0x45);
		
		/* VCOM Control 2(C7h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xC7);
		ILI9341_Write_Data(0xA2);
		
		/* Enable 3G (F2h)*/
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0xF2);
		ILI9341_Write_Data(0x00);
		
		/* Gamma Set (26h) */
		ILI9341_Write_Cmd(0x26);
		ILI9341_Write_Data(0x01);
		DEBUG_DELAY();
		
		/* Positive Gamma Correction (E0h)*/
		ILI9341_Write_Cmd(0xE0); //Set Gamma
		ILI9341_Write_Data(0x0F);
		ILI9341_Write_Data(0x26);
		ILI9341_Write_Data(0x24);
		ILI9341_Write_Data(0x0B);
		ILI9341_Write_Data(0x0E);
		ILI9341_Write_Data(0x09);
		ILI9341_Write_Data(0x54);
		ILI9341_Write_Data(0xA8);
		ILI9341_Write_Data(0x46);
		ILI9341_Write_Data(0x0C);
		ILI9341_Write_Data(0x17);
		ILI9341_Write_Data(0x09);
		ILI9341_Write_Data(0x0F);
		ILI9341_Write_Data(0x07);
		ILI9341_Write_Data(0x00);
		
		/* Negative Gamma Correction (E1h) */
		ILI9341_Write_Cmd(0XE1); //Set Gamma
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x19);
		ILI9341_Write_Data(0x1B);
		ILI9341_Write_Data(0x04);
		ILI9341_Write_Data(0x10);
		ILI9341_Write_Data(0x07);
		ILI9341_Write_Data(0x2A);
		ILI9341_Write_Data(0x47);
		ILI9341_Write_Data(0x39);
		ILI9341_Write_Data(0x03);
		ILI9341_Write_Data(0x06);
		ILI9341_Write_Data(0x06);
		ILI9341_Write_Data(0x30);
		ILI9341_Write_Data(0x38);
		ILI9341_Write_Data(0x0F);
		
		/* memory access control set(36h) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0x36);
		ILI9341_Write_Data(0xC8);  //���� ���Ͻǵ����½�ɨ�跽ʽ
		DEBUG_DELAY();
		
		/* column address control set */
		ILI9341_Write_Cmd(CMD_SetCoordinateX);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0xEF);
		
		/* page address control set */
		ILI9341_Write_Cmd(CMD_SetCoordinateY);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x00);
		ILI9341_Write_Data(0x01);
		ILI9341_Write_Data(0x3F);
		
		/* Pixel Format Set (3Ah) */
		DEBUG_DELAY();
		ILI9341_Write_Cmd(0x3A);
		ILI9341_Write_Data(0x55);
		
		/* Sleep Out(11h) */
		ILI9341_Write_Cmd(0x11);
		ILI9341_Delay(0xAFFf<<2);
		DEBUG_DELAY();
		
		/* Display ON(29h) */
		ILI9341_Write_Cmd(0x29);
	}
	
	else if( lcdid == LCDID_ST7789V)
	{
		/*  Power control B (CFh)  */
    DEBUG_DELAY  ();
    ILI9341_Write_Cmd ( 0xCF  );
    ILI9341_Write_Data ( 0x00  );
    ILI9341_Write_Data ( 0xC1  );
    ILI9341_Write_Data ( 0x30  );
    
    /*  Power on sequence control (EDh) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xED );
    ILI9341_Write_Data ( 0x64 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x12 );
    ILI9341_Write_Data ( 0x81 );
    
    /*  Driver timing control A (E8h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xE8 );
    ILI9341_Write_Data ( 0x85 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x78 );
    
    /*  Power control A (CBh) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xCB );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x2C );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x34 );
    ILI9341_Write_Data ( 0x02 );
    
    /* Pump ratio control (F7h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xF7 );
    ILI9341_Write_Data ( 0x20 );
    
    /* Driver timing control B */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xEA );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    
    
    /* Power Control 1 (C0h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xC0 );   //Power control
    ILI9341_Write_Data ( 0x21 );  //VRH[5:0]
    
    /* Power Control 2 (C1h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xC1 );   //Power control
    ILI9341_Write_Data ( 0x11 );  //SAP[2:0];BT[3:0]
    
    /* VCOM Control 1 (C5h) */
    ILI9341_Write_Cmd ( 0xC5 );
    ILI9341_Write_Data ( 0x2D );
    ILI9341_Write_Data ( 0x33 );
    
    /*  VCOM Control 2 (C7h)  */
//	ILI9341_Write_Cmd ( 0xC7 );
//	ILI9341_Write_Data ( 0XC0 );
    
    /* memory access control set */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0x36 );   //Memory Access Control
    ILI9341_Write_Data ( 0x00 );  /*����  ���Ͻǵ� (���)�����½� (�յ�)ɨ�跽ʽ*/
    DEBUG_DELAY ();
    
    ILI9341_Write_Cmd(0x3A);   
    ILI9341_Write_Data(0x55); 
    
      /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xB1 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x17 );
    
    /*  Display Function Control (B6h) */
    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xB6 );
    ILI9341_Write_Data ( 0x0A );
    ILI9341_Write_Data ( 0xA2 );
    
    ILI9341_Write_Cmd(0xF6);    			
    ILI9341_Write_Data(0x01); 
    ILI9341_Write_Data(0x30); 
    
    /* Enable 3G (F2h) */
    ILI9341_Write_Cmd ( 0xF2 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Gamma Set (26h) */
    ILI9341_Write_Cmd ( 0x26 );
    ILI9341_Write_Data ( 0x01 );
    DEBUG_DELAY ();
    
    /* Positive Gamma Correction */
    ILI9341_Write_Cmd(0xe0); //Positive gamma
    ILI9341_Write_Data(0xd0);         
    ILI9341_Write_Data(0x00); 
    ILI9341_Write_Data(0x02); 
    ILI9341_Write_Data(0x07); 
    ILI9341_Write_Data(0x0b); 
    ILI9341_Write_Data(0x1a); 
    ILI9341_Write_Data(0x31); 
    ILI9341_Write_Data(0x54); 
    ILI9341_Write_Data(0x40); 
    ILI9341_Write_Data(0x29); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x17);

    /* Negative Gamma Correction (E1h) */
    ILI9341_Write_Cmd(0xe1); //Negative gamma
    ILI9341_Write_Data(0xd0); 
    ILI9341_Write_Data(0x00); 
    ILI9341_Write_Data(0x02); 
    ILI9341_Write_Data(0x07); 
    ILI9341_Write_Data(0x05); 
    ILI9341_Write_Data(0x25); 
    ILI9341_Write_Data(0x2d); 
    ILI9341_Write_Data(0x44); 
    ILI9341_Write_Data(0x45); 
    ILI9341_Write_Data(0x1c); 
    ILI9341_Write_Data(0x18); 
    ILI9341_Write_Data(0x16); 
    ILI9341_Write_Data(0x1c); 
    ILI9341_Write_Data(0x1d); 
  
	
//	/* column address control set */
//	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0xEF );
//	
//	/* page address control set */
//	DEBUG_DELAY ();
//	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x01 );
//	ILI9341_Write_Data ( 0x3F );
	
	
    /* Sleep Out (11h)  */
    ILI9341_Write_Cmd ( 0x11 );	  //Exit Sleep
    ILI9341_Delay ( 0xAFFf<<2 );
    DEBUG_DELAY ();
    
    /* Display ON (29h) */
    ILI9341_Write_Cmd ( 0x29 );   //Display on
    
    ILI9341_Write_Cmd(0x2c);
	}
	
	
}

/* ILI9341��ʼ�����������Ҫ�õ�LCD��һ��Ҫ����������� */
void ILI9341_Init(void)
{
	ILI9341_GPIO_Config();
	ILI9341_FSMC_Config();
	ILI9341_BackLed_Control(ENABLE);  //���������
	ILI9341_Rst();
	ILI9341_REG_Config();
	
	//����Ĭ��ɨ�跽������ 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	//ILI9341_GramScan(LCD_SCAN_MODE);
}


/* ILI9341����LED���ƣ�
enumState:�����Ƿ�ʹ�ܱ���*/
void ILI9341_BackLed_Control(FunctionalState enumstate)
{
	if(enumstate)
		GPIO_ResetBits(ILI9341_BK_PORT,ILI9341_BK_PIN);
	else
		GPIO_SetBits(ILI9341_BK_PORT,ILI9341_BK_PIN);
}

/* ��ȡLCD��������ID�����������ڲ��Եײ�Ķ�д���� */
uint16_t ILI9341_ReadID(void)
{
	uint16_t id = 0;
	
	ILI9341_Write_Cmd(0x04);
	ILI9341_Read_Data();
	ILI9341_Read_Data();
	id = ILI9341_Read_Data();
	id <<=8;
	id |= ILI9341_Read_Data();
	
	if(id == LCDID_ST7789V)
	{
		return id;
	}
	else
	{
		ILI9341_Write_Cmd(0xD3);
		ILI9341_Read_Data();
		ILI9341_Read_Data();
		id = ILI9341_Read_Data();
		id <<= 8;
		id |= ILI9341_Read_Data();
		if(id ==LCDID_ILI9341)
		{
			return id;
		}
	}
	
	return LCDID_UNKNOWN;
	
}


/* ILI9341�����λ */
void ILI9341_Rst(void)
{
	GPIO_ResetBits(ILI9341_RST_PORT,ILI9341_RST_PIN); // �͵�ƽ��λ
	ILI9341_Delay(0xAFFF<<2);
	GPIO_SetBits(ILI9341_RST_PORT,ILI9341_RST_PIN);
	ILI9341_Delay(0xAFFF<<2);
	
}

/**
 * @brief  ����ILI9341��GRAM��ɨ�跽�� 
 * @param  ucOption ��ѡ��GRAM��ɨ�跽�� 
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 1 :���Ͻ�->���½�
  *     @arg 2 :���½�->���Ͻ�
  *     @arg 3 :���½�->���Ͻ�
  *     @arg 4 :���Ͻ�->���½�
 * @retval ��
 */
 
void ILI9341_GramScan(uint8_t ucOption)
{
	switch(ucOption)
	{
		case 1:
//   ���Ͻ�->���½�      ��ʾ��Ӣ��ʱ�õ�������ģʽ 
//		____ x(240)       //Һ������Ϊ�ο���
//	 |  
//	 |	y(320) 		

			ILI9341_Write_Cmd(0x36);
			if(lcdid == LCDID_ILI9341)
			{
				ILI9341_Write_Data(0xC8);  
			}
			else if(lcdid == LCDID_ST7789V)
			{
				ILI9341_Write_Data(0xC0); 
			}
			
			ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* ����X���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x00 );      /* x ���������8λ */	
			ILI9341_Write_Data ( 0xEF );	    /* x ���������8λ */

			ILI9341_Write_Cmd ( CMD_SetCoordinateY );   /* ����Y���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x01 );/* Y ���������8λ */	
			ILI9341_Write_Data ( 0x3F );	/* Y ���������8λ */
			
			break;
			
		case 2:
//		���½�->���Ͻ�      ��ʾ����ͷͼ��ʱ�õ�������ģʽ 
//		|x(320)            //Һ������Ϊ�ο���
//		|
//		|___ y(240)		

			ILI9341_Write_Cmd(0x36);
			if(lcdid == LCDID_ILI9341)
			{
				ILI9341_Write_Data(0x68);  
			}
			else if(lcdid == LCDID_ST7789V)
			{
				ILI9341_Write_Data(0x60); 
			}
			
			ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* ����X���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x01 );      /* x ���������8λ */	
			ILI9341_Write_Data ( 0x3F );	    /* x ���������8λ */

			ILI9341_Write_Cmd ( CMD_SetCoordinateY );   /* ����Y���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x00 );/* Y ���������8λ */	
			ILI9341_Write_Data ( 0xEF );	/* Y ���������8λ */
			
			break;
			
		case 3:
//		���½�->���Ͻ�        ��ʾBMPͼƬʱ�õ�������ģʽ 
//		           |x(320)   //Һ������Ϊ�ο���
//		           |           
//		y(240) ____|
		
			ILI9341_Write_Cmd(0x36);
			if(lcdid == LCDID_ILI9341)
			{
				ILI9341_Write_Data(0x28);  
			}
			else if(lcdid == LCDID_ST7789V)
			{
				ILI9341_Write_Data(0x20); 
			}
			
			ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* ����X���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x01 );      /* x ���������8λ */	
			ILI9341_Write_Data ( 0x3F );	    /* x ���������8λ */

			ILI9341_Write_Cmd ( CMD_SetCoordinateY );   /* ����Y���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x00 );/* Y ���������8λ */	
			ILI9341_Write_Data ( 0xEF );	/* Y ���������8λ */
			
			break;
			
			case 4:
//		���½ǽ�->���Ͻ�
//		|y(320)              //Һ������Ϊ�ο���
//		|
//		|___ x(240)	
		
			ILI9341_Write_Cmd(0x36);
			if(lcdid == LCDID_ILI9341)
			{
				ILI9341_Write_Data(0x48);  
			}
			else if(lcdid == LCDID_ST7789V)
			{
				ILI9341_Write_Data(0x40); 
			}
			
			ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* ����X���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x00 );      /* x ���������8λ */	
			ILI9341_Write_Data ( 0xEF );	    /* x ���������8λ */

			ILI9341_Write_Cmd ( CMD_SetCoordinateY );   /* ����Y���� */
			ILI9341_Write_Data ( 0x00 );      /* ��ʼ�����ȸ�8λ���ٵ�8λ */
			ILI9341_Write_Data ( 0x00 );      
			ILI9341_Write_Data ( 0x01 );/* Y ���������8λ */	
			ILI9341_Write_Data ( 0x3F );	/* Y ���������8λ */
			
			break;
	}
	
	ILI9341_Write_Cmd(CMD_SetPixel);
}

void ILI9341_OpenWindow(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	/* column address control set */
	ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* ����X���� */
	ILI9341_Write_Data ( usX >> 8 );      /* �ȸ�8λ���ٵ�8λ */
	ILI9341_Write_Data ( usX&0xFF );      /* ������ʼ��ͽ����� */
	ILI9341_Write_Data ( (usX+usWidth -1 ) >> 8 );
	ILI9341_Write_Data ( (usX+usWidth - 1)&0xff );	

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( usY >> 8 );      /* �ȸ�8λ���ٵ�8λ */
	ILI9341_Write_Data ( usY&0xFF );      /*  */
	ILI9341_Write_Data ( (usY+usHeight -1 ) >> 8 );
	ILI9341_Write_Data ( (usY+usHeight - 1)&0xff );	
}

/* �趨ILI9341�Ĺ������   usX�����ض�ɨ�跽���¹���X����  usY�����ض�ɨ�跽���¹���Y���� */
static void ILI9341_SetCursor(uint16_t usX ,uint16_t usY)
{
	ILI9341_OpenWindow(usX ,usY ,1,1);
}


/* ��ILI9341��ʾ������ĳһ��ɫ������ص�
	 ulAmount_Point:Ҫ�����ɫ�����ص������Ŀ
	 usColor����ɫ*/
static __inline void ILI9341_FillColor(uint32_t ulAmount_Point,uint16_t usColor)
{
	uint32_t i = 0;
	
	/* ���Ĵ���д�� */
	ILI9341_Write_Cmd( CMD_SetPixel );
	
	for( i = 0 ; i < ulAmount_Point ; i++)
		ILI9341_Write_Data(usColor);
}

/* ��ILI9341��ʾ����ĳһ������ĳ����ɫ��������
	 usX�����ض�ɨ�跽���´��ڵ����X����
	 usY�����ض�ɨ�跽���´��ڵ����Y����
	 usWidth�����ڵĿ��
	 usHeigh�����ڵĸ߶�*/
void ILI9341_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	ILI9341_OpenWindow(usX,usY,usWidth,usHeight);
	ILI9341_FillColor(usWidth*usHeight,CurrentBackColor);
}

/* ��ILI9341��ʾ����ĳһ����ĳ����ɫ�������
	 usX�����ض�ɨ�跽���´��ڵ����X����
	 usY�����ض�ɨ�跽���´��ڵ����Y����*/
void ILI9341_SetPointPixel(uint16_t usX,uint16_t usY)
{
	if((usX < ILI9341_DispWindow_COLUMN) && (usY < ILI9341_DispWindow_PAGE))
	{
		ILI9341_SetCursor(usX,usY);
		ILI9341_FillColor(1,CurrentTextColor);
	}
}

/* ��ȡGRAM��һ���������� */
static uint16_t ILI9341_Read_PixelData(void)
{
	uint16_t usRG = 0,usB= 0;
	
	ILI9341_Write_Cmd(0x2E);  //������
	
	ILI9341_Read_Data();  // ȥ����һ�ζ�ȡ���
	
	//��ȡ��ɫͨ������ɫͨ����ֵ
	usRG = ILI9341_Read_Data();
	usB = ILI9341_Read_Data();
	
	//��5λΪ��ɫ���м�6λΪ��ɫ������5λΪ��ɫ
	return ((usRG & 0xF800) | ((usRG << 3) &0x7E0) | (usB >> 11));
	
}

/* ��ȡILI9341��ʾ����ĳһ���������������
	 usX�����ض�ɨ�跽���´��ڵ����X����
	 usY�����ض�ɨ�跽���´��ڵ����Y����*/
uint16_t ILI9341_GetPointPixel( uint16_t usX, uint16_t usY)
{
	uint16_t usPixelData;
	
	ILI9341_SetCursor(usX,usY);
	usPixelData = ILI9341_Read_PixelData();
	
	return usPixelData;
}

/* ��ILI9341��ʾ����ʹ��Bresenham�㷨���߶�
	 usX1:���ض�ɨ�跽�����߶ε�һ���˵�X����
	 usY1:���ض�ɨ�跽�����߶ε�һ���˵�Y����
	 usX2:���ض�ɨ�跽�����߶ε���һ���˵�X����
	 usY2:���ض�ɨ�跽�����߶ε���һ���˵�Y����*/
void ILI9341_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //������������ 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //���õ������� 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//��ֱ�� 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//ˮƽ�� 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //ѡȡ�������������� 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//������� 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current );//���� 
		
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		
		if ( lError_X > lDistance ) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		
		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
		
	}  
	
	
}   

/* ��ILI9341��ʾ���ϻ�һ������ 
	 usX_Start:���ض�ɨ�跽���¾��ε���ʼ��X����
	 usY_Start�����ض�ɨ�跽���¾ؾ��ε���ʼ��Y����
	 usWidth:���εĿ��
	 usHeigh:���εĸ߶�
	 ucFilled��ѡ���Ƿ����þ��� 0Ϊ���ľ��� 1Ϊʵ��*/
void ILI9341_DrawRectangle(uint16_t usX_Start,uint16_t usY_Start,uint16_t usWidth,uint16_t usHeight,uint8_t ucFilled)
{
	if(ucFilled)
	{
		ILI9341_OpenWindow(usX_Start,usY_Start,usWidth,usHeight);
		ILI9341_FillColor(usWidth*usHeight,CurrentTextColor);
	}
	else
	{
		ILI9341_DrawLine(usX_Start,usY_Start,usX_Start+usWidth-1,usY_Start);
		ILI9341_DrawLine(usX_Start,usY_Start,usX_Start,usY_Start+usHeight -1);
		ILI9341_DrawLine(usX_Start,usY_Start+usHeight -1,usX_Start+usWidth-1,usY_Start+usHeight -1);
		ILI9341_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
		
		
	}
}

/* ��ILI9341��ʾ����ʹ��Bresenham�㷨��Բ 
	 usX_Center:���ض�ɨ�跽����Բ�ĵ���ʼ��X����
	 usY_Center�����ض�ɨ�跽����Բ�ĵ���ʼ��Y����
	 usRadius:Բ�뾶
	 ucFilled��ѡ���Ƿ�����Բ 0Ϊ����Բ 1Ϊʵ��*/
void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center ,uint16_t usRadius, uint8_t ucFilled)
{
	int16_t sCurrentX , sCurrentY;
	int16_t sError;
	
	sCurrentX = 0; sCurrentY = usRadius;
	
	sError = 3 - ( usRadius << 1);  //�ж��¸���λ�õ�����
	
	while(sCurrentX <= sCurrentY)
	{
		int16_t sCountY;
		
		if(ucFilled)
			for( sCountY = sCurrentX ; sCountY <= sCurrentY ; sCountY ++)
			{
				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center + sCountY);
				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center + sCountY);
				ILI9341_SetPointPixel( usX_Center - sCountY , usY_Center + sCurrentX);
				ILI9341_SetPointPixel( usX_Center - sCountY , usY_Center - sCurrentX);
				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center - sCountY);
				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center - sCountY);
				ILI9341_SetPointPixel( usX_Center + sCountY , usY_Center - sCurrentX);
				ILI9341_SetPointPixel( usX_Center + sCountY , usY_Center + sCurrentX);
			}
			else
			{
				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center + sCurrentY);
				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center + sCurrentY);
				ILI9341_SetPointPixel( usX_Center - sCurrentY , usY_Center + sCurrentX);
				ILI9341_SetPointPixel( usX_Center - sCurrentY , usY_Center - sCurrentX);
				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center - sCurrentY);
				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center - sCurrentY);
				ILI9341_SetPointPixel( usX_Center + sCurrentY , usY_Center - sCurrentX);
				ILI9341_SetPointPixel( usX_Center + sCurrentY , usY_Center + sCurrentX);
			}
			
			sCurrentX++;
			
			if( sError < 0)
				sError += 4* sCurrentX +6;
			
			else
			{
				sError += 10 + 4 * ( sCurrentX - sCurrentY);
				sCurrentY--;
			}
	}
}

//void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center ,uint16_t usRadius, uint8_t ucFilled)
//{
//	int16_t x ,y;
//	
//	for( y = - usRadius; y <= usRadius ; y++)
//	{
//		for( x = - usRadius ;  x <= usRadius ;x++)
//		{
//			if( x*y + y*y <= usRadius * usRadius)
//			{
//				int drawX = usX_Center + x;
//				int drawY = usY_Center + y;
//				
//				if( ucFilled || ( x*y + y*y <= usRadius * usRadius))
//				{
//					ILI9341_SetPointPixel(drawX ,drawY);
//				}
//			}
//		}
//	}
//}

/* ��ILI9341��ʾ������ʾһ��Ӣ���ַ�
	 usX�����ض�ɨ�跽�����ַ�����ʼX����
	 usY�����ض�ɨ�跽�����ַ�����ʼY����
	 cChar��Ҫ��ʾ��Ӣ���ַ�*/
void ILI9341_DispChar_EN(uint16_t usX,uint16_t usY,const char cChar)
{
	uint8_t ucTemp,ucRelativePosition,ucPage,ucColumn;
	
	//��ASCII�����ģ������ASCII��ǰ32����ͼ�η��ţ�
	ucRelativePosition = cChar - ' ';
	
	//������ʾ����
	ILI9341_OpenWindow(usX,usY,WIDTH_EN_CHAR,HEIGHT_EN_CHAR);
	
	ILI9341_Write_Cmd(CMD_SetPixel);
	
	//���ֽڶ�ȡ��ģ����
	//����ǰ��ֱ����������ʾ���ڣ���ʾ���ݻ������л�
	for(ucPage = 0; ucPage < HEIGHT_EN_CHAR;ucPage++)
	{
		ucTemp = ucAscii_1608[ucRelativePosition][ucPage];
		//һλһλ����Ҫ��ʾ����ɫ
		for(ucColumn = 0; ucColumn < WIDTH_EN_CHAR; ucColumn++)
		{
			if(ucTemp & 0x01)
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
			
			ucTemp >>=1;
		}/* д��һ�� */
	}/* ȫ��д�� */
	
}


/* ��ILI9341 ��ʾ������ʾӢ���ַ���
	 usX�����ض�ɨ�跽�����ַ�����ʼX����
   usY�����ض�ɨ�跽�����ַ�����ʼY����
	 pStr��Ҫ��ʾ��Ӣ���ַ������׵�ַ*/
void ILI9341_DispString_EN(uint16_t usX,uint16_t usY,char *pStr)
{
	while(*pStr != '\0')
	{
		if( (usX - ILI9341_DispWindow_X_Star + WIDTH_EN_CHAR) > ILI9341_DispWindow_COLUMN)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += HEIGHT_EN_CHAR;
		}
		
		if( (usY - ILI9341_DispWindow_Y_Star + HEIGHT_EN_CHAR) > ILI9341_DispWindow_PAGE)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN(usX,usY,*pStr);
		
		pStr++;
		usX += WIDTH_EN_CHAR;
	}
}


/* ��ILI9341 ��ʾ������ʾһ�������ַ�
	 usX�����ض�ɨ�跽�����ַ�����ʼX����
   usY�����ض�ɨ�跽�����ַ�����ʼY����
	 usChar��Ҫ��ʾ�������ַ��������룩*/
void ILI9341_DispChar_CH(uint16_t usX , uint16_t usY, uint16_t usChar)
{
	uint8_t rowCount,bitCount;
	uint8_t ucBuffer[WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8];
	uint16_t usTemp;
	
	//������ʾ����
	ILI9341_OpenWindow(usX,usY,WIDTH_CH_CHAR,HEIGHT_CH_CHAR);
	
	ILI9341_Write_Cmd(CMD_SetPixel);
	
	//ȡ��ģ����
	GetGBKCode(ucBuffer,usChar);
	
	for(rowCount = 0 ; rowCount < HEIGHT_CH_CHAR ; rowCount ++)
	{
		/* ȡ�������ֽڵ����ݣ���lcd�ϼ��Ǻ��ֵ�һ�� */
		usTemp = ucBuffer[rowCount *2];
		usTemp = (usTemp << 8);
		usTemp |= ucBuffer [rowCount*2 +1];
		
		for(bitCount = 0 ; bitCount < WIDTH_CH_CHAR ; bitCount++)
		{
			if(usTemp & (0x01<<15))  //��λ��ǰ 
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
			
			usTemp <<=1;
		}
	}
}

/* ��ILI9341��ʾ������ʾ�����ַ���
	 line�����ض�ɨ�跽�����ַ�������ʼY���꣬
	 ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽ������������
	 ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ����ʾ
	 ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
	 pStr��Ҫ��ʾ�������ַ������׵�ַ*/
void ILI9341_DispString_CH(uint16_t usX,uint16_t usY,char *pStr)
{
	uint16_t usCh;
	
	while(* pStr != '\0')
	{
		if( (usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > ILI9341_DispWindow_COLUMN )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += HEIGHT_CH_CHAR;
		}
		
		if( (usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > ILI9341_DispWindow_PAGE )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}
		
		usCh = * (uint16_t *)pStr;
		usCh = (usCh << 8) + (usCh >> 8);  // �����ߵ��ֽ�
		
		ILI9341_DispChar_CH(usX, usY , usCh);
		
		usX += WIDTH_CH_CHAR;
		
		pStr += 2;  //һ�����������ֽ�
	}
}

/* ��ILI9341��ʾ������ʾ��Ӣ���ַ���
	 line�����ض�ɨ�跽�����ַ�������ʼY���꣬
	 ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽ������������
	 ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ����ʾ
	 ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
	 pStr��Ҫ��ʾ����Ӣ���ַ������׵�ַ*/

void ILI9341_DispStringLine_EN_CH(uint8_t usX,uint8_t usY , char* pStr)
{
	uint16_t usCh;
	
	while(*pStr != '\0')
	{
		if(*pStr <= 126) // Ӣ���ַ�
		{
			if( (usX - ILI9341_DispWindow_X_Star + WIDTH_EN_CHAR) > ILI9341_DispWindow_COLUMN )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += HEIGHT_EN_CHAR;
			}
			
			if( (usY - ILI9341_DispWindow_Y_Star + HEIGHT_EN_CHAR) > ILI9341_DispWindow_PAGE )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}
			
			ILI9341_DispChar_EN(usX, usY , *pStr);
			
			usX += WIDTH_EN_CHAR;
			
			pStr ++;  
		}
		else
		{
			if( (usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR) > ILI9341_DispWindow_COLUMN )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += HEIGHT_CH_CHAR;
			}
			
			if( (usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR) > ILI9341_DispWindow_PAGE )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}
			
			usCh = * (uint16_t *)pStr;
			usCh = (usCh << 8) + (usCh >> 8);  // �����ߵ��ֽ�
			
			ILI9341_DispChar_CH(usX, usY , usCh);
			
			usX += WIDTH_CH_CHAR;
			
			pStr += 2;  //һ�����������ֽ�
		}
	}
}

