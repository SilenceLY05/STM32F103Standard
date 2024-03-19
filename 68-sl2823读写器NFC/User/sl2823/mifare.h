#ifndef MIFARE_H
#define MIFARE_H
#include "sl2x23.h"


#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_RESET            0xE0               //��λ


//UltraLight
#define PICC_WRITE_ULTRALIGHT 0xA2              //���Ῠд��


int8_t pcd_auth_state(uint8_t auth_mode,uint8_t block,uint8_t *psnr, uint8_t *pkey);
int8_t pcd_read(uint8_t addr,uint8_t *preaddata);
int8_t pcd_write(uint8_t addr,uint8_t *pwritedata);
int8_t pcd_write_ultralight(uint8_t addr,uint8_t *pwritedata);
char pcd_value(unsigned char dd_mode,unsigned char addr,unsigned char *pValue);
char pcd_bak_value(unsigned char sourceaddr, unsigned char goaladdr);
char InitBlock(unsigned char addr,unsigned char *value);
#endif

