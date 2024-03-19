#include "sl2x23_if.h"
#include "sl2x23.h"
#include <stdint.h>


/* RST IRQ�������� */
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



/* IRQ�����ⲿ�ж����� */
void SL2x23_IRQ_EXTI_Config(void)
{
	EXTI_InitTypeDef       EXTI_InitStructure;
	NVIC_InitTypeDef       NVIC_InitStructure;
	
	/* ��������ʱ�Ӻ�������EXTI */
	SL2x23_IRQ_EXTI_CLK_FUN(SL2x23_IRQ_EXTI_CLK,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = SL2x23_IRQ_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
	
	/* ѡ��EXTI���ź�Դ */
	GPIO_EXTILineConfig(SL2x23_IRQ_EXTI_PORTSOURCE,SL2x23_IRQ_EXTI_PINSOURCE);
	
	/* �����־λ */
	EXTI_ClearITPendingBit(SL2x23_IRQ_EXTI_LINE);
	
	EXTI_InitStructure.EXTI_Line = SL2x23_IRQ_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_InitStructure);
}


/* IRQ�жϷ����� */
void SL2x23_IRQPin_IRQHandler(void)
{
	/* �����ж� */
	if(EXTI_GetITStatus(SL2x23_IRQ_EXTI_LINE) == SET)
	{
		irq_flag_io = 1; /* ��־λ��1 Ȼ��ص���ѭ������-���� */
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
    #error "����ͨ����δʵ�֣�"
#else
    #error "���ȶ���ͨ�ŷ�ʽ��"
#endif

    SL2x23_Reset_Disable(); /* NRSTPD ��λ���Ÿߵ�ƽ */
    Delay_us(1000); /* SL2823 �ϵ�/NRSTPD �ܽ����ߺ������Ҫ�ȴ� 800us ���ܶԼĴ������ж�д */
}


/* ͨ��SPI��ʽ��һ���ֽ�д��PCD�ļĴ����� addrҪд��ĵ�ַ RegValueҪд�����ֵ */
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


/* �ӼĴ����������Ķ�ȡ���� */
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



/* ��SL2x23�ڲ��Ĵ��� */
uint8_t sl2x23_read_reg(uint8_t addr)
{
	//�Ĵ�����ַ���� 0x3f ��С�� 0x80����Ҫ�Ƚ���ַ 0x3f ����Ϊ 1
  if (addr > 0x3f && addr < 0x80)
	{
		write_reg(0x3F,0x1);
		
	}
	//�Ĵ�����ַ���� 0x7f����Ҫ�Ƚ���ַ 0x3f ����Ϊ 2
	if(addr>0x7F)
	{
		write_reg(0x3F,0x2);
	}
	addr = read_reg(addr);
	write_reg(0x3F,0x0);
	return addr;
}


/* ͨѶ���� */
void Communication_Test(void)
{
	int ok = 0;
	uint8_t err = 0;
	int i =0;
	
	uint8_t ret = 0xFF;
	pcd_reset();
	delay_1ms(1);
	
#ifdef SL2X23_IIC_MODE
	//�������������Σ���һ�μ�⵽�豸����
	if((i2c_CheckDevice(SL2x23_DEV_ADDRESS <<1) == 0 )|| (i2c_CheckDevice(SL2x23_DEV_ADDRESS<<1) == 0))
	{
		printf("SL2823 I2C�豸�洢\r\n");
	}
	else
	{
		printf("δ��⵽ SL2823\r\n");
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


