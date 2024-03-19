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
	
#include <string.h>
#include "bsp_usart.h"
#include "bsp_SysTick.h"


#include "sl2x23.h"
#include "lpcd.h"
#include "Rfid_interface.h"


/* �� "sl2x23_if.h" ͷ�ļ������ͨ�Žӿ�
   �� "lpcd.h" ͷ�ļ�������/�ر� LPCD ģʽ
   �� "sl2x23.h" ͷ�ļ��ﶨ��� NOT_IRQ ��ʹ��IRQ���ţ����������ѯ�ķ�ʽ
   �� LPCD ģʽ��Ҫʹ�� IRQ ����
*/


#define OPEN_SYSTEM_DEBUG         //�򿪵��Ժ�

#ifdef OPEN_SYSTEM_DEBUG
#define PRINTF(x,...) printf(x,##__VA_ARGS__)
#else
#define PRINTF(x,...)
#endif



extern uint8_t card_A;
extern uint8_t card_B;

extern uint8_t sak;
extern  uint16_t atqa;


//demo ����
int8_t nfc_demo_user(void);
int8_t nfc_demo(void);
void lpcd_demo(void);




int main(void)
{
	SysTick_Init();
	USART_Config();
	
	/* SL2823�ӿڳ�ʼ�� */
	SL2x23_Init();
	Communication_Test();   //����ͨ��
	
	/* SL2823 RFID���ܳ�ʼ�� */
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
		lpcd_demo();    /* LPCD�ж�+��ѯ */
#else
		#if 1
		nfc_demo();   /* ����ѯģʽ */
		#else //�Ȱ������#if 1��Ϊ#if 0
		nfc_demo_user();  //ģ���û�ˢ����ˢһ�ν�ʶ��һ��
		#endif
#endif
	}
}


/* ģ������û�ˢ���������ˢһ�ν�ʶ��һ�� */
int8_t nfc_demo_user(void)
{
	int8_t status;
	
	pcd_antenna_off();
	delay_1ms(3);
	pcd_set_rate(1);
	pcd_config('A');      //����Ϊ����A��
	pcd_antenna_on();
	delay_1ms(5);
	
	while(1)
	{
		/* ��REQA��ʽѰ�� -- ����HALT״̬�Ŀ�������Ӧ */
		status = pcd_polling_reqa(g_tag_info.serial_num);
		
		if(status == MI_OK)
		{
			printf("request (cmd REQA) typeA successful!\r\n");
			delay_1ms(2);
			
			/* ������ */
			if((sak == 0x08) || (sak == 0x18))
			{
				PRINTF("�����ͣ�NFC_A MIFARE \r\n");
				NFC_A_Mifare_Test(g_tag_info.serial_num);
			}
			
			if(atqa == 0x000C)
			{
				PRINTF("�����ͣ�NFC_A_Tag1\r\n");
				NFC_A_Tag1_Test(g_tag_info.serial_num);
			}
			
			if(atqa == 0x4400)
			{
				PRINTF("�����ͣ�NFC_A_Tag2 MIFARE UltraLight \r\n");
				
				/* ��ӡUID */
				PRINTF("��ǰ��UID��");
				PRINTF("0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\r\n",g_tag_info.serial_num[0],
				g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3],
				g_tag_info.serial_num[4],g_tag_info.serial_num[5],g_tag_info.serial_num[6]);
			}
			
			/* �˳����β��� -- ʹ������HALT״̬ */
			if(pcd_hlta() == MI_OK)
			{
				printf("�����߳ɹ�\r\n");
			}
		}
	}
}


/* ��ѯģʽ */
int8_t nfc_demo(void) 
{
    uint8_t g_ret = 0;
    uint8_t rate = 0;

    while(1) 
		{
			card_A = 0;/* �������¼��card_A */
      card_B = 0;/* �������¼��card_B */
       
			/* ѯ�� */
      g_ask_block_flag = 0;
      pcd_antenna_off();
      delay_1ms(3);

       pcd_set_rate(1);
       g_ret = pcd_polling(g_tag_info.serial_num);
			
       if(g_ret == 0) 
			 {
         rate = 1;
         printf("Ӧ�� atqa = 0x%x sak = 0x%x\r\n",atqa,sak);    
       } 
			 else 
			 {
					return 0;
       }

       if(card_A) 
			 {
         if((sak == 0x08) || (sak == 0x18)) 
				 { //MIFARE
					 PRINTF("�����ͣ�NFC_A MIFARE\r\n");
           NFC_A_Mifare_Test(g_tag_info.serial_num);
         }
         if(atqa == 0x000c) 
				 {
					 PRINTF("�����ͣ�NFC_A_Tag1\r\n");
           NFC_A_Tag1_Test(g_tag_info.serial_num);
         }
         if(atqa == 0x4400) 
				 {
           PRINTF("�����ͣ�NFC_A_Tag2 MIFARE UltraLight\r\n");

					 PRINTF("��ǰ��UID��");
					 PRINTF("0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x",g_tag_info.serial_num[0],
					 g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3],
					 g_tag_info.serial_num[4],g_tag_info.serial_num[5],g_tag_info.serial_num[6]);
         }
				 // ������ɺ�ֱ���˳������¼���ѯ��
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
    
    /* ��ʾ��⵽�˿���������дͨѶ��������ɡ�����ͨѶ������������ˣ���Ҫ����1 */
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
    lpcd_cfg.t1 = LPCD_DETECT_PERIOD;      //LPCD ̽�������趨Ϊ 0x8 = 500ms
    lpcd_cfg.sense = LPCD_DETECT_SENS;     //LPCD ��Ӧ�������趨Ϊ 0x1 = 9mV
	
    printf("******* start lpcd_demo() *******\r\n");
	
    osc_calib();
    lpcd_init(1, 0x40);  //fst_en = 1 ��һ�γ�ʼ��lpcd������Ҫ������ʼ�� adc ���Զ�У׼AGC
    lpcd_entry();        /* ����LPCDģʽ��ʵ���Ͻ����������ģʽ���͹��ģ� */
	
    while(1) 
		{
        #ifdef NOT_IRQ
        if(!(INT_PIN == 0))          //�������ѯIRQ�����Ƿ�����ж�
        #else
        if(irq_flag_io)              //�ж�IRQ�����Ƿ񴥷��ж�
        #endif
        {
					/* �˳�LPCDģʽ ��ʼ�������� */
            printf("...................................\r\n");
            printf("\r\n******* l��pcd_exit() *********\r\n");
            adc_value = lpcd_exit();   /* ��ȡlpcd�˳�ʱ�ĳ��� adcֵ, 
                                        ����lpcd��ʼ��ʱҪ�����ж��Ƿ�Ҫ�Զ�У׼AGC */
						irq_val = read_reg(0x5);
					
					if((irq_val & 0x40) == 0x40)
					{
						lpcd_init(0,adc_value);   //fst_en=0 �������³�ʼ��adc��У׼AGC
						pcd_antenna_off();
						printf("******** first lpcd_entry() do nothing ********\r\n");
						lpcd_entry();    //���½���LPCDģʽ
					}
					else
					{
						/* ��ʼ����ѯģʽ Ȼ�����nfc_demo() */
						Rfid_Init_LPCD();
						printf("******** 2��nfc_demo() ********\r\n");
						
						/* ����Ʒ�Ķ������� */
						if(nfc_demo() == 1)        //nfc_demo�����ɹ�����1
						{
							lpcd_init(1,adc_value);  //�����ɹ���Ҫ���³�ʼ��adc��У׼AGC
						}
						else
						{
							lpcd_init(0,adc_value);
						}
						pcd_antenna_off();
						
						printf("******** 3�� lpcd_entry() ********\r\n");
						lpcd_entry();  //���½���LPCDģʽ
					}
					irq_flag_io = 0;  /* ��־λ������0 */
           
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

