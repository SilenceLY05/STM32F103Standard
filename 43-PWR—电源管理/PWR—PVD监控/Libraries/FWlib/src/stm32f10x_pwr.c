/**
  ******************************************************************************
  * @file    stm32f10x_pwr.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   This file provides all the PWR firmware functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"

/** @addtogroup STM32F10x_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR 
  * @brief PWR driver modules
  * @{
  */ 

/** @defgroup PWR_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup PWR_Private_Defines
  * @{
  */

/* --------- PWR registers bit address in the alias region ---------- */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of DBP bit */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* Alias word address of PVDE bit */
#define PVDE_BitNumber           0x04
#define CR_PVDE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of EWUP bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)


/**
  * @}
  */

/** @defgroup PWR_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup PWR_Private_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup PWR_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup PWR_Private_Functions
  * @{
  */

/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void PWR_DeInit(void)
{
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
}

/**
  * @brief  Enables or disables access to the RTC and backup registers.
  * @param  NewState: new state of the access to the RTC and backup registers.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Power Voltage Detector(PVD).
  * @param  NewState: new state of the PVD.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *   This parameter can be one of the following values:
  *     @arg PWR_PVDLevel_2V2: PVD detection level set to 2.2V
  *     @arg PWR_PVDLevel_2V3: PVD detection level set to 2.3V
  *     @arg PWR_PVDLevel_2V4: PVD detection level set to 2.4V
  *     @arg PWR_PVDLevel_2V5: PVD detection level set to 2.5V
  *     @arg PWR_PVDLevel_2V6: PVD detection level set to 2.6V
  *     @arg PWR_PVDLevel_2V7: PVD detection level set to 2.7V
  *     @arg PWR_PVDLevel_2V8: PVD detection level set to 2.8V
  *     @arg PWR_PVDLevel_2V9: PVD detection level set to 2.9V
  * @retval None
  */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(PWR_PVDLevel));
  tmpreg = PWR->CR;
  /* Clear PLS[7:5] bits */
  tmpreg &= CR_PLS_MASK;
  /* Set PLS[7:5] bits according to PWR_PVDLevel value */
  tmpreg |= PWR_PVDLevel;
  /* Store the new value */
  PWR->CR = tmpreg;
}

/**
  * @brief  Enables or disables the WakeUp Pin functionality.
  * @param  NewState: new state of the WakeUp Pin functionality.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)NewState;
}

/**
* @brief 进入停止模式
*
* @note   在停止模式下所有I/O的会保持在停止前的状态
* @note   从停止模式唤醒后，会使用HSI作为时钟源
* @note   调压器若工作在低功耗模式，可减少功耗，但唤醒时会增加延迟
* @param  PWR_Regulator: 设置停止模式时调压器的工作模式
*            @arg PWR_MainRegulator_ON: 调压器正常运行
*            @arg PWR_Regulator_LowPower: 调压器低功耗运行
* @param  PWR_STOPEntry: 设置使用WFI还是WFE进入停止模式
*            @arg PWR_STOPEntry_WFI: WFI进入停止模式
*            @arg PWR_STOPEntry_WFE: WFE进入停止模式
* @retval None
*/
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  /* 检查参数 */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));
  
   /* 设置调压器的模式 ------------*/
  tmpreg = PWR->CR;
  /* 清除 PDDS 及 LPDS 位 */
  tmpreg &= CR_DS_MASK;
   /* 根据PWR_Regulator 的值(调压器工作模式)配置LPDS,MRLVDS及LPLVDS位*/
  tmpreg |= PWR_Regulator;
  /* 写入参数值到寄存器 */
  PWR->CR = tmpreg;
  /* 设置内核寄存器的SLEEPDEEP位 */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;
  
  /* 设置进入停止模式的方式-----------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    /* 需要中断唤醒 */
    __WFI();
  }
  else
  {
    /* 需要事件唤醒 */
    __WFE();
  }
  
  /* 以下的程序是当重新唤醒时才执行的，清除SLEEPDEEP位的状态 */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);  
}

/**
* @brief 进入待机模式
* @note   待机模式时，除以下引脚，其余引脚都在高阻态:
*          -复位引脚
*          - RTC_AF1 引脚 (PC13) (需要使能侵入检测、时间戳事件或RTC闹钟事件)
*          - RTC_AF2 引脚 (PI8) (需要使能侵入检测或时间戳事件)
*          - WKUP 引脚 (PA0) (需要使能WKUP唤醒功能)
* @note  在调用本函数前还需要清除WUF寄存器位
* @param  None
* @retval None
*/
void PWR_EnterSTANDBYMode(void)
{
  /* 清除 Wake-up 标志 */
  PWR->CR |= PWR_CR_CWUF;
   /* 选择待机模式 */
  PWR->CR |= PWR_CR_PDDS;
  /* 设置内核寄存器的SLEEPDEEP位  */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;
/* 存储操作完毕时才能进入待机模式，使用以下语句确保存储操作执行完毕 */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* 等待中断唤醒 */
  __WFI();
}

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  *     @arg PWR_FLAG_PVDO: PVD Output
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));
  
  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the PWR's pending flags.
  * @param  PWR_FLAG: specifies the flag to clear.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  * @retval None
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  /* Check the parameters */
  assert_param(IS_PWR_CLEAR_FLAG(PWR_FLAG));
         
  PWR->CR |=  PWR_FLAG << 2;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
