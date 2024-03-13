/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_systick.h"
#include "bsp_usart_blt.h"


#define TASK_DELAY_NUM       2        //总任务个数
#define TASK_DELAY_0         500      //任务0延时 500*10ms后执行 检测蓝牙是否链接
#define TASK_DELAY_1         50       //任务1延时 50*10ms后执行

uint32_t TASK_Delay_Group[TASK_DELAY_NUM];   //任务组数，用来计时 判断是否执行对应任务

/* 每5秒检查蓝牙是否链接 */
int hc05_inquery_connect;     //每5S检测蓝牙是否连接
int hc05_check_receivebuff;  //每500ms检查蓝牙是否连接


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	int i;
	
	for(i = 0 ; i< TASK_DELAY_NUM;i++)
	{
		TASK_Delay_Group[i]++;        //任务计时，时间到后执行
	}
	
	/* 处理任务0 */
	if(TASK_Delay_Group[0]>= TASK_DELAY_0)  //判断是否执行任务0
	{ 
		TASK_Delay_Group[0] = 0;        //置0 重新计时
		
		/* 任务0 检测蓝牙是否链接 */
		hc05_inquery_connect = 1;  //如果蓝牙没有连接 标志位置1
	}
	
	/* 处理任务1 */
	if(TASK_Delay_Group[1] >= TASK_DELAY_1)
	{
		TASK_Delay_Group[1] = 0;
		
		hc05_check_receivebuff = 1;
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

extern volatile uint16_t usart_p;
extern uint8_t usart_buff[USART_BUFF_SIZE];


void BLT_USART_IRQHandler(void)
{
	if(usart_p < USART_BUFF_SIZE)
	{
		if(USART_GetITStatus(BLT_USARTx,USART_IT_RXNE) == RESET)
		{
			usart_buff[usart_p] = USART_ReceiveData(BLT_USARTx);
			usart_p++;
		}
	}
	else
	{
		USART_ClearITPendingBit(BLT_USARTx,USART_IT_RXNE);
		clean_rebuff();
	}

}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
