#ifndef __RFID_INTERFACE_H__
#define __RFID_INTERFACE_H__

#include "sl2x23.h"
#include "iso14443a.h"
#include "iso14443b.h"
#include "iso14443_4.h"
#include "mifare.h"

//#include "sys.h"
#include "stm32f10x.h"


#if 1
#define PRINT_BUF(x,y,z){\
					int16_t loop;printf(x);\
					for(loop=0;loop<(z);loop++)\
					printf("%02x",(uint8_t)((int8_t*)y)[loop]);\
					printf("\r\n");}
#else
#define PRINT_BUF(x,y,z) do{} while(0);
#endif



#define REQA_CMD      0x26
#define WAKEUPA_CMD   0x52

#define REQB_CMD      0x00
#define WAKEUPB_CMD   0x08
//Type A
#define COM_PKT_CMD_INIT_TYPEA                  0x1E
#define COM_PKT_CMD_INIT_TYPEB                  0x1F
#define COM_PKT_CMD_REQA                        0x20
#define COM_PKT_CMD_TYPEA_HALT                  0x21
#define COM_PKT_CMD_TYPEA_MF1_READ              0x22
#define COM_PKT_CMD_TYPEA_MF1_WRITE             0x23
#define COM_PKT_CMD_TYPEA_MF1_WALLET_INIT       0x24
#define COM_PKT_CMD_TYPEA_MF1_WALLET_INCREMENT  0x25
#define COM_PKT_CMD_TYPEA_MF1_WALLET_DECREMENT  0x26
#define COM_PKT_CMD_TYPEA_MF1_WALLET_READ       0x27
#define COM_PKT_CMD_TYPEA_MF0_READ              0x28
#define COM_PKT_CMD_TYPEA_MF0_WRITE             0x29
#define COM_PKT_CMD_TYPEA_RATS                  0x2A
#define COM_PKT_CMD_EXCHANGE                    0x2B
#define COM_PKT_CMD_DESELECT                    0x2C
#define COM_PKT_CMD_MULTI_EXCHANGE_TEST         0x2D
#define COM_PKT_CMD_TEST_STOP                   0x2E
//TYPE B
#define COM_PKT_CMD_REQB                        0x30
#define COM_PKT_CMD_TYPEB_HALT                  0x31
#define COM_PKT_CMD_TYPEB_SR176_READ            0x32
#define COM_PKT_CMD_TYPEB_SR176_WRITE           0x33
#define COM_PKT_CMD_SELECT_SR                   0x34
#define COM_PKT_CMD_PROTECT_SR176               0x35
#define COM_PKT_CMD_COMPLECT_SR176              0x36
#define COM_PKT_CMD_TYPEB_UID                   0x37

#define UID_4  4
#define UID_7  7
#define UID_10 10

#define PICC_CID 0x00 // 0~14 随意指定








typedef struct tag_info
{
    uint8_t opt_step;
    uint8_t uid_length;
    uint8_t tag_type;
    uint8_t tag_type_bytes[2];
    uint8_t serial_num[12];
    uint8_t uncoded_key[6];
} tag_info;

typedef struct statistics_t
{
    uint32_t reqa_cnt;
    uint32_t reqa_fail;
    uint32_t reqb_cnt;
    uint32_t reqb_fail;
    uint32_t write_fail;
    uint32_t read_fail;
    uint32_t check_fail;
    uint32_t atqa_fail;
    uint32_t cos_fail;
    uint32_t atqa_cnt;
    uint32_t cos_cnt;
    uint32_t lpcd_cnt;
    uint32_t lpcd_fail;
} statistics_t;

extern statistics_t  g_statistics;
extern tag_info  g_tag_info;
extern uint8_t g_ask_block_flag;


extern volatile uint8_t sl2523_rfcfg_init;// 0x26
extern volatile uint8_t sl2523_rfcfg1_init; // 0x45
extern volatile uint8_t sl2523_cwgsp_init;  //0x28
extern volatile uint8_t sl2523_modgsp_init; //0x29
extern volatile uint8_t sl2523_cwgsp_exch;  //0x28
extern volatile uint8_t sl2523_modgsp_exch; //0x29
extern volatile uint8_t sl2523_cwgsp_tran;  //0x28
extern volatile uint8_t sl2523_modgsp_tran; //0x29
extern volatile uint8_t sl2523_digit0_val;  
extern volatile uint8_t sl2523_digit1_val;
extern volatile uint8_t sl2523_calc_init;
extern volatile uint8_t irq_flag_io;
extern volatile uint8_t sl2523_emv_mode;





void SL2x23_Para_Init(void);
void SL2x23_Para_Init_LPCD(void);
void Rfid_Init(void);
void Rfid_Init_LPCD(void);  //LPCD

void rfid_operation(uint8_t *pcmd);
int8_t com_reqa(uint8_t *pcmd);
int8_t com_reqb(uint8_t *pcmd);

void com_exchange(uint8_t * pcmd);
void com_typea_rats(uint8_t * pcmd);
void Rfid_Close(void);
int8_t nfc_demo(void);

int8_t reqb_wakeup(uint8_t *guid);
//int8_t Detect_ContactlessCard(void);
uint16_t iISO14443TransMit(uint8_t* pbInData, uint16_t dwInLen, uint8_t* pbOutData, uint16_t* pwOutLen);

int8_t pcd_polling_reqa(uint8_t *guid); /* 用REQA方式询卡 */
int8_t pcd_polling_typeA(uint8_t *guid); /* 仅轮询A卡 */
int8_t pcd_polling(uint8_t *guid);
int8_t pcd_active(uint8_t rate);
void Iblock_getrand(uint8_t rate);

int8_t pcd_read_tag2_block(uint8_t block, uint8_t *out, uint8_t *outlen);

void NFC_A_Mifare_Test(uint8_t *uid);
void NFC_A_Tag1_Test(uint8_t *uid);
void NFC_A_Tag2_Test(uint8_t *uid);


#endif  /* __RFID_INTERFACE_H__ */


