#ifndef __BSP_PROTOCOL_H
#define __BSP_PROTOCOL_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#ifdef _cplusplus
extern "C"{
#endif
	
/* 数据接收缓冲区大小 */
#define PROT_FRAME_LEN_RECV          128

/* 校验数据的长度 */	
#define PROT_FRAME_LEN_CRC_16        2
	

/* 图像格式宏定义 */
#define PIC_FORMAT_JPEG              0x01u     //图像为 JPEG 格式
#define PIC_FORMAT_BMP               0x02u     //图像为 BMP 格式
#define PIC_FORMAT_PNG               0x03u     //图像为 PNG 格式
#define PIC_FORMAT_RGB565            0x04u     //图像为 RGB565 格式
#define PIC_FORMAT_RGB888            0x05u     //图像为 RGB888 格式
#define PIC_FORMAT_WB                0x06u     //图像为二值化数据
	
/* 帧头宏定义 */
#define FRAME_HEADER                 0x59485A53u  //帧头

/* 命令宏定义 */
#define CMD_ACK                      0x00u     // 应答包指令
#define CMD_FORMAT                   0x01u     // 设置上位机图像格式、宽高指令
#define CMD_PIC_DATA                 0x02u     // 发送图像数据指令
#define CMD_WRITE_REG                0x10u     // 写寄存器指令
#define CMD_READ_REG                 0x11u     // 读寄存器指令
#define CMD_NONE                     0xFFu     // 空的类型

/* 索引宏定义 */
#define LEN_INDEX_VAL                0x5u      //相对包头包长索引值（4字节）
#define CMD_INDEX_VAL                0x9u      //相对包头命令索引值（1字节）

#define COMPOUND_32BIT(data)         (((*(data+0) <<24) & 0xFF000000) | ((*(data+1) <<16) & 0x00FF0000) | ((*(data+2) <<8) & 0x0000FF00) | ((*(data+3) <<0) & 0x000000FF) )


/* 接收数据处理 *data:要计算的数据的数组 data_len:数据的大小 */
void protocol_data_recv(uint8_t *data,uint16_t data_len);

/* 查询帧类型（命令） *data:帧数据 data_len：帧数据的大小 */
uint8_t protocol_frame_parse(uint8_t *data,uint16_t *data_len);

int32_t protocol_init(void);

#ifdef _cplusplus
}

#endif

#endif


