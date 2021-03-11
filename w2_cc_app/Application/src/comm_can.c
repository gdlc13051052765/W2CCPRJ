#include "comm_can.h" 
#include "crc.h"
#include "can_def_fifo.h"
#include <can_fifo.h>
#include "command.h"

#include "application.h"

extern _App_Param mApp_Param;

_Mutil_Ring 	 mMutil1_Ring[MAX_MUTIL_ITEM_NUM];		//������� 
_Mutil_Ring 	 mMutil2_Ring[MAX_MUTIL_ITEM_NUM];		//������� 

_Can_Instance mCan1_Instance;
_Can_Instance mCan2_Instance;
 
//static _Can_Msg can1_sed_fifo[CAN_MAX_CACHE_LEN];	//can���ͻ���
static send_queue_t	can1_queue;
_Can_Msg can1_rev_fifo[CAN_MAX_CACHE_LEN];	//can���ջ��棨��֡��

//static _Can_Msg can2_sed_fifo[CAN_MAX_CACHE_LEN];	//can���ͻ���
static send_queue_t	can2_queue;
static _Can_Msg can2_rev_fifo[CAN_MAX_CACHE_LEN];	//can���ջ��棨��֡��

static uint8_t get_to_box_msg_id(void);
static uint8_t get_to_android_msg_id(void);
static uint8_t can_send_one_pkg(CAN_HandleTypeDef hcan, uint8_t host_cmd, uint8_t s1_addr, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len);
static uint8_t msg_queue_ready(p_send_queue_t p_queue_buff);
static void msg_queue_send(CAN_HandleTypeDef hcan, p_send_queue_t p_queue_buff);

/*==================================================================================
* �� �� ���� can_instance_init
* ��    ���� None
* ��������:  can��ʼ��
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 170617
=================================================================================*/
void can_instance_init(CAN_HandleTypeDef hcan)
{  
	_Can_Instance * mCan_Instance;
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��

	if (CAN1 == hcan.Instance)
	{
		mCan_Instance = &mCan1_Instance;
		mMutil_Ring = mMutil1_Ring;
		mCan_Instance->p_ffunc	= pCan1_Fifo_Func,
		//���ͽ��ջ����ʼ������֡�� 

		msg_queue_init(&can1_queue);
 		//mCan_Instance->Sed_Fifo = mCan_Instance->p_ffunc->init_m(can1_sed_fifo, CAN_MAX_CACHE_LEN);
		mCan_Instance->Rcv_Fifo = mCan_Instance->p_ffunc->init_m(can1_rev_fifo, CAN_MAX_CACHE_LEN); 

	} else if (CAN2 == hcan.Instance)
	{	
		mCan_Instance = &mCan2_Instance;
		mMutil_Ring = mMutil2_Ring;
		mCan_Instance->p_ffunc	= pCan2_Fifo_Func,
		//���ͽ��ջ����ʼ������֡�� 
		msg_queue_init(&can2_queue);
 		//mCan_Instance->Sed_Fifo = mCan_Instance->p_ffunc->init_m(can2_sed_fifo, CAN_MAX_CACHE_LEN);
		mCan_Instance->Rcv_Fifo = mCan_Instance->p_ffunc->init_m(can2_rev_fifo, CAN_MAX_CACHE_LEN); 

	}


	//��֡���ճ�ʼ��
	mCan_Instance->pMutil_Fifo = mMutil_Ring;
	
	for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)
	{
		mMutil_Ring[i].ex_id.EX_ID = 0;
		mMutil_Ring[i].in_use = 0;
		mMutil_Ring[i].is_complete = 0;
		mMutil_Ring[i].recv_pkg_num = 0;
		memset(mMutil_Ring[i].r_data, 0, sizeof(mMutil_Ring[i].r_data));
	}
}
 
/*==================================================================================
* �� �� ���� find_avalib_node
* ��    ���� None
* ��������:  ���ҿ��Խڵ�
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 161141
==================================================================================*/
static uint8_t find_null_node(CAN_HandleTypeDef hcan)
{
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}
	//��ѯ���ÿսڵ�����
	for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//�Ӻ���ǰ���ҿ��õ�ID��
	{
		if(mMutil_Ring[i].in_use == 0)
		{
			return i;
		}
	}

	return 0xFF;	//�գ�û�п���
}

/*==================================================================================
* �� �� ���� delete_item_node
* ��    ���� None
* ��������:  ��սڵ�
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 161141
==================================================================================*/
static uint8_t delete_item_node(CAN_HandleTypeDef hcan, uint8_t index)
{  
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}
	//��ջ���
	mMutil_Ring[index].in_use = 0;	//ɾ�����Ϸ�֡ 
	mMutil_Ring[index].is_complete = 0;
	mMutil_Ring[index].ex_id.EX_ID = 0;
	mMutil_Ring[index].recv_pkg_num = 0;
	mMutil_Ring[index].r_len = 0;

	return 0;
}

static uint8_t compare_id(uint32_t id1, uint32_t id2)
{
	uint32_t id_mask = 0xFFFFE07F;
	uint8_t ret = 0;
	if (id1 == id2)
	{
		ret = SAME_ID;
	} else if ((id1 & id_mask) == (id2 & id_mask))
	{
		ret = SAME_PKG_ID;
	}
	return ret;
}
/*==================================================================================
* �� �� ���� item_is_exist
* ��    ���� None
* ��������:  �鿴�Ƿ��Ѿ�����
* �� �� ֵ�� 0xFF:˵��δ�洢  0xFE: ��֡�Ѿ����洢������Ҫ�ٴδ洢��
			����ֵΪ�ñ��ڻ����еĴ洢��ַ�����Ϊ MAX_MUTIL_ITEM_NUM
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 161141
==================================================================================*/
static uint8_t item_is_exist(CAN_HandleTypeDef hcan, uint32_t ex_id)
{
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��

	uint8_t ret_index = 0xFF;	//���صĽڵ�����
	_pEx_id pmsg = (_pEx_id)&ex_id; 

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance) {	
		mMutil_Ring = mMutil2_Ring;
	}
	
	//��ѯ��ǰ��ַ�Ƿ��Ѿ��л���֡
	for(int j=0; j<MAX_MUTIL_ITEM_NUM; j++)	//�Ӻ���ǰ���ҿ��õ�ID��
	{
		uint8_t temp = compare_id(mMutil_Ring[j].ex_id.EX_ID , ex_id);
		//�ж��Ƿ�Ϊͬһ������ID
		if( SAME_PKG_ID == temp)	
		{
			ret_index = j;
		} else if (SAME_ID == temp)
		{
			ret_index = 0xFE;//��ȫ��ͬ��ֱ֡�ӷ���0xFE
		}
	}
	
	//û�н�����ɵİ������˽�β֡���
	if(ret_index == 0xFF)
	{
		for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//�Ӻ���ǰ���ҿ��õ�ID��
		{
			if(mMutil_Ring[i].ex_id._bit.s1_addr == pmsg->_bit.s1_addr)	//���������ͬ��ַ
			{
				if((mMutil_Ring[i].is_complete == 0)&&(mMutil_Ring[i].in_use != 0))	//û�н������
				{
					//ֱ�Ӹ���û�н��������ڵ�
					mMutil_Ring[i].ex_id.EX_ID = ex_id;
					mMutil_Ring[i].recv_pkg_num = 0;
					mMutil_Ring[i].r_len = 0;
					ret_index = i;
				}
			}
		}
	}

	return ret_index;	//�գ�û�п���
}

/*==================================================================================
* �� �� ���� can_recv_frame
* ��    ���� _pCan_Msg
* ��������:  can�������ݽ���
* �� �� ֵ�� None
* ��    ע�� ���ж�
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 154449
==================================================================================*/
uint8_t can_recv_mutil_frame(CAN_HandleTypeDef hcan, void *can_msg)
{
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��

	uint8_t new_index = 0;	//��������
	_pCan_Msg pmsg = can_msg;

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}

	if(pmsg->ex_id._bit.pkg_id == 0)	//֡����:���pkg_id����Ϊ0
	{
		return 0x81;
	}
	else
	{
		//�жϵ�ǰid�Ƿ����
		if((new_index = item_is_exist(hcan, pmsg->ex_id.EX_ID)) == 0xFF)
		{
			//�����ڣ��������±�
			new_index = find_null_node(hcan);			
			//���浽����
			mMutil_Ring[new_index].in_use = 0x01;	//ʹ����
			mMutil_Ring[new_index].ex_id.EX_ID = pmsg->ex_id.EX_ID;
			mMutil_Ring[new_index].r_len = pmsg->byte_count;
			memcpy(mMutil_Ring[new_index].r_data, pmsg->data, pmsg->byte_count);
			//����֡���ۼ�
			mMutil_Ring[new_index].recv_pkg_num = mMutil_Ring[new_index].recv_pkg_num + 1;
			//printf("r:%d p:%d\n", mMutil_Ring[new_index].recv_pkg_num, pmsg->ex_id._bit.pkg_id);
			//debug_print("   rev_data:");
			//debug_print_hex(pmsg->data, pmsg->byte_count);
			return 0;
		} else if (new_index != 0xFE) { //������ȫ��ͬID֡���������֡ID��ȫ��ͬ��˵����֡�Ѿ������棬û��Ҫ�ٴδ洢
			//��ʱ��֡��ʱ�򱣴浽����
			mMutil_Ring[new_index].in_use = 0x01;	//ʹ����
			mMutil_Ring[new_index].r_len += pmsg->byte_count;
			memcpy(mMutil_Ring[new_index].r_data + (pmsg->ex_id._bit.pkg_id - 1) * 8, pmsg -> data, pmsg -> byte_count);	
			//����֡���ۼ�
			mMutil_Ring[new_index].recv_pkg_num = mMutil_Ring[new_index].recv_pkg_num + 1;
			//printf("r:%d p:%d\n", mMutil_Ring[new_index].recv_pkg_num, pmsg->ex_id._bit.pkg_id);
			//debug_print("   rev_data:");
			//debug_print_hex(pmsg->data, pmsg->byte_count);
			//�ж��Ƿ�Ϊĩβ֡
			if(pmsg->ex_id._bit.is_end == 0x01)	//ĩβ֡
			{
				if(mMutil_Ring[new_index].recv_pkg_num == pmsg->ex_id._bit.pkg_id)
				{ 
					mMutil_Ring[new_index].is_complete = 0x01;	//�������
					mMutil_Ring[new_index].ex_id.EX_ID = pmsg->ex_id.EX_ID;//�����һ֡��ID����Ϊ��֡ID
					//CRCУ������Э���������ֹ����ռ���ж�
					printf("rev mMutil\n");
				}
				else
				{  
					//��սڵ�
					delete_item_node(hcan, new_index);
				}
			}
		}
	}
	 
	return 0;
}
 
/*==================================================================================
* �� �� ���� can_recv_frame
* ��    ���� _pCan_Msg
* ��������:  can�������ݽ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 154449
==================================================================================*/
uint8_t can_recv_signal_frame(CAN_HandleTypeDef hcan, void *can_msg)
{  
	_Can_Instance * mCan_Instance;
	if (CAN1 == hcan.Instance)
	{
		mCan_Instance = &mCan1_Instance;
	} else if (CAN2 == hcan.Instance)
	{	
		mCan_Instance = &mCan2_Instance;
	}

	mCan_Instance->p_ffunc->push(mCan_Instance->Rcv_Fifo, can_msg);
	
	return 0;
}

/*==================================================================================
* �� �� ���� can_pop_one_frame
* ��    ���� _pRet_Msg
* ��������:  can�������ݽ���
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 154449
==================================================================================*/ 
uint8_t can_pop_one_frame(CAN_HandleTypeDef hcan, void *ret_msg)
{
	_Can_Instance * mCan_Instance;
	_Mutil_Ring * 	 mMutil_Ring;		//�������ָ��
	
	_pRet_Msg pmsg = ret_msg;
	
	if (CAN1 == hcan.Instance)
	{
		mCan_Instance = &mCan1_Instance;
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mCan_Instance = &mCan2_Instance;
		mMutil_Ring = mMutil2_Ring;
	}
	//�Ȳ�ѯ��֡
	for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//�Ӻ���ǰ���ҿ��õ�ID��
	{
		if(mMutil_Ring[i].is_complete == 0x01)
		{
			pmsg->ex_id.EX_ID = mMutil_Ring[i].ex_id.EX_ID;
			pmsg->byte_count = mMutil_Ring[i].r_len;
			memcpy(pmsg->data, mMutil_Ring[i].r_data,  mMutil_Ring[i].r_len);

			//��սڵ�
			delete_item_node(hcan, i);
			return pmsg->byte_count;
		}
	}	

	//��ѯ��֡ 
	if(	mCan_Instance->p_ffunc->pop(mCan_Instance->Rcv_Fifo, ret_msg) != 0) 
	{
		 return pmsg->byte_count;
		//���ҳɹ�
	}
	
	return 0xFF;
}

/*==================================================================================
* �� �� ���� can_sed_loop_check
* ��    ���� None
* ��������:  CANѭ������Ƿ��з��͵�����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 171529
==================================================================================*/
void can_sed_loop_check(CAN_HandleTypeDef hcan)
{
	p_send_queue_t p_queue_buff;
	
	if (CAN1 == hcan.Instance)
	{
		p_queue_buff = &can1_queue;
	} else if (CAN2 == hcan.Instance)
	{	
		p_queue_buff = &can2_queue;
	}
	//����Ƿ���Ϸ�������
	if(msg_queue_ready(p_queue_buff))
	{
		//��������
		msg_queue_send(hcan, p_queue_buff);
		//can1_msg_queue_pop(1);
	}
}

uint8_t can_send_one_pkg_to_Android_by_radio(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan1, host_cmd, 0xE, 0, msg_id, buff, len);
}

uint8_t can_send_one_pkg_to_Android_by_link(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan1, host_cmd, mApp_Param.can_addr, 0, msg_id, buff, len);//0x7���ͱ�����ַ
}

uint8_t can_send_one_pkg_to_Box_by_radio(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan2, host_cmd, 0xD, 0xF, msg_id, buff, len);
}

uint8_t can_send_one_pkg_to_Box_by_link(uint8_t host_cmd, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan2, host_cmd, 0xD, s2_addr, msg_id, buff, len);//0x7���ͱ�����ַ
}

uint8_t can_send_ka_to_Box(uint8_t s2_addr)
{
	uint8_t send_buff[8] = {0};

	send_buff[0] = 1;

	return can_send_one_pkg_to_Box_by_link(CC_BOX_HEART, s2_addr, get_to_box_msg_id(), send_buff, 1);
}

static uint8_t can_send_one_pkg(CAN_HandleTypeDef hcan, uint8_t host_cmd, uint8_t s1_addr, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len)//ͨ���㲥��ʽ������з��ͣ������ĶԷ��Ƿ��յ�
{
	_Can_Msg c_msg;

	c_msg.ex_id.EX_ID = 0;
	c_msg.ex_id._bit.s1_addr = s1_addr;
	c_msg.ex_id._bit.s2_addr = s2_addr;
	c_msg.ex_id._bit.png_cmd = host_cmd;

	c_msg.ex_id._bit.is_end = 1;
	c_msg.ex_id._bit.pkg_id = 0;
	c_msg.ex_id._bit.msg_id = msg_id;
	c_msg.byte_count = len;

	memset(c_msg.data, 0, 8);
	for(int j = 0; j < len; j ++)
	{
		c_msg.data[j] = buff[j]; 
	}
	
	return can_sed_pkg_without_cache(hcan, &c_msg);
}

static unsigned int last_ts_1 = 0;//��¼can1�������ʱ��
static unsigned int last_ts_2 = 0;//��¼can2�������ʱ��
static void msg_queue_send(CAN_HandleTypeDef hcan, p_send_queue_t p_queue_buff)
{
	_Can_Msg c_msg;

	if (CAN1 == hcan.Instance)
	{
    	last_ts_1 = HAL_GetTick();//��¼����ʱ��
	} else if (CAN2 == hcan.Instance)
	{	
		last_ts_2 = HAL_GetTick();
	}

	memcpy(&c_msg, &p_queue_buff->queue[p_queue_buff->rd], sizeof(_Can_Msg));
    can_sed_pkg_without_cache(hcan, &c_msg);
    
    //log_printf(PRINT_DEBUG"Send Q %d len: %d\r\n",p_queue_buff->count, p_queue_buff->queue[p_queue_buff->rd].len);
}

void can1_msg_queue_pop(uint32_t msg_id)
{

    msg_queue_pop(&can1_queue, msg_id);
    
    //log_printf(PRINT_DEBUG"Send Q %d len: %d\r\n",p_queue_buff->count, p_queue_buff->queue[p_queue_buff->rd].len);
}

void can2_msg_queue_pop(uint32_t msg_id)
{

    msg_queue_pop(&can2_queue, msg_id);
    
    //log_printf(PRINT_DEBUG"Send Q %d len: %d\r\n",p_queue_buff->count, p_queue_buff->queue[p_queue_buff->rd].len);
}

static uint8_t msg_queue_ready(p_send_queue_t p_queue_buff)
{
    unsigned int num;
    unsigned int ts;
    unsigned int last_ts;

    if (p_queue_buff == (&can1_queue))
    {
    	last_ts = last_ts_1;
    } else if (p_queue_buff == (&can2_queue))
    {
    	last_ts = last_ts_2;
    }
    
    ts = HAL_GetTick();//���η��ͼ��&& (ts - last_ts > 3)

    num = msg_queue_num(p_queue_buff);

    if (num > 0 && (ts - last_ts > RETRY_TIME))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void can_upload_event_to_android(uint32_t host_cmd, uint8_t* buff, uint16_t len)//�ظ���׿���͵�����
{
	_Can_Msg c_msg;

	c_msg.ex_id.EX_ID = 0;
	c_msg.ex_id._bit.s1_addr = mApp_Param.can_addr;
	c_msg.ex_id._bit.s2_addr = 0;
	c_msg.ex_id._bit.png_cmd = host_cmd;

	c_msg.ex_id._bit.is_end = 1;
	c_msg.ex_id._bit.pkg_id = 0;
	c_msg.ex_id._bit.msg_id = get_to_android_msg_id();
	c_msg.byte_count = len;

	memset(c_msg.data, 0, 8);
	memcpy(c_msg.data, buff, len);
	msg_queue_push(&can1_queue, &c_msg);

	return ;
}

void can_push_data_box(uint32_t host_cmd, uint8_t box_addr, uint8_t* buff, uint16_t len)//�ظ���׿���͵�����
{
	_Can_Msg c_msg;

	c_msg.ex_id.EX_ID = 0;
	c_msg.ex_id._bit.s1_addr = 0xD;
	c_msg.ex_id._bit.s2_addr = box_addr;
	c_msg.ex_id._bit.png_cmd = host_cmd;

	c_msg.ex_id._bit.is_end = 1;
	c_msg.ex_id._bit.pkg_id = 0;
	c_msg.ex_id._bit.msg_id = get_to_box_msg_id();
	c_msg.byte_count = len;

	memset(c_msg.data, 0, 8);
	memcpy(c_msg.data, buff, len);
	msg_queue_push(&can2_queue, &c_msg);

	return ;
}

static uint8_t get_to_android_msg_id(void)
{
	static uint8_t msg_id = 0;
	msg_id = (msg_id + 1) % 4;

	return msg_id;
}

static uint8_t get_to_box_msg_id(void)
{
	static uint8_t msg_id = 0;
	msg_id = (msg_id + 1) % 4;

	return msg_id;
}
