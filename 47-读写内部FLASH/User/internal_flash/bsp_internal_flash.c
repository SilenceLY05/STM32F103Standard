#include "bsp_internal_flash.h"
#include "bsp_usart.h"


/* internalFLASH对内部FLASH进行读写测试 */
int internalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00;   //记录要擦写多少页
	uint32_t Address = 0x00;        //记录写入的地址
	uint32_t Data = 0x3210ABCD;     //记录要写入的数据
	uint32_t NumberOfPage = 0x00;   //记录要写入的页数

	FLASH_Status FLASHStatus = FLASH_COMPLETE;  //记录每次擦除的结果
	TestStatus MemoryProgramStatus = PASSED;    //记录整个测试过程
	
	/* 解锁 */
	FLASH_Unlock();
	
	/* 计算要擦除多少页 */
	NumberOfPage = ( WRITE_END_ADDR - WRITE_START_ADDR ) / FLASH_PAGE_SIZE;
	
	/* 清空所有标志位 */
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP );
	
	/* 按页擦除 */
	for(EraseCounter = 0 ; (EraseCounter < NumberOfPage) && (FLASHStatus == FLASH_COMPLETE) ;EraseCounter++ )
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (EraseCounter * FLASH_PAGE_SIZE));
	}
	
	printf("\r\n 内部FLASH页擦除成功 \r\n");
	
	/* 向内部FLASH写入数据 */
	Address = WRITE_START_ADDR;
	
	while((Address<WRITE_END_ADDR)&&(FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address,Data);
		Address = Address + 4;
	}
	
	FLASH_Lock();
	
	printf("\r\n 内部FLASH页写入成功 \r\n");
	
	/* 检查写入的数据是否正确 */
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

