#include "includes.h"
#include "uart_debug.h"
#include "debug_task.h"
 
#define DEBUG_UART								UART4 
#define DEBUG_IRQ_HANDLE(NAME)    NAME##_IRQHandler 


void debug_send(uint8_t value)
{
	DEBUG_UART->DR = value;
	while((DEBUG_UART->SR & UART_IT_TXE) == RESET);
}

uint8_t debug_read(void)
{
	uint8_t value = 0;
	while((DEBUG_UART->SR & UART_IT_RXNE) == RESET);
	value = DEBUG_UART->DR;
	return value;
}

/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
	debug_send(ch); 
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  ch = debug_read();
  return ch;
}


//�����ж�
void DEBUG_IRQ_HANDLE(USART4)(void)
{
	uint8_t value = 0;
	if((DEBUG_UART->SR & UART_IT_RXNE) != 0)
	{
		value = DEBUG_UART->DR;  
		push_cmd_cache(value);		//��������
	}
	
	if((DEBUG_UART->SR & UART_IT_IDLE) != 0)
	{
		//���жϱ�־
		value = DEBUG_UART->DR;
		value = DEBUG_UART->SR;
		//д��ɱ�־ 
	}
}


void print_hex(uint8_t *buff, uint16_t len)
{
	for(int i=0; i<len; i++)
	{
		printf("%02x ", buff[i]);
	}
}

