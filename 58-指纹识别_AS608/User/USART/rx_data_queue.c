#include "rx_data_queue.h"


/*********************** ���λ������������ڽ����ⲿ����ʱ�������� *********************/

/* ʵ�����ڵ��������� */
QUEUE_DATA_TYPE   node_data[QUEUE_NODE_NUM];
/* ʵ������������ */
QueueBuffer   rx_queue;

/* ���л��������ڴ�� */
__align(4) unsigned char node_buff[QUEUE_NODE_NUM][QUEUE_NODE_DATA_LEN];


/* ��ʼ��������� cb��������нṹ��  size:������е�Ԫ�ظ��� ��ʼ��ʱ����Ҫ��cb->elemsָ�븳ֵ */
void cbInit(QueueBuffer *cb,int size)
{
	cb->size = size;
	cb->read = 0;
	cb->write = 0;
	//    cb->elems = (uint8_t *)calloc(cb->size, sizeof(uint8_t));  /*elems Ҫ�����ʼ��*/
}

/* �жϻ�������ǣ�1����0������ */
int cbIsFull(QueueBuffer *cb)
{
	return cb->write == (cb->read ^cb->size); //�⽫�ڱȽ�ǰ��ת���������Чλ
}

/* �жϻ�������ǣ�1����0��ȫ�� */
int cbIsEmpty(QueueBuffer *cb)
{
	return cb->write == cb->read;
}

/* �Ի�����е�ָ���1 pҪ��1��ָ�� */
int cbIncr(QueueBuffer *cb,int p)
{
	return (p+1)&(2*cb->size-1);      //��дָ���������2*��СΪģ
}

/* ��ȡ��д��Ļ�����ָ��  */
QUEUE_DATA_TYPE* cbWrite(QueueBuffer *cb)
{
	if(cbIsFull(cb))
	{
		return NULL;
	}
	else
	{
		/*��wriet��write_using���ʱ����ʾ��һ����������д����ϣ���Ҫ��дָ���1*/
		if(cb->write == cb->write_using)
		{
			cb->write_using = cbIncr(cb,cb->write);   //δ����������
		}
	}
	return cb->elems[cb->write_using&(cb->size-1)];
}

/* ����д����ϣ�����дָ�뵽����ṹ�� */
void cbWriteFinish(QueueBuffer *cb)
{
	cb->write = cb->write_using;
}

/* ��ȡ�ɶ�ȡ�Ļ�����ָ��  */
QUEUE_DATA_TYPE* cbRead(QueueBuffer *cb)
{
	if(cbIsEmpty(cb))
	{
		return NULL;
	}
	/*��read��read_using���ʱ����ʾ��һ���������Ѷ�ȡ���(���ѵ���cbReadFinish),
	��Ҫ��дָ���1*/
	if(cb->read == cb->read_using)
	{
		cb->read_using = cbIncr(cb,cb->read);   //δ����������
	}
	
	return cb->elems[cb->read_using&(cb->size-1)];
}

/* ���ݶ�ȡ��ϣ�����дָ�뵽����ṹ�� */
void cbReadFinish(QueueBuffer *cb)
{
	/*���õ�ǰ��������ݽڵ�ĳ���*/
	cb->elems[cb->read_using&(cb->size-1)]->len = 0;
	
	cb->read = cb->read_using;
}

/* ���е�ָ��ָ��Ļ�����ȫ������ */
void camera_queue_free(void)
{
	uint32_t i= 0;
	
	for(i=0;i<QUEUE_NODE_NUM;i++)
	{
		if(node_data[i].head!=NULL)
		{
			/*���Ƕ�̬����Ŀռ��Ҫfree*/
//          free(node_data[i].head);
			node_data[i].head = NULL;
		}
	}
	return;
}

/* ������г�ʼ�� �����ڴ� ʹ�û������ʱ */
void rx_queue_init(void)
{
	uint32_t i = 0;
	memset(node_data,0,sizeof(node_data));
	
	/* ��ʼ��������� */
	cbInit(&rx_queue,QUEUE_NODE_NUM);
	
	for(i= 0;i<QUEUE_NODE_NUM;i++)
	{
		node_data[i].head = node_buff[i];
		
		/* ��ʼ�����л���ָ�� ָ��ʵ�ʵ��ڴ� */
		rx_queue.elems[i] = &node_data[i];
		
		DATA_QUEUE_LOG("node_data[i].head = 0x%x,\r\n rx_queue.elems[i] = 0x%x",(uint32_t)node_data[i].head,(uint32_t)rx_queue.elems[i]->head);
	
		memset(node_data[i].head,0,QUEUE_NODE_DATA_LEN);
	}
	cbPrint(rx_queue);
	
}

/* ��������д�����ݵ����� */
void push_data_to_queue(char *src_data,uint16_t src_len)
{
	QUEUE_DATA_TYPE *data_p;
	uint8_t i;
	
	for(i=0;i<src_len;i++)
	{
		/* ��ȡд������ָ�룬׼��д�������� */
		data_p = cbWrite(&rx_queue);
		
		if(data_p != NULL)  /* ���������δ������ʼ���� */
		{
			/* ��������д������ */
			*(data_p->head+i) = src_data[i];
			data_p->len++;
			printf("\r\n data_p->len = %d \r\n",data_p->len);
		}else return;
		
		cbPrint(rx_queue);
	}
	/*д�뻺�������*/
	cbWriteFinish(&rx_queue);
	cbPrint(rx_queue);
}

/* �Ӷ����ж�ȡ���ݵ����� */
void pull_data_from_queue(void)
{
	QUEUE_DATA_TYPE *rx_data;

	/* ��ȡд������ָ�룬׼��д�������� */
	rx_data = cbRead(&rx_queue);
		
	if(rx_data != NULL)  /* ���������δ������ʼ���� */
	{
		/* ��������д������ */
		*(rx_data->head+rx_data->len) = '\0';
		
		QUEUE_DEBUG("���յ������ݣ�%s",rx_data->head);
		QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);

		/*ʹ�������ݱ������cbReadFinish���¶�ָ��*/
		cbReadFinish(&rx_queue);
	}
		
	
}



