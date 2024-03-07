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

/* 配置中断输入引脚 */
void AP_INT_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	/* 开启引脚时钟 */
	AP_INT_SCK_APBxClock_FUN(AP_INT_GPIO_CLK,ENABLE);
	
	/* 配置引脚 选择输入模式 */
	GPIO_InitStructure.GPIO_Pin = AP_INT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(AP_INT_GPIO_PORT,&GPIO_InitStructure);
}

/* 写寄存器值 */
static void write_reg(uint8_t reg,uint8_t data)
{
	/*　第0步：发停止信号，启动内部写操作　*/
	i2c_Stop();
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);  //此处是写指令
	
	/* 第3步：发送一个时钟，判断器件是否正确应答 */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail;  /* EEPROM器件写超时 */
	}
	
	/* 第4步：开始写入寄存器地址 */
	i2c_SendByte(reg);
	
	/* 第5步：检查ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* 器件无响应 */
	}
	/* 第5步：开始写入数据 */
	i2c_SendByte(data);
	
	/* 第5步：检查ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* 器件无响应 */
	}
	
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop();
	return;
	
cmd_fail:/* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return ;
}

/* 读寄存器的值 */
static void read_reg(uint8_t reg,uint8_t len,uint8_t *buf)
{
	/*　第0步：发停止信号，启动内部写操作　*/
	i2c_Stop();
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);  //此处是写指令
	
	/* 第3步：发送一个时钟，判断器件是否正确应答 */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail;  /* EEPROM器件写超时 */
	}
	
	/* 第4步：开始写入寄存器地址 */
	i2c_SendByte(reg);
	
	/* 第5步：检查ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* 器件无响应 */
	}
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	
	/* 第6步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_RD);  //此处是读指令
	
	/* 第8步：检查ACK */
	if(i2c_WaitAck() != 0)
	{
		goto cmd_fail; /* 器件无响应 */
	}
	while(len)
	{
		*buf = i2c_ReadByte();
		buf++;
		
		if(len == 1)
		{
			i2c_NAck();/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
		else
		{
			i2c_Ack();/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
		}
		len--;
	}
	i2c_Stop();
	return ;
	
cmd_fail:/* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return ;
}

/* 软件复位传感器 */
static void reset_sensor(void)
{
	write_reg(AP3216C_SYS_CONFIGURATION_REG,AP3216C_MODE_SW_RESET);  //RESET
}

/* 读取低位寄存器和高位寄存器 */
uint32_t read_low_and_high(uint8_t reg,uint8_t len)
{
	uint32_t data;
	uint8_t buf = 0;
	
	read_reg(reg,len,&buf);  //读低字节
	data = buf;
	read_reg(reg+1,len,&buf);  //读高字节
	data = data+(buf<<len*8);  //合并数据
	
	return data;
}

/* 设置AP3216C传感器参数 */
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

/* 获取AP3216C传感器参数 */
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



/* 设置阈值，不设置过滤次数 */
static void set_als_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold)
{
	uint8_t Resolution;
	double DB;
	
	/* 读取光照强度的范围 */
	ap3216c_get_param(AP3216C_ALS_RANGE,&Resolution);
	
	if(Resolution == AP3216C_ALS_RANGE_20661)  //光照强度范围0~20661
	{
		DB= 0.35;   //光照强度的分辨率
	}
	else if(Resolution == AP3216C_ALS_RANGE_5162) //光照强度范围 0~5162
	{
		DB = 0.0788; //光照强度的分辨率
	}
	else if(Resolution == AP3216C_ALS_RANGE_1291) //光照强度范围 0~1291
	{
		DB = 0.0197; //光照强度的分辨率
	}
	else if(Resolution == AP3216C_ALS_RANGE_323) //光照强度范围 0~323
	{
		DB = 0.0049; //光照强度的分辨率
	}
	threshold.min /= DB;   //根据不用的分辨率来设置
	threshold.max /= DB;
	
	ap3216c_set_param(cmd,(threshold.min &0xff));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+1),(threshold.min>>8));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+2),(threshold.max & 0xff));
	ap3216c_set_param((ap3216c_cmd_t)(cmd+3),threshold.max>>8);
}

static void set_ps_threshold(ap3216c_cmd_t cmd,ap3216c_threshold_t threshold)
{
	if(threshold.min > 1020)   //大于1020时需要设置低字节的低两位
		ap3216c_set_param(cmd,(threshold.min - 1020&0x03));
	
	ap3216c_set_param((ap3216c_cmd_t)(cmd+1),threshold.min /4);  //设置高字节参数
	
	if(threshold.max > 1020)   //大于1020时需要设置低字节的低两位
		ap3216c_set_param((ap3216c_cmd_t)(cmd+2),(threshold.max - 1020&0x03));
	
	ap3216c_set_param((ap3216c_cmd_t)(cmd+3),threshold.max /4);  //设置高字节参数
}

uint8_t ap3216c_get_IntStatus(void)
{
	uint8_t IntStatus;
	
	/* 读中断状态寄存器 */
	read_reg(AP3216C_SYS_INT_STATUS_REG,1,&IntStatus);
	//IntStatus 第0位表示ALS中断，第1位表示PS中断
	
	return IntStatus;
}

static void ap3216c_int_init(void)
{
	ap3216c_threshold_t threshold;   //设置报警值阈值结构体
	
	threshold.min = 100;    //光照下限产生报警 
	threshold.max = 1000;    //光照上限产生报警 
	threshold.noises_time = 1;  //ALS达到阈值后持续 threshold.noises_time *4个周期后开始报警，threshold.noises_time最大位15
	set_als_threshold(AP3216C_ALS_LOW_THRESHOLD_L,threshold);  //设置ALS报警值
	
	threshold.min = 120;    //PS低于120产生报警值
	threshold.max = 600;    //PS高于600产生报警值
	set_ps_threshold(AP3216C_PS_LOW_THRESHOLD_L,threshold);    //设置PS报警值
	
}

/* 初始化AP3216C设备 */
void ap3216c_init(void)
{
	i2c_GPIO_Config();
	
	reset_sensor();
	Delay_MS(0xFFFFF);
	ap3216c_set_param(AP3216C_SYSTEM_MODE,AP3216C_MODE_ALS_AND_PS);
	Delay_MS(0xFFFFF);
	
	/* 配置中断和中断数据 */
	AP_INT_Config();
	ap3216c_int_init();
}

/* 读取光通过ap3216c传感器测量 */
float ap3216c_read_ambient_light(void)
{
	float brightness = 0.0;
	uint32_t read_data;
	uint8_t temp;
	
	read_data = read_low_and_high(AP3216C_ALS_DATA_L_REG,1);
	
	ap3216c_get_param(AP3216C_ALS_RANGE,&temp);
	
	if(temp == AP3216C_ALS_RANGE_20661)  //光照强度范围0~20661
	{
		brightness= 0.35 * read_data;   //光照强度的分辨率
	}
	else if(temp == AP3216C_ALS_RANGE_5162) //光照强度范围 0~5162
	{
		brightness = 0.0788 * read_data; //光照强度的分辨率
	}
	else if(temp == AP3216C_ALS_RANGE_1291) //光照强度范围 0~1291
	{
		brightness = 0.0197 * read_data; //光照强度的分辨率
	}
	else if(temp == AP3216C_ALS_RANGE_323) //光照强度范围 0~323
	{
		brightness = 0.0049 * read_data; //光照强度的分辨率
	}
	return brightness;
	
}

/* 通过AP3216C传感器测量读取接近度 */
uint16_t ap3216c_read_ps_data(void)
{
	uint16_t proximity = 0;
	
	uint32_t read_data;
	read_data = read_low_and_high(AP3216C_PS_DATA_L_REG,1);
	
	if( 1 == ((read_data >>6)&0x01 ||(read_data>>14)&0x01 ))
	{
		return proximity = 55555;  // 红外过高（IR），PS无效，返回一个55555数值
	}
	
	proximity = (read_data & 0x000f) + (((read_data>>8)&0x3f)<<4);
	proximity |= read_data&0x8000;   //取最高位 0表示物体原理 1表示物体接近
		
	return proximity;
}

/* 通过AP3216C传感器进行度数测量 */
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

