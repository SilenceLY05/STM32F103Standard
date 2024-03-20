#ifndef __BSP_AD7192_H
#define __BSP_AD7192_H


#include "stm32f10x.h"

extern uint32_t AD7192Registers[8]; //AD7192Registers[8]的数组元素为无符号长整型，每个元素占4个字节(即32位)
extern uint32_t AD7192Data;         //AD7192Data为无符号长整型，占4个字节

/* 定义SPI接口 */
#define AD7192_SPIx                         SPI1
#define AD7192_SPI_APBxClockCmd             RCC_APB2PeriphClockCmd
#define AD7192_SPI_CLK                      RCC_APB2Periph_SPI1

/* CS片选引脚 */
#define AD7192_SPI_GPIO_APBxClockCmd        RCC_APB2PeriphClockCmd
#define AD7192_SPI_CS_CLK                   RCC_APB2Periph_GPIOA
#define AD7192_SPI_CS_PORT                  GPIOA
#define AD7192_SPI_CS_PIN                   GPIO_Pin_4

/* SCK引脚 */
#define AD7192_SPI_SCK_CLK                  RCC_APB2Periph_GPIOA
#define AD7192_SPI_SCK_PORT                 GPIOA
#define AD7192_SPI_SCK_PIN                  GPIO_Pin_5

/* MISO脚 */
#define AD7192_SPI_MISO_CLK                 RCC_APB2Periph_GPIOA
#define AD7192_SPI_MISO_PORT                GPIOA
#define AD7192_SPI_MISO_PIN                 GPIO_Pin_6

/* MOSI脚 */
#define AD7192_SPI_MOSI_CLK                 RCC_APB2Periph_GPIOA
#define AD7192_SPI_MOSI_PORT                GPIOA
#define AD7192_SPI_MOSI_PIN                 GPIO_Pin_7

#define AD7192_SCLK_H                       GPIO_SetBits(AD7192_SPI_SCK_PORT,AD7192_SPI_SCK_PIN)
#define AD7192_SCLK_L                       GPIO_ResetBits(AD7192_SPI_SCK_PORT,AD7192_SPI_SCK_PIN)

#define AD7192_CS_ENABLE                    GPIO_SetBits(AD7192_SPI_CS_PORT,AD7192_SPI_CS_PIN)
#define AD7192_CS_DISABLE                   GPIO_ResetBits(AD7192_SPI_CS_PORT,AD7192_SPI_CS_PIN)

#define AD7192_RDY_STATUS                   GPIO_ReadInputDataBit(AD7192_SPI_MISO_PORT,AD7192_SPI_MISO_PIN)



/********************************** 通信寄存器 **************************************/

#define WRITE_EN             0x00           //写使能   CR7写入使能位 为0可以写入为1不会写入直至此位写入0
#define WRITE_DIS            0x80           //写进制
#define RW_W                 0x00           //写操作   CR6位为0下一个操作是写操作，为1下一个操作对指定寄存器执行读操作
#define RW_R                 0x40           //读操作

//对AD7192内部寄存器地址进行宏定义   位于通信寄存器CR5-CR3
#define REG_COM_STA          0x00           //选定状态寄存器进行操作；写操作期间为通信寄存器寄存器 读操作期间为状态寄存器大小8位
#define REG_MODE             0x01           //选定模式寄存器进行操作；寄存器大小24位
#define REG_CONF             0x02           //选定配置寄存器进行操作；寄存器大小24位
#define REG_DATA             0x03           //选定数据寄存器/数据寄存器加状态信息进行操作；寄存器大小为24/32位 
#define REG_ID               0x04           //选定ID寄存器进行操作；寄存器大小8位
#define REG_GPOCON           0x05           //选定通用数字输出寄存器进行操作；寄存器大小8位
#define REG_OFFSET           0x06           //选定失调寄存器进行操作；寄存器大小24位
#define REG_FS               0x07           //选定满量程寄存器进行操作；寄存器大小24位

//CR2位此位为1时串行接口可以连续读取数据寄存器 为0时禁用连续读取模式
#define CREAD_EN             0x04           //连续读取使能
#define CREAD_DIS            0x00           //连续读取禁止


/********************************** 状态寄存器 **************************************/
#define RDY_H                0x80           //ADC就绪位，数据写入后清0
#define RDY_L                0x00           //ADC就绪位SR7位清0
#define ERR_H                0x40           //ADC错误位，与RDY位同时写入 为1表示数据寄存器结果出错
#define ERR_L                0x00           //ADC错误位SR6清0
#define NOREF_H              0x20           //无外部基准电压位，置1说明选定的基准电压引脚低于规定的阈值
#define NOREF_L              0x00           //无外部基准电压位清0，说明对基准电压引脚试驾了有效基准电压
#define PARITY_H             0x01           //数据寄存器的奇偶校验位,若数据寄存器中有奇数个1则该位置1
#define PARITY_L             0x00           //若数据寄存器中有偶数个1则该位置0，使用此位时，模式寄存器中的DAT_STA置1
                                            
//SR2至SR0位 指示哪一通道对应数据寄存器的内容
#define CHDST_AIN1_AIN2      0x00
#define CHDST_AIN3_AIN4      0x01
#define CHDST_TEMP           0x02
#define CHDST_AIN2_AIN2      0x03
#define CHDST_AIN1_AINCOM    0x04
#define CHDST_AIN2_AINCOM    0x05
#define CHDST_AIN3_AINCOM    0x06
#define CHDST_AIN4_AINCOM    0x07



/********************************** 模式寄存器 **************************************/
//只要对模式寄存器执行写操作，会将状态寄存器的RDY位置1
//MR23至MR21位 模式选择位
#define MODE_CONT            0x000000        //连续转换模式(默认)，转换结束 DOUT引脚和状态寄存器RDY位变低
#define MODE_SING            0x200000        //单次转换模式，转换结果置于数据寄存器RDY变低，返回关断模式
#define MODE_IDLE            0x400000        //空闲模式
#define MODE_PD              0x600000        //关断模式
#define MODE_INZCL           0x800000        //内部零电平校准，校准启动时RDY为高完成变低，校准完成后处于空闲模式
#define MODE_INFCL           0xA00000        //内部满量程校准，校准启动时RDY为高完成变低，校准完成后处于空闲模式，数据保存在满量程寄存器,若AVDD小于4.75 CLK_DIV置1
#define MODE_SYSZCL          0xC00000        //系统零电平校准，校准启动时RDY为高完成变低，校准完成后处于空闲模式，数据保存在满量程寄存器
#define MODE_SYSFCL          0xE00000        //系统满电平校准，校准启动时RDY为高完成变低，校准完成后处于空闲模式，数据保存在满量程寄存器
#define DAT_STA_EN           0x100000        //使能状态传输
#define DAT_STA_DIS          0x000000        //禁用状态传输
#define EXT_XTAL             0x000000        //外部晶振
#define EXT_CLK              0x040000        //外部时钟
#define INCLK_MCLK2TRI       0x080000        //内部时钟，MCLK2引脚为三态
#define INCLK_MCLK2EN        0x0C0000        //内部时钟从MCLK2获得
#define SINC3                0x008000        //使用SINC3滤波器
#define SINC4                0x000000        //使用SINC4滤波器(默认值)
#define ENPAR_EN             0x002000        //使能奇偶校验位，使用时DAT_STA应置1
#define ENPAR_DIS            0x000000        //禁止奇偶校验位
#define CLK_DIV_2            0x001000        //置1时，主时钟2分频，若AVDD低于4.75此位必须置1
#define CLK_DIV_DIS          0x000000        //主时钟不分频，正常转换时，此位应置0
#define SINGLE_EN            0x000800        //单周期转化使能位，置1时，AD7192在一个转换周期内建立
#define SINGLE_DIS           0x000000        //禁用单周期转换，若使能多个模拟输入通道，或单词转换模式
#define	REJ60_EN				     0x000400        //使能60Hz陷波频率,实现50Hz/60Hz同时抑制
#define	REJ60_DIS				     0x000000        //失能60Hz陷波频率,只抑制50Hz





/********************************** 配置寄存器 **************************************/
#define CHOP_EN              0x800000        //斩波使能位
#define CHOP_DIS             0x000000        //斩波禁用
#define REFSEL_IN1           0x000000        //基准电压选择位，REFIN1之间施加外部基准电压
#define REFSEL_IN2           0x100000        //基准电压选择位，REFIN2之间施加外部基准电压
#define AIN1_AIN2    			   0x000100        //使能通道(AIN+)AIN1 (AIN-)AIN2
#define AIN3_AIN4   			   0x000200        //使能通道(AIN+)AIN3 (AIN-)AIN4
#define TEMP       			     0x000400        //使能通道  温度传感器
#define AIN2_AIN2    			   0x000800        //使能通道(AIN+)AIN2 (AIN-)AIN2
#define AIN1_AINCOM 		 	   0x001000        //使能通道(AIN+)AIN1 (AIN-)AINCOM 
#define AIN2_AINCOM  			   0x002000        //使能通道(AIN+)AIN2 (AIN-)AINCOM  
#define AIN3_AINCOM  			   0x004000        //使能通道(AIN+)AIN3 (AIN-)AINCOM 
#define AIN4_AINCOM  				 0x008000        //使能通道(AIN+)AIN4 (AIN-)AINCOM
#define BURN_EN              0x000080        //使能激励电流,仅缓冲器处于有效状态且斩波禁用
#define BURN_DIS             0x000000        //禁用激励电流
#define REFDET_EN            0x000040        //使能基准电压检测功能
#define REFDET_DIS           0x000000        //禁用基准电压检测功能
#define BUF_EN               0x000010        //使能模拟输入端的缓冲器
#define BUF_DIS              0x000000        //模拟输入不进行缓冲
#define UB_UNI               0x000008        //极性选择位，单极性工作模式
#define UB_BI                0x000000        //极性选择位，双极性工作模式
#define	GAIN_1				    	 0x000000        //增益选择位，ADC基准电压输入范围±5V
#define	GAIN_8               0x000003        //增益选择位，ADC基准电压输入范围±625mV
#define	GAIN_16              0x000004        //增益选择位，ADC基准电压输入范围±312.5mV
#define	GAIN_32              0x000005        //增益选择位，ADC基准电压输入范围±156.2mV
#define	GAIN_64              0x000006        //增益选择位，ADC基准电压输入范围±78.125mV
#define	GAIN_128             0x000007        //增益选择位，ADC基准电压输入范围±39.06mV



/********************************** 配置寄存器 **************************************/
#define BPDSW_EN             0x40            //电桥关断控制位，置1闭合电桥关断开关BPDSW
#define BPDSW_DIS            0x00            //电桥关断控制位，置0闭合电桥关断开关BPDSW
#define GP32EN_EN            0x20            //数字输出P3和数字输出P2使能，置1时，数字输出P3和P2为有效状态
#define GP32EN_DIS           0x00            //数字输出P3和数字输出P2使能，置0时，P3和P2引脚为三态
#define GP10EN_EN            0x10            //数字输出P1和数字输出P0使能，置1时，数字输出P1和P0为有效状态
#define GP10EN_DIS           0x00            //数字输出P1和数字输出P0使能，置0时，P1和P0引脚为三态
#define P3DAT_EN             0x08            //数字输出P3位，GP32EN置1时，P3DAT设置P3引脚值
#define P3DAT_DIS            0x00            //数字输出P3位，GP32EN置1时，P3DAT设置P3引脚值
#define P2DAT_EN             0x04            //数字输出P2位，GP32EN置1时，P2DAT设置P2引脚值
#define P2DAT_DIS            0x00            //数字输出P2位，GP32EN置1时，P2DAT设置P2引脚值
#define P1DAT_EN             0x02            //数字输出P1位，GP10EN置1时，P1DAT设置P1引脚值
#define P1DAT_DIS            0x00            //数字输出P1位，GP10EN置1时，P1DAT设置P1引脚值
#define P0DAT_EN             0x01            //数字输出P0位，GP10EN置1时，P0DAT设置P0引脚值
#define P0DAT_DIS            0x00            //数字输出P0位，GP10EN置1时，P0DAT设置P0引脚值

#define NOP                  0x00            //空操作



void AD7192_Config(void);
void SPI_TransmitReceive(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte,uint32_t SPITimeOut);
void AD7192ReadData(unsigned char *WriteBuff,unsigned char *ReadBuff,unsigned char NumberOfByte);
unsigned char ReadFromAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToRead,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	);
unsigned char WriteToAD7192ViaSPI(const char RegisterStartAddress,const unsigned char NumberOfRegistersToWrite,
uint32_t *DataBuff,const unsigned char OffsetInBuffer	);
void AD7192_Mode_Conf_Registers(uint32_t mode,uint32_t cfg);
void AD7192_SoftWareReset(void);
void AD7192InternalZeroScaleCalibration(void);
void AD7192InternalFullScaleCalibration(void);
void AD7192ExternalZeroScaleCalibration(void);
void AD7192ExternalFullScaleCalibration(void);
void AD7192StartContinuousRead(void);
uint32_t AD7192ReadConvertData(void);
uint32_t AD7192ContinuousRead(void);
void AD7192ExitContinousRead(void);
uint32_t AD7192ReadTemperature(void);
float RealTemperature(uint32_t TemperatureData);
void AD7192StartSingleConvertion(uint32_t Channel);
void AD7192StartContinuousConvertion(uint32_t Channels);



#endif /* __BSP_AD7192_H */

