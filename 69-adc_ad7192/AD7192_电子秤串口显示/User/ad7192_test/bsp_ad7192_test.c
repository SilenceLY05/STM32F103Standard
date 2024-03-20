#include "bsp_ad7192_test.h"
#include "bsp_ad7192.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"
#include "bsp_key.h"


/* ���ӳ�ʵ�� */
void electronic_scale_test(void)
{
	static float coefficient = 173.6f;   //ת��ϵ��
	int32_t ad_data = 0;
	int32_t ad_data_init = 0;
	uint32_t mode = 0,cfg = 0;
	int i;
	
	printf("\r\n Ұ��AD7192 ���ӳ�ʵ�� \r\n");
	
	/* ��ȡAD7192�Ĵ��� */
	ReadFromAD7192ViaSPI(REG_COM_STA,8,AD7192Registers,REG_COM_STA);
	
	for(i =0;i<8;i++)
	{
		printf("\r\n AD7192Registers[%d] = 0x%06x \r\n",i+REG_COM_STA,AD7192Registers[i+REG_COM_STA]);
	}
	
	/* ����ת��|����״̬����|�ⲿ����|sinc4�˲���|������żУ��|ʱ�Ӳ���Ƶ|���õ�����ת��|ʹ��60Hz�ݲ�|1023 */
	mode = MODE_SING | DAT_STA_DIS | EXT_XTAL | SINC4 | ENPAR_DIS | CLK_DIV_DIS | SINGLE_DIS | REJ60_EN | 0x3FF;
	
	/*ʹ��ն��|�ⲿ��׼��ѹ1|12���ͨ��(��)|���ü�������|���û�׼��ѹ���|����ģ�����뻺��|˫����ģʽ|����Ϊ128 */
	cfg = CHOP_EN | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_128;
	
	/* ����ģʽ�Ĵ��������üĴ��� */
	AD7192_Mode_Conf_Registers(mode,cfg);
	
	/* У׼ */
	printf("\r\n �ڲ�У׼�� \r\n");
	AD7192InternalZeroScaleCalibration();
	AD7192InternalFullScaleCalibration();
	printf("\r\n �ڲ�У׼��� \r\n");
	
	/* ������� */
	AD7192StartContinuousConvertion(AIN1_AIN2);
	
	/* ��¼��ʼֵ */
	ad_data_init = AD7192ReadConvertData();
	
	while(1)
	{
		ad_data = AD7192ReadConvertData();
		
		printf("ad_data - ad_data_init = %d\n",ad_data - ad_data_init);
		printf("����Ϊ��%.2fg\n",(ad_data - ad_data_init) / coefficient);
		
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			/* ȥƤ���� */
			ad_data_init = AD7192ReadConvertData();
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			/* У׼�����ڲ�ͬ�Ĵ������ı仯�ʲ�һ�������п�����Ҫʱ��У׼ */
			/* ����50g���밴��KEY2 */
			ad_data_init = AD7192ReadConvertData();
			coefficient = (ad_data - ad_data_init) / 50.0;
			printf("coefficient = %f\r\n",coefficient);
		}
		SysTick_Delay_Ms(100);
		
	}
	
	
}


