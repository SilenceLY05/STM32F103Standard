#include "bsp_ap3216c.h"
#include "bsp_i2c_ee.h"

static void Delay_MS(__IO uint32_t nCount);
static void write_reg(uint8_t reg,uint8_t data);
static void read_reg(uint8_t reg,uint8_t len,uint8_t *buf);
static void reset_sensor(void);
static void set_als_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold);
static void set_ps_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold);
static void ap3216c_int_init(void);





#define PKG_USING_AP3216C

#ifdef PKG_USING_AP3216C

/* �����ж��������� */
void AP_INT_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* ��������ʱ�� */
	AP_INT_SCK_APBxClock_FUN(AP_INT_GPIO_CLK,ENABLE);
	
	/* �������� ѡ������ģʽ */
	GPIO_InitStructure.GPIO_Pin = AP_INT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(AP_INT_GPIO_PORT,&GPIO_InitStructure);
}

/* д�Ĵ���ֵ */
static void write_reg(uint8_t reg,uint8_t data)
{
	/*����0������ֹͣ�źţ������ڲ�д������*/
	i2c_Stop();
	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);  //�˴���дָ��
	
	/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail;  /* EEPROM����д��ʱ */
	}
	
	/* ��4������ʼд��Ĵ�����ַ */
	i2c_SendByte(reg);
	
	/* ��5�������ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* ��������Ӧ */
	}
	/* ��5������ʼд������ */
	i2c_SendByte(data);
	
	/* ��5�������ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* ��������Ӧ */
	}
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	i2c_Stop();
	return;
	
cmd_fail:/* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return ;
}

/* ���Ĵ�����ֵ */
static void read_reg(uint8_t reg,uint8_t len,uint8_t *buf)
{
	/*����0������ֹͣ�źţ������ڲ�д������*/
	i2c_Stop();
	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);  //�˴���дָ��
	
	/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail;  /* EEPROM����д��ʱ */
	}
	
	/* ��4������ʼд��Ĵ�����ַ */
	i2c_SendByte(reg);
	
	/* ��5�������ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* ��������Ӧ */
	}
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	
	/* ��6��������I2C���������ź� */
	i2c_Start();
	
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_RD);  //�˴��Ƕ�ָ��
	
	/* ��8�������ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* ��������Ӧ */
	}
	while(len)
	{
		*buf = i2c_ReadByte();
		buf++;
		
		if(len == 1)
		{
			i2c_NAck();/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
		else
		{
			i2c_Ack();/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */  
		}
		len--;
	}
	i2c_Stop();
	return ;
	
cmd_fail:/* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return ;
}

/* �����λ������ */
static void reset_sensor(void)
{
	write_reg(AP3216C_SYS_CONFIGURATION_REG,AP3216C_MODE_SW_RESET);  //RESET
}

/* ��ȡ��λ�Ĵ����͸�λ�Ĵ��� */
uint32_t read_low_and_high(uint8_t reg,uint8_t len)
{
	uint32_t data;
	uint8_t buf = 0;
	
	read_reg(reg,len,&buf);  //�����ֽ�
	data = buf;
	read_reg(reg+1,len,&buf);  //�����ֽ�
	data = data+(buf<<len*8);  //�ϲ�����
	
	return data;
}

/* ����AP3216C���������� */
void ap3216c_set_param(ap3216c_cmd_t cmd,uint8_t value)
{
	switch(cmd)
	{
		case AP3216C_SYSTEM_MODE:
		{
			write_reg(AP3216C_SYS_CONFIGURATION_REG,value);
			
			break;
		}
		case AP3216C_INT_PARAM:
		{
			write_reg(AP3216C_SYS_CONFIGURATION_REG,value);
			
			break;
		}
		case AP3216C_ALS_RANGE:
		{
			uint8_t args;
			
			read_reg(AP3216C_ALS_CONFIGURATION_REG,1,&args);
			args &= 0xcf;
			args |= value<<4;
			write_reg(AP3216C_ALS_CONFIGURATION_REG,args);
			
			break;
		}
		case AP3216C_ALS_PERSIST:
		{
			uint8_t args = 0;
			
			read_reg(AP3216C_ALS_CONFIGURATION_REG,1,&args);
			args &= 0xf0;
			args |= value;
			write_reg(AP3216C_ALS_CONFIGURATION_REG,args);
			
			break;
		}
		case AP3216C_ALS_LOW_THRESHOLD_L:
		{
			write_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG,value);
			
			break;
		}
		case AP3216C_ALS_LOW_THRESHOLD_H:
		{
			write_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG,value);
			
			break;
		}
		case AP3216C_ALS_HIGH_THRESHOLD_L:
		{
			write_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG,value);
			
			break;
		}
		case AP3216C_ALS_HIGH_THRESHOLD_H:
		{
			write_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG,value);
			
			break;
		}
		case AP3216C_PS_GAIN:
		{
			uint8_t args = 0;
			
			read_reg(AP3216C_PS_CONFIGURATION_REG,1,&args);
			args &= 0xf3;
			args |= value;
			write_reg(AP3216C_PS_CONFIGURATION_REG,args);
			
			break;
		}
		
		case AP3216C_PS_PERSIST:
		{
			uint8_t args = 0;
			
			read_reg(AP3216C_PS_CONFIGURATION_REG,1,&args);
			args &= 0xfc;
			args |= value;
			write_reg(AP3216C_PS_CONFIGURATION_REG,args);
			
			break;
		}
		case AP3216C_PS_LOW_THRESHOLD_L:
		{
			write_reg(AP3216C_PS_THRESHOLD_LOW_L_REG,value);
			
			break;
		}
		case AP3216C_PS_LOW_THRESHOLD_H:
		{
			write_reg(AP3216C_PS_THRESHOLD_LOW_H_REG,value);
			
			break;
		}
		case AP3216C_PS_HIGH_THRESHOLD_L:
		{
			write_reg(AP3216C_PS_THRESHOLD_HIGH_L_REG,value);
			
			break;
		}
		case AP3216C_PS_HIGH_THRESHOLD_H:
		{
			write_reg(AP3216C_PS_THRESHOLD_HIGH_H_REG,value);
			
			break;
		}
		
		default:
		{
		}
	}
}

/* ��ȡAP3216C���������� */
void ap3216c_get_param(ap3216c_cmd_t cmd,uint8_t *value)
{
	switch(cmd)
	{
		case AP3216C_SYSTEM_MODE:
		{
			read_reg(AP3216C_SYS_CONFIGURATION_REG,1,value);
			
			break;
		}
		case AP3216C_INT_PARAM:
		{
			read_reg(AP3216C_SYS_CONFIGURATION_REG,1,value);
			
			break;
		}
		case AP3216C_ALS_RANGE:
		{
			uint8_t temp;
			
			read_reg(AP3216C_ALS_CONFIGURATION_REG,1,value);
			temp = (*value & 0xff)>>4;
			*value = temp;
			
			break;
		}
		case AP3216C_ALS_PERSIST:
		{
			uint8_t temp;
			
			read_reg(AP3216C_ALS_CONFIGURATION_REG,1,value);
			temp = *value & 0x0f;
			*value = temp;
			
			break;
		}
		case AP3216C_ALS_LOW_THRESHOLD_L:
		{
			read_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG,1,value);
			
			break;
		}
		case AP3216C_ALS_LOW_THRESHOLD_H:
		{
			read_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG,1,value);
			
			break;
		}
		case AP3216C_ALS_HIGH_THRESHOLD_L:
		{
			read_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG,1,value);
			
			break;
		}
		case AP3216C_ALS_HIGH_THRESHOLD_H:
		{
			read_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG,1,value);
			
			break;
		}
		case AP3216C_PS_GAIN:
		{
			uint8_t temp;
			
			read_reg(AP3216C_PS_CONFIGURATION_REG,1,&temp);
			*value = (temp & 0xc)>>2;
			
			break;
		}
		
		case AP3216C_PS_PERSIST:
		{
			uint8_t temp;
			
			read_reg(AP3216C_PS_CONFIGURATION_REG,1,&temp);
			*value = temp & 0x3;
			
			break;
		}
		case AP3216C_PS_LOW_THRESHOLD_L:
		{
			read_reg(AP3216C_PS_THRESHOLD_LOW_L_REG,1,value);
			
			break;
		}
		case AP3216C_PS_LOW_THRESHOLD_H:
		{
			read_reg(AP3216C_PS_THRESHOLD_LOW_H_REG,1,value);
			
			break;
		}
		case AP3216C_PS_HIGH_THRESHOLD_L:
		{
			read_reg(AP3216C_PS_THRESHOLD_HIGH_L_REG,1,value);
			
			break;
		}
		case AP3216C_PS_HIGH_THRESHOLD_H:
		{
			read_reg(AP3216C_PS_THRESHOLD_HIGH_H_REG,1,value);
			
			break;
		}
		
		default:
		{
		}
	}
}



/* ������ֵ�������ù��˴��� */
static void set_als_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold)
{
	uint8_t Resolution;
	double DB;
	
	/* ��ȡ����ǿ�ȵķ�Χ */
	ap3216c_get_param(AP3216C_ALS_RANGE,&Resolution);
	
	if(Resolution == AP3216C_ALS_RANGE_20661)  //����ǿ�ȷ�Χ0~20661
	{
		DB= 0.35;   //����ǿ�ȵķֱ���
	}
	else if(Resolution == AP3216C_ALS_RANGE_5162) //����ǿ�ȷ�Χ 0~5162
	{
		DB = 0.0788; //����ǿ�ȵķֱ���
	}
	else if(Resolution == AP3216C_ALS_RANGE_1291) //����ǿ�ȷ�Χ 0~1291
	{
		DB = 0.0197; //����ǿ�ȵķֱ���
	}
	else if(Resolution == AP3216C_ALS_RANGE_323) //����ǿ�ȷ�Χ 0~323
	{
		DB = 0.0049; //����ǿ�ȵķֱ���
	}
	threshold.min /= DB;   //���ݲ��õķֱ���������
	threshold.max /= DB;
	
	ap3216c_set_param(cmd,(threshold.min &0xff));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+1),(threshold.min>>8));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+2),(threshold.max & 0xff));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+3),threshold.max>>8);
}

static void set_ps_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold)
{
	if(threshold.min > 1020)   //����1020ʱ��Ҫ���õ��ֽڵĵ���λ
		ap3216c_set_param(cmd,(threshold.min - 1020&0x03));
	
	ap3216c_set_param((ap3216c_cmd_t)(cmd+1),threshold.min /4);  //���ø��ֽڲ���
	
	if(threshold.max > 1020)   //����1020ʱ��Ҫ���õ��ֽڵĵ���λ
		ap3216c_set_param((ap3216c_cmd_t)(cmd+2),(threshold.max - 1020&0x03));
	
	ap3216c_set_param((ap3216c_cmd_t)(cmd+3),threshold.max /4);  //���ø��ֽڲ���
}

uint8_t ap3216c_get_IntStatus(void)
{
	uint8_t IntStatus;
	
	/* ���ж�״̬�Ĵ��� */
	read_reg(AP3216C_SYS_INT_STATUS_REG,1,&IntStatus);
	//IntStatus ��0λ��ʾALS�жϣ���1λ��ʾPS�ж�
	
	return IntStatus;
}

static void ap3216c_int_init(void)
{
	ap3216c_threshold_t threshold;   //���ñ���ֵ��ֵ�ṹ��
	
	threshold.min = 100;    //�������޲������� 
	threshold.max = 1000;    //�������޲������� 
	threshold.noises_time = 1;  //ALS�ﵽ��ֵ����� threshold.noises_time *4�����ں�ʼ������threshold.noises_time���λ15
	set_als_threshold(AP3216C_ALS_LOW_THRESHOLD_L,threshold);  //����ALS����ֵ
	
	threshold.min = 120;    //PS����120��������ֵ
	threshold.max = 600;    //PS����600��������ֵ
	set_ps_threshold(AP3216C_PS_LOW_THRESHOLD_L,threshold);    //����PS����ֵ
	
}

/* ��ʼ��AP3216C�豸 */
void ap3216c_init(void)
{
	i2c_GPIO_Config();
	
	reset_sensor();
	Delay_MS(0xFFFFF);
	ap3216c_set_param(AP3216C_SYSTEM_MODE,AP3216C_MODE_ALS_AND_PS);
	Delay_MS(0xFFFFF);
	
	/* �����жϺ��ж����� */
	AP_INT_Config();
	ap3216c_int_init();
}

/* ��ȡ��ͨ��ap3216c���������� */
float ap3216c_read_ambient_light(void)
{
	float brightness = 0.0;
	uint32_t read_data;
	uint8_t temp;
	
	read_data = read_low_and_high(AP3216C_ALS_DATA_L_REG,1);
	
	ap3216c_get_param(AP3216C_ALS_RANGE,&temp);
	
	if(temp == AP3216C_ALS_RANGE_20661)  //����ǿ�ȷ�Χ0~20661
	{
		brightness= 0.35 * read_data;   //����ǿ�ȵķֱ���
	}
	else if(temp == AP3216C_ALS_RANGE_5162) //����ǿ�ȷ�Χ 0~5162
	{
		brightness = 0.0788 * read_data; //����ǿ�ȵķֱ���
	}
	else if(temp == AP3216C_ALS_RANGE_1291) //����ǿ�ȷ�Χ 0~1291
	{
		brightness = 0.0197 * read_data; //����ǿ�ȵķֱ���
	}
	else if(temp == AP3216C_ALS_RANGE_323) //����ǿ�ȷ�Χ 0~323
	{
		brightness = 0.0049 * read_data; //����ǿ�ȵķֱ���
	}
	return brightness;
	
}

/* ͨ��AP3216C������������ȡ�ӽ��� */
uint16_t ap3216c_read_ps_data(void)
{
	uint16_t proximity = 0;
	
	uint32_t read_data;
	read_data = read_low_and_high(AP3216C_PS_DATA_L_REG,1);
	
	if( 1 == ((read_data >>6)&0x01 ||(read_data>>14)&0x01 ))
	{
		return proximity = 55555;  // ������ߣ�IR����PS��Ч������һ��55555��ֵ
	}
	
	proximity = (read_data & 0x000f) + (((read_data>>8)&0x3f)<<4);
	proximity |= read_data&0x8000;   //ȡ���λ 0��ʾ����ԭ�� 1��ʾ����ӽ�
		
	return proximity;
}

/* ͨ��AP3216C���������ж������� */
uint16_t ap3216c_read_ir_data(void)
{
	uint16_t proximity = 0;
	
	uint32_t read_data;
	read_data = read_low_and_high(AP3216C_IR_DATA_L_REG,1);
	
	proximity = (read_data & 0x0003) + ((read_data>>8)&0xff);
		
	return proximity;
}



static void Delay_MS(__IO uint32_t nCount)
{
	for(;nCount!=0;nCount--);
}




#endif /* PKG_USING_AP3216C */

