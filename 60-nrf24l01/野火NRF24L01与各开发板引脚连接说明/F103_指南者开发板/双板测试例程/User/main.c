/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/slave 测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-V3 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"
#include "./key/bsp_key.h"  

u8 status = 0;		 //用于判断接收/发送状态
u8 txbuf[32];	 //发送缓冲
u8 rxbuf[32];	 //接收缓冲
u8 i; 

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{    
  /* 串口1初始化 */
  USART1_Config(); 

  /*SPI接口初始化*/   
  SPI_NRF_Init(); 
  
  /* 按键初始化 */
  
  Key_GPIO_Config();

  printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 从机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

  /*检测NRF模块与MCU的连接*/
  status = NRF_Check();   		
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功\r\n");  
  else	  
    printf("\r\n   请检测NRF与MCU是否正常连接。。。\r\n");
  
  printf("\r\n NRF进入接收模式，按 K1 发送数据\r\n"); 


  for(i=0;i<32;i++)
  {	
    txbuf[i] = i; 
  }
  
  NRF_RX_Mode();     // NRF 进入接收模式
  
  while(1)
  {  		 	
    /* 等待接收数据 */
    status = NRF_Rx_Dat(rxbuf);

    /* 判断接收状态 */
    if(status == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n 接收数据为：%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n进入接收模式，按 K1 发送数据\r\n"); 
    }
     
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)    // 按键按下，开始送数据
    { 
      /* 发送数据 */
      NRF_TX_Mode();
           
      status = NRF_Tx_Dat(txbuf);
      
      /* 发送数据的状态 */
       if(status == TX_DS)
      {
        printf("\r\n发送数据成功\r\n");
      }
      else
      {
        printf("\r\n发送数据失败  %d\r\n", status);
      }
      
      printf("\r\n 进入接收模式，按 K1 发送数据\r\n"); 

      NRF_RX_Mode();
    }
  } 
}
/*********************************************END OF FILE**********************/
