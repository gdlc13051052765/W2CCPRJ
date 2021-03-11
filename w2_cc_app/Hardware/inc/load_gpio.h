#ifndef  __LOAD_GPIO_H
#define  __LOAD_GPIO_H
#include "main.h"

#define ALL_BOX_POWER_ON			0x0FFF		//�����е�BOX��Դ


#define BOX_POWER_PIN_NUM			12				//

extern uint16_t g_box_power_pin[];

extern void *g_box_power_port[];

typedef enum
{
	POWER_OFF = 0,		//���ùر�
	POWER_ON = 1, 		//���ô�
	CLOSE_OTHERS,			//�رճ���֮��Ľڵ�,�˽ڵ��
	OPEN_OTHERS,			//�򿪳���֮��Ľڵ㣬�˽ڵ�ر�
}_Operate_Type;

void set_box_power(uint8_t index, _Operate_Type opt_type);

#define SET_BOX_POWER(index, type)    set_box_power(index, type)

void box_power_all_open(void);	//�������е�Դ

void box_power_test_func(void); 
#endif
