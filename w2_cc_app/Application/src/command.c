#include "command.h"
#include "includes.h"
#include "struct_type.h"
#include "config_can.h"
#include "box_info.h"

#include "iap_protocols.h"

//CAN 协议处理函数
static void can_send_sw_and_hw_to_android(char cmd, char msg_id);
static void can_send_heart_to_android(char cmd, char msg_id);
static void iap_simply_ack(uint32_t can_id, uint8_t ret, uint16_t ret_id);
static void can_send_addr_config_to_android(char cmd, char msg_id);

void can1_frame_parse(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
	uint8_t buffer[8] = {0};
	uint16_t ret_id = 0;
	uint8_t ret_s = 0;
	uint32_t temp;
	
//	char buff[8] = {0x11, 0x12, 0x33,0x41, 0x02, 0x03,0x01, 0x02};
//	test_struct_t test_data;
	if(pmsg->ex_id._bit.s1_addr !=mApp_Param.can_addr &pmsg->ex_id._bit.s1_addr!=0x0f)
		return;

	debug_print("can1_rev,");
	debug_print("png_cmd:%4x, ", pmsg->ex_id._bit.png_cmd);
	debug_print("s1_addr:%4x, ", pmsg->ex_id._bit.s1_addr);
	debug_print("s2_addr:%4x, ", pmsg->ex_id._bit.s2_addr);
	debug_print("is_end:%4x, ", pmsg->ex_id._bit.is_end);
	debug_print("msg_id:%4x, ", pmsg->ex_id._bit.msg_id);
	debug_print("lens=%d, ", pmsg->byte_count);
	debug_print("rev_data:");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\n");
	

	//进行协议解析
	switch(pmsg->ex_id._bit.png_cmd)
	{
		case Android_CC_GET_SW_AND_HW_VER:
		{
			debug_print("send hw and sw \r\n");
			can_send_sw_and_hw_to_android(pmsg->ex_id._bit.png_cmd, pmsg->ex_id._bit.msg_id);
			//can_send_pkg_to_android();
			break;
		}
		case Android_CC_EMPTY_ADDR_AND_UP_CPUID:
		{
			int a = rand()%299 +1000;
			int b = rand()%1999 +1000;
			TaskSetTimes(TASK_CC_RADIO_ANDROID,a+b);
			EnableTask(TASK_CC_RADIO_ANDROID);//使能CC广播任务
			debug_print("send Android_CC_EMPTY_ADDR_AND_UP_CPUID \r\n");
			//清空地址，并上传cpuID，			
			break;
		}
		case Android_CC_ASSIGNED_ADDR:	//分配CAN总线地址
		{
			//判断是不是自己的can地址广播，需要判断cpuid，
			//如果是保存can地址，并使用分配到的can地址，主动上报消息，如果不是，不响应	
			debug_print("Android_CC_ASSIGNED_ADDR \r\n");
			
			temp = get_mcu_id();
			if(temp ==*(__IO uint32_t*)(pmsg->data))
			{
				DisableTask(TASK_CC_RADIO_ANDROID);//disable CC广播任务
				mApp_Param.can_addr = pmsg->data[4];
				MX_CAN_Init(mApp_Param.can_addr, 0);
				write_config_param(Cc_Set_Addr,mApp_Param.can_addr);//修改地址写入flash
				can_send_addr_config_to_android(CC_Android_UP_ADDR,0);
				debug_print("CC new addr == %d config ok \n",mApp_Param.can_addr);
				
				set_box_power(0xff,0);
				set_box_power(mApp_Param.can_addr,1);
			}
			
			break;
		}
		case Android_CC_CONTROL_BEEP:
		{
			debug_print("Android_CC_CONTROL_BEEP \r\n");
			break;
		}
		case Android_CC_CONTROL_BOX_POWER:
		{
			debug_print("Android_CC_CONTROL_BOX_POWER \r\n");
			break;
		}
		case Android_CC_HEART:
		{
			if(pmsg->byte_count)
				return;
			//memcpy(test_data.buff, pmsg->data, pmsg->byte_count);
			//test_data.data ++;
			//debug_print("Android_CC_HEART \r\n");
			//debug_print("Android_CC_HEART:%4x, \r\n", pmsg->ex_id._bit.png_cmd);
			//can_send_one_pkg_to_Android_by_link(pmsg->ex_id._bit.png_cmd, pmsg->ex_id._bit.msg_id, test_data.buff, pmsg->byte_count);
			can_send_heart_to_android(pmsg->ex_id._bit.png_cmd, pmsg->ex_id._bit.msg_id);
			break;
		}
		
		case Android_CC_GET_BOX_STATUS://获取餐格状态
		{
			buffer[0] = g_box_info.box_item[pmsg->data[0]].box_state>>8;
			buffer[1] = g_box_info.box_item[pmsg->data[0]].box_state;
			can_send_one_pkg_to_Android_by_link(pmsg->ex_id._bit.png_cmd, pmsg->ex_id._bit.msg_id, buffer, 2);
			debug_print("Android_CC_GET_BOX_STATUS \r\n");
			break;
		}
		
		case Android_CC_SCAN_CONTROL:
		{
			debug_print("Android_CC_SCAN_CONTROL \r\n");
			break;
		}
		
		case Android_CC_GET_BOX_CURRENT:
		{
			debug_print("Android_CC_SCAN_CONTROL \r\n");
			break;
		}
		
		case Android_CC_UPDATE_INFO:
		{
			debug_print("Android_CC_UPDATE_INFO \r\n");
			ret_s = pIap_Func->info_opt(pmsg->data, &ret_id);
			iap_simply_ack(pmsg->ex_id.EX_ID, ret_s, ret_id);
			break;
		}
		
		case Android_CC_UPDATE_DATA:
		{
			debug_print("Android_CC_UPDATE_DATA \r\n");
			ret_s = pIap_Func->data_opt(pmsg->data, &ret_id);
			iap_simply_ack(pmsg->ex_id.EX_ID, ret_s, ret_id);
			break;
		}
		
		case Android_CC_UPDATE_LOST:
		{
			debug_print("Android_CC_UPDATE_LOST \r\n");
			ret_s = pIap_Func->patch_opt(pmsg->data, &ret_id);
			iap_simply_ack(pmsg->ex_id.EX_ID, ret_s, ret_id);
			break;
		}
		
		case Android_CC_UPDATE_CHECK:
		{
			debug_print("Android_CC_UPDATE_CHECK \r\n");
			ret_s = pIap_Func->check_opt(&ret_id);
			iap_simply_ack(pmsg->ex_id.EX_ID, ret_s, ret_id);
			break;
		}
		
		case Android_CC_UPDATE_RESET:
		{
			debug_print("Android_CC_UPDATE_RESET \r\n");
			iap_simply_ack(pmsg->ex_id.EX_ID, 0, 0);	//先响应再进行复位
			HAL_Delay(30);	//等待响应完成			
			ret_s = pIap_Func->reset_opt();	//执行复位操作
			break;
		}
		 
		default:
		{
			break;
		}
	}
}

void can2_frame_parse(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
//	char buff[8] = {0x11, 0x12, 0x33,0x41, 0x02, 0x03,0x01, 0x02};
//	test_struct_t test_data;

/*	debug_print("can2_rev,");
	debug_print("png_cmd:%4x, ", pmsg->ex_id._bit.png_cmd);
	debug_print("s1_addr:%4x, ", pmsg->ex_id._bit.s1_addr);
	debug_print("s2_addr:%4x, ", pmsg->ex_id._bit.s2_addr);
	debug_print("is_end:%4x, ", pmsg->ex_id._bit.is_end);
	debug_print("msg_id:%4x, ", pmsg->ex_id._bit.msg_id);
	debug_print("lens=0x%02x, ", pmsg->byte_count);
	debug_print("rev_data:");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\n");	debug_print("lens=%02x, ", pmsg->byte_count);
	debug_print("rev_data:");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\n");*/

	//进行协议解析
	switch(pmsg->ex_id._bit.png_cmd)
	{	
		case CC_BOX_HEART:
		{
			debug_print("CC_BOX_HEART \r\n");
			box_heart_info_parse(pmsg);
			break;
		}
		case CC_BOX_SCAN_CONTROL:
		{
			debug_print("CC_BOX_SCAN_CONTROL \r\n");
			break;
		}
		case CC_BOX_ASSIGNED_ADDR:
		{
			debug_print("CC_BOX_ASSIGNED_ADDR \r\n");
			break;
		}

		default:
		{
			break;
		}
	}
}

static void can_send_sw_and_hw_to_android(char cmd, char msg_id)
{	
	uint8_t send_buff[8] = {0};

	send_buff[0] = BOX_SUCCESS;
	send_buff[1] = mApp_Param.hard_ver;	//硬件版本号
	send_buff[2] = SOFT_VERSION;	//软件版本号
	can_send_one_pkg_to_Android_by_link(cmd, msg_id, send_buff, 3);
}

static void can_send_addr_config_to_android(char cmd, char msg_id)
{	
	uint8_t send_buff[8] = {0};

	send_buff[0] = mApp_Param.can_addr;
	can_send_one_pkg_to_Android_by_link(cmd, msg_id, send_buff, 1);
}

static void can_send_heart_to_android(char cmd, char msg_id)
{	
	uint8_t send_buff[8] = {0};
	uint16_t temp = 0;

	for(int i = 1; i < g_box_info.total_box_num; i ++)//box_id  1--9
	{
		temp |= ((g_box_info.box_item[i].is_online & 0x01) << (i - 1));
		printf("temp %d \n", temp);
	}

	send_buff[0] = BOX_SUCCESS;
	send_buff[1] = temp & 0x00FF;
	send_buff[2] = temp>>8 & 0x01;
	printf("send_buff==%2X %2X",send_buff[1],send_buff[2]);
	can_send_one_pkg_to_Android_by_link(cmd, msg_id, send_buff, 3);
}


/*
static void cc_update(void* ret_msg)
{
	_pRet_Msg pmsg = ret_msg;
	uint8_t send_buff[8] = {0};

	debug_print("update pkg:");
	debug_print_hex(pmsg->data, pmsg->byte_count);
	debug_print("\n");

	send_buff[0] = BOX_SUCCESS;
	can_send_one_pkg_to_Android_by_link(pmsg->ex_id._bit.png_cmd, pmsg->ex_id._bit.msg_id, send_buff, 1);

}*/

/*==================================================================================
* 函 数 名： iap_simply_ack
* 参    数： None
* 功能描述:  升级响应
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-07 025540
==================================================================================*/
static void iap_simply_ack(uint32_t can_id, uint8_t ret_reuslt, uint16_t ret_id)
{
	_Ret_Msg msg;
	msg.ex_id.EX_ID = can_id;
	
	
	if(ret_reuslt == 0)
	{
		msg.data[0] = 0;	//正常
		msg.byte_count = 0x01;
	}
	else
	{
		msg.data[0] = 0x08;	//异常
		msg.data[1] = ret_reuslt;	//状态值
		msg.byte_count = 0x02;
		if(ret_id != 0)
		{
			msg.data[2] = (uint8_t)((ret_id>>0)&0x00FF);	//状态值参数
			msg.data[3] = (uint8_t)((ret_id>>8)&0x00FF);
			msg.byte_count = 0x04;
		}
	} 
	
	can_send_one_pkg_to_Android_by_link(msg.ex_id._bit.png_cmd, msg.ex_id._bit.msg_id, msg.data, msg.byte_count);
}
