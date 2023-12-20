#include "bsp_adc.h"

__IO uint32_t ADC_ConvertedValue[NOFCHANEL] = {0};


/* ADC GPIO��ʼ�� */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	//��ADC1 IO�˿ڵ�ʱ��
	ADCx_1_GPIO_APBxClock_FUN(ADCx_1_GPIO_CLK,ENABLE);
	
	//����ADC1 IO����ģʽ  ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = ADCx_1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//��ʼ�� ADC1 IO
	GPIO_Init(ADCx_1_PORT,&GPIO_InitStructure);
	
	//��ADC2 IO�˿ڵ�ʱ��
	ADCx_1_GPIO_APBxClock_FUN(ADCx_2_GPIO_CLK,ENABLE);
	
	//����ADC2 IO����ģʽ  ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = ADCx_2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//��ʼ�� ADC2 IO
	GPIO_Init(ADCx_2_PORT,&GPIO_InitStructure);
}

/* ����ADC����ģʽ */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef    ADC_InitStructure;
	DMA_InitTypeDef    DMA_InitSturcture;
	
	//��DMAʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	
	//��ADCʱ��
	ADCx_1_APBxClock_FUN(ADCx_1_CLK,ENABLE);
	ADCx_2_APBxClock_FUN(ADCx_2_CLK,ENABLE);
	
	/* ------------------DMAģʽ����---------------- */	
	
	//��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// ���� DMA ��ʼ���ṹ��
	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitSturcture.DMA_BufferSize = NOFCHANEL;
	
	//����Դ��������
	DMA_InitSturcture.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	//��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitSturcture.DMA_M2M = DMA_M2M_Disable;
	
	//�洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
	DMA_InitSturcture.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	
	//�ڴ����ݴ�С�����������ݴ�С��ͬ
	DMA_InitSturcture.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	
	//�洢����ַ����
	DMA_InitSturcture.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	//ѭ������ģʽ
	DMA_InitSturcture.DMA_Mode = DMA_Mode_Circular;
	
	//�����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitSturcture.DMA_PeripheralBaseAddr = (uint32_t) (&(ADCx_1->DR));
	
	//�������ݴ�С
	DMA_InitSturcture.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	
	//����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitSturcture.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	//DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitSturcture.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(ADC_DMA_CHANNEL,&DMA_InitSturcture);
	
	DMA_Cmd(ADC_DMA_CHANNEL,ENABLE);
	
	
	/* ----------------ADCx_1 ģʽ����--------------------- */
	
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	// ˫ADC�Ĺ���ͬ��
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	
	// ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;
	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	
	// ��ʼ��ADC
	ADC_Init(ADCx_1,&ADC_InitStructure);
	
	//����ADCʱ��ΪPCLK2��8��Ƶ����9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// ���� ADC ͨ��ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADCx_1,ADCx_1_CHANNEL,1,ADC_SampleTime_239Cycles5);
	
	
	//ʹ��ADC DMA����
	ADC_DMACmd(ADCx_1,ENABLE);
	
	/* ----------------ADCx_2 ģʽ����--------------------- */
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	//ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	//�����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	//˫ADC�Ĺ���ͬ��
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	
	//ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;
	
	//ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	
	//��ʼ��ADC
	ADC_Init(ADCx_2,&ADC_InitStructure);
	
	//����ADCʱ��ΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADCx_2,ADCx_2_CHANNEL,1,ADC_SampleTime_239Cycles5);
	
	//ʹ��ADCx_2���ⲿ����ת��
	ADC_ExternalTrigConvCmd(ADC2,ENABLE);
	
	
	/* ----------------ADCx_1 У׼--------------------- */
	
	//����ADC������ʼת��
	ADC_Cmd(ADCx_1,ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ��� 
	ADC_ResetCalibration(ADCx_1);
	
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADCx_1);
	
	//�ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADCx_1));
	
		/* ----------------ADCx_2 У׼--------------------- */
	
	//����ADC������ʼת��
	ADC_Cmd(ADCx_2,ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ��� 
	ADC_ResetCalibration(ADCx_2);
	
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADCx_2);
	
	//�ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADCx_2));
	
	// ����û�в����ⲿ����������ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADCx_1,ENABLE);
	

	
}



/* ADC��ʼ�� */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();

}

