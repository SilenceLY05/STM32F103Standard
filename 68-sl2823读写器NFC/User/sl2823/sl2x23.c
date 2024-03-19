//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "sl2x23_if.h"
#include "sl2x23.h"
#include "iso14443_4.h"
#include "Rfid_interface.h"
#include "lpcd.h"


#define WATER_LEVEL 16 // 
#define FIFO_SIZE   64
#define FSD 256 //Frame Size for proximity coupling Device


#define READ_REG_CTRL   0x80
#define TP_FWT_302us    2048
#define TP_dFWT 192


//transceive_buffer  mf_com_data;

extern ST_PCDINFO   gtPcdModuleInfo;



void pcd_reset(void) {
    #if(NFC_DEBUG)
    printf("pcd_reset\r\n");
    #endif
    write_reg(CommandReg, PCD_RESETPHASE);
}

void pcd_antenna_on(void) {
    write_reg(TxControlReg, read_reg(TxControlReg) | 0x03); //Tx1RFEn=1 Tx2RFEn=1
}

void pcd_antenna_off(void) {
    write_reg(TxControlReg, read_reg(TxControlReg) & (~0x03));
}

void pcd_sleep(void) {
    write_reg(CommandReg, PCD_CMD_SLEEP);
}

#if 0
void delay_1ms(uint32_t delay_time) { //62.5ns
    uint32_t i;
    uint8_t j;
    for(j = 0; j < delay_time; j++) {
        for(i = 0; i < 16000; i++);
    }
}
#endif
void pcd_sleep_exit(void) {
    write_reg(CommandReg, PCD_IDLE);
    delay_10us(12);
}

/**
 * @brief  the config of type A and type B
 * @param  type: 'A' A card ,  'B' B card
 * @param
 * @retval
 */
int8_t pcd_config(uint8_t type) {
    if('A' == type) {
        write_reg(0x3f, 0x1);   // 11 // CRC seed:6363
        write_reg(EXT_TRMOD, 0x00);//12 //Tx Framing type-v
        write_reg(0x3f, 0x0);//13 //Rx framing type-v
        
        write_reg(ModeReg, 0x3D);   // CRC seed:6363
        write_reg(TxModeReg, 0x00); // Tx Framing A
        write_reg(RxModeReg, 0x00); // Rx framing A
        write_reg(TxASKReg, 0x40);  // typeA
        write_reg(BitFramingReg, 0x00); //TxLastBits=0
    } else if('B' == type) {
        write_reg(0x3f, 0x1);   // 11 // CRC seed:6363
        write_reg(EXT_TRMOD, 0x00);//12 //Tx Framing type-v
        write_reg(0x3f, 0x0);//13 //Rx framing type-v

        write_reg(ModeReg, 0x3F);   // CRC seed:FFFF
        write_reg(TxModeReg, 0x83); // Tx Framing B
        write_reg(RxModeReg, 0x83); // Rx framing B
        write_reg(TxASKReg, 0x00);  // typeB
        write_reg(BitFramingReg, 0x00); //TxLastBits=0
        
        write_reg(Status2Reg, 0x00);// clear MFCrypto1On
        write_reg(GsNReg, 0xF8);
        write_reg(AutoTestReg, 0x00);
        write_reg(ModGsPReg, sl2523_modgsp_init);
        /* improve efficiency of read B card */
        write_reg(TypeBReg, 0x13);  // 0xc3
//        write_reg(0x46,0x0f);
//        write_reg(0x47,0x6f);
//        write_reg(0x3f,0x0);
    } else if('V' == type) {
        write_reg(0x3f, 0x1);   // 11 // CRC seed:6363
        write_reg(EXT_TRMOD, 0x22);//12 //Tx Framing type-v
        write_reg(0x3f, 0x0);//13 //Rx framing type-v
        
        write_reg(ModeReg, 0x3D);   // CRC seed:6363
        write_reg(TxModeReg, 0x82); // Tx Framing B
        write_reg(RxModeReg, 0x82); // Rx framing B
        write_reg(TxASKReg, 0x40);  // typeA
        write_reg(BitFramingReg, 0x00); //TxLastBits=0
    } else if('F' == type) {
        write_reg(0x3f, 0x1);   // 11 //
        write_reg(EXT_TRMOD, 0x11);//12 //
        write_reg(0x3f, 0x0);//13 //
        
        write_reg(ModeReg, 0x3D);   // CRC seed:6363
        write_reg(TxModeReg, 0x91); // txcrc_en
        write_reg(RxModeReg, 0x91); // rxcrc_en
        write_reg(TxASKReg, 0x00);  // typeA
        write_reg(BitFramingReg, 0x00); //TxLastBits=0
        // set_bit_mask(NFCPHY_FELICA_CTRL, GPIO6);  // stop receving by length in frame header, not by judging subcarrier off
        
        //write_reg(0x28,0x18);//GAIM  <6:4>
        //write_reg(0x29,0x18);//GAIM  <6:4>
        
        write_reg(0x26,0x18);//GAIM  <6:4>
        write_reg(0x3f, 0x1);
        write_reg(0x45,0x40);//迟滞区间<6:4>
        write_reg(0x61,0x12);//高通滤波器带宽<3:2>
        write_reg(0x3f, 0x0);
    } else {
        return USER_ERROR;
    }
    return MI_OK;
}

//int8_t pcd_config(uint8_t type)
//{
//    if('A' == type)
//    {
//        write_reg(ModeReg, 0x3D);   // CRC seed:6363
//        write_reg(TxModeReg, 0x00); // Tx Framing A
//        write_reg(RxModeReg, 0x00); // Rx framing A
//        write_reg(TxASKReg, 0x40);  // typeA
//    }
//    else if('B' == type)
//    {

//        write_reg(Status2Reg, 0x00);// clear MFCrypto1On
//        write_reg(ModeReg, 0x3F);   // CRC seed:FFFF
//        write_reg(GsNReg, 0xF8);
//        write_reg(AutoTestReg, 0x00);
//        write_reg(TxASKReg, 0x00);  // typeB
//        write_reg(TxModeReg, 0x83); // Tx Framing B
//        write_reg(RxModeReg, 0x83); // Rx framing B
//        write_reg(BitFramingReg, 0x00); //TxLastBits=0
//        write_reg(ModGsPReg, sl2523_modgsp_init);
//        /* improve efficiency of read B card */
//        write_reg(TypeBReg, 0x13);  // 0xc3
//        write_reg(0x3f,0x1);
//        write_reg(0x46,0x0f);
//        write_reg(0x47,0x6f);
//        write_reg(0x3f,0x0);
//    }
//    else
//    {
//        return USER_ERROR;
//    }
//    return MI_OK;
//}

/**
 * @brief  : the base api of transceive of reader chip. referenced all command
 * @param  pi : see the transceive_buffer struct
 * @retval 0: ok
 * @retval other: err or timeout.
 */


int8_t pcd_com_transceive(struct transceive_buffer *pi) {
    uint8_t  recebyte = 0;
    int8_t  status;
    uint8_t  irq_en = 0;
    uint8_t  wait_for = 0;
    uint8_t  last_bits;
    uint8_t  j;
    uint8_t  val;
    uint8_t  err = 0;
    uint8_t irq_inv;
    uint16_t  len_rest = 0;
    uint8_t  len;
    
    irq_flag_io = 0;
    switch(pi->mf_command) {
        case PCD_IDLE:
            irq_en   = 0x00;
            wait_for = 0x00;
            break;
        case PCD_AUTHENT:
            irq_en = IdleIEn | TimerIEn;
            wait_for = IdleIRq;
            break;
        case PCD_RECEIVE:
            irq_en   = RxIEn | IdleIEn;
            wait_for = RxIRq;
            recebyte = 1;
            break;
        case PCD_TRANSMIT:
            irq_en   = TxIEn | IdleIEn;
            wait_for = TxIRq;
            break;
        case PCD_TRANSCEIVE:
            irq_en = RxIEn | TimerIEn | TxIEn;
            //irq_en = RxIEn | IdleIEn | TxIEn;
            wait_for = RxIRq ;
            recebyte = 1;
            break;
        default:
            pi->mf_command = MI_UNKNOWN_COMMAND;
            break;
    }
    if((pi->mf_command != MI_UNKNOWN_COMMAND)
       && (((pi->mf_command == PCD_TRANSCEIVE || pi->mf_command == PCD_TRANSMIT) && pi->mf_length > 0)
           || (pi->mf_command != PCD_TRANSCEIVE && pi->mf_command != PCD_TRANSMIT))
      ) {
        write_reg(CommandReg, PCD_IDLE);
        irq_inv = read_reg(ComIEnReg) & BIT7;
        write_reg(ComIEnReg, irq_inv | irq_en | BIT0);
        write_reg(ComIrqReg, 0x7F); //Clear INT
        write_reg(DivIrqReg, 0x7F); //Clear INT
        //Flush Fifo
        set_bit_mask(FIFOLevelReg, BIT7);
        if(pi->mf_command == PCD_TRANSCEIVE || pi->mf_command == PCD_TRANSMIT || pi->mf_command == PCD_AUTHENT)
				{
            #if (NFC_DEBUG)
            printf(" PCD_tx:");
          
            for(j = 0; j < pi->mf_length; j++) 
						{
               printf("%02x ", (uint16_t)pi->mf_data[j]);
            }
            printf("\r\n");
            #endif
						
            len_rest = pi->mf_length;
            if(len_rest >= FIFO_SIZE) {
                len = FIFO_SIZE;
            } else {
                len = len_rest;
            }
            for(j = 0; j < len; j++) {
                write_reg(FIFODataReg, pi->mf_data[j]);
            }
            len_rest -= len;
            if(len_rest != 0) 
						{
                write_reg(ComIrqReg, BIT2);   // clear LoAlertIRq
                set_bit_mask(ComIEnReg, BIT2);// enable LoAlertIRq
            }
            write_reg(CommandReg, pi->mf_command);
						
            if(pi->mf_command == PCD_TRANSCEIVE) 
						{
               set_bit_mask(BitFramingReg, 0x80);
            }
            while(len_rest != 0) 
						{
                /* Wait TxEnd or LoAlert irq  */
                #ifdef NOT_IRQ
                while(INT_PIN == 0);
                #else
                while(irq_flag_io == 0);
                irq_flag_io = 0;
                #endif
                if(len_rest > (FIFO_SIZE - WATER_LEVEL)) {
                    len = FIFO_SIZE - WATER_LEVEL;
                } else {
                    len = len_rest;
                }
                for(j = 0; j < len; j++) {
                    write_reg(FIFODataReg, pi->mf_data[pi->mf_length - len_rest + j]);
                }
                write_reg(ComIrqReg, BIT2);
                len_rest -= len;
                if(len_rest == 0) {
                    clear_bit_mask(ComIEnReg, BIT2);// disable LoAlertIRq
                }
            }
            if(pi->mf_command != PCD_AUTHENT) {
                #ifdef NOT_IRQ
                while(INT_PIN == 0);
                #else
                while(irq_flag_io == 0);
                irq_flag_io = 0;
                #endif
                val = read_reg(ComIrqReg);
                if(val & TxIRq) {
                    write_reg(ComIrqReg, TxIRq);
                    val = 0x7F & read_reg(FIFOLevelReg);
                }
            }
        }
        if(PCD_RECEIVE == pi->mf_command) {
            set_bit_mask(ControlReg, BIT6);// TStartNow
        }
        len_rest = 0; // bytes received
        write_reg(ComIEnReg, irq_inv);
        write_reg(ComIEnReg, irq_inv|irq_en|BIT0);
        write_reg(ComIrqReg, BIT3);    // clear HoAlertIRq
        set_bit_mask(ComIEnReg, BIT3); // enable HoAlertIRq
        while(1) 
				{
            /* Wait Rx or HoAlertIRq or TimeOut  irq */
            #ifdef NOT_IRQ
            while(INT_PIN == 0);
            #else
					
            while(irq_flag_io == 0);
            irq_flag_io = 0;
            #endif
            val = read_reg(ComIrqReg);
            if((val & BIT3) && !(val & BIT5)) 
						{
                clear_bit_mask(ComIEnReg, RxIEn);
                if(len_rest + FIFO_SIZE - WATER_LEVEL > 255) {
                    #if (NFC_DEBUG)
                    printf("AF RX_LEN > 255B\r\n");
                    #endif
                    break;
                }
                for(j = 0; j < FIFO_SIZE - WATER_LEVEL; j++) {
                    pi->mf_data[len_rest + j] = read_reg(FIFODataReg);
                }
                write_reg(ComIrqReg, BIT3);
                len_rest += FIFO_SIZE - WATER_LEVEL;
                set_bit_mask(ComIEnReg, RxIEn);
            } else {
                clear_bit_mask(ComIEnReg, BIT3);//disable HoAlertIRq
                break;
            }
        }
        val = read_reg(ComIrqReg);
        #if (NFC_DEBUG)
        printf(" INT:fflvl=%d,rxlst=%02x ,ien=%02x,cirq=%02x\r\n", (uint16_t)read_reg(FIFOLevelReg), read_reg(ControlReg) & 0x07, read_reg(ComIEnReg), val); //XU
        #endif
        write_reg(ComIrqReg, val);
        if(val & BIT0) {
            status = MI_NOTAGERR;
            //printf("error : 0x%x\r\n", read_reg(ErrorReg));
        } 
				else {
            err = read_reg(ErrorReg);
            if(!(err & 0x7f)) {  // if not real errirq, except wrrE
                val &= ~ErrIRq;
            }
            err &= 0x7f;  // clear bit7 wrErr
            status = MI_COM_ERR;
            if((val & wait_for) && (val & irq_en)) {
                if(!(val & ErrIRq)) {
                    status = MI_OK;
                    if(recebyte) 
										{
                        val = 0x7F & read_reg(FIFOLevelReg);
                        last_bits = read_reg(ControlReg) & 0x07;
                        if(len_rest + val > MAX_TRX_BUF_SIZE) {
                            status = MI_COM_ERR;
                            #if (NFC_DEBUG)
                            printf("RX_LEN > 255B\r\n");
                            #endif
                        } 
												else 
												{
                            if(last_bits && val) {
                                pi->mf_length = (val - 1) * 8 + last_bits;
                            } else {
                                pi->mf_length = val * 8;
                            }
                            pi->mf_length += len_rest * 8;
                            #if (NFC_DEBUG)
                            printf(" RX:len=%02x,last_bits=%02x,,BitFramingReg=%02x,dat:", (uint16_t)pi->mf_length, last_bits, read_reg(BitFramingReg));
                            #endif
                            if(val == 0) {
                                val = 1;
                            }
                            for(j = 0; j < val; j++) {
                                pi->mf_data[len_rest + j] = read_reg(FIFODataReg);
                            }
                            #if (NFC_DEBUG)
                            for(j = 0; j < pi->mf_length / 8 + !!(pi->mf_length % 8); j++) {
                                printf("%02X ", (uint16_t)pi->mf_data[j]);
                            }
                            printf("\r\n");
                            #endif
                        }
                    }
                } 
								else if((err & CollErr) && (!(read_reg(CollReg) & BIT5))) {
                    //a bit-collision is detected
                    status = MI_COLLERR;
                    if(recebyte) {
                        val = 0x7F & read_reg(FIFOLevelReg);
                        last_bits = read_reg(ControlReg) & 0x07;
                        if(len_rest + val > MAX_TRX_BUF_SIZE) {
                            #if (NFC_DEBUG)
                            printf("COLL RX_LEN > 255B\r\n");
                            #endif
                        } else {
                            if(last_bits && val) {
                                pi->mf_length = (val - 1) * 8 + last_bits;
                            } else {
                                pi->mf_length = val * 8;
                            }
                            pi->mf_length += len_rest * 8;
                            #if (NFC_DEBUG)
                            printf(" RX: pi_cmd=%02x,last_bits=%02x,BitFramingReg=%02x,pi_len=%02x,pi_dat:", (uint16_t)pi->mf_command, last_bits, read_reg(BitFramingReg), (uint16_t)pi->mf_length);
                            #endif
                            if(val == 0) {
                                val = 1;
                            }
                            for(j = 0; j < val; j++) {
                                pi->mf_data[len_rest + j + 1] = read_reg(FIFODataReg);
                            }
                            #if (NFC_DEBUG)
                            for(j = 0; j < pi->mf_length / 8 + !!(pi->mf_length % 8); j++) {
                                printf("%02X ", (uint16_t)pi->mf_data[j + 1]);
                            }
                            printf("\r\n");
                            #endif
                        }
                    }
                    pi->mf_data[0] = (read_reg(CollReg) & CollPos);
                    if(pi->mf_data[0] == 0) {
                        pi->mf_data[0] = 32;
                    }
                    #if(NFC_DEBUG)
                    printf("\n COLL_DET pos=%02x\r\n", (uint16_t)pi->mf_data[0]);
                    printf("\n collreg=%02x\r\n", read_reg(CollReg));
                    #endif
                    pi->mf_data[0]--;
                } else if((err & CollErr) && (read_reg(CollReg) & BIT5)) {
                    //printf("COLL_DET,but CollPosNotValid=1\n");
                } else if(err & (ProtocolErr)) {
                    #if (NFC_DEBUG)
                    printf("protocol err=%b02x\r\n", err);
                    #endif
                    status = MI_FRAMINGERR;
                } else if((err & (CrcErr | ParityErr)) && !(err & ProtocolErr)) {
                    //EMV  parity err EMV 307.2.3.4
                    val = 0x7F & read_reg(FIFOLevelReg);
                    last_bits = read_reg(ControlReg) & 0x07;
                    if(len_rest + val > MAX_TRX_BUF_SIZE) {
                        status = MI_COM_ERR;
                        #if (NFC_DEBUG)
                        printf("RX_LEN > 255B\r\n");
                        #endif
                    } else {
                        if(last_bits && val) {
                            pi->mf_length = (val - 1) * 8 + last_bits;
                        } else {
                            pi->mf_length = val * 8;
                        }
                        pi->mf_length += len_rest * 8;
                    }
                    #if (NFC_DEBUG)
                    val =  pi->mf_length / 8;
                    for(j = 0; j < val; j++) {
                        pi->mf_data[len_rest + j + 1] = read_reg(FIFODataReg);
                    }
                    for(j = 0; j < pi->mf_length / 8 + !!(pi->mf_length % 8); j++) {
                        printf("%02X ", (uint16_t)pi->mf_data[j + 1]);
                    }
                    printf("\r\n");
                    #endif
                    #if (NFC_DEBUG)
                    printf("crc-parity err=%b02x\r\n", err);
                    printf("l=%d\n", pi->mf_length);
                    #endif
                    status = MI_INTEGRITY_ERR;
                } else {
                    #if (NFC_DEBUG)
                    printf("unknown ErrorReg=%02bx\r\n", err);
                    #endif
                    status = MI_INTEGRITY_ERR;
                }
            } else {
                status = MI_COM_ERR;
                #if (NFC_DEBUG)
                printf(" MI_COM_ERR\r\n");
                #endif
            }
        }
        set_bit_mask(ControlReg, BIT7);// TStopNow =1
        write_reg(ComIrqReg, 0x7F);
        write_reg(DivIrqReg, 0x7F);
        clear_bit_mask(ComIEnReg, 0x7F);
        clear_bit_mask(DivIEnReg, 0x7F);
        write_reg(CommandReg, PCD_IDLE);
    } else {
        status = USER_ERROR;
        #if (NFC_DEBUG)
        printf("USER_ERROR\r\n");
        #endif
    }
    #if (NFC_DEBUG)
    printf(" pcd_com: sta=0x%x,err=0x%x\r\n\r\n", status, err);
    #endif
    return status;
}


/**
 * @brief  The timer of reader.  one etu is 302us
 * @param  fwi: 0~14. The 2 power.
 * @retval None
 */
void pcd_set_tmo(uint8_t fwi) {
    write_reg(TPrescalerReg, (TP_FWT_302us) & 0xFF);
    write_reg(TModeReg, BIT7 | (((TP_FWT_302us) >> 8) & 0xFF));
    write_reg(TReloadRegL, (1 << fwi)  & 0xFF);
    write_reg(TReloadRegH, ((1 << fwi)  & 0xFF00) >> 8);
}

/**
 * @brief  The delay api of timer of reader.  one etu is 302us
 * @param  sfgi: 0~14. The 2 power.
 * @retval None
 */

void pcd_delay_sfgi(uint8_t sfgi) {
    //SFGT = (SFGT+dSFGT) = [(256 x 16/fc) x 2^SFGI] + [384/fc x 2^SFGI]
    //dSFGT =  384 x 2^FWI / fc
    write_reg(TPrescalerReg, (TP_FWT_302us + TP_dFWT) & 0xFF);
    write_reg(TModeReg, BIT7 | (((TP_FWT_302us + TP_dFWT) >> 8) & 0xFF));
    if(sfgi > 14 || sfgi < 1) {
        //FDTA,PCD,MIN = 6078 * 1 / fc
        sfgi = 1;
    }
    write_reg(TReloadRegL, (1 << sfgi) & 0xFF);
    write_reg(TReloadRegH, ((1 << sfgi) >> 8) & 0xFF);
    clear_bit_mask(TModeReg, BIT7); // clear TAuto
    set_bit_mask(ControlReg, BIT6); // set TStartNow
    while(!(read_reg(ComIrqReg)  & TimerIRq));
    write_reg(ComIrqReg, TimerIRq);  //clear tmr
}

/**
 * @brief  The rate between PCD and PICC
 * @param  rate:
 * @retval None
 */
void pcd_set_rate(uint8_t rate) {
    uint8_t val, rxwait;
    switch(rate) {
        case 1:
            clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
            clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
            write_reg(ModWidthReg, 0x26);//Miller Pulse Length
            write_reg(RxSelReg, 0x88);
            break;
        case 2:
            clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(TxModeReg, BIT4);
            clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(RxModeReg, BIT4);
            write_reg(ModWidthReg, 0x12);//Miller Pulse Length
            val = read_reg(RxSelReg);
            rxwait = ((val & 0x3F) * 2);
            if(rxwait > 0x3F) {
                rxwait = 0x3F;
            }
            write_reg(RxSelReg, (rxwait | (val & 0xC0)));
            break;
        case 3:
            clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(TxModeReg, BIT5);
            clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(RxModeReg, BIT5);
            write_reg(ModWidthReg, 0x0A);//Miller Pulse Length
            val = read_reg(RxSelReg);
            rxwait = ((val & 0x3F) * 4);
            if(rxwait > 0x3F) {
                rxwait = 0x3F;
            }
            write_reg(RxSelReg, (rxwait | (val & 0xC0)));
            break;
        case 4:
            clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(TxModeReg, BIT4 | BIT5);
            clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
            set_bit_mask(RxModeReg, BIT4 | BIT5);
            write_reg(ModWidthReg, 0x05);//Miller Pulse Length
            val = read_reg(RxSelReg);
            rxwait = ((val & 0x3F) * 8);
            if(rxwait > 0x3F) {
                rxwait = 0x3F;
            }
            write_reg(RxSelReg, (rxwait | (val & 0xC0)));
            break;
        default:
            clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
            clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
            write_reg(ModWidthReg, 0x26);//Miller Pulse Length
            break;
    }
}

#if 1
void calculate_crc(uint8_t *pin, uint8_t len, uint8_t *pout) {
    uint8_t  i, n;
    set_bit_mask(DivIrqReg, 0x04);
    write_reg(CommandReg, PCD_IDLE);
    set_bit_mask(FIFOLevelReg, 0x80);
    for(i = 0; i < len; i++) {
        write_reg(FIFODataReg, *(pin + i));
    }
    write_reg(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = read_reg(DivIrqReg);
        i--;
    } while((i != 0) && !(n & 0x04));
    #if (NFC_DEBUG)
    printf("crc:i=%02x,n=%02x\n", i, n);
    #endif
    pout[0] = read_reg(CRCResultRegL);
    pout[1] = read_reg(CRCResultRegM);
    clear_bit_mask(DivIrqReg, 0x04);
}
#endif


/* 测试函数：设置AGC寄存器，启动AD转换，获取对应的ADC值 */
void test_setagc_getadc(void)
{
	write_reg(0x3F,0x01);
	write_reg(0x5F,0x82);   //0x00= 关闭AGC功能，0x80 = 开启AGC功能
	write_reg(0x3F,0x00);
}


void pcd_soft_poweroff_init(void)
{
	write_reg(0x3F,0x01);
	write_reg(0x67,0x00);      //电源稳定时间
	write_reg(0x4E,0x00);      //OSC16K频率调高
	write_reg(0x3F,0x02);
	write_reg(0x82,0x00);      //adc采样时间
	
	write_reg(0x5F,0x07);      //关闭AGC功能    由于没有0x3f没有写入0x01，所以该句无效
	write_reg(0x86,0x01);      //打开16K时钟
	write_reg(0x8A,0x00);      //开场稳定时间
	write_reg(0x8B,0xFF);      //配置LPCD寻卡周期为最大，大于2s
	write_reg(0x8D,0x00);      //预启动时间
	write_reg(0x3F,0x00);
	
}

/* 普通查询模式 */
void pcd_init(void)
{
	write_reg(0x01, 0x00);
    delay_1ms(1);
    pcd_reset();
    delay_1ms(1);
    clear_bit_mask(Status2Reg, BIT3);
    write_reg(DivIEnReg, 0x80);   // irqpin  cmos output
    write_reg(WaterLevelReg, 0x10);//  set waterlevel 16byte
    write_reg(RxSelReg, 0x88);//RxWait
    write_reg(CWGsPReg, sl2523_cwgsp_init);  //
    write_reg(RFCfgReg, sl2523_rfcfg_init); //
    write_reg(ControlReg, 0x10);  //default val
    write_reg(0x3f, 0x1);
    write_reg(0x45, sl2523_rfcfg1_init);
    
    write_reg(0x3f, 0x0);
   
}


/**
 * @brief  The init of reader .
 * @param  None
 * @retval None
 */
//#define SPD_MODE
void pcd_init_lpcd(void) {
    #ifdef _ENABLE_LPCD_
    uint8_t agc_val;
    write_reg(0x3f, 0x01);
    agc_val = read_reg(0x5f);
    write_reg(0x3f, 0x00);
    printf("agc_val:0x%x\r\n",    agc_val);
    #endif
    write_reg(0x01, 0x00);
    delay_1ms(1);
    pcd_reset();
    delay_1ms(1);
    clear_bit_mask(Status2Reg, BIT3);
    write_reg(DivIEnReg, 0x80);   // irqpin  cmos output
    write_reg(WaterLevelReg, 0x10);//  set waterlevel 16byte
    write_reg(RxSelReg, 0x88);//RxWait
    write_reg(CWGsPReg, sl2523_cwgsp_init);  //
    write_reg(RFCfgReg, sl2523_rfcfg_init); //
    write_reg(ControlReg, 0x10);  //default val
    write_reg(0x3f, 0x1);
    write_reg(0x45, sl2523_rfcfg1_init);
    set_bit_mask(0x43, 0x80);
    //write_reg(0x46, 0x68);
    SL_WR_REG(0x61, 0x1a);
    //write_reg(0x68, 0x8);
    #ifdef _ENABLE_LPCD_
    write_reg(0x5f, agc_val); // 读卡可以以此值作为参考，并固定该值
    #else
    write_reg(0x5f, 0x84); // 相当于内部串了一个电阻，外部的R1电阻不需要。捷联的Demo板R2电阻=1.8K。注释：客户板子R1电阻去掉，只需调R2电阻
    #endif
    //delay_10us(50);
    //  write_reg(0x5f, 0x84);
    write_reg(0x3f, 0x0);
    #ifdef  SPD_MODE
    write_reg(0x3f, 0x01);
    write_reg(0x67, 0x00); //电源稳定时间
    write_reg(0x4e, 0x00); //OSC16K频率调高
    write_reg(0x5f, 0x07); //关闭AGC功能
    write_reg(0x3f, 0x02);
    write_reg(0x82, 0x00); //打开16K时钟
    write_reg(0x86, 0x01); //打开16K时钟
    write_reg(0x8a, 0x00); //打开16K时钟
    write_reg(0x8b, 0xFF); //配置LPCD寻卡周期为最大，大于2s
    write_reg(0x8d, 0x00); //打开16K时钟
    write_reg(0x3f, 0x00);
    #endif
}



/* 打印LPCD相关寄存器 */
void print_lpcd_reg(void)
{
	unsigned char reg;
	
	/* 延时至稳定 */
	write_reg(0x3F,0x00);
	reg = read_reg(0x01);
	printf("\r\n reg(0x01)=%x \r\n",reg);
	reg = read_reg(0x15);
	printf("\r\n reg(0x15)=%x \r\n",reg);
	reg = read_reg(0x14);
	printf("\r\n reg(0x14)=%x \r\n",reg);
	reg = read_reg(0x28);
	printf("\r\n reg(0x28)=%x \r\n",reg);
	reg = read_reg(0x29);
	printf("\r\n reg(0x29)=%x \r\n",reg);
	write_reg(0x3F,0x01);
	reg = read_reg(0x65);
	printf("\r\n reg(0x65)=%x \r\n",reg);
	reg = read_reg(0x66);
	printf("\r\n reg(0x66)=%x \r\n",reg);
	write_reg(0x3F,0x00);
	reg = read_reg(0x03);
	printf("\r\n reg(0x03)=%x \r\n",reg);
	
	printf("停止程序\r\n");
	while(1);
}


/* 打印所以寄存器 */
void print_all_reg(void)
{
	unsigned char reg,i;
	
	write_reg(0x3F,0x00);
	printf("Page 0\r\n");
	for(i=0x00;i<0x0F;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	printf("Page 1\r\n");
	for(i=0x10;i<0x1F;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	printf("Page 2\r\n");
	for(i=0x20;i<0x2F;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	printf("Page 3\r\n");
	for(i=0x30;i<0x3F;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	
	printf("\r\n SL2823 \r\n");
	write_reg(0x3F,0x01);
	for(i=0x40;i<0x80;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	
	printf("\r\n SL2823 \r\n");
	write_reg(0x3F,0x02);
	for(i=0x80;i<0x9F;i++)
	{
		reg = read_reg(i);
		printf("reg(0x%2x) = %x\r\n",i,reg);
	}
	write_reg(0x3F,0x00);
	printf("停止程序\r\n");
	while(1);
}


