#include "bsp_ad7192_test.h"
#include "bsp_ad7192.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"


/* 4·����ת����ѹ���� */
void single_conversion_voltage(void)
{
	uint32_t ad_data = 0;
	float v= 0.0;
	uint32_t mode = 0,cfg = 0;
	int i;
	
	printf("\r\n Ұ��AD7192 4·����ת����ȡ��ѹʵ�� \r\n");
	
	/* ��ȡAD7192�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_COM_STA,8,AD7192Registers,REG_COM_STA);
	
	for(i =0;i<8;i++)
	{
		printf("\r\n AD7192Registers[%d] = 0x%06x \r\n",i+REG_COM_STA,AD7192Registers[i+REG_COM_STA]);
	}
	
	/* ����ת��|����״̬����|�ⲿ����|sinc4�˲���|������żУ��|ʱ�Ӳ���Ƶ|���õ�����ת��|����60Hz�ݲ�|1023 */
	mode = MODE_SING | DAT_STA_DIS | EXT_XTAL | SINC4 | ENPAR_DIS | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x3FF;
	
	/*����ն��|�ⲿ��׼��ѹ1|12���ͨ��(��)|���ü�������|���û�׼��ѹ���|����ģ�����뻺��|˫����ģʽ|����Ϊ1 */
	cfg = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	
	/* ����ģʽ�Ĵ��������üĴ��� */
	AD7192_Mode_Conf_Registers(mode,cfg);
	
	/* У׼ */
	printf("\r\n �ڲ�У׼�� \r\n");
	AD7192InternalZeroScaleCalibration();
	AD7192InternalFullScaleCalibration();
	printf("\r\n �ڲ�У׼��� \r\n");
	
	while(1)
	{
		/* ��ȡͨ��1ת������ */
		AD7192StartSingleConvertion(AIN1_AINCOM);
		ad_data = AD7192ReadConvertData();
		v= (ad_data / 8388608.0 -1) * 3.3;
		printf("AIN1_COM = %fV\n",v);
		
		/* ��ȡͨ��2ת������ */
		AD7192StartSingleConvertion(AIN2_AINCOM);
		ad_data = AD7192ReadConvertData();
		v= (ad_data / 8388608.0 -1) * 3.3;
		printf("AIN2_COM = %fV\n",v);
		
		/* ��ȡͨ��3ת������ */
		AD7192StartSingleConvertion(AIN3_AINCOM);
		ad_data = AD7192ReadConvertData();
		v= (ad_data / 8388608.0 -1) * 3.3;
		printf("AIN3_COM = %fV\n",v);
		
		/* ��ȡͨ��4ת������ */
		AD7192StartSingleConvertion(AIN4_AINCOM);
		ad_data = AD7192ReadConvertData();
		v= (ad_data / 8388608.0 -1) * 3.3;
		printf("AIN4_COM = %fV\n",v);
		
	}
	
	
}


