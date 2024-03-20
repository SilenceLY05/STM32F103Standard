#include "bsp_ad7192_test.h"
#include "bsp_ad7192.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"
#include "bsp_key.h"


/* 电子秤实验 */
void electronic_scale_test(void)
{
	static float coefficient = 173.6f;   //转换系数
	int32_t ad_data = 0;
	int32_t ad_data_init = 0;
	uint32_t mode = 0,cfg = 0;
	int i;
	
	printf("\r\n 野火AD7192 电子秤实验 \r\n");
	
	/* 读取AD7192寄存器 */
	ReadFromAD7192ViaSPI(REG_COM_STA,8,AD7192Registers,REG_COM_STA);
	
	for(i =0;i<8;i++)
	{
		printf("\r\n AD7192Registers[%d] = 0x%06x \r\n",i+REG_COM_STA,AD7192Registers[i+REG_COM_STA]);
	}
	
	/* 单次转换|禁用状态传输|外部晶振|sinc4滤波器|禁用奇偶校验|时钟不分频|禁用单周期转换|使能60Hz陷波|1023 */
	mode = MODE_SING | DAT_STA_DIS | EXT_XTAL | SINC4 | ENPAR_DIS | CLK_DIV_DIS | SINGLE_DIS | REJ60_EN | 0x3FF;
	
	/*使能斩波|外部基准电压1|12差分通道(单)|禁用激励电流|禁用基准电压检测|禁用模拟输入缓冲|双极性模式|增益为128 */
	cfg = CHOP_EN | REFSEL_IN1 | AIN1_AIN2 | BURN_DIS | REFDET_DIS | BUF_DIS | UB_BI | GAIN_128;
	
	/* 配置模式寄存器和配置寄存器 */
	AD7192_Mode_Conf_Registers(mode,cfg);
	
	/* 校准 */
	printf("\r\n 内部校准中 \r\n");
	AD7192InternalZeroScaleCalibration();
	AD7192InternalFullScaleCalibration();
	printf("\r\n 内部校准完毕 \r\n");
	
	/* 差分输入 */
	AD7192StartContinuousConvertion(AIN1_AIN2);
	
	/* 记录初始值 */
	ad_data_init = AD7192ReadConvertData();
	
	while(1)
	{
		ad_data = AD7192ReadConvertData();
		
		printf("ad_data - ad_data_init = %d\n",ad_data - ad_data_init);
		printf("重量为：%.2fg\n",(ad_data - ad_data_init) / coefficient);
		
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			/* 去皮清零 */
			ad_data_init = AD7192ReadConvertData();
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			/* 校准，由于不同的传感器的变化率不一样所以有可能需要时间校准 */
			/* 放上50g砝码按下KEY2 */
			ad_data_init = AD7192ReadConvertData();
			coefficient = (ad_data - ad_data_init) / 50.0;
			printf("coefficient = %f\r\n",coefficient);
		}
		SysTick_Delay_Ms(100);
		
	}
	
	
}


