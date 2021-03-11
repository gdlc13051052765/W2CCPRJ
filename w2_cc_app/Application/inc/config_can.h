#ifndef  __CONFIG_CAN_H
#define __CONFIG_CAN_H

#include <stdint.h>
#include <stdio.h>
#include "load_gpio.h"

//��ʼ��
//��׿to CAN��ַ

typedef struct
{ 
	uint8_t box_addr;				//ͨ�ŵ�ַ
	GPIO_TypeDef * box_pw_prot;
	uint16_t box_pw_pin;		//box��Ӧ�ĵ�Դ��������
	uint8_t power_addr;			//��Դ��ַ 
}_IO_Node_Tab,*_pIO_Node_Tab;


typedef enum
{
	cfg_power = 0,	//�ر�������Դ
	is_exist,				//�ж���ǰ��ַ�Ƿ����
	sed_addr,				//���ͷ����ַ
	wait_ack,				//�ȴ��ӻ���Ӧ
	cfg_complete,		//����ɹ�
	
}_Cfg_State;		//����CAN���ߵ�ַ


typedef uint8_t (*can_parse)(void*);			//CAN�������ݽ�����������

extern _IO_Node_Tab mANDROID_CAN_MAP_Tab[];


void can_power_and_addr_init(void);
//���ôӻ�CAN��ַ 
void config_can_addr_task(void *argv);

#endif
