#include "bsp_adc.h"

__IO uint32_t ADC_ConvertedValue[NOFCHANEL] = {0};


/* ADC GPIO初始化 */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	//打开ADC1 IO端口的时钟
	ADCx_1_GPIO_APBxClock_FUN(ADCx_1_GPIO_CLK,ENABLE);
	
	//配置ADC1 IO引脚模式  必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = ADCx_1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//初始化 ADC1 IO
	GPIO_Init(ADCx_1_PORT,&GPIO_InitStructure);
	
	//打开ADC2 IO端口的时钟
	ADCx_1_GPIO_APBxClock_FUN(ADCx_2_GPIO_CLK,ENABLE);
	
	//配置ADC2 IO引脚模式  必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = ADCx_2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//初始化 ADC2 IO
	GPIO_Init(ADCx_2_PORT,&GPIO_InitStructure);
}

/* 配置ADC工作模式 */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef    ADC_InitStructure;
	DMA_InitTypeDef    DMA_InitSturcture;
	
	//打开DMA时钟
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	
	//打开ADC时钟
	ADCx_1_APBxClock_FUN(ADCx_1_CLK,ENABLE);
	ADCx_2_APBxClock_FUN(ADCx_2_CLK,ENABLE);
	
	/* ------------------DMA模式配置---------------- */	
	
	//复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitSturcture.DMA_BufferSize = NOFCHANEL;
	
	//数据源来自外设
	DMA_InitSturcture.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	//禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitSturcture.DMA_M2M = DMA_M2M_Disable;
	
	//存储器地址，实际上就是一个内部SRAM的变量
	DMA_InitSturcture.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	
	//内存数据大小，跟外设数据大小相同
	DMA_InitSturcture.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	
	//存储器地址递增
	DMA_InitSturcture.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	//循环传输模式
	DMA_InitSturcture.DMA_Mode = DMA_Mode_Circular;
	
	//外设基址为：ADC 数据寄存器地址
	DMA_InitSturcture.DMA_PeripheralBaseAddr = (uint32_t) (&(ADCx_1->DR));
	
	//外设数据大小
	DMA_InitSturcture.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	
	//外设寄存器只有一个，地址不用递增
	DMA_InitSturcture.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	//DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitSturcture.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(ADC_DMA_CHANNEL,&DMA_InitSturcture);
	
	DMA_Cmd(ADC_DMA_CHANNEL,ENABLE);
	
	
	/* ----------------ADCx_1 模式配置--------------------- */
	
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	// 双ADC的规则同步
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;
	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	
	// 初始化ADC
	ADC_Init(ADCx_1,&ADC_InitStructure);
	
	//配置ADC时钟为PCLK2的8分频，即9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// 配置 ADC 通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx_1,ADCx_1_CHANNEL,1,ADC_SampleTime_239Cycles5);
	
	
	//使能ADC DMA请求
	ADC_DMACmd(ADCx_1,ENABLE);
	
	/* ----------------ADCx_2 模式配置--------------------- */
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	//转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	//不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	//双ADC的规则同步
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	
	//转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;
	
	//扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	
	//初始化ADC
	ADC_Init(ADCx_2,&ADC_InitStructure);
	
	//配置ADC时钟为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx_2,ADCx_2_CHANNEL,1,ADC_SampleTime_239Cycles5);
	
	//使能ADCx_2的外部触发转换
	ADC_ExternalTrigConvCmd(ADC2,ENABLE);
	
	
	/* ----------------ADCx_1 校准--------------------- */
	
	//开启ADC，并开始转换
	ADC_Cmd(ADCx_1,ENABLE);
	
	// 初始化ADC 校准寄存器 
	ADC_ResetCalibration(ADCx_1);
	
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	
	// ADC开始校准
	ADC_StartCalibration(ADCx_1);
	
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADCx_1));
	
		/* ----------------ADCx_2 校准--------------------- */
	
	//开启ADC，并开始转换
	ADC_Cmd(ADCx_2,ENABLE);
	
	// 初始化ADC 校准寄存器 
	ADC_ResetCalibration(ADCx_2);
	
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	
	// ADC开始校准
	ADC_StartCalibration(ADCx_2);
	
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADCx_2));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADCx_1,ENABLE);
	

	
}



/* ADC初始化 */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();

}

