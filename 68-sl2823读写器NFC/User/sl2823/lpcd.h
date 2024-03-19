#ifndef __LPCD_H__
#define __LPCD_H__



#include "sl2x23.h"
//------------------ SPI OPERATION DEFINE ------------------

#define SL_WR_REG(addr, data)    write_reg(addr,data)

#define SL_RD_REG(addr)          read_reg(addr)


//---------------- SPI OPERATION DEFINE END -----------------

//------------------ DELAY DEFINE --------------------------

#define DELAY_1MS       delay_1ms(1)
#define delay_ms        delay_1ms
//---------------- DELAY DEFINE END ------------------------

//------------------ PARAMETERS DEFINE ---------------------
#define THD_ADJ_CNT 5
#define ADJ_TO_TIME 300       // amp_test runs every 16ms, 300 times means about 5 seconds
//---------------- PARAMETERS DEFINE END --------------------


#define INDEX_NUM   46

#define LPCD_IDX_MAX  (INDEX_NUM-2)
#define LPCD_IDX_MIN  (0)

#define LPCD_CALIB_OK         (0)
#define LPCD_CALIB_TIMEOUT    (-1)
#define LPCD_CALIB_FS_LARGE   (-2)
#define LPCD_CALIB_FS_SMALL   (-3)
#define LPCD_CALIB_UNEXPECTED (-4)


#define AGC_CTRL   0x5f
#define ADC_CTRL   0x81
#define ADC_TIME0  0x82
#define ADC_TIME1  0x83
#define ADC_VALUE0 0x84
#define ADC_VALUE1 0x85

#define LPCD_CTRL0  0x86
#define LPCD_CTRL1  0x87
#define LPCD_CTRL2  0x88
#define LPCD_CTRL3  0x89
#define LPCD_TIME0  0x8a
#define LPCD_TIME1  0x8b
#define LPCD_TIME2  0x8d
#define LPCD_TIME3  0x9d
#define LPCD_PARAM0 0x8e
#define LPCD_PARAM1 0x8f
#define LPCD_PARAM2 0x90
#define LPCD_PARAM3 0x91
#define LPCD_PARAM4 0x92
#define LPCD_PARAM5 0x93
#define LPCD_PARAM6 0x94
#define LPCD_PARAM7 0x95
#define LPCD_STATE  0x96
#define LPCD_STATUS 0x97
#define LPCD_HYS    0x98
#define OSC_SETTLE  0x99





/* 开启LPCD和AGC功能 */
//#define _ENABLE_LPCD_

/* 开启LPCD调试 */
#define LPCD_DEBUG

#define LPCD_DETECT_PERIOD (0x08) //0x5 320ms; //0x8 500ms ; //0x10 1s
#define LPCD_DETECT_SENS (0x1) //0x2 17mv; //0x4 35mv
#define AGC_RX_MAX (0x80) //-0x80~3.3V; //0x60~3.0V


typedef struct
{
    unsigned char t1;
    unsigned char phase;
    char phase_offset;
    unsigned char default_amp;
    unsigned char amp;
    unsigned char max_amp;
    unsigned char min_amp;
    unsigned char sense;
    unsigned char idx[8];
} lpcd_cfg_t;

extern lpcd_cfg_t lpcd_cfg;

extern double voltage[];

extern unsigned char lut[];

unsigned char lpcd_amp_test(unsigned char amp);

unsigned char  osc_calib(void);
unsigned char  agc_calib(void);
void adc_init(void);
void test_agc(void);
unsigned char  get_adc_value(void);
unsigned char get_adc_value_mean(unsigned char n);
unsigned char phase_calib(void);

void lpcd_init(unsigned char fst_en,unsigned char adc_value);

void lpcd_entry(void);

uint8_t lpcd_exit(void);

int lpcd_sen_adj(void);

unsigned char lpcd_amp_search_floor(unsigned char lpcd_amp_target, unsigned char amp_low, unsigned char amp_high);
unsigned char lpcd_amp_search_ceil(unsigned char lpcd_amp_target, unsigned char amp_low, unsigned char amp_high);

/* 打印寄存器测试 */
void lpcd_print(void);

void lpcd_dummy_tr(void);

#define SL_SET_BIT_MASK(reg, mask)  \
    SL_WR_REG(reg, SL_RD_REG(reg) | mask)

#define SL_CLR_BIT_MASK(reg, mask)  \
    SL_WR_REG(reg, SL_RD_REG(reg) & ~mask)

#if 0
#define LPCD_AMP_TEST_INFO(str, amp_info, rlt_info)  \
    printf(str);                                                  \
    printf("\n");                                                 \
    for (i = 0; i < 8; i++)                                       \
    {                                                             \
        printf("idx[%d]: %0.2d\t", i, lpcd_cfg.idx[i]);           \
    }                                                             \
    printf("\namp: %0.2x, phase: %0.2x, lpcd_amp_rlt: %x\n", amp_info, lpcd_cfg.phase, rlt_info);

#define LPCD_ADJ_INFO                                             \
    printf("lpcd_info:\n");                                       \
    if (adj_fail == 1)                                            \
        printf("lpcd_sen_adj: fail\n");                           \
    else                                                          \
        printf("lpcd_sen_adj: succeed\n");                        \
    printf("lpcd_cfg.t1: %x\n", lpcd_cfg.t1);                     \
    printf("lpcd_cfg.phase: %x\n", lpcd_cfg.phase);               \
    printf("lpcd_cfg.amp: %x\n", lpcd_cfg.amp);                   \
    printf("lpcd_cfg.sense: %x\n", lpcd_cfg.sense);               \
    for (i = 0; i < 8; i++)                                       \
    {                                                             \
        printf("lpcd_cfg.idx[%d]: %0.2d\n", i, lpcd_cfg.idx[i]);  \
    }
#else
#define LPCD_AMP_TEST_INFO(str, amp_info, rlt_info)
#define LPCD_ADJ_INFO
#endif

#endif    /* __LPCD_H__ */
// endfile
		
