#include "ex_flash.h"
#include "iap_struct.h"
#include "gd25q_driver.h"

static uint8_t ex_flash_init(void);
static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t Len);
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len);
static uint8_t WriteFlash(uint32_t FlashAddr,const uint8_t *wBbuff,uint32_t Len); 
static uint8_t WriteFlashNoCheck(uint32_t FlashAddr, const uint8_t *wBbuff,uint32_t Len);

const _Flash_Func mEx_Flash_Func = 
{
	.init						= ex_flash_init,
  .erase 					= EraseFlash,
  .write					= WriteFlash,
	.write_nocheck 	= WriteFlashNoCheck,
  .read						= ReadFlash,
};

/*==================================================================================
* �� �� ���� ex_flash_init
* ��    ���� 
* ��������:  �ⲿflash�ӿڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2020-03-20 114907
==================================================================================*/
static uint8_t ex_flash_init(void)
{
	//spi�ӿڳ�ʼ�� 
	
	//flash��ʼ��
	
	return 0;
}

/*==================================================================================
* �� �� ���� EarseFlash
* ��    ���� 
* ��������:  ����Flash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-12-02 123204
==================================================================================*/
static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t Len)
{
	//У�����ݺϷ���
  if(Len <= 0)
    return 0x00;
  
		//����������ַ��Ҫ����ɾ���ĳ���
	W25QXX_Erase(FlashAddr, Len);
	
	//��������
 
	
  return 0x01;
}

/*==================================================================================
* �� �� ���� ReadFlash
* ��    ���� 
* ��������:  ��Flash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-12-02 123204
==================================================================================*/
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len)
{
	//У�����ݺϷ���
  if(Len <= 0)
    return 0x00;
  
	//��ȡ����
  W25QXX_Read(rBuff, FlashAddr, Len);

  return 0x01;
}

/*==================================================================================
* �� �� ���� EarseFlash
* ��    ���� 
* ��������:  дFlash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-12-02 123204
==================================================================================*/
static uint8_t WriteFlash(uint32_t FlashAddr, const uint8_t *wBbuff,uint32_t Len)
{
	uint32_t nAddress = FlashAddr;
	 
	//У�����ݺϷ���
  if(Len <= 0)
    return 0x00; 
	 
	//����FLASH
	W25QXX_Write(wBbuff, FlashAddr, Len);
	//д������

	//��סFLASH 
	
  return 0x01; 
}


/*==================================================================================
* �� �� ���� WriteFlashNoCheck
* ��    ���� 
* ��������:  дFlash
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-18 034829
==================================================================================*/
static uint8_t WriteFlashNoCheck(uint32_t FlashAddr, const uint8_t *wBbuff,uint32_t Len)
{
	uint32_t nAddress = FlashAddr;
	 
	//У�����ݺϷ���
  if(Len <= 0)
    return 0x00; 
	 
	//����FLASH
	W25QXX_Write_NoCheck(wBbuff, FlashAddr, Len);
	//д������

	//��סFLASH 
	
  return 0x01; 
}

