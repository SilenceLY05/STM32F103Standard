#include "bsp_protocol.h"
#include <string.h>

struct prot_frame_parser_t 
{
	uint8_t *recv_ptr;
	uint16_t r_oft;
	uint16_t w_oft;
	uint16_t frame_len;
	uint16_t found_frame_head;
};

static struct prot_frame_parser_t parser;

static uint8_t recv_buf[PROT_FRAME_LEN_RECV];

/* 计算包的CRC_16 CRC寄存器值由rcr_init给出，这样可以不一次计算所有数据的结果，*data要计算的数据的数组 length数据的大小 rcr_init初始值 */
static uint16_t calc_crc_16(uint8_t *data,uint16_t length,uint16_t rcr_init)
{
	uint16_t crc = rcr_init;
	
	for(int n = 0 ; n < length;n++)
	{
		crc = data[n]^crc;
		for(int i = 0; i <8;i++)
		{
			if(crc&0x01)
			{
				crc = (crc>>1)^0xA001;
			}
			else
			{
				crc = crc>>1;
			}
		}
	}
	return crc;
}

/* 得到帧类型（帧命令） *frame:数据帧 head_oft帧头的位置 */
static uint8_t get_frame_type(uint8_t*frame,uint16_t head_oft)
{
	return (frame[(head_oft+CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

/* 得到帧长度 *buf数据缓冲区 head_oft帧头的位置 */
static uint16_t get_frame_len(uint8_t *frame,uint16_t head_oft)
{
	return((frame[(head_oft+LEN_INDEX_VAL+0) % PROT_FRAME_LEN_RECV] <<0) |
				 (frame[(head_oft+LEN_INDEX_VAL+1) % PROT_FRAME_LEN_RECV] <<8) |
			   (frame[(head_oft+LEN_INDEX_VAL+2) % PROT_FRAME_LEN_RECV] <<16) |
				 (frame[(head_oft+LEN_INDEX_VAL+3) % PROT_FRAME_LEN_RECV] <<24));  //合成帧长度
}

/* 获取crc_16校验值 *frame数据缓冲区 head_oft帧头偏移位置  frame_len帧长*/
static uint16_t get_frame_crc_16(uint8_t*frame,uint16_t head_oft,uint16_t frame_len)
{
	return((frame[(head_oft+frame_len-1) % PROT_FRAME_LEN_RECV] <<6) |
				 (frame[(head_oft+frame_len-2) % PROT_FRAME_LEN_RECV] ));
}

/* 查找帧头 *buf数据缓冲区 ring_buf_len缓冲区大小 start其实位置 len需要查找的长度 -1没有找到帧头，其他值表示帧头的位置 */
static int32_t recvbuf_find_header(uint8_t *buf,uint16_t ring_buf_len,uint16_t start,uint16_t len)
{
	uint16_t i = 0;
	
	for(i=0;i<(len-3);i++)
	{
		if((buf[(start + i +0)& ring_buf_len] <<0) |(buf[(start + i +1)& ring_buf_len] <<8)|
			 (buf[(start + i +2)& ring_buf_len] <<16) |(buf[(start + i +3)& ring_buf_len] <<24) == FRAME_HEADER)
		{
			return ((start+i)%ring_buf_len);
		}
	}
	return -1;
}

/* 计算未解析的数据长度 frame_len帧长 ring_buf_len缓冲区大小 start起始位置 end结束位置 */
static int32_t recvbuf_get_len_to_parse(uint16_t frame_len,uint16_t ring_buf_len,uint16_t start,uint16_t end)
{
	uint16_t unparsed_data_len = 0;
	
	if(start <= end)
		unparsed_data_len = end - start;
	else
		unparsed_data_len = ring_buf_len-start+end;
	
	if(frame_len>unparsed_data_len)
		return 0;
	else
		return unparsed_data_len;
}

/* 接收数据写入缓冲区 *buf数据缓冲区 ring_buf_len缓冲区大小 w_oft写偏移 *data需要写入的数据  data_len需要写入的数据长度*/
static void recvbuf_put_data(uint8_t *buf,uint16_t ring_buf_len ,uint16_t w_oft,uint8_t *data,uint16_t data_len)
{
	if((w_oft+data_len)>ring_buf_len)                              // 超过缓冲区尾
	{
		uint16_t data_len_part = ring_buf_len - w_oft;               // 缓冲区剩余长度
		
		/* 数据分两段写入缓冲区*/
		memcpy(buf+w_oft,data,data_len_part);                        // 写入缓冲区尾
		memcpy(buf,data+data_len_part,data_len-data_len_part);       // 写入缓冲区头
	}
	else
		memcpy(buf+w_oft,data,data_len);     // 数据写入缓冲区
}

/* 接收数据处理 *data需要写入的数据 data_len需要写入的数据长度 */
void protocol_data_recv(uint8_t *data,uint16_t data_len)
{
	recvbuf_put_data(parser.recv_ptr,PROT_FRAME_LEN_RECV,parser.w_oft,data,data_len);  // 接收数据
	parser.w_oft = (parser.w_oft+data_len)%PROT_FRAME_LEN_RECV;                        // 计算写偏移
}

/* 查询帧类型（命令） *data帧数据 data_len帧数据大小 */
uint8_t protocol_frame_parse(uint8_t *data, uint16_t *data_len)
{
	uint8_t frame_type = CMD_NONE;
	uint16_t need_to_parse_len = 0;
	int16_t header_oft = -1;
	uint16_t crc_16 = 0xFFFF;
	
	need_to_parse_len = recvbuf_get_len_to_parse(parser.frame_len,PROT_FRAME_LEN_RECV,parser.r_oft,parser.w_oft);//得到未解析的数据长度
	if(need_to_parse_len < 9)  // 肯定还不能同时找到帧头和帧长度
		return frame_type;
	
	/* 还未找到帧头，需要进行查找 */
	if(0 == parser.found_frame_head)
	{
		/* 同步头为四字节，可能存在未解析的数据中最后一个字节刚好为同步头第一个字节的情况，
           因此查找同步头时，最后一个字节将不解析，也不会被丢弃*/
		header_oft = recvbuf_find_header(parser.recv_ptr,PROT_FRAME_LEN_RECV,parser.r_oft,parser.frame_len);
		if(0<header_oft)
		{
			/* 已找到帧头 */
			parser.found_frame_head = 1;
			parser.r_oft = header_oft;
			
			/* 确认是否可以计算帧长 */
			if(recvbuf_get_len_to_parse(parser.frame_len,PROT_FRAME_LEN_RECV,parser.r_oft,parser.w_oft)<9)
				return frame_type;
		}
		else
		{
			/* 未找到未解析数据的帧头，舍弃数据 */
			parser.r_oft = ((parser.r_oft + need_to_parse_len -3) % PROT_FRAME_LEN_RECV);
			return frame_type;
		}
	}
	
	/* 计算帧长，并确定是否可以进行数据解析*/
	if(0==parser.frame_len)
	{
		parser.frame_len = get_frame_len(parser.recv_ptr,parser.r_oft);
		if(need_to_parse_len<parser.frame_len)
			return frame_type;
	}
	
	/* 帧头位置确定，且未解析的数据超过帧长，可以计算校验和 */
	if((parser.frame_len + parser.r_oft - PROT_FRAME_LEN_CRC_16)> PROT_FRAME_LEN_RECV)
	{
		/* 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头 */
		crc_16 = calc_crc_16(parser.recv_ptr +parser.r_oft,PROT_FRAME_LEN_RECV-parser.r_oft,crc_16);
		crc_16 = calc_crc_16(parser.recv_ptr ,parser.frame_len-PROT_FRAME_LEN_CRC_16+parser.r_oft-PROT_FRAME_LEN_RECV,crc_16);
		
	}
	else
	{
		/* 数据帧可以一次性取完*/
		crc_16 = calc_crc_16(parser.recv_ptr+parser.r_oft,parser.frame_len-PROT_FRAME_LEN_CRC_16,crc_16);
	}
	
	if(crc_16 == get_frame_crc_16(parser.recv_ptr,parser.r_oft,parser.frame_len))
	{
		/* 校验成功，拷贝整帧数据 */
		if((parser.r_oft+parser.frame_len)>PROT_FRAME_LEN_RECV)
		{
			uint16_t data_len_part = PROT_FRAME_LEN_RECV-parser.r_oft;
			
			memcpy(data,parser.recv_ptr+parser.r_oft,data_len_part);                        // 写入缓冲区尾
			memcpy(data+data_len_part,parser.recv_ptr,parser.frame_len-data_len_part);       // 写入缓冲区头
		}
		else
		{
			/* 数据帧可以一次性取完*/
			memcpy(data,parser.recv_ptr+parser.r_oft,parser.frame_len);
		}
		*data_len = parser.frame_len;
		frame_type = get_frame_type(parser.recv_ptr,parser.r_oft);
		
		/* 丢弃缓冲区中的命令帧*/
		parser.r_oft = (parser.r_oft+parser.frame_len)%PROT_FRAME_LEN_RECV;
	}
	else
	{
		/* 校验错误，说明之前找到的帧头只是偶然出现的废数据*/
		parser.r_oft = (parser.r_oft+1)%PROT_FRAME_LEN_RECV;
	}
	parser.frame_len = 0;
	parser.found_frame_head = 0;
	
	return frame_type;
}



/* 初始化接收协议 */
int32_t protocol_init(void)
{
	memset(&parser,0,sizeof(struct prot_frame_parser_t));
	
	/* 初始化分配数据接收和解析缓冲区 */
	parser.recv_ptr = recv_buf;
	
	return 0;
}
