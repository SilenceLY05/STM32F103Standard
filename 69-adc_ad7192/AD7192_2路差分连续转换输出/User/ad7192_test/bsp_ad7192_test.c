#include "bsp_ad7192_test.h"
#include "bsp_ad7192.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"


/* 2路差分连续转换电压试验 */
void difference_continuous_conversion_voltage(void)
{
	uint32_t ad_data = 0;
	float v= 0.0;
	uint32_t mode = 0,cfg = 0;
	int i;
	
	printf("\r\n 野火AD7192 2路差分连续转换读取电压实验 \r\n");
	
	/* 读取AD7192寄存器 */
	ReadFromAD7192ViaSPI(REG_COM_STA,8,AD7192Registers,REG_COM_STA);
	
	for(i =0;i<8;i++)
	{
		printf("\r\n AD7192Registers[%d] = 0x%06x \r\n",i+REG_COM_STA,AD7192Registers[i+REG_COM_STA]);
	}
	
	/* 单次转换|使能状态传输|外部晶振|sinc4滤波器|禁用奇偶校验|时钟不分频|禁用单周期转换|禁用60Hz陷波|1023 */
	mode = MODE_CONT | DAT_STA_EN | EXT_XTAL | SINC4 | ENPAR_DIS | CLK_DIV_DIS | SINGLE_DIS | REJ60_DIS | 0x3FF;
	
	/*禁用斩波|外部基准电压1|12差分通道(单)|禁用激励电流|禁用基准电压检测|禁用模拟输入缓冲|双极性模式|增益为1 */
	cfg = CHOP_DIS | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_1;
	
	/* 配置模式寄存器和配置寄存器 */
	AD7192_Mode_Conf_Registers(mode,cfg);
	
	/* 校准 */
	AD7192InternalZeroScaleCalibration();
	AD7192InternalFullScaleCalibration();
	
	/* 启动连续转换 */
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


