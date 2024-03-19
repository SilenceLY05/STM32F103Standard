#include "sl2x23_if.h"
#include "sl2x23.h"
#include <stdint.h>


/* RST IRQ引脚配置 */
void SL2x23_RST_IRQ_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SL2x23_GPIO_RST_CLK | SL2x23_GPIO_IRQ_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_RST_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SL2x23_GPIO_RST_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SL2x23_GPIO_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = SL2x23_GPIO_IRQ_Mode;
	GPIO_Init(SL2x23_GPIO_IRQ_PORT,&GPIO_InitStructure);
}



/* IRQ引脚外部中断配置 */
void SL2x23_IRQ_EXTI_Config(void)
{
	EXTI_InitTypeDef       EXTI_InitStructure;
	NVIC_InitTypeDef       NVIC_InitStructure;
	
	/* 开启复用时钟后在配置EXTI */
	SL2x23_IRQ_EXTI_CLK_FUN(SL2x23_IRQ_EXTI_CLK,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = SL2x23_IRQ_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
	
	/* 选择EXTI的信号源 */
	GPIO_EXTILineConfig(SL2x23_IRQ_EXTI_PORTSOURCE,SL2x23_IRQ_EXTI_PINSOURCE);
	
	/* 清零标志位 */
	EXTI_ClearITPendingBit(SL2x23_IRQ_EXTI_LINE);
	
	EXTI_InitStructure.EXTI_Line = SL2x23_IRQ_EXTI_LINE;
	
	/* EXTI为中断模式 */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_InitStructure);
}


/* IRQ中断服务函数 */
void SL2x23_IRQPin_IRQHandler(void)
{
	/* 数据中断 */
	if(EXTI_GetITStatus(SL2x23_IRQ_EXTI_LINE) == SET)
	{
		irq_flag_io = 1; /* 标志位置1 然后回到主循环处理-读卡 */
		EXTI_ClearITPendingBit(SL2x23_IRQ_EXTI_LINE);
	}
}


void SL2x23_Init(void)
{
	SL2x23_RST_IRQ_Config();
	SL2x23_IRQ_EXTI_Config();
	
#if defined SL2X23_IIC_MODE
	SL2x23_I2C_Init();
#elif defined SL2X23_SPI_Mode
    SL2x23_SPI_Config();
#elif defined SL2X23_UART_Mode
    #error "串口通信暂未实现！"
#else
    #error "请先定义通信方式！"
#endif

    SL2x23_Reset_Disable(); /* NRSTPD 复位引脚高电平 */
    Delay_us(1000); /* SL2823 上电/NRSTPD 管脚拉高后，软件需要等待 800us 才能对寄存器进行读写 */
}


/* 通过SPI方式将一个字节写入PCD的寄存器中 addr要写入的地址 RegValue要写入的数值 */
void write_reg(uint8_t addr,uint8_t RegValue)
{
#if defined SL2X23_IIC_MODE
	i2c_write_word(SL2x23_DEV_ADDRESS << 1,addr,RegValue);
#else
	addr = ((addr&0x3F)<<1) & 0x7F;
	DEV_WriteSR(addr,RegValue);
	
#endif
}


uint8_t read_reg(uint8_t addr)
{
#if defined SL2X23_IIC_MODE
	uint8_t reg_value;
	i2c_read_word(SL2x23_DEV_ADDRESS<<1,addr,&reg_value);
	return reg_value;
#else
	addr = ((addr & 0x3F)<<1) | 0x80;
	return DEV_ReadSR(addr);
#endif
}

void set_bit_mask(uint8_t reg,uint8_t mask)
{
	int8_t tmp;
	tmp = read_reg(reg);
	write_reg(reg,tmp | mask);
}


void write_buf(uint8_t addr,uint8_t *data,uint32_t lenth)
{
	addr = ((addr & 0x3F)<<1)&0x7F;
	DEV_SPI_Write(addr,data,lenth);
}


/* 从寄存器中连续的读取数据 */
void read_buf(uint8_t addr,uint8_t *data,uint32_t lenth)
{
	addr = ((addr & 0x3F)<<1) | 0x80;
	DEV_SPI_Read(addr,data,lenth);
}


/**
 * @brief  set zero the bit of a register
 * @param  reg: register
 * @param  mask : the bit tobe clear
 * @retval None
 */
void clear_bit_mask(uint8_t reg,uint8_t mask)
{
	int8_t tmp;
	tmp = read_reg(reg);
	write_reg(reg,tmp &~mask);
}



/* 读SL2x23内部寄存器 */
uint8_t sl2x23_read_reg(uint8_t addr)
{
	//寄存器地址大于 0x3f 且小于 0x80，需要先将地址 0x3f 配置为 1
  if (addr > 0x3f && addr < 0x80)
	{
		write_reg(0x3F,0x1);
		
	}
	//寄存器地址大于 0x7f，需要先将地址 0x3f 配置为 2
	if(addr>0x7F)
	{
		write_reg(0x3F,0x2);
	}
	addr = read_reg(addr);
	write_reg(0x3F,0x0);
	return addr;
}


/* 通讯测试 */
void Communication_Test(void)
{
	int ok = 0;
	uint8_t err = 0;
	int i =0;
	
	uint8_t ret = 0xFF;
	pcd_reset();
	delay_1ms(1);
	
#ifdef SL2X23_IIC_MODE
	//最多连续检查两次，有一次检测到设备即可
	if((i2c_CheckDevice(SL2x23_DEV_ADDRESS <<1) == 0 )|| (i2c_CheckDevice(SL2x23_DEV_ADDRESS<<1) == 0))
	{
		printf("SL2823 I2C设备存储\r\n");
	}
	else
	{
		printf("未检测到 SL2823\r\n");
	}
#endif
	
	for( i = 0 ;i< 100;i++)
	{
		write_reg(0x09,i);
		ret = read_reg(0x09);
		if(i != ret)
		{
			err ++;
			printf("i = 0x%x,errnum = 0x%x,data = 0x%x\r\n",i,err,ret);
		}
		else
		{
			ok++;
		}
	}
	if(ok == 100)
	{
#if defined SL2X23_IIC_MODE
		printf("iic test pass\r\n");
#elif defined SL2X23_SPI_MODE
		printf("spi test pass \r\n");
#endif
	}
	else
	{
#if defined SL2X23_IIC_MODE
		printf("iic test fail\r\n");
#elif defined SL2X23_SPI_MODE
		printf("spi test fail \r\n");
#endif		
		while(1);
	}
}


