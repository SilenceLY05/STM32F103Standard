#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;


/* ADC GPIO初始化 */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	//打开ADC IO端口的时钟
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK,ENABLE);
	
	//配置ADC IO引脚模式  必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//初始化 ADC IO
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);
}

/* 配置ADC工作模式 */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef    ADC_InitStructure;
	
	//打开ADC时钟
	ADC_APBxClock_FUN(ADC_CLK,ENABLE);
	
	// ADC 模式配置
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	// 只使用一个ADC，属于独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	
	// 初始化ADC
	ADC_Init(ADC_x,&ADC_InitStructure);
	
	//配置ADC时钟为PCLK2的8分频，即9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// 配置 ADC 通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADC_x,ADC_CHANNEL,1,ADC_SampleTime_55Cycles5);
	
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADC_x,ADC_IT_EOC,ENABLE);
	
	//开启ADC，并开始转换
	ADC_Cmd(ADC_x,ENABLE);
	
	// 初始化ADC 校准寄存器 
	ADC_ResetCalibration(ADC_x);
	
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC开始校准
	ADC_StartCalibration(ADC_x);
	
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADC_x,ENABLE);
	

	
}

/* 配置嵌套向量中断控制器NVIC */
static void ADC_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/* 嵌套向量中断控制器组选择 */
	/* 提示NVIC_PriorityGroupConfig()在整个工程只需要调用一次来配置优先级分组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* 配置ADC为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
	/* 抢断优先级 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
	
}

/* ADC初始化 */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	ADC_NVIC_Config();
}

