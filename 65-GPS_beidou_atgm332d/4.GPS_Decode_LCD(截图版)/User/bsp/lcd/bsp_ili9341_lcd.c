#include "bsp_ili9341_lcd.h"
#include "fonts.h"

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向会自动更改
//LCD初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;

//保存液晶屏驱动ic的 ID
uint16_t lcdid = LCDID_UNKNOWN;

static sFONT *LCD_Currentfonts = &Font8x16; // 英文字体
static uint16_t CurrentTextColor = BLACK;  //前景色
static uint16_t CurrentBackColor = WHITE;  //背景色


static void                   ILI9341_Delay               ( __IO uint32_t nCount );
static void                   ILI9341_GPIO_Config         ( void );
static void                   ILI9341_FSMC_Config         ( void );
static void                   ILI9341_REG_Config          ( void );
static void                   ILI9341_SetCursor           ( uint16_t usX, uint16_t usY );
static __inline void          ILI9341_FillColor           ( uint32_t ulAmout_Point, uint16_t usColor );
static uint16_t               ILI9341_Read_PixelData      ( void );


/* 向ILI9341写入命令 usCmd为要写入的命令 */
void ILI9341_Write_Cmd(uint16_t usCmd)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_CMD) = usCmd;
}


/* 向ILI9341写入数据 usData为要写入的数据 */
void ILI9341_Write_Data(uint16_t usData)
{
	*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA) = usData;
}

/* 从ILI9341读取数据  */
uint16_t ILI9341_Read_Data(void)
{
	return (*(__IO uint16_t *)(FSMC_Addr_ILI9341_DATA));
}

/* 用于ILI9341简单延时函数 nCount延时计数值 */
static void ILI9341_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}


//初始化ILI9341的IO引脚
static void ILI9341_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能FSMC相关的GPIO时钟 */
												
	RCC_APB2PeriphClockCmd(
												 /* 控制信号线 */	
												 ILI9341_CS_CLK | ILI9341_DC_CLK |ILI9341_WR_CLK |
												 ILI9341_RD_CLK | ILI9341_RST_CLK |ILI9341_BK_CLK |												 
												 /* 数据信号线 */
	                       ILI9341_D0_GPIO_CLK | ILI9341_D1_GPIO_CLK | ILI9341_D2_GPIO_CLK |
												 ILI9341_D3_GPIO_CLK | ILI9341_D4_GPIO_CLK | ILI9341_D5_GPIO_CLK |
												 ILI9341_D6_GPIO_CLK | ILI9341_D7_GPIO_CLK | ILI9341_D8_GPIO_CLK |
												 ILI9341_D9_GPIO_CLK | ILI9341_D10_GPIO_CLK | ILI9341_D11_GPIO_CLK |
												 ILI9341_D12_GPIO_CLK | ILI9341_D13_GPIO_CLK | ILI9341_D14_GPIO_CLK | 
												 ILI9341_D15_GPIO_CLK ,ENABLE);
	
	/* --GPIO配置----------------------------------------------------- */
	
	/* 配置FSMC相对应的数据线,FSMC-D0~D15 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //配置为复用功能
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
	
	/* 配置FSMC相对应的控制线
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16   :LCD-DC
	 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //配置为复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
	GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
	GPIO_Init(ILI9341_WR_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
	GPIO_Init(ILI9341_RD_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
	GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);
	
	//配置LCD复位RST控制管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
	GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);
	
	//配置LCD背光BK控制管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
	GPIO_Init(ILI9341_BK_PORT, &GPIO_InitStructure);

}

/* 初始化LCD_FSMC模式配置 */
static void ILI9341_FSMC_Config(void)
{
	FSMC_NORSRAMInitTypeDef   FSMC_NORSRAMInitStructure = {0};
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming = {0};
	
	/* 使能FSMC外设时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	
	//地址建立时间（ADDSET）为1个HCLK 2/72M=28ns
	readWriteTiming.FSMC_AddressSetupTime = 0x01;   //地址建立时间
	
	//数据保持时间（DATAST）+1个HCLK = 5/72M = 70ns(对EM的STAM芯片)
	readWriteTiming.FSMC_DataSetupTime = 0x04;  //数据建立时间
	
	//选择控制的模式
	//模式B，异步NOR FLASH模式，与ILI9341的8080时序匹配
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;
	
	//以下配置与模式B无关
	//地址保持时间（ADDHOLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	
	//设置总线转换周期，仅用于复用模式的NOR操作
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	//设置时钟分频，仅用于同步类型的存储器
	readWriteTiming.FSMC_CLKDivision = 0x00;
	
	//数据保持时间，仅用于同步型的NOR
	readWriteTiming.FSMC_DataLatency = 0x00;
	
	
	//选择FSMC映射的存储区域：Bank1 sram3
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAMx;
	
	//选择地址总线与数据总线是否复用，仅用于NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	
	//设置要控制的存储器类型
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	
	//存储器数据宽度：16位
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	
	//设置是否使用突发访问模式，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	
	//设置等待信号的有效极性，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	
	//设置是否支持把非对齐的突发操作 仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	
	//设置等待信号的插入时间，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	
	//存储写使能
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	
	//不使用等待使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	
	//不使用扩展模式，读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	
	//突发写操作
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	
	//读写时序配置
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	
	//读写同样时序，使用扩展模式时这个配置才有效
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); // 初始化FSMC配置  把配置写入到寄存器
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  // 使能BANK
}

/* 初始化ILI9341寄存器 */
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
		ILI9341_Write_Data(0x06);//原来是02可以改成06可防止液晶显示屏白屏时出现条纹情况
		
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
		ILI9341_Write_Data(0xC8);  //竖屏 左上角到右下角扫描方式
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
    ILI9341_Write_Data ( 0x00 );  /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
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

/* ILI9341初始化函数，如果要用到LCD，一定要调用这个函数 */
void ILI9341_Init(void)
{
	ILI9341_GPIO_Config();
	ILI9341_FSMC_Config();
	ILI9341_BackLed_Control(ENABLE);  //点亮背光灯
	ILI9341_Rst();
	ILI9341_REG_Config();
	
	//设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9341_GramScan(LCD_SCAN_MODE);
}


/* ILI9341背光LED控制，
enumState:决定是否使能背光*/
void ILI9341_BackLed_Control(FunctionalState enumstate)
{
	if(enumstate)
		GPIO_ResetBits(ILI9341_BK_PORT,ILI9341_BK_PIN);
	else
		GPIO_SetBits(ILI9341_BK_PORT,ILI9341_BK_PIN);
}

/* 读取LCD驱动函数ID函数，可用于测试底层的读写函数 */
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


/* ILI9341软件复位 */
void ILI9341_Rst(void)
{
	GPIO_ResetBits(ILI9341_RST_PORT,ILI9341_RST_PIN); // 低电平复位
	ILI9341_Delay(0xAFF);
	GPIO_SetBits(ILI9341_RST_PORT,ILI9341_RST_PIN);
	ILI9341_Delay(0xAFF);
	
}

/**
 * @brief  设置ILI9341的GRAM的扫描方向 
 * @param  ucOption ：选择GRAM的扫描方向 
 *     @arg 0-7 :参数可选值为0-7这八个方向
 *
 *	！！！其中0、3、5、6 模式适合从左至右显示文字，
 *				不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 *		
 *	其中0、2、4、6 模式的X方向像素为240，Y方向像素为320
 *	其中1、3、5、7 模式下X方向像素为320，Y方向像素为240
 *
 *	其中 6 模式为大部分液晶例程的默认显示方向
 *	其中 3 模式为摄像头例程使用的方向
 *	其中 0 模式为BMP图片显示例程使用的方向
 *
 * @retval 无
 * @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
					X表示X轴，Y表示Y轴

											 LCDID_ILI9341
------------------------------------------------------------
模式0：				.		模式1：		.	模式2：			.	模式3：					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：			.	模式7：					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		
---------------------------------------------------------				
											 LCD屏示例
								|-----------------|
								|			野火Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								屏幕正面（宽240，高320）
								
								
								
								
								      LCDID_ST7789V							
------------------------------------------------------------
模式0：				.		模式1：		.	模式2：			.	模式3：					
	o 0X--->  	.		o 1Y--->  .	<--- X2 o		.	<--- Y3 o				
	0						.		1					.					2		.					3			
	Y						.		X					.					Y		.					X		
	|						.		|					.					|		.					|			
	V								V					.					V		.					V		
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：		.	模式7：					
	A						.		A					.					A	.					A	
	|						.		|					.					|	.					|
	Y						.		X					.					Y	.					X				
	4						.		5					.					6	.					7				
	o 4X--->  	.		o 5Y--->  .	<--- X6 o	.	<--- Y7 o	
---------------------------------------------------------				
											 LCD屏示例
								|-----------------|
								|			野火Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								屏幕正面（宽240，高320）		

 *******************************************************/
 
void ILI9341_GramScan(uint8_t ucOption)
{
	//参数检测，只可输入0-7
	if(ucOption >7)
		return;
	
	//根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
	LCD_SCAN_MODE = ucOption;
	
	//根据模式更新XY方向的像素宽度
	if(ucOption % 2 == 0)
	{
		//0、2、4、6模式下X方向像素宽度为240，Y方向为320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH = ILI9341_MORE_PIXEL;
	}
	else
	{
		//1、3、5、7、模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH = ILI9341_LESS_PIXEL;
	}
	
	//0x36命令参数的高3位可用于设置GRAM扫描方向
	ILI9341_Write_Cmd(0x36);
	if(lcdid == LCDID_ILI9341)
	{
		ILI9341_Write_Data(0x08 | (ucOption << 5));  //根据ucOption的值设置LCD参数，共0-7种模式
	}
	else if(lcdid == LCDID_ST7789V)
	{
		ILI9341_Write_Data(0x00 | (ucOption << 5));  //根据ucOption的值设置LCD参数，共0-7种模式
	}
	
	ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* 设置X坐标 */
	ILI9341_Write_Data ( 0x00 );      /* 起始坐标先高8位，再低8位 */
	ILI9341_Write_Data ( 0x00 );      
	ILI9341_Write_Data ( ((LCD_X_LENGTH -1 ) >> 8)&0xFF );/* x 结束坐标高8位 */	
	ILI9341_Write_Data ( (LCD_X_LENGTH - 1)&0xFF );	/* x 结束坐标低8位 */

	ILI9341_Write_Cmd ( CMD_SetCoordinateY );   /* 设置Y坐标 */
	ILI9341_Write_Data ( 0x00 );      /* 起始坐标先高8位，再低8位 */
	ILI9341_Write_Data ( 0x00 );      
	ILI9341_Write_Data ( ((LCD_Y_LENGTH -1 ) >> 8)&0xFF );/* Y 结束坐标高8位 */	
	ILI9341_Write_Data ( (LCD_Y_LENGTH - 1)&0xFF );	/* Y 结束坐标低8位 */
	
	ILI9341_Write_Cmd(CMD_SetPixel);
}

void ILI9341_OpenWindow(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	/* column address control set */
	ILI9341_Write_Cmd ( CMD_SetCoordinateX );   /* 设置X坐标 */
	ILI9341_Write_Data ( usX >> 8 );      /* 先高8位，再低8位 */
	ILI9341_Write_Data ( usX&0xFF );      /* 设置起始点和结束点 */
	ILI9341_Write_Data ( (usX+usWidth -1 ) >> 8 );
	ILI9341_Write_Data ( (usX+usWidth - 1)&0xff );	

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( usY >> 8 );      /* 先高8位，再低8位 */
	ILI9341_Write_Data ( usY&0xFF );      /*  */
	ILI9341_Write_Data ( (usY+usHeight -1 ) >> 8 );
	ILI9341_Write_Data ( (usY+usHeight - 1)&0xff );	
}

/* 设定ILI9341的光标坐标   usX：在特定扫描方向下光标的X坐标  usY：在特定扫描方向下光标的Y坐标 */
static void ILI9341_SetCursor(uint16_t usX ,uint16_t usY)
{
	ILI9341_OpenWindow(usX ,usY ,1,1);
}


/* 在ILI9341显示器上以某一颜色填充像素点
	 ulAmount_Point:要填充颜色的像素点的总数目
	 usColor：颜色*/
static __inline void ILI9341_FillColor(uint32_t ulAmount_Point,uint16_t usColor)
{
	uint32_t i = 0;
	
	/* 往寄存器写入 */
	ILI9341_Write_Cmd( CMD_SetPixel );
	
	for( i = 0 ; i < ulAmount_Point ; i++)
		ILI9341_Write_Data(usColor);
}

/* 对ILI9341显示器的某一窗口以某种颜色进行清屏
	 usX：在特定扫描方向下窗口的起点X坐标
	 usY：在特定扫描方向下窗口的起点Y坐标
	 usWidth：窗口的宽度
	 usHeigh：窗口的高度*/
void ILI9341_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	ILI9341_OpenWindow(usX,usY,usWidth,usHeight);
	ILI9341_FillColor(usWidth*usHeight,CurrentBackColor);
}

/* 对ILI9341显示器的某一点以某种颜色进行填充
	 usX：在特定扫描方向下窗口的起点X坐标
	 usY：在特定扫描方向下窗口的起点Y坐标*/
void ILI9341_SetPointPixel(uint16_t usX,uint16_t usY)
{
	if((usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH))
	{
		ILI9341_SetCursor(usX,usY);
		ILI9341_FillColor(1,CurrentTextColor);
	}
}

/* 读取GRAM的一个像素数据 */
static uint16_t ILI9341_Read_PixelData(void)
{
	uint16_t usRG = 0,usB= 0;
	
	ILI9341_Write_Cmd(0x2E);  //读数据
	
	ILI9341_Read_Data();  // 去掉第一次读取结果
	
	//获取红色通道和绿色通道的值
	usRG = ILI9341_Read_Data();
	usB = ILI9341_Read_Data();
	
	//高5位为红色，中间6位为绿色，后面5位为蓝色
	return ((usRG & 0xF800) | ((usRG << 3) &0x7E0) | (usB >> 11));
	
}

/* 获取ILI9341显示器的某一点坐标的像素数据
	 usX：在特定扫描方向下窗口的起点X坐标
	 usY：在特定扫描方向下窗口的起点Y坐标*/
uint16_t ILI9341_GetPointPixel( uint16_t usX, uint16_t usY)
{
	uint16_t usPixelData;
	
	ILI9341_SetCursor(usX,usY);
	usPixelData = ILI9341_Read_PixelData();
	
	return usPixelData;
}

/* 在ILI9341显示器上使用Bresenham算法画线段
	 usX1:在特定扫描方向下线段的一个端点X坐标
	 usY1:在特定扫描方向下线段的一个端点Y坐标
	 usX2:在特定扫描方向下线段的另一个端点X坐标
	 usY2:在特定扫描方向下线段的另一个端点Y坐标*/
void ILI9341_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //计算坐标增量 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //设置单步方向 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//垂直线 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//水平线 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //选取基本增量坐标轴 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//画线输出 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current );//画点 
		
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

/* 在ILI9341显示器上画一个矩形 
	 usX_Start:在特定扫描方向下矩形的起始点X坐标
	 usY_Start：在特定扫描方向下矩矩形的起始点Y坐标
	 usWidth:矩形的宽度
	 usHeigh:矩形的高度
	 ucFilled：选择是否填充该矩形 0为空心矩形 1为实心*/
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

/* 在ILI9341显示器上使用Bresenham算法画圆 
	 usX_Center:在特定扫描方向下圆心的起始点X坐标
	 usY_Center：在特定扫描方向下圆心的起始点Y坐标
	 usRadius:圆半径
	 ucFilled：选择是否填充该圆 0为空心圆 1为实心*/
//void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center ,uint16_t usRadius, uint8_t ucFilled)
//{
//	int16_t sCurrentX , sCurrentY;
//	int16_t sError;
//	
//	sCurrentX = 0; sCurrentY = usRadius;
//	
//	sError = 3 - ( usRadius << 1);  //判断下个点位置的坐标
//	
//	while(sCurrentX <= sCurrentY)
//	{
//		int16_t sCountY;
//		
//		if(ucFilled)
//			for( sCountY = sCurrentX ; sCountY <= sCurrentY ; sCountY ++)
//			{
//				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center + sCountY);
//				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center + sCountY);
//				ILI9341_SetPointPixel( usX_Center - sCountY , usY_Center + sCurrentX);
//				ILI9341_SetPointPixel( usX_Center - sCountY , usY_Center - sCurrentX);
//				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center - sCountY);
//				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center - sCountY);
//				ILI9341_SetPointPixel( usX_Center + sCountY , usY_Center - sCurrentX);
//				ILI9341_SetPointPixel( usX_Center + sCountY , usY_Center + sCurrentX);
//			}
//			else
//			{
//				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center + sCurrentY);
//				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center + sCurrentY);
//				ILI9341_SetPointPixel( usX_Center - sCurrentY , usY_Center + sCurrentX);
//				ILI9341_SetPointPixel( usX_Center - sCurrentY , usY_Center - sCurrentX);
//				ILI9341_SetPointPixel( usX_Center - sCurrentX , usY_Center - sCurrentY);
//				ILI9341_SetPointPixel( usX_Center + sCurrentX , usY_Center - sCurrentY);
//				ILI9341_SetPointPixel( usX_Center + sCurrentY , usY_Center - sCurrentX);
//				ILI9341_SetPointPixel( usX_Center + sCurrentY , usY_Center + sCurrentX);
//			}
//			
//			sCurrentX++;
//			
//			if( sError < 0)
//				sError += 4* sCurrentX +6;
//			
//			else
//			{
//				sError += 10 + 4 * ( sCurrentX - sCurrentY);
//				sCurrentY--;
//			}
//	}
//}

void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center ,uint16_t usRadius, uint8_t ucFilled)
{
	int16_t x ,y;
	
	for( y = - usRadius; y <= usRadius ; y++)
	{
		for( x = - usRadius ;  x <= usRadius ;x++)
		{
			if( x*y + y*y <= usRadius * usRadius)
			{
				int drawX = usX_Center + x;
				int drawY = usY_Center + y;
				
				if( ucFilled || ( x*y + y*y <= usRadius * usRadius))
				{
					ILI9341_SetPointPixel(drawX ,drawY);
				}
			}
		}
	}
}

/* 在ILI9341显示器上显示一个英文字符
	 usX：在特定扫描方向下字符的起始X坐标
	 usY：在特定扫描方向下字符的起始Y坐标
	 cChar：要显示的英文字符*/
void ILI9341_DispChar_EN(uint16_t usX,uint16_t usY,const char cChar)
{
	uint8_t byteCount,bitCount,fontLength;
	uint16_t ucRelativePosition;
	uint8_t *Pfont;
	
	//对ASCII码表（字模表不包含ASCII的前32个非图形符号）
	ucRelativePosition = cChar - ' ';
	
	//每个字模的字节数
	fontLength = (LCD_Currentfonts -> Width*LCD_Currentfonts ->Height)/8;
	
	//字模首地址
	/*ASCII码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8_t*)&LCD_Currentfonts -> table[ucRelativePosition * fontLength];
	
	//设置显示窗口
	ILI9341_OpenWindow(usX,usY,LCD_Currentfonts->Width,LCD_Currentfonts->Height);
	
	ILI9341_Write_Cmd(CMD_SetPixel);
	
	//按字节读取字模数据
	//由于前面直接设置了显示窗口，显示数据会自行切换
	for(byteCount = 0; byteCount < fontLength;byteCount++)
	{
		//一位一位处理要显示的颜色
		for(bitCount = 0; bitCount < 8; bitCount++)
		{
			if(Pfont[byteCount] & (0x80>>bitCount))
				ILI9341_Write_Data(CurrentTextColor);
			else
				ILI9341_Write_Data(CurrentBackColor);
		}
	}
	
}

/* 在ILI9341显示器上显示英文字符串
	 line：在特定扫描方向下字符串的起始Y坐标，
	 本参数可使用宏LINE(0)、LINE(1)等方式计算文字坐标
	 宏LINE(x)会根据当前选择的字体来计算Y坐标值。显示
	 中文且使用LINE宏时，需要把英文字体设置成Font8x16
	 pStr：要显示的英文字符串的首地址*/
void ILI9341_DispStringLine_EN(uint16_t line,char *pStr)
{
	uint16_t usX = 0;
	
	while( *pStr != '\0')
	{
		if( (usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			line += LCD_Currentfonts->Height;
		}
		
		if( (line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			line += ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN(usX,line,*pStr);
		
		pStr++;
		
		usX += LCD_Currentfonts->Width;
	}
}

/* 在ILI9341 显示器上显示英文字符串
	 usX：在特定扫描方向下字符的起始X坐标
   usY：在特定扫描方向下字符的起始Y坐标
	 pStr：要显示的英文字符串的首地址*/
void ILI9341_DispString_EN(uint16_t usX,uint16_t usY,char *pStr)
{
	while(*pStr != '\0')
	{
		if( (usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += LCD_Currentfonts->Height;
		}
		
		if( (usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN(usX,usY,*pStr);
		
		pStr++;
		usX += LCD_Currentfonts->Width;
	}
}


/* 在ILI9341 显示器上显示英文字符串(沿Y轴方向)
	 usX：在特定扫描方向下字符的起始X坐标
   usY：在特定扫描方向下字符的起始Y坐标
	 pStr：要显示的英文字符串的首地址*/
void ILI9341_DispString_EN_YDir(uint16_t usX,uint16_t usY,char *pStr)
{
	while(*pStr != '\0')
	{
		if( (usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usY = ILI9341_DispWindow_Y_Star;
			usX += LCD_Currentfonts->Width;
		}
		
		if( (usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN(usX,usY,*pStr);
		
		pStr++;
		usY += LCD_Currentfonts->Height;
	}
}

/* 设置英文字体类型 */
void LCD_SetFont(sFONT *fonts)
{
	LCD_Currentfonts = fonts;
}

/* 获取当前字体类型 */
sFONT *LCD_GetFont(void)
{
	return LCD_Currentfonts;
}

/* 设置LCD的前景（字体）及背景颜色RGB565
	 TextColor: 指定前景(字体)颜色
	 BackColor: 指定背景颜色*/
void LCD_SetColors(uint16_t TextColor,uint16_t BackColor)
{
	CurrentTextColor = TextColor;
	CurrentBackColor = BackColor;
}

/* 获取LCD的前景（字体 ）及背景颜色RGB565 color：指定前景（字体）颜色*/
void LCD_GetColors(uint16_t *TextColor,uint16_t *BackColor)
{
	*TextColor = CurrentTextColor;
	*BackColor = CurrentBackColor;
}

/* 设置LCD的前景（字体）颜色，RGB565，color：指定前景（字体）颜色 */
void LCD_SetTextColor(uint16_t Color)
{
	CurrentTextColor = Color;
}

/* 设置LCD的背景颜色，RGB565，color：指定背景颜色 */
void SetBackColor(uint16_t Color)
{
	CurrentBackColor = Color;
}

/* 清除某行文字 Line：指定要删除的行
	 本参数可使用宏LINE(0)、LINE(1)等方式指定要删除的行，
	 宏LINE(x)会根据当前选择的字体来计算Y坐标值，并删除当前字体高度的第x行*/
void LCD_ClearLine(uint16_t Line)
{
	ILI9341_Clear(0,Line,LCD_X_LENGTH,((sFONT *) LCD_GetFont())->Height); /* 清屏 显示全黑 */
}

 

