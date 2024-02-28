#include "bsp_readWriteProtect.h"
#include "bsp_usart.h"


/* ��תд���������ã�������ʾоƬ����д����״̬������������д����״̬�����ó�д���� */
void WriteProtect_Toggle(void)
{
	/* ����ȡд�����Ĵ�����ֵ�����жϣ��Ĵ���λΪ0��ʾ�б�����1��ʾ�ޱ��� */
	/* ��������0xFFFFFFFF,��˵���в���ҳ��д������ */
	if(FLASH_GetWriteProtectionOptionByte()!=0xFFFFFFFF)
	{
		FLASH_DEBUG("оƬ����д����״̬������ִ�нⱣ������...");
		
		/* �����FLASH_CR�Ĵ����ķ������� */
		FLASH_Unlock();
		
		/* ��������ѡ���ֽڵ����� */
		FLASH_EraseOptionBytes();
		
		/* ������ҳ��� */
		FLASH_EnableWriteProtection(0x00000000);
		FLASH_DEBUG("������ɣ�оƬ���Զ���λ���������ã���λ��оƬ����д����״̬\r\n");
		
		/* оƬ��λ����ʹѡ���ֽ���Ч */
		NVIC_SystemReset();
			
	}
	else
	{
		FLASH_DEBUG("оƬ�����ޱ���״̬������ִ��д��������...");
		
		/* �����FLASH_CR�Ĵ����ķ������� */
		FLASH_Unlock();
		
		/* �Ȳ�������ѡ���ֽڵ����ݣ���ֹ��Ϊԭ�е�д���������޷�д���µı������� */
		FLASH_EraseOptionBytes();
		
		/* ������ҳ����д���� */
		FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);
		FLASH_DEBUG("������ɣ�оƬ���Զ���λ���������ã���λ��оƬ�ᴦ��д����״̬\r\n");
		
		/* оƬ��λ����ʹѡ���ֽ���Ч */
	}
}

/* ��ת�����������ã�������ʾоƬ���ڶ�����״̬�����������Ƕ�����״̬�����ó�д���� */
void ReadProtect_Toggle(void)
{
	if(FLASH_GetReadOutProtectionStatus() == SET)
	{
		FLASH_DEBUG("оƬ���ڶ�����״̬\r\n");
		
		/* �����FLASH_CR�Ĵ����ķ������� */
		FLASH_Unlock();
		
		FLASH_DEBUG("���������������������������FLASH�������������");
		FLASH_DEBUG("���ڽ�������������Գ���ᱻ��������治�ٻ�����ʾ");
		FLASH_DEBUG("�ȴ�20��󼴿ɸ�оƬ�����µĳ��򡣡���\r\n");
		
		FLASH_ReadOutProtection(DISABLE);
		
		/* ��ʹ�ڴ˴�����printf���ڵ���Ҳ����ִ�еģ���Ϊ�洢�������ƬFLASH���ѱ����� */
		FLASH_DEBUG("����FLASH������գ����Դ��в���ִ�У������ڲ����б���������SRAM����ģʽ�����⣩\r\n");
			
	}
	else
	{
		FLASH_DEBUG("оƬ�����޶�����״̬������ִ�ж���������...");
		
		/* �����FLASH_CR�Ĵ����ķ������� */
		FLASH_Unlock();
		
		FLASH_ReadOutProtection(ENABLE);
		
		printf("оƬ�ѱ�����Ϊ���������ϵ縴λ����Ч���������¸��������ϵ磬ֻ����λ����Ч��\r\n");
		printf("���ڱ���״̬���޷����������³��򣬱���Ҫ�������״̬������");
		
		
	}
}

