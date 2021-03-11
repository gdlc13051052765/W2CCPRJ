#ifndef  __BOX_INFO_H
#define  __BOX_INFO_H

#include <stdint.h>

#define TAG_UID_LENS				8
#define MAX_BLOCK_NUM				8
#define MAX_TAG_NUM					16
#define MAX_BOX_NUM					9
#define MAX_BOX_ADDR				10		//�͸������ַ��1-9
#define MAX_LOST_HEART			3

#pragma pack(1)


//node state
#define OLED_ERR                 0
#define HEAT_ERR                 1
#define LED_ERR                  2
#define RFID_ERR     						 3
#define DOOR_ERR						     4

#define MODE_ERR                 8
#define NODE_COMMERR             9
#define NODE_OVERLOAD            10
#define POWERON            		   11
#define POWERDOWN      				   12
#define NODE_ONLINE              13

typedef struct
{
	uint8_t is_online;		//�Ƿ�����  0������  1������
	uint8_t lost_heart_cnt;	//��ʧ���������������ж��Ƿ����ߣ���ʧMAX_LOST_HEART��Ϊ����
	uint8_t slave_addr;		//�ӻ���ַ
	uint8_t door_state;		//��״̬
	uint8_t soft_ver;			//�͸�����汾
	uint8_t hard_ver;			//�͸�Ӳ���汾
	uint16_t box_temp;		//�͸��¶�
	uint16_t box_ev;			//�͸����
	uint16_t box_state;		//�͸�״̬��Ϣ
}_Box_Item,*_pBox_Item;

typedef struct
{
	uint8_t total_box_num;		//�ܹ���box

	_Box_Item box_item[MAX_BOX_ADDR];
}_Box_Info,*_pBox_Info;

extern _Box_Info g_box_info;

#pragma pack()

void box_info_init(void);
uint8_t box_heart_info_parse(void* ret_msg);
void send_heart_pkg_to_box(void);//��������

#endif
