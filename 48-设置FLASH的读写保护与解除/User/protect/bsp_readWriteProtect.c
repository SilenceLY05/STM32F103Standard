#include "bsp_readWriteProtect.h"
#include "bsp_usart.h"


/* 反转写保护的配置，用于演示芯片处于写保护状态则解除，若不是写保护状态则设置成写保护 */
void WriteProtect_Toggle(void)
{
	/* 若读取写保护寄存器的值进行判断，寄存器位为0表示有保护，1表示无保护 */
	/* 若不等于0xFFFFFFFF,则说明有部分页被写保护了 */
	if(FLASH_GetWriteProtectionOptionByte()!=0xFFFFFFFF)
	{
		FLASH_DEBUG("芯片处于写保护状态，即将执行解保护过程...");
		
		/* 解除对FLASH_CR寄存器的访问限制 */
		FLASH_Unlock();
		
		/* 擦除所有选项字节的内容 */
		FLASH_EraseOptionBytes();
		
		/* 对所有页解除 */
		FLASH_EnableWriteProtection(0x00000000);
		FLASH_DEBUG("配置完成，芯片将自动复位加载新配置，复位后芯片会解除写保护状态\r\n");
		
		/* 芯片复位，以使选项字节生效 */
		NVIC_SystemReset();
			
	}
	else
	{
		FLASH_DEBUG("芯片处于无保护状态，即将执行写保护过程...");
		
		/* 解除对FLASH_CR寄存器的访问限制 */
		FLASH_Unlock();
		
		/* 先擦除所有选项字节的内容，防止因为原有的写保护导致无法写入新的保护配置 */
		FLASH_EraseOptionBytes();
		
		/* 对所有页进行写保护 */
		FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);
		FLASH_DEBUG("配置完成，芯片将自动复位加载新配置，复位后芯片会处于写保护状态\r\n");
		
		/* 芯片复位，以使选项字节生效 */
	}
}

/* 反转读保护的配置，用于演示芯片处于读保护状态则解除，若不是读保护状态则设置成写保护 */
void ReadProtect_Toggle(void)
{
	if(FLASH_GetReadOutProtectionStatus() == SET)
	{
		FLASH_DEBUG("芯片处于读保护状态\r\n");
		
		/* 解除对FLASH_CR寄存器的访问限制 */
		FLASH_Unlock();
		
		FLASH_DEBUG("即将解除读保护，解除读保护会把FLASH的所有内容情况");
		FLASH_DEBUG("由于解除读保护后所以程序会被情况，后面不再会有提示");
		FLASH_DEBUG("等待20秒后即可给芯片下载新的程序。。。\r\n");
		
		FLASH_ReadOutProtection(DISABLE);
		
		/* 即使在此处加入printf串口调试也不会执行的，因为存储程序的整片FLASH都已被擦除 */
		FLASH_DEBUG("由于FLASH程序被清空，所以此行不会执行，，串口不会有本语句输出（SRAM调试模式下例外）\r\n");
			
	}
	else
	{
		FLASH_DEBUG("芯片处于无读保护状态，即将执行读保护过程...");
		
		/* 解除对FLASH_CR寄存器的访问限制 */
		FLASH_Unlock();
		
		FLASH_ReadOutProtection(ENABLE);
		
		printf("芯片已被设置为读保护，上电复位后生效（必须重新给开发板上电，只按复位键无效）\r\n");
		printf("处于保护状态下无法正常下载新程序，必须要解除保护状态在下载");
		
		
	}
}

