#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;


/* ADC GPIO��ʼ�� */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	//��ADC IO�˿ڵ�ʱ��
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK,ENABLE);
	
	//����ADC IO����ģʽ  ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//��ʼ�� ADC IO
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);
}

/* ����ADC����ģʽ */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef    ADC_InitStructure;
	DMA_InitTypeDef    DMA_InitSturcture;
	
	//��DMAʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	
	//��ADCʱ��
	ADC_APBxClock_FUN(ADC_CLK,ENABLE);
	
	//��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// ���� DMA ��ʼ���ṹ��
	// ��������СΪ1���������Ĵ�СӦ�õ��ڴ洢���Ĵ�С
	DMA_InitSturcture.DMA_BufferSize = 1;
	
	//����Դ��������
	DMA_InitSturcture.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	//��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitSturcture.DMA_M2M = DMA_M2M_Disable;
	
	//�洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
	DMA_InitSturcture.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	
	//�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitSturcture.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	//�洢����ַ�̶�
	DMA_InitSturcture.DMA_MemoryInc = DMA_MemoryInc_Disable;
	
	//ѭ������ģʽ
	DMA_InitSturcture.DMA_Mode = DMA_Mode_Circular;
	
	//�����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitSturcture.DMA_PeripheralBaseAddr = (uint32_t) (&(ADC_x->DR));
	
	//�������ݴ�СΪ���֣��������ֽ�
	DMA_InitSturcture.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	//����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitSturcture.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	//DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitSturcture.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(ADC_DMA_CHANNEL,&DMA_InitSturcture);
	
	DMA_Cmd(ADC_DMA_CHANNEL,ENABLE);
	
	
	// ADC ģʽ����
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	// ֻʹ��һ��ADC�����ڶ���ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	
	// ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	
	// ��ʼ��ADC
	ADC_Init(ADC_x,&ADC_InitStructure);
	
	//����ADCʱ��ΪPCLK2��8��Ƶ����9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	// ���� ADC ͨ��ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC_x,ADC_CHANNEL,1,ADC_SampleTime_55Cycles5);
	
	
	//ʹ��ADC DMA����
	ADC_DMACmd(ADC_x,ENABLE);
	
	//����ADC������ʼת��
	ADC_Cmd(ADC_x,ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ��� 
	ADC_ResetCalibration(ADC_x);
	
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC_x);
	
	//�ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// ����û�в����ⲿ����������ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADC_x,ENABLE);
	

	
}



/* ADC��ʼ�� */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();

}

