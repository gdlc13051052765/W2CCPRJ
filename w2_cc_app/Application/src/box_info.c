#include "box_info.h" 
#include <string.h>
#include "includes.h"
#include "struct_type.h"

_Box_Info g_box_info;
/*==================================================================================
* �� �� ���� box_info_init
* ��    ���� None
* ��������:  box��Ϣ��ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-11 150006
==================================================================================*/
void box_info_init(void)
{
	g_box_info.total_box_num = MAX_BOX_ADDR;
	
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		g_box_info.box_item[i].slave_addr = i;
		g_box_info.box_item[i].lost_heart_cnt = 0;
		g_box_info.box_item[i].box_state =0;//�͸�״̬��ʼ��
	}
}

void send_heart_pkg_to_box(void)//��������
{
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		g_box_info.box_item[i].lost_heart_cnt ++;
		if (g_box_info.box_item[i].lost_heart_cnt >= MAX_LOST_HEART)//��ʧ���������������ж��Ƿ����ߣ���ʧMAX_LOST_HEART��Ϊ����
		{
			/* code */
			g_box_info.box_item[i].is_online = 0x00;//����
			g_box_info.box_item[i].box_state &= ~(1<<NODE_ONLINE);  //����
			g_box_info.box_item[i].box_state &= ~(1<<NODE_COMMERR);  //ͨѶ����
		}
		can_send_ka_to_Box(i);
	}

}

void monitor_box(void)
{
	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		//��ȡbox�ĸ�����Ϣ�������м�أ����������ֵҪ���б�����
//		g_box_info.box_item[i].box_ev = get_ev(i);//get_ev() Ŀǰδʵ��
	}
}

/*==================================================================================
* �� �� ���� heatbeat_info_param
* ��    ���� None
* ��������:  ������Ϣ����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-11 152805
==================================================================================*/
uint8_t box_heart_info_parse(void* ret_msg)
{ 
	_pRet_Msg pmsg = ret_msg;
	 uint8_t c_addr = pmsg->ex_id._bit.s2_addr;	//�ӻ���ַ 
	
	if((c_addr < MAX_BOX_ADDR) && (c_addr != 0))	//�жϵ�ַ�ĺϷ���
	{
		//�洢�͸���Ϣ
		g_box_info.box_item[c_addr].lost_heart_cnt = 0;
		g_box_info.box_item[c_addr].is_online = 0x01;		//����Ϊ����
		g_box_info.box_item[c_addr].box_state |= (1<<NODE_ONLINE); //�͸�״̬����
		g_box_info.box_item[c_addr].box_state |= (1<<NODE_COMMERR); //�͸�ͨѶ����
		g_box_info.box_item[c_addr].door_state = (uint8_t)(pmsg->data[1]&0x03);		//��״̬
		g_box_info.box_item[c_addr].box_temp = (uint16_t)((pmsg->data[2]<<8) | (pmsg->data[3]));		//�͸��¶�
		g_box_info.box_item[c_addr].box_ev = (uint16_t )((pmsg->data[4]<<8) | (pmsg->data[5]));		//�͸����	
		g_box_info.box_item[c_addr].box_state &= ((uint16_t)pmsg->data[6]&0x00FF);//�͸�״̬
	}
	debug_print("c_addr==%d,door_state==%2X,box_temp==%d,box_ev==%d",c_addr,g_box_info.box_item[c_addr].door_state,g_box_info.box_item[c_addr].box_temp,g_box_info.box_item[c_addr].box_ev);
	return 0;
}
