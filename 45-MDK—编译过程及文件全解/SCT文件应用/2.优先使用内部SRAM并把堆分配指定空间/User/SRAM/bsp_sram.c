#include "bsp_sram.h"


static void SRAM_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 使能SRAM相关的GPIO时钟 */
												/* 地址信号线 */		
	RCC_APB2PeriphClockCmd(FSMC_A0_GPIO_CLK | FSMC_A1_GPIO_CLK |FSMC_A2_GPIO_CLK |
												 FSMC_A3_GPIO_CLK | FSMC_A4_GPIO_CLK |FSMC_A5_GPIO_CLK |
											   FSMC_A6_GPIO_CLK | FSMC_A7_GPIO_CLK |FSMC_A8_GPIO_CLK |
												 FSMC_A9_GPIO_CLK | FSMC_A10_GPIO_CLK |FSMC_A11_GPIO_CLK |
											   FSMC_A12_GPIO_CLK | FSMC_A13_GPIO_CLK |FSMC_A14_GPIO_CLK |
												 FSMC_A15_GPIO_CLK | FSMC_A16_GPIO_CLK |FSMC_A17_GPIO_CLK |FSMC_A18_GPIO_CLK |
												/* 数据信号线 */
	                       FSMC_D0_GPIO_CLK | FSMC_D1_GPIO_CLK | FSMC_D2_GPIO_CLK |
												 FSMC_D3_GPIO_CLK | FSMC_D4_GPIO_CLK | FSMC_D5_GPIO_CLK |
												 FSMC_D6_GPIO_CLK | FSMC_D7_GPIO_CLK | FSMC_D8_GPIO_CLK |
												 FSMC_D9_GPIO_CLK | FSMC_D10_GPIO_CLK | FSMC_D11_GPIO_CLK |
												 FSMC_D12_GPIO_CLK | FSMC_D13_GPIO_CLK | FSMC_D14_GPIO_CLK | FSMC_D15_GPIO_CLK |
												/* 控制信号线 */
												 FSMC_CS_GPIO_CLK | FSMC_WE_GPIO_CLK | FSMC_OE_GPIO_CLK |
												 FSMC_UDQM_GPIO_CLK | FSMC_LDQM_GPIO_CLK ,ENABLE);
	
	/* --GPIO配置----------------------------------------------------- */
	
	/* 通用GPIO配置 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //配置为复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* A地址信号线 针对引脚配置 */
	GPIO_InitStructure.GPIO_Pin = FSMC_A0_GPIO_PIN;
	GPIO_Init(FSMC_A0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A1_GPIO_PIN;
	GPIO_Init(FSMC_A1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A2_GPIO_PIN;
	GPIO_Init(FSMC_A2_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A3_GPIO_PIN;
	GPIO_Init(FSMC_A3_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A4_GPIO_PIN;
	GPIO_Init(FSMC_A4_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A5_GPIO_PIN;
	GPIO_Init(FSMC_A5_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A6_GPIO_PIN;
	GPIO_Init(FSMC_A6_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A7_GPIO_PIN;
	GPIO_Init(FSMC_A7_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A8_GPIO_PIN;
	GPIO_Init(FSMC_A8_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A9_GPIO_PIN;
	GPIO_Init(FSMC_A9_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A10_GPIO_PIN;
	GPIO_Init(FSMC_A10_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A11_GPIO_PIN;
	GPIO_Init(FSMC_A11_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A12_GPIO_PIN;
	GPIO_Init(FSMC_A12_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A13_GPIO_PIN;
	GPIO_Init(FSMC_A13_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A14_GPIO_PIN;
	GPIO_Init(FSMC_A14_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A15_GPIO_PIN;
	GPIO_Init(FSMC_A15_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A16_GPIO_PIN;
	GPIO_Init(FSMC_A16_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A17_GPIO_PIN;
	GPIO_Init(FSMC_A17_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A18_GPIO_PIN;
	GPIO_Init(FSMC_A18_GPIO_PORT, &GPIO_InitStructure);
	
	/* D数据信号线 针对引脚配置 */
	GPIO_InitStructure.GPIO_Pin = FSMC_D0_GPIO_PIN;
	GPIO_Init(FSMC_D0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D1_GPIO_PIN;
	GPIO_Init(FSMC_D1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D2_GPIO_PIN;
	GPIO_Init(FSMC_D2_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D3_GPIO_PIN;
	GPIO_Init(FSMC_D3_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D4_GPIO_PIN;
	GPIO_Init(FSMC_D4_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D5_GPIO_PIN;
	GPIO_Init(FSMC_D5_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D6_GPIO_PIN;
	GPIO_Init(FSMC_D6_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D7_GPIO_PIN;
	GPIO_Init(FSMC_D7_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D8_GPIO_PIN;
	GPIO_Init(FSMC_D8_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D9_GPIO_PIN;
	GPIO_Init(FSMC_D9_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D10_GPIO_PIN;
	GPIO_Init(FSMC_D10_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D11_GPIO_PIN;
	GPIO_Init(FSMC_D11_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D12_GPIO_PIN;
	GPIO_Init(FSMC_D12_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D13_GPIO_PIN;
	GPIO_Init(FSMC_D13_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D14_GPIO_PIN;
	GPIO_Init(FSMC_D14_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D15_GPIO_PIN;
	GPIO_Init(FSMC_D15_GPIO_PORT, &GPIO_InitStructure);
	
	/* 控制信号线 */
  GPIO_InitStructure.GPIO_Pin = FSMC_CS_GPIO_PIN;
	GPIO_Init(FSMC_CS_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_WE_GPIO_PIN;
	GPIO_Init(FSMC_WE_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_OE_GPIO_PIN;
	GPIO_Init(FSMC_OE_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = FSMC_UDQM_GPIO_PIN;
	GPIO_Init(FSMC_UDQM_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_LDQM_GPIO_PIN;
	GPIO_Init(FSMC_LDQM_GPIO_PORT, &GPIO_InitStructure);

}

/* 初始化FSMC外设 */
void FSMC_SRAM_Init(void)
{
	FSMC_NORSRAMInitTypeDef   FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	
	/* 初始化SRAM相关的GPIO */
	SRAM_GPIO_Config();
	
	/* 使能FSMC外设时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	
	//地址建立时间（ADDSET）为1个HCLK 1/72M=14ns
	readWriteTiming.FSMC_AddressSetupTime = 0x00;
	
	//地址保持时间（ADDHOLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	
	//数据保持时间（DATAST）+1个HCLK = 3/72M = 42ns(对EM的STAM芯片)
	readWriteTiming.FSMC_DataSetupTime = 0x02;  //2是经验值，理论值为1，后面再测试
	
	//设置总线转换周期，仅用于复用模式的NOR操作
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	//设置时钟分频，仅用于同步类型的存储器
	readWriteTiming.FSMC_CLKDivision = 0x00;
	
	//数据保持时间，仅用于同步型的NOR
	readWriteTiming.FSMC_DataLatency = 0x00;
	
	//选择匹配SRAM的模式
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;
	
	
	//选择FSMC映射的存储区域：Bank1 sram3
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	
	//选择地址总线与数据总线是否复用，仅用于NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	
	//设置要控制的存储器类型：SRAM类型
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	
	//存储器数据宽度：16位
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	
	//设置是否使用突发访问模式，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	
	//设置是否使能等待信号，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	
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
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK
}

/* 以‘字’为单位向SDRAM写入数据 */
void SRAM_WriteBuffer(uint32_t *pBuffer, uint32_t uwWriteAddress,uint32_t uwBufferSize)
{
	__IO uint32_t write_pointer = (uint32_t)uwWriteAddress;
	
	/* 循环写入数据 */
	for(; uwBufferSize != 0; uwBufferSize--)
	{
		/* 发送数据到SRAM */
		*(uint32_t *)(Bank1_SRAM3_ADDR + write_pointer) = *pBuffer++;
		
		/* 地址自增 */
		write_pointer += 4;
	}
}

/* 从SRAM中读取数据 */
void SRAM_ReadBuffer(uint32_t* pBuffer,uint32_t uwReadAddress,uint32_t uwBufferSize)
{
	__IO uint32_t write_pointer = (uint32_t)uwReadAddress;
	
	/* 读取数据 */
	for(;uwBufferSize != 0x00; uwBufferSize--)
	{
		*pBuffer++ = *(__IO uint32_t *)(Bank1_SRAM3_ADDR + write_pointer);
		
		/* 地址自增 */
		write_pointer += 4;
	}
}

/* 测试SRAM是否正常 */
uint8_t SRAM_Test(void)
{
	/* 写入计数器 */
	uint32_t counter = 0;
	
	/* 8位数据 */
	uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;
	
	/* 16位数据 */
	uint16_t ubWritedata_16b = 0, ubReaddata_16b = 0;
	
	SRAM_INFO("正在检测SRAM，以8位、16位的方式读写sram……");
	
	/* 按8位格式读写数据，并校验 */
	
	/* 把SRAM数据全部重置为0，IS62WV51216_SIZE是以8位为单位的 */
	for(counter = 0 ;counter < IS62WV51216_SIZE ; counter++)
	{
		*(__IO uint8_t*)(Bank1_SRAM3_ADDR + counter) = (uint8_t)0x0;
	}
	
	/* 向整个SRAM写入8位数据 */
	for(counter = 0 ;counter < IS62WV51216_SIZE ; counter++)
	{
		*(__IO uint8_t*)(Bank1_SRAM3_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
	}
	
	/* 读取SRAM数据并测试 */
	for(counter = 0; counter < IS62WV51216_SIZE ; counter++)
	{
		ubReaddata_8b = *(__IO uint8_t*)(Bank1_SRAM3_ADDR + counter); // 从该地址读出数据
		
		if(ubReaddata_8b != *(__IO uint8_t*)(Bank1_SRAM3_ADDR + counter))
		{
			SRAM_ERROR("8位数据读写错误！");
			return 0;
		}
	}
	
	/* 按16位格式读写数据，并校验 */
	
	/* 把SRAM数据全部重置为0，IS62WV51216_SIZE是以8位为单位的 */
	for(counter = 0x00 ;counter < IS62WV51216_SIZE/2 ; counter++)
	{
		*(__IO uint16_t*)(Bank1_SRAM3_ADDR + 2*counter) = (uint16_t)0x00;
	}
	
	/* 向整个SRAM写入16位数据 */
	for(counter = 0x00 ;counter < IS62WV51216_SIZE/2 ; counter++)
	{
		*(__IO uint16_t*)(Bank1_SRAM3_ADDR + 2*counter) = (uint16_t)(ubWritedata_16b + counter);
	}
	
	/* 读取SRAM数据并测试 */
	for(counter = 0; counter < IS62WV51216_SIZE/2 ; counter++)
	{
		ubReaddata_16b = *(__IO uint16_t*)(Bank1_SRAM3_ADDR + 2*counter); // 从该地址读出数据
		
		if(ubReaddata_16b != *(__IO uint16_t*)(Bank1_SRAM3_ADDR + 2*counter))
		{
			SRAM_ERROR("16位数据读写错误！");
			return 0;
		}
	}
	
	SRAM_INFO("SRAM读写测试正常！"); 
  /*检测正常，return 1 */
  return 1;
	
}

