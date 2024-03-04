#include "bsp_dht11.h"
#include "core_delay.h"

/****************** �궨����ʱ���� ****************/
#define DHT11_DELAY_US(us)          CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)          CPU_TS_Tmr_Delay_MS(ms)


static void                           DHT11_GPIO_Config                       ( void );
static void                           DHT11_Mode_IPU                          ( void );
static void                           DHT11_Mode_Out_PP                       ( void );
static uint8_t                        DHT11_ReadByte                          ( void );



/* DHT11 GPIO���ų�ʼ�� */
static void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	
	DHT11_Dout_SCK_ABPxClock_FUN(DHT11_Dout_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11_Mode_IPU  ʹDHT11-DATA���ű�Ϊ����ģʽ */
static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11_Mode_Out_PP  ʹDHT11-DATA���ű�Ϊ�������ģʽ */
static void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef     GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_Dout_GPIO_PORT,&GPIO_InitStructure);
}

/* DHT11��ʼ�� */
void DHT11_Init(void)
{
	DHT11_GPIO_Config();
	
	DHT11_Dout_1;               //����GPIO��������
}

/* ��DHT11��ȡһ���ֽڣ�MSB���� */
static uint8_t DHT11_ReadByte(void)
{
	uint8_t i,temp = 0;
	
	for(i=0;i<8;i++)
	{
		/* ÿbit��50us�͵�ƽ��־��ʼ����ѯֱ���ӻ�����50us�͵�ƽ���� */
		while(DHT11_Dout_IN() == Bit_RESET);
		
		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		DHT11_DELAY_US(40);  //��ʱ40us ��������0�ߵ�ƽ������ʱ��
		
		if(DHT11_Dout_IN() == Bit_SET)  //40us��ʱ������Ϊ�ߵ�ƽ��Ϊ1
		{
			while(DHT11_Dout_IN() == Bit_SET);
			
			temp |= (uint8_t)(0x01<<(7-i));   //�ѵڣ�7-i��λ��1��MSB���� 
		}
		else
		{
			temp &= (uint8_t)~(0x01<<(7-i));  //�ѵڣ�7-i��λ��0��MSB���� 
		}
		
	}
	return temp;
}

/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_Typedef * DHT11_Data)
{
	/* ���ģʽ */
	DHT11_Mode_Out_PP();
	
	/* �������� */
	DHT11_Dout_0;
	
	/* ��ʱ18ms */
	DHT11_DELAY_MS(18);
	
	/* �������� ��ʱ30us */
	DHT11_Dout_1;
	DHT11_DELAY_US(30);
	
	/* ����ģʽ */
	DHT11_Mode_IPU();
	
	/* �жϴӻ��Ƿ��е͵�ƽ��Ӧ�źţ��粻��Ӧ��������Ӧ���������� */
	if(DHT11_Dout_IN() == Bit_RESET)
	{
		/* ��ѯ ֱ���ӻ�������80us�͵�ƽ��Ӧ���� */
		while(DHT11_Dout_IN() == Bit_RESET);
		
		/* ��ѯ ֱ���ӻ�������80us�ߵ�ƽ��Ӧ���� */
		while(DHT11_Dout_IN() == Bit_SET);
		
		/* �������� */
		DHT11_Data->humi_int = DHT11_ReadByte();
		DHT11_Data->humi_deci = DHT11_ReadByte();
		DHT11_Data->temp_int = DHT11_ReadByte();
		DHT11_Data->temp_deci = DHT11_ReadByte();
		DHT11_Data->check_sum = DHT11_ReadByte();
		
		/* תΪ���״̬ */
		DHT11_Mode_Out_PP();
		
		/* �������� */
		DHT11_Dout_1;
		
		/* �����ȡ�������Ƿ���ȷ */
		if(DHT11_Data->check_sum == DHT11_Data->temp_int + DHT11_Data->temp_deci + DHT11_Data->humi_int + DHT11_Data->humi_deci )
			return SUCCESS;
		else
			return ERROR;
	}
	else
		return ERROR;
	
	
}
