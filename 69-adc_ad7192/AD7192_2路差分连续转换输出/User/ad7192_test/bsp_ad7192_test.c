#include "bsp_ad7192_test.h"
#include "bsp_ad7192.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"


/* 2·�������ת����ѹ���� */
void difference_continuous_conversion_voltage(void)
{
	uint32_t ad_data = 0;
	float v= 0.0;
	uint32_t mode = 0,cfg = 0;
	int i;
	
	printf("\r\n Ұ��AD7192 2·�������ת����ȡ��ѹʵ�� \r\n");
	
	/* ��ȡAD7192�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_COM_STA,8,AD7192Registers,REG_COM_STA);
	
	for(i =0;i<8;i++)
	{
		printf("\r\n AD7192Registers[%d] = 0x%06x \r\n",i+REG_COM_STA,AD7192Registers[i+REG_COM_STA]);
	}
	
	/* ����ת��|ʹ��״̬����|�ⲿ����|sinc4�˲���|������żУ��|ʱ�Ӳ���Ƶ|���õ�����ת��|����60Hz�ݲ�|1023 */
	mode = MODE_CONT | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_DIS | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x3FF;
	
	/*����ն��|�ⲿ��׼��ѹ1|12���ͨ��(��)|���ü�������|���û�׼��ѹ���|����ģ�����뻺��|˫����ģʽ|����Ϊ1 */
	cfg = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	
	/* ����ģʽ�Ĵ��������üĴ��� */
	AD7192_Mode_Conf_Registers(mode,cfg);
	
	/* У׼ */
	AD7192InternalZeroScaleCalibration();
	AD7192InternalFullScaleCalibration();
	
	/* ��������ת�� */
	AD7192StartContinuousConvertion(AIN1_AIN2 | AIN3_AIN4);
	AD7192StartContinuousRead();
	
	while(1)
	{
		ad_data = AD7192ReadConvertData();
		
		switch(ad_data & 7)
		{
			case 0:
				v= ((ad_data>>8) / 8388608.0 -1) *3.3;
				printf("AIN1_AIN2 = %fV\n",v);
			break;
			
			case 1:
				v = ((ad_data >> 8) / 8388608.0 -1) *3.3;
				printf("AIN3_AIN4 = %fV\n",v);
			break;
		}
		
		SysTick_Delay_Ms(200);
	}
	
	
}


