#include <stdio.h>

#include "lpcd.h"



lpcd_cfg_t lpcd_cfg;
unsigned char lpcd_amp_test(unsigned char amp)
{
	unsigned char reg_14;
	unsigned char reg_15;
	unsigned char reg_28;
	unsigned char reg_29;
	unsigned char reg_65;
	unsigned char reg_66;
	unsigned char calib_rlt;
	unsigned char calib_rlt_filt;
	unsigned char fount_1;
	unsigned char recv_data;
	unsigned char i;
	
	SL_WR_REG(0x3F,0x00);
	reg_14 = SL_RD_REG(0x14);
	reg_15 = SL_RD_REG(0x15);
	reg_28 = SL_RD_REG(0x28);
	reg_29 = SL_RD_REG(0x29);
	SL_WR_REG(0x14,0x80);
	SL_WR_REG(0x15,0x00);      // Force100ASK = 0
	SL_WR_REG(0x28,amp^0x28);
	SL_WR_REG(0x29,amp^0x28);
	SL_WR_REG(0x3F,0x01);
	reg_65 = SL_RD_REG(0x65);
	reg_66 = SL_RD_REG(0x66);
	SL_WR_REG(0x65,0x00);
	SL_WR_REG(0x66,lpcd_cfg.phase); 
	SL_WR_REG(0x54,0x81);           // lpcd calib mode
	SL_WR_REG(0x51,0x00);           // T1
	SL_WR_REG(0x3F,0x00);
	SL_WR_REG(0x05,0x20); 
	SL_WR_REG(0x01,0x10);           // enter lpcd calib mode
	
	// wait lpcd calib done
	do
	{
		recv_data = SL_RD_REG(0x05);
		
	}while((recv_data & 0x20)!=0x20);
	SL_WR_REG(0x01,0x00);
	
	DELAY_1MS;
	SL_WR_REG(0x05,0x20);
	SL_WR_REG(0x3F,0x01);
	calib_rlt = 0;
	for(i=0;i<8;i++)
	{
		calib_rlt >>= 1;
		calib_rlt |= (SL_RD_REG(0x5B+i)&0x80);
	}
	SL_WR_REG(0x3F,0x00);
	SL_WR_REG(0x3F,0x01); 
	SL_WR_REG(0x65,reg_65);           
	SL_WR_REG(0x66,reg_66);           
	SL_WR_REG(0x51,lpcd_cfg.t1);
	SL_WR_REG(0x54,0x92); 
	SL_WR_REG(0x3F,0x00); 
	SL_WR_REG(0x15,reg_15);
	SL_WR_REG(0x14,reg_14 |0x03); 
	SL_WR_REG(0x29,reg_29);           
	SL_WR_REG(0x28,reg_28);      
	fount_1 = 0;
	
	calib_rlt_filt = calib_rlt;
	for(i=0;i<8;i++)
	{
		if(fount_1 == 1)
		{
			calib_rlt_filt |= (0x01<<i);
		}
		if((calib_rlt>>i)&0x01)
		{
			fount_1 = 1;
		}
	}
	return calib_rlt_filt;

}


unsigned char osc_calib() {
    uint8_t val;
    SL_WR_REG(0x3f, 0x00);
    SL_WR_REG(0x01, 0x00);    //取消命令执行，进入idle
    delay_ms(1);
    SL_WR_REG(0x01, 0x06);    //执行 0x06 命令
    delay_ms(10);
    SL_WR_REG(0x3f, 0x01);
    val = SL_RD_REG(0x4e);
    SL_WR_REG(0x3f, 0x00);
    if(val & 0x80) {          //判断 OSC 频率校准结束标志位
        return 0;
    } else {
        printf("osc_calib fail\r\n");
        return 1;
    }
}

//unsigned char slm_reg_14, slm_reg_15, slm_reg_28, slm_reg_29, slm_reg_65, slm_reg_66;

void lpcd_init(unsigned char fst_en, unsigned char adc_value) {
    if((fst_en == 0) && (adc_value > AGC_RX_MAX)) {
        fst_en = 1;
        printf("Warning! adc_value is too large, agc need rejust\r\n");
    }
    //if(fst_en) osc_calib();
    SL_WR_REG(0x3f, 0x0); // adc_sample time
    SL_WR_REG(DivIEnReg, 0x60); // fst lpcd int en, lpcd_int_en;
    SL_WR_REG(ComIrqReg, 0x7f); // clear irq
    SL_WR_REG(DivIrqReg, 0x7f); // clear irq
    SL_WR_REG(TxASKReg, 0x00);
    SL_WR_REG(ModGsPReg, SL_RD_REG(CWGsPReg));
    SL_WR_REG(0x3f, 0x01);
    SL_WR_REG(0x61, 0x10); //set vmid res as min=1.4k, default=3.3K, bqliu 20211124
    SL_WR_REG(0x3f, 0x00);
    if(fst_en) {           /* 如果 fst_en=1, */
        pcd_antenna_on(); //open field
        delay_1ms(5);
        adc_init();
        agc_calib();      /* 判断adc_value参数不在范围内的话,会自动校准AGC、并重新赋值给adc_value */
    }
    SL_WR_REG(0x3f, 0x2); // adc_sample time
    SL_WR_REG(LPCD_CTRL0, 0x11 | (fst_en << 5)); //adc_sample_time,0x13->0x11, to turn off vmid(1mA), during spd mode in lpcd, bqliu 20211124
    SL_WR_REG(LPCD_PARAM0, adc_value + lpcd_cfg.sense); // config abs feild strength low threshold
    SL_WR_REG(LPCD_PARAM1, adc_value - lpcd_cfg.sense); // config abs feild strength high threshold
    SL_WR_REG(LPCD_PARAM2, lpcd_cfg.sense); // config relative feild strength threshold
    SL_WR_REG(LPCD_PARAM3, 0x2c); //lpcd_fst_en=1,lpcd_out_inv=1;lpcd_envsel=0;lpcd_en=1;lpcd_work=1;
    // SL_WR_REG(LPCD_CTRL2,0x03);//ant_stable_time
    SL_WR_REG(LPCD_TIME0, 0x0); //ant_stable_time
    SL_WR_REG(LPCD_TIME1, lpcd_cfg.t1); //lpcd_piorid
    SL_WR_REG(LPCD_TIME2, 0x2); //start up time
    SL_WR_REG(0x3f, 0x0); //
}

// set 0x14/0x15/0x28/0x29/0x65/0x66 and turn off rf field
void lpcd_entry() {
    SL_WR_REG(0x01, 0x10);    // 进入软掉电模式 然后等待lpcd中断的时候唤醒

}


// restore 0x14/0x15/0x28/0x29/0x65/0x66
uint8_t lpcd_exit() {
    uint8_t adc_value;

    SL_WR_REG(0x01, 0x00);// 退出软掉电模式
    DELAY_1MS;            // wait wakeup done

    SL_WR_REG(0x3f, 0x2);  // 读 adc
    adc_value = read_reg(ADC_VALUE0);
    printf("after lpcd: read_adc_value=%x\r\n", adc_value);
    SL_WR_REG(0x3f, 0x0);
    
    return adc_value;
}



void adc_init() {
    //     SL_WR_REG(LPCD_CTRL2,0x80); //adc_sample_time
    SL_WR_REG(0x3f, 0x1); //
    clear_bit_mask(0x65, 0x80);
    SL_WR_REG(0x66, 0x00); // clkdelay=0s
    SL_WR_REG(0x3f, 0x2); //
    SL_WR_REG(LPCD_CTRL0, 0x13); //lpcd_out_inv=1,lpcd_envsel=0;lpcd_sample=0;lpcd_en=1;lpcd_work=1
    SL_WR_REG(ADC_TIME0, 0xFF); //adc_sample_time
    SL_WR_REG(ADC_TIME1, 0x28); //adc_compare_time
};
unsigned char get_adc_value() {
    SL_WR_REG(0x3f, 0x2);
    SL_WR_REG(ADC_CTRL, 0x1);
    delay_10us(8);
    return SL_RD_REG(ADC_VALUE0);
};

unsigned char get_adc_value_mean(unsigned char n) {
    int i;
    unsigned char adc_value;
    unsigned char mean_adc_value;
    u16 adc_value_acc;
    adc_value_acc = 0;
    for(i = 0; i < n; i = i + 1) {
        adc_value = get_adc_value();
        adc_value_acc = adc_value_acc + adc_value;
        #ifdef LPCD_DEBUG
        printf("i=%d: adc_value=%x\r\n", i, adc_value);
        #endif
    };
    mean_adc_value =  adc_value_acc / n;
    return     mean_adc_value;
};
unsigned char agc_calib() {
    unsigned char adc_value;
    unsigned char agc_ctrl;
    #if 1
    agc_ctrl = 0xd;
    while(1) {
        SL_WR_REG(0x3f, 0x1);
        SL_WR_REG(AGC_CTRL, 0x80 + agc_ctrl);
        delay_10us(50);
        SL_WR_REG(0x3f, 0x2);
        adc_value = get_adc_value();
#ifdef LPCD_DEBUG
        printf("agc_ctrl=%x,adc_value=%x\r\n", agc_ctrl, adc_value);
#endif
        if(adc_value > AGC_RX_MAX && agc_ctrl > 0x1) {
            agc_ctrl--;
        } else if(adc_value > AGC_RX_MAX && agc_ctrl == 0x0) {
            printf("Warning: The field is overflow! agc can not reduce it!\r\n");
            break;
        } else {
#ifdef LPCD_DEBUG
            printf("agc ok agc_ctrl=%x,adc_value=%x\r\n", agc_ctrl, adc_value);
#endif
            break;
        }
    }
    printf("agc_ctrl=%x,adc_value=%x\r\n", agc_ctrl, adc_value);
    return agc_ctrl;
    #endif
};

void test_agc() {
    unsigned char adc_value;
//    int i;
    SL_WR_REG(0x3f, 0x2);
    adc_init();
    SL_WR_REG(0x3f, 0x0);
    SL_WR_REG(TxControlReg, 0x83); // open feild
    delay_10us(500);
    SL_WR_REG(0x3f, 0x1);
    SL_WR_REG(AGC_CTRL, 0x00);
    SL_WR_REG(0x3f, 0x2);
    adc_value = get_adc_value();
    printf("agc_ctrl=%x,adc_value=%x\r\n", 0x0, adc_value);
    delay_10us(100);
    #if 0
    for(i = 0; i < 0xe; i = i + 1) {
        agc_ctrl = 0;
        SL_WR_REG(0x3f, 0x1);
        SL_WR_REG(AGC_CTRL, 0); //0x00+agc_ctrl);
        delay_10us(100);
        SL_WR_REG(0x3f, 0x2);
        adc_value = get_adc_value();
        printf("agc_ctrl=%x,adc_value=%x\r\n", agc_ctrl, adc_value);
    }
    #endif
    SL_WR_REG(0x3f, 0x0);
//while(1);
};


void test_lpcd_conf() {
    //unsigned char adc_value;
    int i;
    SL_WR_REG(0x3f, 0x0);
    SL_WR_REG(TxControlReg, 0x83); // open feild
    delay_10us(500);
    SL_WR_REG(0x3f, 0x1);
    SL_WR_REG(AGC_CTRL, 0x84);
    SL_WR_REG(0x3f, 0x2);
    SL_WR_REG(LPCD_CTRL0, 0x37); //adc_sample_time
    for(i = 0; i < 256; i = i + 1) {
        SL_WR_REG(LPCD_HYS, i); //adc_compare_time
        delay_10us(2);
    }
    for(i = 256; i > 0; i = i - 1) {
        SL_WR_REG(LPCD_HYS, i); //adc_compare_time
        delay_10us(2);
    }
}

void lpcd_print(void) {
    unsigned char reg;
    SL_WR_REG(0x3f, 0x00);
    reg = SL_RD_REG(0x01);
    printf("\r\n SL_RD_REG(0x01)= %x\n", reg);
    reg = SL_RD_REG(0x15);
    printf("SL_RD_REG(0x15)= %x\n", reg);
    reg = SL_RD_REG(0x14);
    printf("SL_RD_REG(0x14)= %x\n", reg);
    reg = SL_RD_REG(0x28);
    printf("SL_RD_REG(0x28)= %x\n", reg);
    reg = SL_RD_REG(0x29);
    printf("SL_RD_REG(0x29)= %x\n", reg);
    SL_WR_REG(0x3f, 0x01);
    reg = SL_RD_REG(0x65);
    printf("SL_RD_REG(0x65)= %x\n", reg);
    reg = SL_RD_REG(0x66);
    printf("SL_RD_REG(0x66)= %x\n", reg);
    SL_WR_REG(0x3f, 0x00);
    reg = SL_RD_REG(0x03);
    printf("SL_RD_REG(0x03)= %x\n", reg);
}
