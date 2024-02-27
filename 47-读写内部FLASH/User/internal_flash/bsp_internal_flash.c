#include "bsp_internal_flash.h"
#include "bsp_usart.h"


/* internalFLASH���ڲ�FLASH���ж�д���� */
int internalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00;   //��¼Ҫ��д����ҳ
	uint32_t Address = 0x00;        //��¼д��ĵ�ַ
	uint32_t Data = 0x3210ABCD;     //��¼Ҫд�������
	uint32_t NumberOfPage = 0x00;   //��¼Ҫд���ҳ��

	FLASH_Status FLASHStatus = FLASH_COMPLETE;  //��¼ÿ�β����Ľ��
	TestStatus MemoryProgramStatus = PASSED;    //��¼�������Թ���
	
	/* ���� */
	FLASH_Unlock();
	
	/* ����Ҫ��������ҳ */
	NumberOfPage = ( WRITE_END_ADDR - WRITE_START_ADDR ) / FLASH_PAGE_SIZE;
	
	/* ������б�־λ */
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP );
	
	/* ��ҳ���� */
	for(EraseCounter = 0 ; (EraseCounter < NumberOfPage) && (FLASHStatus == FLASH_COMPLETE) ;EraseCounter++ )
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (EraseCounter * FLASH_PAGE_SIZE));
	}
	
	printf("\r\n �ڲ�FLASHҳ�����ɹ� \r\n");
	
	/* ���ڲ�FLASHд������ */
	Address = WRITE_START_ADDR;
	
	while((Address<WRITE_END_ADDR)&&(FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address,Data);
		Address = Address + 4;
	}
	
	FLASH_Lock();
	
	printf("\r\n �ڲ�FLASHҳд��ɹ� \r\n");
	
	/* ���д��������Ƿ���ȷ */
	Address = WRITE_START_ADDR;
	while((Address < WRITE_END_ADDR)&&(MemoryProgramStatus != FAILED))
	{
		if((*(__IO uint32_t*)Address) != Data)
		{
			MemoryProgramStatus = FAILED;
		}
		Address= Address+4;
	}
	return MemoryProgramStatus;
}

