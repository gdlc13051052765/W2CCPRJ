#include "interface.h"
#include "includes.h" 
#include "application.h"
#include "st_flash.h"

/*==================================================================================
* 函 数 名： inter_init
* 参    数： None
* 功能描述:  接口协议初始化
* 返 回 值： None
* 备    注： 默认间隔10ms进行接收数据解析
* 作    者： xiaozh
* 创建时间： 2019-09-23 180348
==================================================================================*/
void inter_init(void)
{  
	
}

/*==================================================================================
* 函 数 名： read_config_param
* 参    数： _pApp_Config_Param
* 功能描述:  读取配置参数
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-06 115746
==================================================================================*/
uint8_t read_config_param(void* c_param)
{
	_pApp_Param pmsg = c_param;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//读取参数
	//判断参数是否合法
	if(crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len) == pmsg->config_param._Use_Param.crc32)	//判断校验是否合法
	{
		//判断每一项参数是否合法
		if(pmsg->config_param._Use_Param.can_addr == ((~pmsg->config_param._Use_Param.can_addr_n)&0x000000FF))
		{
			pmsg->can_addr = pmsg->config_param._Use_Param.can_addr;
		}
		
		if(pmsg->config_param._Use_Param.hard_ver == ((~pmsg->config_param._Use_Param.hard_ver_n)&0x000000FF))
		{
			pmsg->hard_ver = pmsg->config_param._Use_Param.hard_ver;
		}
		
		if(pmsg->config_param._Use_Param.hard_ver == ((~pmsg->config_param._Use_Param.hard_ver_n)&0x000000FF))
		{
			pmsg->hard_ver = pmsg->config_param._Use_Param.hard_ver;
		}
		
		if(pmsg->config_param._Use_Param.soft_ver == ((~pmsg->config_param._Use_Param.soft_ver_n)&0x000000FF))
		{
			pmsg->soft_ver = pmsg->config_param._Use_Param.soft_ver;
		}
		  
	}
	else
	{
		//校验出错使用默认配置参数
		uint8_t addr = rand()%10+1;
		//addr +=rand()%10;
		pmsg->can_addr = addr;
		pmsg->hard_ver = HARD_VERSION;
		pmsg->soft_ver = SOFT_VERSION; 
	}
	return 0;
}
/*==================================================================================
* 函 数 名： write_config_param
* 参    数： _pApp_Config_Param
* 功能描述:  修改配置参数
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-02-05 115746
==================================================================================*/
void write_config_param(_Config_cmd cmd,uint8_t data)
{
	_pApp_Param pmsg ;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//读取参数	
	pmsg->config_param._Use_Param.crc_data_len = CONFIG_DATA_LEN -4;
	pmsg->config_param._Use_Param.crc_data_len_n = ~pmsg->config_param._Use_Param.crc_data_len;
	switch(cmd)
	{
		case Cc_Set_Addr:
			pmsg->can_addr = data;
		break;
		case Cc_Set_Soft_Ver:
			pmsg->soft_ver = data;			
		break;
		case Cc_Set_Hard_Ver:
			pmsg->hard_ver = data;
		break;
	}

	pmsg->config_param._Use_Param.can_addr = pmsg->can_addr;
	pmsg->config_param._Use_Param.can_addr_n = ~pmsg->can_addr;
	pmsg->config_param._Use_Param.hard_ver = pmsg->hard_ver;
	pmsg->config_param._Use_Param.hard_ver_n = ~pmsg->hard_ver;
	pmsg->config_param._Use_Param.soft_ver = pmsg->soft_ver;
	pmsg->config_param._Use_Param.soft_ver_n = ~pmsg->soft_ver;
	
	pmsg->config_param._Use_Param.crc32 = crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len);
	stFlash_Func.write(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//写参数
}
/*==================================================================================
* 函 数 名： can_rev_decode
* 参    数： None
* 功能描述:  can协议解析
* 返 回 值： None
* 备    注： none
* 作    者： xiaozh
* 创建时间： 2019-10-28 172557
==================================================================================*/
void can1_rev_decode(void)
{
	_Ret_Msg c_msg; 
	 
	if(can_pop_one_frame(hcan1, &c_msg) < 0xFF)		//获取数据
	{ 
		//协议解析
		can1_frame_parse(&c_msg);		
	} 
}

void can2_rev_decode(void)
{
	_Ret_Msg c_msg; 
	if(can_pop_one_frame(hcan2, &c_msg) < 0xFF)		//获取数据
	{ 
		//printf("can2_rev_decode\n");
		//协议解析
		can2_frame_parse(&c_msg);		
	} 
}
/*==================================================================================
* 函 数 名： can_sed_loop_task
* 参    数： None
* 功能描述:  CAN定时发送任务
* 返 回 值： None
* 备    注： 默认间隔20ms发送一次
* 作    者： xiaozh
* 创建时间： 2019-09-23 180348
==================================================================================*/
void can_sed_loop_task(void* argv)
{
	static test_struct_t test_data;

	test_data.data ++;
	//can_upload_event_to_android(CC_Android_UP_CPU_ID, test_data.buff, 8);//回复安卓发送的命令
	can_sed_loop_check(hcan1);
	can_sed_loop_check(hcan2);
}

/*==================================================================================
* 函 数 名： HAL_CAN_RxFifo0MsgPendingCallback
* 参    数： None
* 功能描述:  接收完成
* 返 回 值： None
* 备    注： 短包
* 作    者：  
* 创建时间： 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	_Can_Msg Can_Msg;
	CAN_RxHeaderTypeDef mCan_Header;
	
	//读取数据
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0, &mCan_Header, (uint8_t*)Can_Msg.data);
	Can_Msg.ex_id.EX_ID = mCan_Header.ExtId;
	Can_Msg.byte_count = mCan_Header.DLC;
	/*
	debug_print("can_rev irq,");
	debug_print("ex_id:%4x, ", Can_Msg.ex_id.EX_ID);
	
	debug_print("cmd:%02x, ", Can_Msg.ex_id._bit.png_cmd);
	debug_print("s1_addr:%02x, ", Can_Msg.ex_id._bit.s1_addr);
	debug_print("s2_addr:%02x, ", Can_Msg.ex_id._bit.s2_addr);
	debug_print("is_end:%02x, ", Can_Msg.ex_id._bit.is_end);
	debug_print("pkg_id:%02x, ", Can_Msg.ex_id._bit.pkg_id);
	debug_print("msg_id:%02x, ", Can_Msg.ex_id._bit.msg_id);
	
	
	debug_print("lens=%02x, ", Can_Msg.byte_count);
	debug_print("rev_data:");
	debug_print_hex(Can_Msg.data, Can_Msg.byte_count);
	debug_print("\n");*/
	switch(mCan_Header.FilterMatchIndex)
	{
		case 0://广播帧
		case 1://安卓->cc
		//	debug_print("Android to cc\r\n"); 
			can_recv_signal_frame(hcan1, &Can_Msg); 
		break;
			
		case 2://安卓->cc多包
		//	debug_print("Android to CC mutil data\r\n");
			can_recv_mutil_frame(hcan1, &Can_Msg);
		break;
		
		case 3://安卓->box 命令帧直接转发
		case 4://广播帧
		case 5:
		//	debug_print("Android to box data\r\n");
		if(Can_Msg.ex_id._bit.s1_addr==0x0f)
			Can_Msg.ex_id._bit.s1_addr = mApp_Param.can_addr;
		can_sed_pkg_without_cache(hcan2, &Can_Msg);
		break;

		default:
			//debug_print("error data\r\n");
		break;

	}	
	//使能中断
	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*==================================================================================
* 函 数 名： HAL_CAN_RxFifo0MsgPendingCallback
* 参    数： None
* 功能描述:  接收完成
* 返 回 值： None
* 备    注： 长包
* 作    者：  
* 创建时间： 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) 
{
	_Can_Msg Can_Msg;
	CAN_RxHeaderTypeDef mCan_Header;
	
	//读取数据
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1, &mCan_Header, (uint8_t*)Can_Msg.data);
	Can_Msg.ex_id.EX_ID = mCan_Header.ExtId;
	Can_Msg.byte_count = mCan_Header.DLC;
	
	switch(mCan_Header.FilterMatchIndex)
	{
		case 0://直接转发安卓
			//debug_print("box to Android\r\n");
			can_sed_pkg_without_cache(hcan1, &Can_Msg);
		break;
		case 1:
			//debug_print("box to CC\r\n");
			can_recv_signal_frame(hcan2, &Can_Msg);//拷贝参数 
		break;

		default:
			//debug_print("error data\r\n");
		break;

	}	 
	
	//使能中断
	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	debug_print("can error\n");
}
static void can_send_mcuid_to_android(char cmd,char msg_id)
{
	uint8_t send_buff[4] = {0};
	uint32_t temp;
	
	mApp_Param.can_addr = (mApp_Param.can_addr++)%9 +1;
	//mApp_Param.can_addr = rand()%9 +1;
	temp = get_mcu_id();
	send_buff[3] = temp>>24;
	send_buff[2] = temp>>16;
	send_buff[1] = temp>>8;
	send_buff[0] = temp;
	can_send_one_pkg_to_Android_by_link(cmd, msg_id, send_buff, 4);
	debug_print("can1 radio to android mcu id == %2X %2X %2X %2X \n",send_buff[0],send_buff[1],send_buff[2],send_buff[3]);

}
/*==================================================================================
* 函 数 名： cc_radio_send_to_android_task
* 参    数： None
* 功能描述:  CC广播数据到安卓
* 返 回 值： None
* 备    注： None
* 作    者： lc
* 创建时间： 2021-02-05 105001
==================================================================================*/

void cc_radio_send_to_android_task(void* argv)
{
	
	uint32_t temp = get_mcu_id();
	srand(temp);
	int a = (rand()%59)*100 +500;
	int b = (rand()%99)*50 +500;
	TaskSetTimes(TASK_CC_RADIO_ANDROID,a+b);
	DisableTask(TASK_CC_RADIO_ANDROID);//disable CC广播任务
//	EnableTask(TASK_CC_RADIO_ANDROID);//使能CC广播任务
	can_send_mcuid_to_android(CC_Android_UP_CPU_ID,0);
}


