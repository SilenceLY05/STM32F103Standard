/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include <string.h>
#include "bsp_usart.h"
#include "bsp_SysTick.h"


#include "sl2x23.h"
#include "lpcd.h"
#include "Rfid_interface.h"


/* 在 "sl2x23_if.h" 头文件里更改通信接口
   在 "lpcd.h" 头文件里启用/关闭 LPCD 模式
   在 "sl2x23.h" 头文件里定义宏 NOT_IRQ 不使用IRQ引脚，而是软件查询的方式
   开 LPCD 模式需要使用 IRQ 引脚
*/


#define OPEN_SYSTEM_DEBUG         //打开调试宏

#ifdef OPEN_SYSTEM_DEBUG
#define PRINTF(x,...) printf(x,##__VA_ARGS__)
#else
#define PRINTF(x,...)
#endif



extern uint8_t card_A;
extern uint8_t card_B;

extern uint8_t sak;
extern  uint16_t atqa;


//demo 函数
int8_t nfc_demo_user(void);
int8_t nfc_demo(void);
void lpcd_demo(void);




int main(void)
{
	SysTick_Init();
	USART_Config();
	
	/* SL2823接口初始化 */
	SL2x23_Init();
	Communication_Test();   //测试通信
	
	/* SL2823 RFID功能初始化 */
#ifdef _ENABLE_LPCD_
	SL2x23_Para_Init_LPCD();
	Rfid_Init_LPCD();
#else
	SL2x23_Para_Init();
	Rfid_Init();
#endif
	
	pcd_default_info();
	
	printf("System Init OK \r\n\r\n");
	while(1)
	{
#ifdef _ENABLE_LPCD_
		lpcd_demo();    /* LPCD中断+查询 */
#else
		#if 1
		nfc_demo();   /* 仅查询模式 */
		#else //先把上面的#if 1改为#if 0
		nfc_demo_user();  //模拟用户刷卡：刷一次仅识别一次
		#endif
#endif
	}
}


/* 模拟测试用户刷卡的情况，刷一次仅识别一次 */
int8_t nfc_demo_user(void)
{
	int8_t status;
	
	pcd_antenna_off();
	delay_1ms(3);
	pcd_set_rate(1);
	pcd_config('A');      //配置为测试A卡
	pcd_antenna_on();
	delay_1ms(5);
	
	while(1)
	{
		/* 用REQA方式寻卡 -- 处于HALT状态的卡不会响应 */
		status = pcd_polling_reqa(g_tag_info.serial_num);
		
		if(status == MI_OK)
		{
			printf("request (cmd REQA) typeA successful!\r\n");
			delay_1ms(2);
			
			/* 操作卡 */
			if((sak == 0x08) || (sak == 0x18))
			{
				PRINTF("卡类型：NFC_A MIFARE \r\n");
				NFC_A_Mifare_Test(g_tag_info.serial_num);
			}
			
			if(atqa == 0x000C)
			{
				PRINTF("卡类型：NFC_A_Tag1\r\n");
				NFC_A_Tag1_Test(g_tag_info.serial_num);
			}
			
			if(atqa == 0x4400)
			{
				PRINTF("卡类型：NFC_A_Tag2 MIFARE UltraLight \r\n");
				
				/* 打印UID */
				PRINTF("当前的UID：");
				PRINTF("0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\r\n",g_tag_info.serial_num[0],
				g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3],
				g_tag_info.serial_num[4],g_tag_info.serial_num[5],g_tag_info.serial_num[6]);
			}
			
			/* 退出本次操作 -- 使卡进入HALT状态 */
			if(pcd_hlta() == MI_OK)
			{
				printf("卡休眠成功\r\n");
			}
		}
	}
}


/* 查询模式 */
int8_t nfc_demo(void) 
{
    uint8_t g_ret = 0;
    uint8_t rate = 0;

    while(1) 
		{
			card_A = 0;/* 允许重新检测card_A */
      card_B = 0;/* 允许重新检测card_B */
       
			/* 询卡 */
      g_ask_block_flag = 0;
      pcd_antenna_off();
      delay_1ms(3);

       pcd_set_rate(1);
       g_ret = pcd_polling(g_tag_info.serial_num);
			
       if(g_ret == 0) 
			 {
         rate = 1;
         printf("应答 atqa = 0x%x sak = 0x%x\r\n",atqa,sak);    
       } 
			 else 
			 {
					return 0;
       }

       if(card_A) 
			 {
         if((sak == 0x08) || (sak == 0x18)) 
				 { //MIFARE
					 PRINTF("卡类型：NFC_A MIFARE\r\n");
           NFC_A_Mifare_Test(g_tag_info.serial_num);
         }
         if(atqa == 0x000c) 
				 {
					 PRINTF("卡类型：NFC_A_Tag1\r\n");
           NFC_A_Tag1_Test(g_tag_info.serial_num);
         }
         if(atqa == 0x4400) 
				 {
           PRINTF("卡类型：NFC_A_Tag2 MIFARE UltraLight\r\n");

					 PRINTF("当前的UID：");
					 PRINTF("0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x",g_tag_info.serial_num[0],
					 g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3],
					 g_tag_info.serial_num[4],g_tag_info.serial_num[5],g_tag_info.serial_num[6]);
         }
				 // 操作完成后直接退出，重新检测和询卡
				 break;
       }
       g_ret = pcd_active(rate);
       if(g_ret == 0) 
			 {
         if(card_A) 
				 {
           Iblock_getrand(rate);
         }
         if(card_B) 
				 {
            g_ask_block_flag = 1;
            g_ret = get_idcard_num(g_tag_info.serial_num);
            if(g_ret == 0) 
					  {
               g_tag_info.uid_length = 10;
               PRINT_BUF("ID_NUM: ", g_tag_info.serial_num, g_tag_info.uid_length);
               printf("## test end##\r\n\r\n");
            } 
						else 
						{
               Iblock_getrand(rate);
            }
               
            }
        }
        
        break; //return 1;
    }
    
    /* 表示检测到了卡（正常读写通讯并操作完成、或者通讯有问题操作不了）就要返回1 */
    return 1;
}


/* Debug */
//#define USER_ADC_REF_VAL    0x6b


void lpcd_demo(void) 
{
    uint8_t adc_value;
		uint8_t irq_val;
    uint32_t wait_cnt = 0;
	
    irq_flag_io = 0;
    lpcd_cfg.t1 = LPCD_DETECT_PERIOD;      //LPCD 探测周期设定为 0x8 = 500ms
    lpcd_cfg.sense = LPCD_DETECT_SENS;     //LPCD 感应灵敏度设定为 0x1 = 9mV
	
    printf("******* start lpcd_demo() *******\r\n");
	
    osc_calib();
    lpcd_init(1, 0x40);  //fst_en = 1 第一次初始化lpcd功能需要开场初始化 adc 和自动校准AGC
    lpcd_entry();        /* 进入LPCD模式（实际上进入了软掉电模式降低功耗） */
	
    while(1) 
		{
        #ifdef NOT_IRQ
        if(!(INT_PIN == 0))          //用软件查询IRQ引脚是否产生中断
        #else
        if(irq_flag_io)              //判断IRQ引脚是否触发中断
        #endif
        {
					/* 退出LPCD模式 开始交换数据 */
            printf("...................................\r\n");
            printf("\r\n******* l、pcd_exit() *********\r\n");
            adc_value = lpcd_exit();   /* 获取lpcd退出时的场的 adc值, 
                                        后面lpcd初始化时要用来判断是否要自动校准AGC */
						irq_val = read_reg(0x5);
					
					if((irq_val & 0x40) == 0x40)
					{
						lpcd_init(0,adc_value);   //fst_en=0 不会重新初始化adc和校准AGC
						pcd_antenna_off();
						printf("******** first lpcd_entry() do nothing ********\r\n");
						lpcd_entry();    //重新进入LPCD模式
					}
					else
					{
						/* 初始化查询模式 然后调用nfc_demo() */
						Rfid_Init_LPCD();
						printf("******** 2、nfc_demo() ********\r\n");
						
						/* 做产品的读卡操作 */
						if(nfc_demo() == 1)        //nfc_demo操作成功返回1
						{
							lpcd_init(1,adc_value);  //操作成功后要重新初始化adc和校准AGC
						}
						else
						{
							lpcd_init(0,adc_value);
						}
						pcd_antenna_off();
						
						printf("******** 3、 lpcd_entry() ********\r\n");
						lpcd_entry();  //重新进入LPCD模式
					}
					irq_flag_io = 0;  /* 标志位重新置0 */
           
        } 
				else 
				{
            wait_cnt++;
            if(wait_cnt == 500000) {
                wait_cnt = 0;
                printf(".");
							
//							printf("\r\n reg1_val = 0x%x", sl2x23_read_reg(0x88));
//                printf("\r\n reg2_val = 0x%x", sl2x23_read_reg(0x89));
//                delay_1ms(20);
            }
        }
    }
}




//endfile

