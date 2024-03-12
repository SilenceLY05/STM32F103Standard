#include "rc522_config.h"

static void RC522_SPI_Config(void);

static void RC522_SPI_Config(void)
{
	GPIO_InitTypeDef          GPIO_InitStructure;
	
	RC522_GPIO_CS_CLK_FUN(RC522_GPIO_CS_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RC522_GPIO_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC522_GPIO_CS_PORT,&GPIO_InitStructure);
	
	RC522_GPIO_SCK_CLK_FUN(RC522_GPIO_SCK_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RC522_GPIO_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC522_GPIO_SCK_PORT,&GPIO_InitStructure);
	
	RC522_GPIO_MOSI_CLK_FUN(RC522_GPIO_MOSI_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RC522_GPIO_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC522_GPIO_MOSI_PORT,&GPIO_InitStructure);
	
	RC522_GPIO_MISO_CLK_FUN(RC522_GPIO_MISO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RC522_GPIO_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC522_GPIO_MISO_PORT,&GPIO_InitStructure);
	
	RC522_GPIO_RST_CLK_FUN(RC522_GPIO_RST_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RC522_GPIO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RC522_GPIO_RST_PORT,&GPIO_InitStructure);
	
	
	
}

void RC522_Init(void)
{
	RC522_SPI_Config();
	RC522_RST_Disable();
	RC522_CS_Disable();
}

