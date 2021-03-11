#include "comm_can.h" 
#include "crc.h"
#include "can_def_fifo.h"
#include <can_fifo.h>
#include "command.h"

#include "application.h"

extern _App_Param mApp_Param;

_Mutil_Ring 	 mMutil1_Ring[MAX_MUTIL_ITEM_NUM];		//多包接收 
_Mutil_Ring 	 mMutil2_Ring[MAX_MUTIL_ITEM_NUM];		//多包接收 

_Can_Instance mCan1_Instance;
_Can_Instance mCan2_Instance;
 
//static _Can_Msg can1_sed_fifo[CAN_MAX_CACHE_LEN];	//can发送缓存
static send_queue_t	can1_queue;
_Can_Msg can1_rev_fifo[CAN_MAX_CACHE_LEN];	//can接收缓存（单帧）

//static _Can_Msg can2_sed_fifo[CAN_MAX_CACHE_LEN];	//can发送缓存
static send_queue_t	can2_queue;
static _Can_Msg can2_rev_fifo[CAN_MAX_CACHE_LEN];	//can接收缓存（单帧）

static uint8_t get_to_box_msg_id(void);
static uint8_t get_to_android_msg_id(void);
static uint8_t can_send_one_pkg(CAN_HandleTypeDef hcan, uint8_t host_cmd, uint8_t s1_addr, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len);
static uint8_t msg_queue_ready(p_send_queue_t p_queue_buff);
static void msg_queue_send(CAN_HandleTypeDef hcan, p_send_queue_t p_queue_buff);

/*==================================================================================
* 函 数 名： can_instance_init
* 参    数： None
* 功能描述:  can初始化
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 170617
=================================================================================*/
void can_instance_init(CAN_HandleTypeDef hcan)
{  
	_Can_Instance * mCan_Instance;
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针

	if (CAN1 == hcan.Instance)
	{
		mCan_Instance = &mCan1_Instance;
		mMutil_Ring = mMutil1_Ring;
		mCan_Instance->p_ffunc	= pCan1_Fifo_Func,
		//发送接收缓存初始化（单帧） 

		msg_queue_init(&can1_queue);
 		//mCan_Instance->Sed_Fifo = mCan_Instance->p_ffunc->init_m(can1_sed_fifo, CAN_MAX_CACHE_LEN);
		mCan_Instance->Rcv_Fifo = mCan_Instance->p_ffunc->init_m(can1_rev_fifo, CAN_MAX_CACHE_LEN); 

	} else if (CAN2 == hcan.Instance)
	{	
		mCan_Instance = &mCan2_Instance;
		mMutil_Ring = mMutil2_Ring;
		mCan_Instance->p_ffunc	= pCan2_Fifo_Func,
		//发送接收缓存初始化（单帧） 
		msg_queue_init(&can2_queue);
 		//mCan_Instance->Sed_Fifo = mCan_Instance->p_ffunc->init_m(can2_sed_fifo, CAN_MAX_CACHE_LEN);
		mCan_Instance->Rcv_Fifo = mCan_Instance->p_ffunc->init_m(can2_rev_fifo, CAN_MAX_CACHE_LEN); 

	}


	//长帧接收初始化
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
* 函 数 名： find_avalib_node
* 参    数： None
* 功能描述:  查找可以节点
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 161141
==================================================================================*/
static uint8_t find_null_node(CAN_HandleTypeDef hcan)
{
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}
	//查询可用空节点索引
	for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//从后往前查找可用的ID号
	{
		if(mMutil_Ring[i].in_use == 0)
		{
			return i;
		}
	}

	return 0xFF;	//空，没有可用
}

/*==================================================================================
* 函 数 名： delete_item_node
* 参    数： None
* 功能描述:  清空节点
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 161141
==================================================================================*/
static uint8_t delete_item_node(CAN_HandleTypeDef hcan, uint8_t index)
{  
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}
	//清空缓存
	mMutil_Ring[index].in_use = 0;	//删除不合法帧 
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
* 函 数 名： item_is_exist
* 参    数： None
* 功能描述:  查看是否已经存在
* 返 回 值： 0xFF:说明未存储  0xFE: 该帧已经被存储，不需要再次存储，
			其他值为该报在缓存中的存储地址，最大为 MAX_MUTIL_ITEM_NUM
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 161141
==================================================================================*/
static uint8_t item_is_exist(CAN_HandleTypeDef hcan, uint32_t ex_id)
{
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针

	uint8_t ret_index = 0xFF;	//返回的节点索引
	_pEx_id pmsg = (_pEx_id)&ex_id; 

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance) {	
		mMutil_Ring = mMutil2_Ring;
	}
	
	//查询当前地址是否已经有缓存帧
	for(int j=0; j<MAX_MUTIL_ITEM_NUM; j++)	//从后往前查找可用的ID号
	{
		uint8_t temp = compare_id(mMutil_Ring[j].ex_id.EX_ID , ex_id);
		//判断是否为同一个包的ID
		if( SAME_PKG_ID == temp)	
		{
			ret_index = j;
		} else if (SAME_ID == temp)
		{
			ret_index = 0xFE;//完全相同的帧直接返回0xFE
		}
	}
	
	//没有接收完成的包，丢了结尾帧情况
	if(ret_index == 0xFF)
	{
		for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//从后往前查找可用的ID号
		{
			if(mMutil_Ring[i].ex_id._bit.s1_addr == pmsg->_bit.s1_addr)	//如果存在相同地址
			{
				if((mMutil_Ring[i].is_complete == 0)&&(mMutil_Ring[i].in_use != 0))	//没有接收完成
				{
					//直接覆盖没有接收完整节点
					mMutil_Ring[i].ex_id.EX_ID = ex_id;
					mMutil_Ring[i].recv_pkg_num = 0;
					mMutil_Ring[i].r_len = 0;
					ret_index = i;
				}
			}
		}
	}

	return ret_index;	//空，没有可用
}

/*==================================================================================
* 函 数 名： can_recv_frame
* 参    数： _pCan_Msg
* 功能描述:  can总线数据接收
* 返 回 值： None
* 备    注： 在中断
* 作    者： xiaozh
* 创建时间： 2019-10-28 154449
==================================================================================*/
uint8_t can_recv_mutil_frame(CAN_HandleTypeDef hcan, void *can_msg)
{
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针

	uint8_t new_index = 0;	//申请索引
	_pCan_Msg pmsg = can_msg;

	if (CAN1 == hcan.Instance)
	{
		mMutil_Ring = mMutil1_Ring;
	} else if (CAN2 == hcan.Instance)
	{	
		mMutil_Ring = mMutil2_Ring;
	}

	if(pmsg->ex_id._bit.pkg_id == 0)	//帧错误:多包pkg_id不能为0
	{
		return 0x81;
	}
	else
	{
		//判断当前id是否存在
		if((new_index = item_is_exist(hcan, pmsg->ex_id.EX_ID)) == 0xFF)
		{
			//不存在，则申请新表
			new_index = find_null_node(hcan);			
			//保存到缓存
			mMutil_Ring[new_index].in_use = 0x01;	//使用中
			mMutil_Ring[new_index].ex_id.EX_ID = pmsg->ex_id.EX_ID;
			mMutil_Ring[new_index].r_len = pmsg->byte_count;
			memcpy(mMutil_Ring[new_index].r_data, pmsg->data, pmsg->byte_count);
			//接收帧数累加
			mMutil_Ring[new_index].recv_pkg_num = mMutil_Ring[new_index].recv_pkg_num + 1;
			//printf("r:%d p:%d\n", mMutil_Ring[new_index].recv_pkg_num, pmsg->ex_id._bit.pkg_id);
			//debug_print("   rev_data:");
			//debug_print_hex(pmsg->data, pmsg->byte_count);
			return 0;
		} else if (new_index != 0xFE) { //抛弃完全相同ID帧，如果两个帧ID完全相同，说明该帧已经被保存，没必要再次存储
			//当时新帧的时候保存到缓存
			mMutil_Ring[new_index].in_use = 0x01;	//使用中
			mMutil_Ring[new_index].r_len += pmsg->byte_count;
			memcpy(mMutil_Ring[new_index].r_data + (pmsg->ex_id._bit.pkg_id - 1) * 8, pmsg -> data, pmsg -> byte_count);	
			//接收帧数累加
			mMutil_Ring[new_index].recv_pkg_num = mMutil_Ring[new_index].recv_pkg_num + 1;
			//printf("r:%d p:%d\n", mMutil_Ring[new_index].recv_pkg_num, pmsg->ex_id._bit.pkg_id);
			//debug_print("   rev_data:");
			//debug_print_hex(pmsg->data, pmsg->byte_count);
			//判断是否为末尾帧
			if(pmsg->ex_id._bit.is_end == 0x01)	//末尾帧
			{
				if(mMutil_Ring[new_index].recv_pkg_num == pmsg->ex_id._bit.pkg_id)
				{ 
					mMutil_Ring[new_index].is_complete = 0x01;	//接收完成
					mMutil_Ring[new_index].ex_id.EX_ID = pmsg->ex_id.EX_ID;//将最后一帧的ID保存为长帧ID
					//CRC校验留在协议解析，防止过多占用中断
					printf("rev mMutil\n");
				}
				else
				{  
					//清空节点
					delete_item_node(hcan, new_index);
				}
			}
		}
	}
	 
	return 0;
}
 
/*==================================================================================
* 函 数 名： can_recv_frame
* 参    数： _pCan_Msg
* 功能描述:  can总线数据接收
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 154449
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
* 函 数 名： can_pop_one_frame
* 参    数： _pRet_Msg
* 功能描述:  can总线数据接收
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 154449
==================================================================================*/ 
uint8_t can_pop_one_frame(CAN_HandleTypeDef hcan, void *ret_msg)
{
	_Can_Instance * mCan_Instance;
	_Mutil_Ring * 	 mMutil_Ring;		//多包接收指针
	
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
	//先查询长帧
	for(int i=0; i<MAX_MUTIL_ITEM_NUM; i++)	//从后往前查找可用的ID号
	{
		if(mMutil_Ring[i].is_complete == 0x01)
		{
			pmsg->ex_id.EX_ID = mMutil_Ring[i].ex_id.EX_ID;
			pmsg->byte_count = mMutil_Ring[i].r_len;
			memcpy(pmsg->data, mMutil_Ring[i].r_data,  mMutil_Ring[i].r_len);

			//清空节点
			delete_item_node(hcan, i);
			return pmsg->byte_count;
		}
	}	

	//查询短帧 
	if(	mCan_Instance->p_ffunc->pop(mCan_Instance->Rcv_Fifo, ret_msg) != 0) 
	{
		 return pmsg->byte_count;
		//查找成功
	}
	
	return 0xFF;
}

/*==================================================================================
* 函 数 名： can_sed_loop_check
* 参    数： None
* 功能描述:  CAN循环检测是否有发送的数据
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-10-28 171529
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
	//检查是否符合发送条件
	if(msg_queue_ready(p_queue_buff))
	{
		//发送数据
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
	return can_send_one_pkg(hcan1, host_cmd, mApp_Param.can_addr, 0, msg_id, buff, len);//0x7发送本机地址
}

uint8_t can_send_one_pkg_to_Box_by_radio(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan2, host_cmd, 0xD, 0xF, msg_id, buff, len);
}

uint8_t can_send_one_pkg_to_Box_by_link(uint8_t host_cmd, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len)
{
	return can_send_one_pkg(hcan2, host_cmd, 0xD, s2_addr, msg_id, buff, len);//0x7发送本机地址
}

uint8_t can_send_ka_to_Box(uint8_t s2_addr)
{
	uint8_t send_buff[8] = {0};

	send_buff[0] = 1;

	return can_send_one_pkg_to_Box_by_link(CC_BOX_HEART, s2_addr, get_to_box_msg_id(), send_buff, 1);
}

static uint8_t can_send_one_pkg(CAN_HandleTypeDef hcan, uint8_t host_cmd, uint8_t s1_addr, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len)//通过广播方式组包进行发送，不关心对方是否收到
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

static unsigned int last_ts_1 = 0;//记录can1的最后发送时间
static unsigned int last_ts_2 = 0;//记录can2的最后发送时间
static void msg_queue_send(CAN_HandleTypeDef hcan, p_send_queue_t p_queue_buff)
{
	_Can_Msg c_msg;

	if (CAN1 == hcan.Instance)
	{
    	last_ts_1 = HAL_GetTick();//记录发送时间
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
    
    ts = HAL_GetTick();//两次发送间隔&& (ts - last_ts > 3)

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

void can_upload_event_to_android(uint32_t host_cmd, uint8_t* buff, uint16_t len)//回复安卓发送的命令
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

void can_push_data_box(uint32_t host_cmd, uint8_t box_addr, uint8_t* buff, uint16_t len)//回复安卓发送的命令
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
