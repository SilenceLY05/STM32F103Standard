/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_mpu6050.h"

/* MPU6050���� */
short Acel[3];
short Gyro[3];
float Temp;


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	LED_GPIO_Config();
	
	USART_Config();
	
	I2C_Bus_Init();
	
	MPU6050_Init();
	
	/* ��⵽MPU6050 */
	if(MPU6050ReadID() == 0)
	{
		printf("\r\n û�м�⵽MPU6050�������� \r\n");
		LED_RED;
		while(1);   // ��ⲻ���������Ȼ����
	}
	
	/* ����Systick��ʱ�����ж� */
	SysTick_Init();  //����SystickΪ1ms�ж�һ�Σ����ж����ȡ����������
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //������ʱ��

	
	while (1)
	{	
		if(task_readdata_finish)  //task_readdata_finish = 1 ��ʾ��ȡMPU6050�������
		{
			printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
			printf("    �����ǣ�%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
			printf("    �¶�%8.2f\r\n",Temp);
			
			task_readdata_finish = 0; //�����־λ
		}
		
	}
}


/*********************************************END OF FILE**********************/
