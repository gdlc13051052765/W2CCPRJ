#include "serial.h"
#include "main.h"


static uint8_t rev_buff[1028] = {0};	//���ջ���
static uint8_t sed_buff[256] = {0};		//���ͻ���

/*==================================================================================
* �� �� ���� serial_init
* ��    ���� None
* ��������:  ���ڳ�ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_register_init(void *instance, void* pdev)
{ 
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	pthis->p_ffunc = pSerial_Fifo_Func;
	pthis->pDev = pdev;
	pthis->p_rev_fifo = pthis->p_ffunc->init_m(rev_buff, sizeof(rev_buff));
	pthis->p_sed_fifo = pthis->p_ffunc->init_m(sed_buff, sizeof(sed_buff)); 
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_sed_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	USART_TypeDef * serial = 	(USART_TypeDef *) pthis->pDev;
	 
	serial->DR = value;
	while((serial->SR & UART_IT_TXE) == RESET);
//		USART1->DR = value;
//	while((USART1->SR & UART_IT_TXE) == RESET);
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
void serial_sed_buff(void *instance, uint8_t *buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		serial_sed_byte(instance, buff[i]);
	}
}

/*==================================================================================
* �� �� ���� serial_sed_buff
* ��    ���� None
* ��������:  ���ڷ�������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 162343
==================================================================================*/ 
uint8_t serial_push_byte(void *instance, uint8_t value)
{
	_pSerial_Info pthis = (_pSerial_Info)instance;
	
	return pthis->p_ffunc->push(pthis->p_rev_fifo, &value);
}

