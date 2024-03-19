

///////////////////////////////////////
//nfc_demo
///////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "Rfid_interface.h"
#include "StatusWord.h"

#include "sl2x23.h"


tag_info  g_tag_info;
statistics_t  g_statistics;



static uint8_t sendBuffer[32];
static uint8_t recvBuffer[32];
typedef struct
{
    uint8_t       CLA;
    uint8_t       INS;
    uint8_t       P1;
    uint8_t       P2;
    uint8_t       Lc;
    uint8_t       DataIn[300];
    uint8_t       LeFlag;
    uint8_t       Le;
} APDU_SEND;

typedef struct
{
    uint32_t        LenOut;
    uint8_t       DataOut[300];
    uint8_t       SWA;
    uint8_t       SWB;
} APDU_RESP;


volatile uint8_t sl2523_rfcfg_init;// 0x26
volatile uint8_t sl2523_rfcfg1_init; // 0x45
volatile uint8_t sl2523_cwgsp_init;  //0x28
volatile uint8_t sl2523_modgsp_init; //0x29
volatile uint8_t sl2523_cwgsp_exch;  //0x28
volatile uint8_t sl2523_modgsp_exch; //0x29

volatile uint8_t sl2523_cwgsp_tran;  //0x28
volatile uint8_t sl2523_modgsp_tran; //0x29
volatile uint8_t sl2523_calc_init; // 0x66

volatile uint8_t sl2523_digit0_val = 0xF;;   //0x46
volatile uint8_t sl2523_digit1_val = 0xCC;   //0x49

volatile uint8_t irq_flag_io;



void SL2x23_Para_Init(void)
{
	sl2523_rfcfg_init = 0x54;   //reg0x26  
	sl2523_rfcfg1_init = 0x04;  //reg0x45
	
	sl2523_cwgsp_init = 0x24;  //reg0x28
	sl2523_modgsp_init = 0x07; //reg0x29

}


void SL2x23_Para_Init_LPCD(void)
{
	sl2523_rfcfg_init = 0x54;   //reg0x26  
	sl2523_rfcfg1_init = 0x24;  //reg0x45
	
	sl2523_cwgsp_init = 0x24;  //reg0x28
	sl2523_modgsp_init = 0x07; //reg0x29

}



void gtag_init()
{
    memset(&g_tag_info, 0, sizeof(g_tag_info));
    memset(&g_statistics, 0, sizeof(g_statistics));
}


/* 普通查询模式的初始化配置 */
void Rfid_Init(void)
{
    pcd_init();//初始化pcd寄存器
		pcd_soft_poweroff_init(); //软掉电初始化
    pcd_antenna_off(); // 关闭场强
    gtag_init();//初始化rfid本地变量
//  write_reg(TxSelReg,0x14);
// write_reg(RxSelReg,0x48);
}



/* LPCD模式的初始化配置 */
void Rfid_Init_LPCD(void)
{
    pcd_init_lpcd();//初始化lpcd寄存器
    pcd_antenna_off(); // 关闭场强
    gtag_init();//初始化rfid本地变量
//  write_reg(TxSelReg,0x14);
// write_reg(RxSelReg,0x48);
}


uint8_t g_ask_block_flag;  //变量声明
uint8_t sak;
uint16_t atqa;

/* 用REQA方式询卡 */
int8_t pcd_polling_reqa(uint8_t *guid)
{
    uint8_t serial_num[12];
    int8_t status;
    pcd_default_info();
    delay_1ms(1);
    status =   pcd_request(REQA_CMD, g_tag_info.tag_type_bytes);
    //一次防冲突及选卡
    if(status == MI_OK)
    {
        printf("\r\nrequest successful,ATQA:0x%x, 0x%x \r\n", g_tag_info.tag_type_bytes[0], g_tag_info.tag_type_bytes[1]);
        atqa = (g_tag_info.tag_type_bytes[0] << 8) + g_tag_info.tag_type_bytes[1];
        if(atqa == 0x000c)
        {
            return MI_OK;    //TAG1
        }
        g_tag_info.uid_length = UID_4;
        //make_packet(COM_PKT_CMD_CARD_TYPE, g_tag_info.tag_type_bytes, sizeof(g_tag_info.tag_type_bytes));
        status = pcd_cascaded_anticoll(PICC_ANTICOLL1, 0, &serial_num[0]);
        //printf("UID << %02x %02x %02x %02x: ", g_tag_info.serial_num[0],g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3]);
        if(status == MI_OK)
        {
            //printf("anticoll success:UID: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", g_tag_info.serial_num[0], g_tag_info.serial_num[1], g_tag_info.serial_num[2], g_tag_info.serial_num[3]);
            status = pcd_cascaded_select(PICC_ANTICOLL1, &serial_num[0], &sak);
            if(!status)
            {
                printf("select successful: SAK: 0x%02x\r\n", sak);
                if(sak & 0x20)
                {
                    g_ask_block_flag = 1;    //cpu卡
                }
            }
        }
    }
    //二次防冲突及选卡
    if(status == MI_OK && (sak & BIT2))
    {
        //printf("SAK: 0x%x\r\n", sak);
        g_tag_info.uid_length = UID_7;
        status = pcd_cascaded_anticoll(PICC_ANTICOLL2, 0, &serial_num[4]);
        if(status == MI_OK)
        {
            //printf("anticoll success:UID: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", g_tag_info.serial_num[4], g_tag_info.serial_num[5], g_tag_info.serial_num[6], g_tag_info.serial_num[7]);
            printf("secend anticoll successful\r\n");
            status = pcd_cascaded_select(PICC_ANTICOLL2, &serial_num[4], &sak);
            if(!status)
            {
                printf("secend select successful: SAK: 0x%02x\r\n", sak);
            }
        }
    }
    //回复uid
    //三次防冲突及选卡
    if(status == MI_OK && (sak & BIT2))
    {
        //printf("SAK: 0x%x\r\n",sak);
        g_tag_info.uid_length = UID_10;
        status = pcd_cascaded_anticoll(PICC_ANTICOLL3, 0, &serial_num[8]);
        if(status == MI_OK)
        {
            //printf("3rd anticoll successful");
            status = pcd_cascaded_select(PICC_ANTICOLL3, &serial_num[8], &sak);
            if(!status)
            {
                printf("3rd anticoll successful: SAK: 0x%02x\r\n", sak);
            }
        }
    }
    //回复uid
    if(status == MI_OK)
    {
        uint8_t i;
        guid[10] = g_tag_info.uid_length;
        if(g_tag_info.uid_length == UID_4)
        {
            memcpy(guid, &serial_num[0], 4);
        }
        else if(g_tag_info.uid_length == UID_7)
        {
            memcpy(guid, &serial_num[1], 3);
            memcpy(guid + 3, &serial_num[4], 4);
        }
        else if(g_tag_info.uid_length == UID_10)
        {
            memcpy(guid, &serial_num[1], 3);
            memcpy(guid + 3, &serial_num[5], 3);
            memcpy(guid + 6, &serial_num[8], 4);
        }
        printf("anticoll success:UID:");
        for(i = 0; i < g_tag_info.uid_length; i++)
        {
            printf("0x%02x ", guid[i]);
        }
        printf("\r\n");
    }
    if(status == MI_OK)
    {
    }
    else
    {
        g_statistics.reqa_fail++;
#if(NFC_DEBUG)
        printf("reqa_fail\r\n");
#endif
        delay_1ms(8);
    }
    return status;
}




#if 1
// 用唤醒的方法进行训卡
uint8_t g_ask_block_flag = 0;
uint8_t sak;
uint16_t atqa;
int8_t reqa_wakeup(uint8_t *guid)
{
    uint8_t serial_num[12];
    int8_t status;
    pcd_default_info();
    delay_1ms(1);
    status =   pcd_request(WAKEUPA_CMD, g_tag_info.tag_type_bytes);
    //一次防冲突及选卡
    if(status == MI_OK)
    {
        printf("\r\nrequest successful,ATQA:0x%x, 0x%x \r\n", g_tag_info.tag_type_bytes[0], g_tag_info.tag_type_bytes[1]);
        atqa = (g_tag_info.tag_type_bytes[0] << 8) + g_tag_info.tag_type_bytes[1];
        if(atqa == 0x000c)
        {
            return MI_OK;    //TAG1
        }
        g_tag_info.uid_length = UID_4;
        //make_packet(COM_PKT_CMD_CARD_TYPE, g_tag_info.tag_type_bytes, sizeof(g_tag_info.tag_type_bytes));
        status = pcd_cascaded_anticoll(PICC_ANTICOLL1, 0, &serial_num[0]);
        //printf("UID << %02x %02x %02x %02x: ", g_tag_info.serial_num[0],g_tag_info.serial_num[1],g_tag_info.serial_num[2],g_tag_info.serial_num[3]);
        if(status == MI_OK)
        {
            //printf("anticoll success:UID: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", g_tag_info.serial_num[0], g_tag_info.serial_num[1], g_tag_info.serial_num[2], g_tag_info.serial_num[3]);
            status = pcd_cascaded_select(PICC_ANTICOLL1, &serial_num[0], &sak);
            if(!status)
            {
                printf("select successful: SAK: 0x%02x\r\n", sak);
                if(sak & 0x20)
                {
                    g_ask_block_flag = 1;    //cpu卡
                }
            }
        }
    }
    //二次防冲突及选卡
    if(status == MI_OK && (sak & BIT2))
    {
        //printf("SAK: 0x%x\r\n", sak);
        g_tag_info.uid_length = UID_7;
        status = pcd_cascaded_anticoll(PICC_ANTICOLL2, 0, &serial_num[4]);
        if(status == MI_OK)
        {
            //printf("anticoll success:UID: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", g_tag_info.serial_num[4], g_tag_info.serial_num[5], g_tag_info.serial_num[6], g_tag_info.serial_num[7]);
            printf("secend anticoll successful\r\n");
            status = pcd_cascaded_select(PICC_ANTICOLL2, &serial_num[4], &sak);
            if(!status)
            {
                printf("secend select successful: SAK: 0x%02x\r\n", sak);
            }
        }
    }
    //回复uid
    //三次防冲突及选卡
    if(status == MI_OK && (sak & BIT2))
    {
        //printf("SAK: 0x%x\r\n",sak);
        g_tag_info.uid_length = UID_10;
        status = pcd_cascaded_anticoll(PICC_ANTICOLL3, 0, &serial_num[8]);
        if(status == MI_OK)
        {
            //printf("3rd anticoll successful");
            status = pcd_cascaded_select(PICC_ANTICOLL3, &serial_num[8], &sak);
            if(!status)
            {
                printf("3rd anticoll successful: SAK: 0x%02x\r\n", sak);
            }
        }
    }
    //回复uid
    if(status == MI_OK)
    {
        uint8_t i;
        guid[10] = g_tag_info.uid_length;
        if(g_tag_info.uid_length == UID_4)
        {
            memcpy(guid, &serial_num[0], 4);
        }
        else if(g_tag_info.uid_length == UID_7)
        {
            memcpy(guid, &serial_num[1], 3);
            memcpy(guid + 3, &serial_num[4], 4);
        }
        else if(g_tag_info.uid_length == UID_10)
        {
            memcpy(guid, &serial_num[1], 3);
            memcpy(guid + 3, &serial_num[5], 3);
            memcpy(guid + 6, &serial_num[8], 4);
        }
        printf("anticoll success:UID:");
        for(i = 0; i < g_tag_info.uid_length; i++)
        {
            printf("0x%02x ", guid[i]);
        }
        printf("\r\n");
    }
    if(status == MI_OK)
    {
    }
    else
    {
        g_statistics.reqa_fail++;
#if(NFC_DEBUG)
        printf("reqa_fail\r\n");
#endif
        delay_1ms(8);
    }
    return status;
}

#endif

uint8_t  ATQB[16];
int8_t reqb_wakeup(uint8_t *guid)
{
    int8_t  status;
    uint8_t  i;
    uint8_t  cnt;
    uint8_t  req_code;
//    uint8_t dsi_dri; // param [1]
    pcd_default_info();
    req_code = 8;//pcmd[1];
    cnt = 1;//应用中 可以使用轮询N次
    status = -1;
    while(cnt--)
    {
        status = pcd_request_b(req_code, 0, 0, ATQB);
        if(status == (int8_t)MI_COLLERR)    // 有冲突，超过一张卡
        {
            if((status = pcd_request_b(req_code, 0, 2, ATQB)) != MI_OK)
            {
                for(i = 1; i < 4; i++)
                {
                    if((status = pcd_slot_marker(i, ATQB)) == MI_OK)
                    {
                        break;
                    }
                }
                if(status == MI_OK)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else if(status == MI_OK)
        {
            PRINT_BUF("ATQB:", ATQB, 16);
            PRINT_BUF("UID :", ATQB + 1, 4);
            g_tag_info.uid_length = UID_4;
            memcpy(guid, ATQB + 1, 4);
            PRINT_BUF("Application :", ATQB + 5, 4);
            PRINT_BUF("ProtocolInfo:", ATQB + 9, 3);
            break;
        }
    }
    return status;
}

int8_t pcd_active_b(uint8_t rate)
{
    int8_t  status;
    S_ATTRIB attrib_val;
    switch(rate)
    {
        case 0x1:
            //dsi_dri =0x0;
            attrib_val.param[1] = 0x0;
            break;
        case 0x2:
            //   dsi_dri=0x5;
            attrib_val.param[1] = 0x5;
            break;
        case 0x3:
            attrib_val.param[1] = 0xa;
            break;
        case 0x04:
            attrib_val.param[1] = 0xf;
            break;
        default:
            attrib_val.param[1] = 0x0;
            break;
    }
    status = pcd_attri_b(&ATQB[1], attrib_val.param[1], ATQB[10] & 0x0f, PICC_CID, ATQB);
    if(status == MI_OK)
    {
        ATQB[0] = 0x50;//恢复默认值
        pcd_set_rate(rate);
    }
    else
    {
        if(rate == 1)
        {
            printf("106K,Unsupport\n");
        }
        else if(rate == 2)
        {
            printf("212K,Unsupport\n");
        }
        else if(rate == 3)
        {
            printf("424K,Unsupport\n");
        }
        else if(rate == 4)
        {
            printf("848K,Unsupport\n");
        }
        //g_statistics.reqb_fail++;
#if(NFC_DEBUG)
        printf("pcd_active_b \n");
#endif
    }
    return status;
}



int32_t Rfid_exchange(uint8_t *senddata, uint16_t tx_len, uint8_t *recdata, uint16_t *rx_len)
{
    return(ISO14443_4_HalfDuplexExchange(&gtPcdModuleInfo, senddata, tx_len, recdata, rx_len));
    //return M_ExchangeByteStream( senddata, tx_len,recdata, rx_len );
}

uint16_t iISO14443TransMit(uint8_t* pbInData, uint16_t dwInLen, uint8_t* pbOutData, uint16_t* pwOutLen)
{
    uint16_t wRet;
    if(0 == Rfid_exchange(pbInData, dwInLen, pbOutData, pwOutLen))
    {
#if 0
        printf("iISO14443TransMit recvlen:0x%x  \r\n", *pwOutLen);
        for(i = 0; i < *pwOutLen; i++)
        {
            printf("0x%x ", pbOutData[i]);
        }
        printf("iISO14443TransMit end !!!!!!!\r\n: ");
        printf("out pbOutData = 0x%x\r\n", pbOutData);
#endif
        wRet = *(pbOutData + *pwOutLen - 2) << 8 | *(pbOutData + *pwOutLen - 1); //Convert(pbOutData+*pwOutLen-2);
        *pwOutLen -= 2;
    }
    else
    {
        wRet = SW_TRANS_ERROR;
        *pwOutLen = 0;
    }
    return wRet;
}




//int32_t Rfid_isoexchange(uint8_t cid, APDU_SEND *ApduSend, APDU_RESP *ApduRecv) {
//    int32_t ucRet = MI_OK;
//    uint8_t ucInData[300];
//    uint8_t ucOutData[300];
//    uint16_t  uiSendLen = 0;
//    uint16_t   uiRecLen = 0;
//    if((ApduSend == NULL) || (ApduRecv == NULL) || cid > 14 || ApduSend->Lc > 255) {
//        return MI_ERR_PARAM;
//    }
//    memset(ucInData, 0x00, sizeof(ucInData));
//    memset(ucOutData, 0x00, sizeof(ucOutData));
//    memcpy(ucInData, (uint8_t*) & (ApduSend->CLA), 4);
//    uiSendLen = 4;
//    if((ApduSend->Lc == 0) && (ApduSend->Le == 0)) {
//        ucInData[uiSendLen] = 0x00;
//        uiSendLen++;
//    }
//    if(ApduSend->Lc) {
//        ucInData[uiSendLen] = ApduSend->Lc;
//        uiSendLen++;
//        memcpy(ucInData + uiSendLen, ApduSend->DataIn, ApduSend->Lc);
//        uiSendLen = uiSendLen + ApduSend->Lc;
//    }
//    if(ApduSend->Le) {
//        if(ApduSend->Le == 256) {
//            ucInData[uiSendLen] = 0x00;
//        } else {
//            ucInData[uiSendLen] = ApduSend->Le;
//        }
//        uiSendLen++;
//    }
//    ucRet = Rfid_exchange(ucInData, uiSendLen, ucOutData, &uiRecLen);
//    if(ucRet != MI_OK) {
//        return ucRet;
//    }
//    if(uiRecLen < 2) {
//        ApduRecv->SWA = 0;
//        ApduRecv->SWB = 0;
//        return MI_ERR_TRANSMIT; // RET_RF_ERR_TRANSMIT
//    }
//    ApduRecv->LenOut = uiRecLen - 2;
//    if((ApduSend->Le < ApduRecv->LenOut) && (ApduSend->Le > 0)) {
//        ApduRecv->LenOut = ApduSend->Le;
//    }
//    memcpy(ApduRecv->DataOut, ucOutData, ApduRecv->LenOut);
//    ApduRecv->SWA = ucOutData[uiRecLen - 2];
//    ApduRecv->SWB = ucOutData[uiRecLen - 1];
//    return MI_OK;
//}



void Rfid_Open(void)
{
    //pcd_sleep_exit();
    // pcd_hlta();
    //Soft_Powerdown(0);
    delay_1ms(8);
    pcd_antenna_on();
}


void Rfid_Close(void)
{
    // pcd_hlta();
    pcd_antenna_off();
//    pcd_sleep();
}



//int8_t Detect_ContactlessCard(void) {
//    uint8_t ats[15];
//    int8_t status = 1;
//    if(reqa_wakeup() == MI_OK) {
//        if(pcd_rats_a(0, ats) == MI_OK) {
//            status = 0;
//        }
//    }
//    return status;
//}

void Iblock_getrand(uint8_t rate)
{
    uint16_t testcnd;
    uint16_t okcnt;
    uint16_t cur_rate;
    uint8_t len;
    uint16_t uiRecLen;
    okcnt = 0;
    testcnd = 10;
    if(g_ask_block_flag == 0)
    {
        return;
    }
    while(testcnd)
    {
        testcnd--;
        len = 5;
        sendBuffer[0] = 0x00;
        sendBuffer[1] = 0x84;
        sendBuffer[2] = 0x00;
        sendBuffer[3] = 0x00;
        sendBuffer[4] = 0x04;
        if(Rfid_exchange(sendBuffer, len, recvBuffer, &uiRecLen) == MI_OK)
        {
            okcnt ++;
            //printf("block_exchange successful \r\n");
            continue;
        }
        else
        {
            printf("block_exchange fail %d times \r\n", testcnd);
            break;
        }
    }
    printf("block_exchange successful \r\n");
    if(rate == 1)
    {
        cur_rate = 106;
    }
    else if(rate == 2)
    {
        cur_rate = 212;
    }
    else if(rate == 3)
    {
        cur_rate = 424;
    }
    else if(rate == 4)
    {
        cur_rate = 848;
    }
    printf("##get the picc random, TEST 10 times total in rate: %d K , pass %d times ##\r\n", cur_rate, okcnt);
    printf("## test end##\r\n\r\n");
    iso14443_4_deselect(0);
    delay_1ms(10);
}



/* 轮询A卡 */
int8_t pcd_polling_typeA(uint8_t *guid)
{
	int8_t status;
	status = -1;
	
	pcd_config('A');
	pcd_antenna_on();
	delay_1ms(5);
	status = reqa_wakeup(guid);
	
	if(status == MI_OK)
	{
		printf("reques typeA successful!\r\n");
		delay_1ms(2);
		return status;
	}
	return status;
}



volatile uint8_t card_A;
volatile uint8_t card_B;
#define TPA ('A')
#define TPB ('B')


int8_t pcd_polling(uint8_t *guid) {
    int8_t  status;
    status = -1;
    if(!card_A)   //card_A
    {
        pcd_config('A');
        pcd_antenna_on();
        delay_1ms(5);

        status = reqa_wakeup(guid);
        if(status  == MI_OK)
        {
            card_A = 1;
            printf("reques typeA successful!\r\n");
            delay_1ms(2);
            return status;
        }
    }
    if(!card_B)
    {
        pcd_config('B');
        pcd_antenna_on();
        delay_1ms(5);

        status = reqb_wakeup(guid);
        if(status  == MI_OK)
        {
            card_B = 1;
            printf("reques typeB successful!\r\n");
            delay_1ms(2);
            return status;
        }
    }
   
    return status;
}

int8_t pcd_active(uint8_t rate)
{
    int8_t status = 1;
    uint8_t ats[30];
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;
    if(card_A)   //card_A
    {
        status = pcd_rats_a(0, ats);
        if(status == MI_OK)
        {
            printf("Rats_success:recv len = %d\r\n",    mf_com_data.mf_length / 8);
            printf("TL:0x%x T0:0x%x TA(1):0x%x TB(1):0x%x TC(1):0x%x \r\n", ats[0], ats[1], ats[2], ats[3], ats[4]);
            pcd_set_tmo(4);
            // status =
            status = pcd_pps_rate(pi, ats, 0, rate) ;
            if(status == MI_OK)
            {
                printf("pcd_pps_rate_successful  \r\n");
            }
        }
    }
    else if(card_B)
    {
        status =  pcd_active_b(rate);
        if(status == MI_OK)
        {
#ifdef FPGA
            if(rate == 1)
            {
                P1OUT &= ~BIT7;
            }
            else
            {
                P1OUT |= BIT7;
            }
#endif
            pcd_set_rate(rate);
        }
    } 
    return status;
}



int8_t pcd_transceive(uint8_t *in, uint8_t inlen, uint8_t *out, uint8_t *outlen)
{
    int8_t status = -1;
    transceive_buffer  mf_com_data;
    pcd_set_tmo(4);
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length = inlen;
    memcpy(mf_com_data.mf_data, in, inlen);
    pcd_set_tmo(4);
    status = pcd_com_transceive(&mf_com_data);
    if(status == MI_OK)
    {
        *outlen = mf_com_data.mf_length / 8 + !!(mf_com_data.mf_length % 8);
        memcpy(out, mf_com_data.mf_data, *outlen);
        PRINT_BUF("pcd_transceive >>: ", in, inlen);
        PRINT_BUF("pcd_transceive <<: ", out, *outlen);
    }
    return status;
}



int8_t pcd_read_tag2_block(uint8_t block, uint8_t *out, uint8_t *outlen) //一次读取4块 16字节 = 4 * 4  block = 4byte
{ 
    uint8_t command[] = {0x30, 0x00};
    command[1] = block;
    return pcd_transceive(command, sizeof(command), out, outlen);
}



void NFC_A_Mifare_Test(uint8_t *uid) {
    uint8_t i, block_addr = 30;
    uint8_t g_ret = pcd_auth_state(PICC_AUTHENT1A, block_addr, uid, (void *)"\xff\xff\xff\xff\xff\xff");
    if(g_ret == 0)
    {
        printf("pcd_auth successful\r\n");
    }
    else
    {
        printf("pcd_auth fail\r\n");
        return;
    }
    for(i = 0; i < 16; i++)
    {
        sendBuffer[i] = i;
    }
    g_ret = pcd_write(block_addr, sendBuffer);
    if(g_ret != 0x00)
    {
        printf("write_block  fail erron:0x%x\r\n", g_ret);
    }
    else
    {
        printf("write_block_%d successful:\r\n", block_addr);
        for(i = 0; i < 16; i++)
        {
            printf("0x%02x ", sendBuffer[i]);
        }
        printf("\r\n");
    }
    g_ret = pcd_read(block_addr, recvBuffer);
    if(g_ret != 0x00)
    {
        printf("read_block fail erron:0x%x\r\n", g_ret);
    }
    else
    {
        printf("read_block_%d  successful:\r\n", block_addr);
        for(i = 0; i < 16; i++)
        {
            printf("0x%02x ", recvBuffer[i]);
        }
        printf("\r\n");
    }
}




//uint8_t open_parity;
//void NFC_A_Tag1_get_uid(uint8_t* payload, uint16_t len, uint8_t *output, uint16_t * p_outlen) {
//    int8_t status;
//    uint16_t  i;
//    transceive_buffer  mf_com_data;
//    pcd_set_tmo(11);
//    set_bit_mask(RxModeReg, BIT7); // enable Rx CRC
//    write_reg(ModeReg, 0x7F);   // 11 // CRC seed:6363
//    open_parity = 0;
//    mf_com_data.mf_command = PCD_TRANSMIT;
//    for(i = 0; i < len; i++) {
//        mf_com_data.mf_data[0] = payload[i];
//        mf_com_data.mf_length = 1;
//        if(i == 0)
//            write_reg(BitFramingReg, 0x07); // Tx last bytes = 7
//        else
//            write_reg(BitFramingReg, 0x00);
//        if(i == len - 1) {
//            mf_com_data.mf_command = PCD_TRANSCEIVE;
//            open_parity = 1;
//            //SLM_DBG("now transceive.\r\n");
//        }
//        status = pcd_com_transceive(&mf_com_data);
//    }
//    if(MI_OK ==  status) {
//        *p_outlen = mf_com_data.mf_length / 8 + 1;
//        memcpy(output, mf_com_data.mf_data, *p_outlen);
//        //output[*p_outlen - 1] = NCI_STATUS_OK;
//    } else {
//        *p_outlen = 1;
//        //output[*p_outlen - 1] = NCI_STATUS_MESSAGE_CORRUPTED;
//    }
//}

void NFC_A_Tag1_Test(uint8_t *uid) {
//    uint8_t command1[] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x43};//一次读取4块 16字节 = 4 * 4  block = 4byte
//    uint8_t command2[] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x63, 0x77, 0x00, 0xB8, 0x50};
//    uint8_t pbuf[256];
//    uint8_t len;
//    NFC_A_Tag1_get_uid(uid);
    printf("## tag1 test end##\r\n\r\n");
}
void NFC_A_Tag2_Test(uint8_t *uid)
{
    uint8_t pbuf[256];
    uint8_t len;
    pcd_read_tag2_block(0, pbuf, &len);
    pcd_read_tag2_block(4, pbuf, &len);
    printf("## tag2 test end##\r\n\r\n");
}
