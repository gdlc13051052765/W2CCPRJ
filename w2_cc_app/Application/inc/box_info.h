#ifndef  __BOX_INFO_H
#define  __BOX_INFO_H

#include <stdint.h>

#define TAG_UID_LENS				8
#define MAX_BLOCK_NUM				8
#define MAX_TAG_NUM					16
#define MAX_BOX_NUM					9
#define MAX_BOX_ADDR				10		//餐格的最大地址从1-9
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
	uint8_t is_online;		//是否在线  0：离线  1：在线
	uint8_t lost_heart_cnt;	//丢失心跳次数，用来判断是否在线，丢失MAX_LOST_HEART认为离线
	uint8_t slave_addr;		//从机地址
	uint8_t door_state;		//门状态
	uint8_t soft_ver;			//餐格软件版本
	uint8_t hard_ver;			//餐格硬件版本
	uint16_t box_temp;		//餐格温度
	uint16_t box_ev;			//餐格电流
	uint16_t box_state;		//餐格状态信息
}_Box_Item,*_pBox_Item;

typedef struct
{
	uint8_t total_box_num;		//总共的box

	_Box_Item box_item[MAX_BOX_ADDR];
}_Box_Info,*_pBox_Info;

extern _Box_Info g_box_info;

#pragma pack()

void box_info_init(void);
uint8_t box_heart_info_parse(void* ret_msg);
void send_heart_pkg_to_box(void);//发送心跳

#endif
