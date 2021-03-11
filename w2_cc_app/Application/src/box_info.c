#include "box_info.h" 
#include <string.h>
#include "includes.h"
#include "struct_type.h"

_Box_Info g_box_info;
/*==================================================================================
* 函 数 名： box_info_init
* 参    数： None
* 功能描述:  box信息初始化
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-11 150006
==================================================================================*/
void box_info_init(void)
{
	g_box_info.total_box_num = MAX_BOX_ADDR;
	
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		g_box_info.box_item[i].slave_addr = i;
		g_box_info.box_item[i].lost_heart_cnt = 0;
		g_box_info.box_item[i].box_state =0;//餐格状态初始化
	}
}

void send_heart_pkg_to_box(void)//发送心跳
{
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		g_box_info.box_item[i].lost_heart_cnt ++;
		if (g_box_info.box_item[i].lost_heart_cnt >= MAX_LOST_HEART)//丢失心跳次数，用来判断是否在线，丢失MAX_LOST_HEART认为离线
		{
			/* code */
			g_box_info.box_item[i].is_online = 0x00;//掉线
			g_box_info.box_item[i].box_state &= ~(1<<NODE_ONLINE);  //掉线
			g_box_info.box_item[i].box_state &= ~(1<<NODE_COMMERR);  //通讯故障
		}
		can_send_ka_to_Box(i);
	}

}

void monitor_box(void)
{
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		//获取box的各种信息，并进行监控，如果超过阈值要进行报警。
//		g_box_info.box_item[i].box_ev = get_ev(i);//get_ev() 目前未实现
	}
}

/*==================================================================================
* 函 数 名： heatbeat_info_param
* 参    数： None
* 功能描述:  心跳信息解析
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-11 152805
==================================================================================*/
uint8_t box_heart_info_parse(void* ret_msg)
{ 
	_pRet_Msg pmsg = ret_msg;
	 uint8_t c_addr = pmsg->ex_id._bit.s2_addr;	//从机地址 
	
	if((c_addr < MAX_BOX_ADDR) && (c_addr != 0))	//判断地址的合法性
	{
		//存储餐格信息
		g_box_info.box_item[c_addr].lost_heart_cnt = 0;
		g_box_info.box_item[c_addr].is_online = 0x01;		//设置为在线
		g_box_info.box_item[c_addr].box_state |= (1<<NODE_ONLINE); //餐格状态在线
		g_box_info.box_item[c_addr].box_state |= (1<<NODE_COMMERR); //餐格通讯正常
		g_box_info.box_item[c_addr].door_state = (uint8_t)(pmsg->data[1]&0x03);		//门状态
		g_box_info.box_item[c_addr].box_temp = (uint16_t)((pmsg->data[2]<<8) | (pmsg->data[3]));		//餐格温度
		g_box_info.box_item[c_addr].box_ev = (uint16_t )((pmsg->data[4]<<8) | (pmsg->data[5]));		//餐格电流	
		g_box_info.box_item[c_addr].box_state &= ((uint16_t)pmsg->data[6]&0x00FF);//餐格状态
	}
	debug_print("c_addr==%d,door_state==%2X,box_temp==%d,box_ev==%d",c_addr,g_box_info.box_item[c_addr].door_state,g_box_info.box_item[c_addr].box_temp,g_box_info.box_item[c_addr].box_ev);
	return 0;
}
