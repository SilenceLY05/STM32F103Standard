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
#include "bsp_usart.h"
#include "bsp_i2c_ee.h"
#include "bsp_ap3216c.h"
#include "bsp_led.h"


#define TASK_DELAY_NUM     2        //����������������Լ�����ʵ������޸�
#define TASK_DELAY_0       1000     //����0��ʱ  1���ִ�� ��תLED
#define TASK_DELAY_1       500      //����1��ʱ 0.5���ִ�ж�ȡ���ݲ���

uint32_t Task_Delay_Group[TASK_DELAY_NUM];    //����������������ʱ�����ж��Ƿ�ִ�ж�Ӧ����

/* ִ�������־����ȡ���� */
// - ��־��1��ʾ��ȡ���ݲ�����ɣ���Ҫ����ѭ����������
// - ��־��0��ʾ��ȡ���ݲ���δ��ɣ���Ҫ���ж��ж�ȡ����

int task_readdata_finish;

/* �����ⲿ���� */
extern float ALS;
extern uint16_t PS;
extern uint16_t IR;
extern uint8_t IntStatus;

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
	for(i=0;i<TASK_DELAY_NUM;i++)
	{
		Task_Delay_Group[i]++;          //�����ʱ��ʱ�䵽ִ��
	}
	
	/* ��������0 */
	if(Task_Delay_Group[0] >= TASK_DELAY_0)
	{
		Task_Delay_Group[0] = 0;        //��0���¼���
		
		LED2_TOGGLE;
	}
	
	/* ��������1 */
	if(Task_Delay_Group[1] >= TASK_DELAY_1)
	{
		Task_Delay_Group[1] = 0;
		
		if(!task_readdata_finish)
		{
			IntStatus = ap3216c_get_IntStatus();   //�ȶ�״̬����ADC����λ�����״̬λ
			ALS = ap3216c_read_ambient_light();
			PS = ap3216c_read_ps_data();
			IR = ap3216c_read_ir_data();
			
			task_readdata_finish = 1;//��־λ��1����ʾ��Ҫ����ѭ����������
		}
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

void DEBUG_USART_IRQHandler(void)
{
	uint8_t ch = 0;
	
	if(USART_GetFlagStatus(DEBUG_USARTx,USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(DEBUG_USARTx);
		USART_SendData(DEBUG_USARTx,ch);
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
