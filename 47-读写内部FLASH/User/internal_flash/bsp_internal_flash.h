#ifndef __BSP_INTERNAL_FLASH_H
#define __BSP_INTERNAL_FLASH_H

#include "stm32f10x.h"

/* STM32��������Ʒÿҳ��С2KByte����С��Ʒÿҳ1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
	#define FLASH_PAGE_SIZE      ((uint16_t)0x800)  // 2048
#else
	#define FLASH_PAGE_SIZE      ((uint16_t)0x400)  // 1024
#endif


//д�����ʼ��ַ�ͽ�����ַ
#define WRITE_START_ADDR          ((uint32_t)0x08008000)
#define WRITE_END_ADDR            ((uint32_t)0x0800C000)

typedef enum{
	FAILED = 0,
	PASSED = !FAILED
}TestStatus;

int internalFlash_Test(void);


#endif  /* __BSP_INTERNAL_FLASH_H */

