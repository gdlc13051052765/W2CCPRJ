#include "includes.h"
#include "st_flash.h" 

static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t Len);
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len);
static uint8_t WriteFlash(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite); 
static uint8_t WriteNotCheck(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite);

static uint8_t FlashBuffer[STM32FLASH_PAGE_SIZE] = {0};


const _Flash_Func stFlash_Func = 
{
  .erase 					= EraseFlash,
	.write_nocheck	= WriteNotCheck,
  .write					= WriteFlash,
  .read						= ReadFlash, 
};
 
  
/*==================================================================================
* �� �� ���� EarseFlash
* ��    ���� 
* ��������:  ����Flash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-04 044124
==================================================================================*/
static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t length)
{ 
	uint32_t page_count = (uint32_t)(length/STM32F105RB_PAGE_SIZE);	//���������ҳ����
	 
  if(length <= 0)
    return 0x00;
  
		//1������FLASH
  HAL_FLASH_Unlock();
	
	//1����մ����־
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	//2������FLASH
	//��ʼ��FLASH_EraseInitTypeDef
	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = FlashAddr;
	f.NbPages = 1;
	//����PageError
	uint32_t PageError = 0;
	//���ò�������
	HAL_FLASHEx_Erase(&f, &PageError);
 
	//4����סFLASH
  HAL_FLASH_Lock();
	  
  return 0x01;
}

/*==================================================================================
* �� �� ���� ReadFlash
* ��    ���� 
* ��������:  ��Flash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-04 044124
==================================================================================*/
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len)
{
  if(Len <= 0)
    return 0x00;
  
		//1����մ����־
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
  while (Len--)
    *rBuff++ = *(uint8_t *)FlashAddr++;     

  return 0x01;
}

/*==================================================================================
* �� �� ���� WriteNotCheck
* ��    ���� 
* ��������:  дFlash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-04 044124
==================================================================================*/
static uint8_t WriteNotCheck(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite)
{
	uint32_t nwrite = NumToWrite/2;
	uint32_t addrmax = STM32FLASH_END;

			//1����մ����־
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	while (nwrite)
	{
		if (Address > addrmax)
			return 0x81; //ʧ��

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, *((uint16_t*)Buffer));
		if ( (*(__IO uint16_t *)Address) != *((uint16_t*)Buffer) )
			return 0x82; //ʧ��

		nwrite--;
		Buffer += 2;
		Address += 2;
	}
	return 0x01; //�ɹ�
}

/*==================================================================================
* �� �� ���� WriteNotCheck
* ��    ���� 
* ��������:  дFlash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-04 044124
==================================================================================*/
static uint8_t WriteFlash(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite)
{
	uint32_t i = 0;
	uint8_t ret = 0;
	uint32_t pagepos = 0;         // ҳλ��
	uint32_t pageoff = 0;         // ҳ��ƫ�Ƶ�ַ
	uint32_t pagefre = 0;         // ҳ�ڿ���ռ�
	uint32_t offset = 0;          // Address��FLASH�е�ƫ��
	uint32_t nwrite = NumToWrite; // ��¼ʣ��Ҫд���������

			//1����մ����־
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	/* �Ƿ���ַ */
	if (Address < STM32FLASH_BASE || Address > (STM32FLASH_END) || NumToWrite == 0 || Buffer == NULL)
		return 0;

	/* ����FLASH */
	HAL_FLASH_Unlock();

	/* ����ƫ�Ƶ�ַ */
	offset = Address - STM32FLASH_BASE;

	/* ���㵱ǰҳλ�� */
	pagepos = offset / STM32FLASH_PAGE_SIZE;

	/* ����Ҫд���ݵ���ʼ��ַ�ڵ�ǰҳ�ڵ�ƫ�Ƶ�ַ */
	pageoff = (offset % STM32FLASH_PAGE_SIZE);

	/* ���㵱ǰҳ�ڿ���ռ� */
	pagefre = ((STM32FLASH_PAGE_SIZE) - pageoff);

	/* Ҫд������������ڵ�ǰҳ������ */
	if (nwrite <= pagefre)
		pagefre = nwrite;

	while (nwrite != 0)
	{
		/* ����Ƿ�ҳ */
		if (pagepos >= STM32FLASH_PAGE_NUM)
			break;

		/* ��ȡһҳ */
		ReadFlash(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE >> 1);

		/* ����Ƿ���Ҫ���� */
		for (i = 0; i < pagefre; i++)
		{
			if (*(FlashBuffer + pageoff + i) != 0xFF) /* FLASH������Ĭ������ȫΪ0xFF */
				break;
		}

		if (i < pagefre)
		{
			uint8_t count = 0;
			uint32_t index = 0;
			uint32_t PageError = 0;
			FLASH_EraseInitTypeDef pEraseInit;

			/* ����һҳ */
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.PageAddress = STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.NbPages = 1;
			if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK)
				break;

			/* ���Ƶ����� */
			for (index = 0; index < pagefre; index++)
			{
				*(FlashBuffer + pageoff + index) = *(Buffer + index);
			}

			/* д��FLASH */
			ret = WriteNotCheck(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE);
			if (ret == (0x01))
			{
				nwrite--;
				break;
			}
		}
		else
		{
			/* ���������ֱ��д */
			ret = WriteNotCheck(Address, Buffer, pagefre);
			if (ret != 0x01)
			{
				nwrite--;
				break;
			}
		}

		Buffer += pagefre;         /* ��ȡ��ַ����         */
		Address += (pagefre << 1); /* д���ַ����         */
		nwrite -= pagefre;         /* ����ʣ��δд�������� */

		pagepos++;   /* ��һҳ               */
		pageoff = 0; /* ҳ��ƫ�Ƶ�ַ����     */

		/* ����ʣ���������´�д�������� */
		pagefre = nwrite >= (STM32FLASH_PAGE_SIZE >> 1) ? (STM32FLASH_PAGE_SIZE >> 1) : nwrite;
	}

	/* ����FLASH */
	HAL_FLASH_Lock();

	return ((NumToWrite - nwrite) << 1);
}
