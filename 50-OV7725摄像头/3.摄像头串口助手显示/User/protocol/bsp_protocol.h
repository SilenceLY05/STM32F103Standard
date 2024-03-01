#ifndef __BSP_PROTOCOL_H
#define __BSP_PROTOCOL_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#ifdef _cplusplus
extern "C"{
#endif
	
/* ���ݽ��ջ�������С */
#define PROT_FRAME_LEN_RECV          128

/* У�����ݵĳ��� */	
#define PROT_FRAME_LEN_CRC_16        2
	

/* ͼ���ʽ�궨�� */
#define PIC_FORMAT_JPEG              0x01u     //ͼ��Ϊ JPEG ��ʽ
#define PIC_FORMAT_BMP               0x02u     //ͼ��Ϊ BMP ��ʽ
#define PIC_FORMAT_PNG               0x03u     //ͼ��Ϊ PNG ��ʽ
#define PIC_FORMAT_RGB565            0x04u     //ͼ��Ϊ RGB565 ��ʽ
#define PIC_FORMAT_RGB888            0x05u     //ͼ��Ϊ RGB888 ��ʽ
#define PIC_FORMAT_WB                0x06u     //ͼ��Ϊ��ֵ������
	
/* ֡ͷ�궨�� */
#define FRAME_HEADER                 0x59485A53u  //֡ͷ

/* ����궨�� */
#define CMD_ACK                      0x00u     // Ӧ���ָ��
#define CMD_FORMAT                   0x01u     // ������λ��ͼ���ʽ�����ָ��
#define CMD_PIC_DATA                 0x02u     // ����ͼ������ָ��
#define CMD_WRITE_REG                0x10u     // д�Ĵ���ָ��
#define CMD_READ_REG                 0x11u     // ���Ĵ���ָ��
#define CMD_NONE                     0xFFu     // �յ�����

/* �����궨�� */
#define LEN_INDEX_VAL                0x5u      //��԰�ͷ��������ֵ��4�ֽڣ�
#define CMD_INDEX_VAL                0x9u      //��԰�ͷ��������ֵ��1�ֽڣ�

#define COMPOUND_32BIT(data)         (((*(data+0) <<24) & 0xFF000000) | ((*(data+1) <<16) & 0x00FF0000) | ((*(data+2) <<8) & 0x0000FF00) | ((*(data+3) <<0) & 0x000000FF) )


/* �������ݴ��� *data:Ҫ��������ݵ����� data_len:���ݵĴ�С */
void protocol_data_recv(uint8_t *data,uint16_t data_len);

/* ��ѯ֡���ͣ���� *data:֡���� data_len��֡���ݵĴ�С */
uint8_t protocol_frame_parse(uint8_t *data,uint16_t *data_len);

int32_t protocol_init(void);

#ifdef _cplusplus
}

#endif

#endif


