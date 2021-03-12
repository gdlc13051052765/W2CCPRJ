#include "interface.h"
#include "includes.h" 
#include "application.h"
#include "st_flash.h"

/*==================================================================================
* �� �� ���� inter_init
* ��    ���� None
* ��������:  �ӿ�Э���ʼ��
* �� �� ֵ�� None
* ��    ע�� Ĭ�ϼ��10ms���н������ݽ���
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-23 180348
==================================================================================*/
void inter_init(void)
{  
	
}

/*==================================================================================
* �� �� ���� read_config_param
* ��    ���� _pApp_Config_Param
* ��������:  ��ȡ���ò���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-06 115746
==================================================================================*/
uint8_t read_config_param(void* c_param)
{
	_pApp_Param pmsg = c_param;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//��ȡ����
	//�жϲ����Ƿ�Ϸ�
	if(crc32(pmsg->config_param.flash_buff, pmsg->config_param._Use_Param.crc_data_len) == pmsg->config_param._Use_Param.crc32)	//�ж�У���Ƿ�Ϸ�
	{
		//�ж�ÿһ������Ƿ�Ϸ�
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
		//У�����ʹ��Ĭ�����ò���
		uint8_t addr = rand()%10+1;
		//addr +=rand()%10;
		pmsg->can_addr = addr;
		pmsg->hard_ver = HARD_VERSION;
		pmsg->soft_ver = SOFT_VERSION; 
	}
	return 0;
}
/*==================================================================================
* �� �� ���� write_config_param
* ��    ���� _pApp_Config_Param
* ��������:  �޸����ò���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� lc
* ����ʱ�䣺 2021-02-05 115746
==================================================================================*/
void write_config_param(_Config_cmd cmd,uint8_t data)
{
	_pApp_Param pmsg ;
	
	stFlash_Func.read(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//��ȡ����	
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
	stFlash_Func.write(APP_CONFIG_ADDR, pmsg->config_param.flash_buff, CONFIG_DATA_LEN);	//д����
}
/*==================================================================================
* �� �� ���� can_rev_decode
* ��    ���� None
* ��������:  canЭ�����
* �� �� ֵ�� None
* ��    ע�� none
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-10-28 172557
==================================================================================*/
void can1_rev_decode(void)
{
	_Ret_Msg c_msg; 
	 
	if(can_pop_one_frame(hcan1, &c_msg) < 0xFF)		//��ȡ����
	{ 
		//Э�����
		can1_frame_parse(&c_msg);		
	} 
}

void can2_rev_decode(void)
{
	_Ret_Msg c_msg; 
	if(can_pop_one_frame(hcan2, &c_msg) < 0xFF)		//��ȡ����
	{ 
		//printf("can2_rev_decode\n");
		//Э�����
		can2_frame_parse(&c_msg);		
	} 
}
/*==================================================================================
* �� �� ���� can_sed_loop_task
* ��    ���� None
* ��������:  CAN��ʱ��������
* �� �� ֵ�� None
* ��    ע�� Ĭ�ϼ��20ms����һ��
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-23 180348
==================================================================================*/
void can_sed_loop_task(void* argv)
{
	static test_struct_t test_data;

	test_data.data ++;
	//can_upload_event_to_android(CC_Android_UP_CPU_ID, test_data.buff, 8);//�ظ���׿���͵�����
	can_sed_loop_check(hcan1);
	can_sed_loop_check(hcan2);
}

/*==================================================================================
* �� �� ���� HAL_CAN_RxFifo0MsgPendingCallback
* ��    ���� None
* ��������:  �������
* �� �� ֵ�� None
* ��    ע�� �̰�
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) 
{
	_Can_Msg Can_Msg;
	CAN_RxHeaderTypeDef mCan_Header;
	
	//��ȡ����
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
		case 0://�㲥֡
		case 1://��׿->cc
		//	debug_print("Android to cc\r\n"); 
			can_recv_signal_frame(hcan1, &Can_Msg); 
		break;
			
		case 2://��׿->cc���
		//	debug_print("Android to CC mutil data\r\n");
			can_recv_mutil_frame(hcan1, &Can_Msg);
		break;
		
		case 3://��׿->box ����ֱ֡��ת��
		case 4://�㲥֡
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
	//ʹ���ж�
	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*==================================================================================
* �� �� ���� HAL_CAN_RxFifo0MsgPendingCallback
* ��    ���� None
* ��������:  �������
* �� �� ֵ�� None
* ��    ע�� ����
* ��    �ߣ�  
* ����ʱ�䣺 2019-09-23 180101
==================================================================================*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) 
{
	_Can_Msg Can_Msg;
	CAN_RxHeaderTypeDef mCan_Header;
	
	//��ȡ����
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1, &mCan_Header, (uint8_t*)Can_Msg.data);
	Can_Msg.ex_id.EX_ID = mCan_Header.ExtId;
	Can_Msg.byte_count = mCan_Header.DLC;
	
	switch(mCan_Header.FilterMatchIndex)
	{
		case 0://ֱ��ת����׿
			//debug_print("box to Android\r\n");
			can_sed_pkg_without_cache(hcan1, &Can_Msg);
		break;
		case 1:
			//debug_print("box to CC\r\n");
			can_recv_signal_frame(hcan2, &Can_Msg);//�������� 
		break;

		default:
			//debug_print("error data\r\n");
		break;

	}	 
	
	//ʹ���ж�
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
* �� �� ���� cc_radio_send_to_android_task
* ��    ���� None
* ��������:  CC�㲥���ݵ���׿
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� lc
* ����ʱ�䣺 2021-02-05 105001
==================================================================================*/

void cc_radio_send_to_android_task(void* argv)
{
	
	uint32_t temp = get_mcu_id();
	srand(temp);
	int a = (rand()%59)*100 +500;
	int b = (rand()%99)*50 +500;
	TaskSetTimes(TASK_CC_RADIO_ANDROID,a+b);
	DisableTask(TASK_CC_RADIO_ANDROID);//disable CC�㲥����
//	EnableTask(TASK_CC_RADIO_ANDROID);//ʹ��CC�㲥����
	can_send_mcuid_to_android(CC_Android_UP_CPU_ID,0);
}


