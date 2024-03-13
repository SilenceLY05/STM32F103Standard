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


#define TASK_DELAY_NUM       2        //���������
#define TASK_DELAY_0         500      //����0��ʱ 500*10ms��ִ�� ��������Ƿ�����
#define TASK_DELAY_1         50       //����1��ʱ 50*10ms��ִ��

uint32_t TASK_Delay_Group[TASK_DELAY_NUM];   //����������������ʱ �ж��Ƿ�ִ�ж�Ӧ����

/* ÿ5���������Ƿ����� */
int hc05_inquery_connect;     //ÿ5S��������Ƿ�����
int hc05_check_receivebuff;  //ÿ500ms��������Ƿ�����


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
		TASK_Delay_Group[i]++;        //�����ʱ��ʱ�䵽��ִ��
	}
	
	/* ��������0 */
	if(TASK_Delay_Group[0]>= TASK_DELAY_0)  //�ж��Ƿ�ִ������0
	{ 
		TASK_Delay_Group[0] = 0;        //��0 ���¼�ʱ
		
		/* ����0 ��������Ƿ����� */
		hc05_inquery_connect = 1;  //�������û������ ��־λ��1
	}
	
	/* ��������1 */
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
