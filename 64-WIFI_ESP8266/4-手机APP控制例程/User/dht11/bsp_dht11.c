#include "bsp_dht11.h"
#include "core_delay.h"

/****************** 宏定义延时函数 ****************/
#define DHT11_DELAY_US(us)          CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)          CPU_TS_Tmr_Delay_MS(ms)


static void                           DHT11_GPIO_Config                       ( void );
static void                           DHT11_Mode_IPU                          ( void );
static void                           DHT11_Mode_Out_PP                       ( void );
static uint8_t                        DHT11_ReadByte                          ( void );



/* DHT11 GPIO引脚初始化 */
static void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	
	DHT11_Dout_SCK_ABPxClock_FUN(DHT11_Dout_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11_Mode_IPU  使DHT11-DATA引脚变为上拉模式 */
static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11_Mode_Out_PP  使DHT11-DATA引脚变为推挽输出模式 */
static void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11初始化 */
void DHT11_Init(void)
{
	DHT11_GPIO_Config();
	
	DHT11_Dout_1;               //拉高GPIO数据引脚
}

/* 从DHT11读取一个字节，MSB先行 */
static uint8_t DHT11_ReadByte(void)
{
	uint8_t i,temp = 0;
	
	for(i=0;i<8;i++)
	{
		/* 每bit以50us低电平标志开始，轮询直到从机发出50us低电平结束 */
		while(DHT11_Dout_IN() == Bit_RESET);
		
		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		DHT11_DELAY_US(40);  //延时40us 大于数据0高电平持续的时间
		
		if(DHT11_Dout_IN() == Bit_SET)  //40us延时后若仍为高电平则为1
		{
			while(DHT11_Dout_IN() == Bit_SET);
			
			temp |= (uint8_t)(0x01<<(7-i));   //把第（7-i）位置1，MSB先行 
		}
		else
		{
			temp &= (uint8_t)~(0x01<<(7-i));  //把第（7-i）位置0，MSB先行 
		}
		
	}
	return temp;
}

/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_Typedef * DHT11_Data)
{
	/* 输出模式 */
	DHT11_Mode_Out_PP();
	
	/* 主机拉低 */
	DHT11_Dout_0;
	
	/* 延时18ms */
	DHT11_DELAY_MS(18);
	
	/* 总线拉高 延时30us */
	DHT11_Dout_1;
	DHT11_DELAY_US(30);
	
	/* 输入模式 */
	DHT11_Mode_IPU();
	
	/* 判断从机是否有低电平响应信号，如不响应跳出，响应则向下运行 */
	if(DHT11_Dout_IN() == Bit_RESET)
	{
		/* 轮询 直到从机发出的80us低电平响应结束 */
		while(DHT11_Dout_IN() == Bit_RESET);
		
		/* 轮询 直到从机发出的80us高电平响应结束 */
		while(DHT11_Dout_IN() == Bit_SET);
		
		/* 接收数据 */
		DHT11_Data->humi_int = DHT11_ReadByte();
		DHT11_Data->humi_deci = DHT11_ReadByte();
		DHT11_Data->temp_int = DHT11_ReadByte();
		DHT11_Data->temp_deci = DHT11_ReadByte();
		DHT11_Data->check_sum = DHT11_ReadByte();
		
		/* 转为输出状态 */
		DHT11_Mode_Out_PP();
		
		/* 主机拉高 */
		DHT11_Dout_1;
		
		/* 检验读取的数据是否正确 */
		if(DHT11_Data->check_sum == DHT11_Data->temp_int + DHT11_Data->temp_deci + DHT11_Data->humi_int + DHT11_Data->humi_deci )
			return SUCCESS;
		else
			return ERROR;
	}
	else
		return ERROR;
	
	
}
