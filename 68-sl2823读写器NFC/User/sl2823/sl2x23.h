#ifndef __SL2X23_H__
#define __SL2X23_H__


#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "sl2x23_if.h"



//#define NOT_IRQ                    //不开启中断，采用查询状态寄存器
//#define SPD_MODE                   //软掉电模式
//#define NFC_DEBUG                  //打印调试信息



#define BIT0                  (1<<0)
#define BIT1                  (1<<1)
#define BIT2                  (1<<2)
#define BIT3                  (1<<3)
#define BIT4                  (1<<4)
#define BIT5                  (1<<5)
#define BIT6                  (1<<6)
#define BIT7                  (1<<7)



/////////////////////////////////////////////////////////////////////
//寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0   命令和状态
#define     RFU00                 0x00     //保留将来用
#define     CommandReg            0x01     //启动和停止命令的执行
#define     ComIEnReg             0x02     //中断请求传递的使能和禁能控制位
#define     DivIEnReg             0x03     //中断请求传递的使能和禁能控制位
#define     ComIrqReg             0x04     //包含中断请求标志
#define     DivIrqReg             0x05     //包含中断请求标志
#define     ErrorReg              0x06     //错误标志，指示执行的上个命令的错误状态
#define     Status1Reg            0x07     //包含通信的状态标志
#define     Status2Reg            0x08     //包含接收器和发送器的状态标志
#define     FIFODataReg           0x09     //64字节FIFO缓冲区的输入和输出
#define     FIFOLevelReg          0x0A     //指示FIFO中存储的字节数
#define     WaterLevelReg         0x0B     //定义FIFO下溢和上溢报警的FIFO深度
#define     ControlReg            0x0C     //不同的控制寄存器
#define     BitFramingReg         0x0D     //面向位的帧的调节
#define     CollReg               0x0E     //RF接口上检测到的第一个位冲突的位的位置
#define     RFU0F                 0x0F     //保留将来用
// PAGE 1        命令
#define     RFU10                 0x10     //保留将来用
#define     ModeReg               0x11     //定义发送和接收的常用模式
#define     TxModeReg             0x12     //定义发送过程的数据传输速率
#define     RxModeReg             0x13     //定义接收过程中的数据传输速率
#define     TxControlReg          0x14     //控制天线驱动器管脚TX1和TX2的逻辑特性
#define     TxASKReg             0x15     //控制天线驱动器的设置
#define     TxSelReg              0x16     //控制天线驱动器的内部源
#define     RxSelReg              0x17     //控制内部的接收器设置
#define     RxThresholdReg        0x18     //选择位译码器
#define     DemodReg              0x19     //定义解调器的设置
#define     RFU1A                 0x1A     //保留将来用
#define     RFU1B                 0x1B     //保留将来用
#define     MfTxReg               0x1C     //
#define     MfRxReg               0x1D     //
#define     TypeBReg              0x1E     //
#define     SerialSpeedReg        0x1F     //选择串行USART接口的速率
// PAGE 2        配置
#define     RFU20                 0x20     //保留将来用
#define     CRCResultRegM         0x21     //显示CRC计算的实际MSB
#define     CRCResultRegL         0x22     //显示CRC计算的实际LSB
#define     RFU23                 0x23     //保留将来用
#define     ModWidthReg           0x24     //控制ModWidth的设置
#define     RFU25                 0x25     //保留将来用
#define     RFCfgReg              0x26     //配置接收器增益
#define     GsNReg                0x27     //选择天线驱动器管脚TX1和TX2的调制电导
#define     CWGsPReg              0x28     //选择天线驱动器管脚TX1和TX2的调制电导
#define     ModGsPReg             0x29     //选择天线驱动器管脚TX1和TX2的调制电导
#define     TModeReg              0x2A     //定义内部定时器的设置
#define     TPrescalerReg         0x2B     //定义内部定时器的设置
#define     TReloadRegH           0x2C     //描述16位长的定时器高8位重装值
#define     TReloadRegL           0x2D     //描述16位长的定时器低8位重装值
#define     TCounterValueRegH     0x2E     //显示16位长的实际定时器高8位值
#define     TCounterValueRegL     0x2F     //显示16位长的实际定时器低8位值
// PAGE 3      
#define     RFU30                 0x30     //保留将来用
#define     TestSel1Reg           0x31     //常用测试信号的配置
#define     TestSel2Reg           0x32     //常用测试信号的配置和PRBS控制
#define     TestPinEnReg          0x33     //D1-D7输出驱动器的使能管脚
#define     TestPinValueReg       0x34     //定义D1-D7用作I/O总线时的值
#define     TestBusReg            0x35     //显示内部测试总线的状态
#define     AutoTestReg           0x36     //控制数字自测试
#define     VersionReg            0x37     //显示版本
#define     AnalogTestReg         0x38     //控制管脚AUX1和AUX2
#define     TestDAC1Reg           0x39     //定义TestDAC1的测试值
#define     TestDAC2Reg           0x3A     //定义TestDAC2的测试值
#define     TestADCReg            0x3B     //显示ADC1和Q通道的实际值
#define     RFU3C                 0x3C     //保留将来用
#define     RFU3D                 0x3D     //保留将来用
#define     RFU3E                 0x3E     //保留将来用
#define     SpecialReg		        0x3F     //




#define   EXT_TRMOD               0x50
#define   EXT_FELICA_SYNC0        0x51
#define   EXT_FELICA_SYNC1        0x52
#define   EXT_FELICA_PWIDTH       0x53
#define   EXT_FELICA_CTRL         0x54
#define   EXT_FELICA_STA          0x55
#define   EXT_FORCE_STP           0x58
#define   EXT_PWTH_RF15693_ADR    0x59
#define   EXT_COLL_RF15693_ADR    0x5A


/* 定义寄存器位 */
#define TxIEn                 BIT6
#define RxIEn                 BIT5
#define IdleIEn               BIT4
#define ErrIEn                BIT1
#define TimerIEn              BIT0
#define TxIRq                 BIT6
#define RxIRq                 BIT5
#define IdleIRq               BIT4
#define ErrIRq                BIT1
#define TimerIRq              BIT0


#define CollErr               BIT3
#define CrcErr                BIT2
#define ParityErr             BIT1
#define ProtocolErr           BIT0


#define CollPos               (BIT0|BIT1|BIT2|BIT3|BIT4)
#define RxAlign               (BIT4|BIT5|BIT6)
#define TxLastBits            (BIT0|BIT1|BIT2)


/////////////////////////////////////////////////////////////////////
//PCD命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算
#define PCD_CMD_MASK          0x0F               //命令字掩码
#define PCD_CMD_SLEEP         0x10               //芯片进入休眠状态



/* 错误码，每一个函数返回一个状态值，该状态值对应于错误码 */
#define MI_OK                           0
#define MI_CHK_OK                       0
#define MI_CRC_ZERO                     0

#define MI_CRC_NOTZERO                  1

#define MI_NOTAGERR                     (-1)
#define MI_CHK_FAILED                   (-1)
#define MI_CRCERR                       (-2)
#define MI_CHK_COMPERR                  (-2)
#define MI_EMPTY                        (-3)
#define MI_AUTHERR                      (-4)
#define MI_PARITYERR                    (-5)
#define MI_CODEERR                      (-6)
#define MI_SERNRERR                     (-8)
#define MI_KEYERR                       (-9)
#define MI_NOTAUTHERR                   (-10)
#define MI_BITCOUNTERR                  (-11)
#define MI_BYTECOUNTERR                 (-12)
#define MI_IDLE                         (-13)
#define MI_TRANSERR                     (-14)
#define MI_WRITEERR                     (-15)
#define MI_INCRERR                      (-16)
#define MI_DECRERR                      (-17)
#define MI_READERR                      (-18)
#define MI_OVFLERR                      (-19)
#define MI_POLLING                      (-20)
#define MI_FRAMINGERR                   (-21)
#define MI_ACCESSERR                    (-22)
#define MI_UNKNOWN_COMMAND              (-23)
#define MI_COLLERR                      (-24)
#define MI_RESETERR                     (-25)
#define MI_INITERR                      (-25)
#define MI_INTERFACEERR                 (-26)
#define MI_ACCESSTIMEOUT                (-27)
#define MI_NOBITWISEANTICOLL            (-28)
#define MI_QUIT                         (-30)
#define MI_INTEGRITY_ERR                (-35) //完整性错误(crc/parity/protocol)


#define MI_ERR_PARAM                    (-36)  //参数错误
#define MI_ERR_TRANSMIT                 (-37)

#define MI_RECBUF_OVERFLOW              (-50)
#define MI_SENDBYTENR                   (-51)
#define MI_SENDBUF_OVERFLOW             (-53)
#define MI_BAUDRATE_NOT_SUPPORTED       (-54)
#define MI_SAME_BAUDRATE_REQUIRED       (-55)
#define MI_WRONG_PARAMETER_VALUE        (-60)
#define MI_BREAK                        (-99)
#define MI_NY_IMPLEMENTED               (-100)
#define MI_NO_MFRC                      (-101)
#define MI_MFRC_NOTAUTH                 (-102)
#define MI_WRONG_DES_MODE               (-103)
#define MI_HOST_AUTH_FAILED             (-104)
#define MI_WRONG_LOAD_MODE              (-106)
#define MI_WRONG_DESKEY                 (-107)
#define MI_MKLOAD_FAILED                (-108)
#define MI_FIFOERR                      (-109)
#define MI_WRONG_ADDR                   (-110)
#define MI_DESKEYLOAD_FAILED            (-111)
#define MI_WRONG_SEL_CNT                (-114)
#define MI_WRONG_TEST_MODE              (-117)
#define MI_TEST_FAILED                  (-118)
#define MI_TOC_ERROR                    (-119)
#define MI_COMM_ABORT                   (-120)
#define MI_INVALID_BASE                 (-121)
#define MI_MFRC_RESET                   (-122)
#define MI_WRONG_VALUE                  (-123)
#define MI_VALERR                       (-124)
#define MI_COM_ERR                      (-125)
#define PROTOCOL_ERR                    (-126)
///用户使用错误
#define USER_ERROR                      (-127)




#define MAX_TRX_BUF_SIZE               255
#define INT_USE_CHECK_REG               1

typedef struct transceive_buffer
{
	int8_t mf_command;
	uint16_t mf_length;
	uint8_t mf_data[MAX_TRX_BUF_SIZE];
}transceive_buffer;



extern volatile uint8_t irq_flag_io;

#define FSDI 8 //在EMV测试中，接近耦合装置的帧尺寸。身份证必须FSDI = 8

#if 0
//中断使用GPIO
#if (MOTHERBOARD_V20)
sbit INT_PIN = P3^2;    //V2.0版的中断管脚是3.2
sbit LED1 = P0^7;//LED4
sbit LED2 = P0^6;//LED3
#elif (SIKAI_V10)
sbit INT_PIN = P3^2;    //外部中断0管脚
sbit LED1 = P0^3;//LED1
sbit LED2 = P0^4;//LED2
#else
sbit INT_PIN = P3^5;    //外部中断0管脚
sbit LED1 = P0^3;
sbit LED2 = P0^4;//LED3
#endif



#if(INT_USE_CHECK_REG)
//中断使用查询寄存器
#undef INT_PIN
#define INT_PIN (read_reg(0x07) & 0x10)
#endif

//extern transceive_buffer  mf_com_data;
extern uint8_t  g_led1_on_time;
extern uint8_t  g_led2_on_time;



#define led_success_on() do {LED1 = 0;  g_led1_on_time = 20; } while(0)
#define led_success_off() LED1 = 1
#define led_fail_on() do {LED2 = 0;  g_led2_on_time = 20; } while(0)
#define led_fail_off() LED2 = 1


sbit P12 = P1^2;

sbit NRSTPD = P0^2; // powerdown 管脚
#else
//中断使用GPIO
#if (MOTHERBOARD_V20)
#define INT_PIN = IomGetVal(5); //V2.0版的中断管脚是3.2
#endif

#if(INT_USE_CHECK_REG)
//中断使用查询寄存器
#undef INT_PIN
#define INT_PIN (read_reg(0x07) & 0x10)
//#define INT_PIN (while(!(read_reg(0x04)&0x7d) ||! (read_reg(0x04)&0x2 && (!read_reg(0x06)&0x80)) );)

#endif



#define P12 //= P1^2;

#define NRSTPD //= P0^2;    // powerdown 管脚


#endif

#define pcd_poweron()  (NRSTPD_HIGH) //NRSTPD = 1    //PCD 上电
#define pcd_powerdown() (NRSTPD_LOW)//NRSTPD = 0  //PCD 掉电

#define FALSE   0
#define TRUE    1


void test_setagc_getadc(void);
void pcd_soft_poweroff_init(void);

void pcd_init(void);
void pcd_init_lpcd(void);
void pcd_reset(void);
int8_t pcd_config(uint8_t type);

void write_reg(uint8_t addr, uint8_t val);
uint8_t read_reg(uint8_t addr);
int8_t pcd_com_transceive(struct transceive_buffer *pi);
int8_t pcd_com_transceive2(struct transceive_buffer *pi, uint8_t rst_timer);

void set_bit_mask(uint8_t reg, uint8_t mask);
void clear_bit_mask(uint8_t reg,uint8_t mask);
void pcd_set_tmo(uint8_t fwi);
void pcd_set_rate(uint8_t rate);
void pcd_antenna_on(void);
void pcd_antenna_off(void);
void page45_lock(void);
void page4_unlock(void);
void page5_unlock(void);
void pcd_lpcd_start(void);
void pcd_lpcd_end(void);
uint8_t pcd_lpcd_check(void);
void pcd_delay_sfgi(uint8_t sfgi);
void pcd_lpcd_start(void);
void pcd_lpcd_config_start(uint8_t delta, u32 t_inactivity_ms, uint8_t skip_times,uint8_t  t_detect_us);
void pcd_lpcd_end(void);
uint8_t pcd_lpcd_check(void);

void pcd_sleep(void);
void pcd_sleep_exit(void);


/* Debug */
void print_lpcd_reg(void);
void print_all_reg(void);


#endif /* __SL2X23_H__ */

