#include "includes.h"
#include "spi_class.h"


_Context mSpi_Context[] = 
{
	{
	  .spi = (void*)SPI1,
	  .cs_gpio = CS_FLASH_GPIO_Port,
	  .cs_pin  = CS_FLASH_Pin,
	},

	{
	  .spi = (void*)SPI3,
	  .cs_gpio = SPI3_NSS1_GPIO_Port,
	  .cs_pin  = SPI3_NSS1_Pin,
	},
	{
	  .spi = (void*)SPI3,
	  .cs_gpio = SPI3_NSS2_GPIO_Port,
	  .cs_pin  = SPI3_NSS2_Pin,
	}
};

static uint8_t cs_gpio_set(void* context, uint8_t level);
static uint8_t sed_rev_byte(void* context, uint8_t value);
static uint8_t sed_rev_buff(void* context, uint8_t *s_buff, uint8_t *r_buff, uint16_t len);

_Spi_Class mSpi_Class[] = 
{
	{
	  .pContext   = &mSpi_Context[0],
	  .cs_set	  	= cs_gpio_set,
	  .sed_rev_byte = sed_rev_byte,
	  .sed_rev_buf  = sed_rev_buff,
	},
	{
	  .pContext   = &mSpi_Context[1],
	  .cs_set	  	= cs_gpio_set,
	  .sed_rev_byte = sed_rev_byte,
	  .sed_rev_buf  = sed_rev_buff,
	},
	{
	  .pContext   = &mSpi_Context[2],
	  .cs_set	  	= cs_gpio_set,
	  .sed_rev_byte = sed_rev_byte,
	  .sed_rev_buf  = sed_rev_buff,
	}
};

/*==================================================================================
* �� �� ���� spi1_sed_rev
* ��    ���� None
* ��������:  SPI1�ӿڷ�������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-05 143321
==================================================================================*/
static uint8_t sed_rev_byte(void* context, uint8_t value)
{
	_pContext pthis = (_pContext)context;
	
	uint8_t ret = 0; 
	while((((pSPI_TYPE)pthis->spi)->SR & SPI_FLAG_TXE) == RESET);	//�ȴ����ͻ�������

	((pSPI_TYPE)pthis->spi)->DR = value; 	//����һ���ֽ�  

	while((((pSPI_TYPE)pthis->spi)->SR & SPI_FLAG_RXNE) == RESET);	 //�ȴ��Ƿ���յ�һ���ֽ�  
	
	ret = ((((pSPI_TYPE)pthis->spi)->DR)); 													//��ø��ֽ�

	return ret; //�����յ����ֽ�
}

/*==================================================================================
* �� �� ���� sed_rev_buff
* ��    ���� None
* ��������:  SPI��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-05 190852
==================================================================================*/
static uint8_t sed_rev_buff(void* context, uint8_t *s_buff, uint8_t *r_buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		r_buff[i] = sed_rev_byte(context, s_buff[i]);
	}
	
	return 0x01;
}

/*==================================================================================
* �� �� ���� cs_gpio_set
* ��    ���� None
* ��������:  ����cs���ŵ�ƽ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-05 190852
==================================================================================*/
static uint8_t cs_gpio_set(void* context, uint8_t level)
{
	_pContext pthis = (_pContext)context;

	HAL_GPIO_WritePin(((pGPIO_TYPE)pthis->cs_gpio), pthis->cs_pin, (GPIO_PinState)level);

	return 0x01;
}
