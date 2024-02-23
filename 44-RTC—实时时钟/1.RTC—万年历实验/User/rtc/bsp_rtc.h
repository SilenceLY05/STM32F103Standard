#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include "stm32f10x.h"
#include "bsp_calendar.h"
#include "bsp_date.h"

//是否使用LCD屏幕显示日期
#define USE_LCD_DISPLAY

//使用LSE外部时钟或者LSI内部时钟
//#define RTC_CLOCK_SOURCE_LSE
#define RTC_CLOCK_SOURCE_LSI

#define RTC_BKP_DRX      BKP_DR1
//写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA     0xA5A5

//北京时间的时区秒数差
#define TIME_ZOOM          (8*60*60)

								

void RTC_NVIC_Config(void);
void RTC_Configuration(void);
void Time_Regulate_Get(struct rtc_time *tm);
void Time_Adjust(struct rtc_time *tm);
void Time_Display(uint32_t TimeVar,struct rtc_time *tm);
void Time_Show(struct rtc_time *tm);
static uint8_t USART_Scanf(uint32_t value);
void RTC_CheckAndConfig(struct rtc_time *tm);

#endif /* __BSP_RTC_H */

