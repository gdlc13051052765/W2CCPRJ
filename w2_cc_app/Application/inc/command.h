#ifndef  __COMMAND_H
#define  __COMMAND_H
#include <stdint.h>

//CAN通信命令
typedef enum
{
	//安卓发起命令0x00--0x7F
	//安卓发起到CC的命令 0x00--0x1F
	Android_CC_GET_SW_AND_HW_VER					= 0x01,
	Android_CC_EMPTY_ADDR_AND_UP_CPUID,
	Android_CC_ASSIGNED_ADDR,
	Android_CC_CONTROL_BEEP,
	Android_CC_CONTROL_BOX_POWER,
	Android_CC_HEART,
	Android_CC_GET_BOX_STATUS,
	Android_CC_NULL,
	Android_CC_SCAN_CONTROL,
	Android_CC_GET_BOX_CURRENT,
	Android_CC_UPDATE_INFO = 0x0B,
	Android_CC_UPDATE_DATA = 0x0C,
	Android_CC_UPDATE_LOST = 0x0D,
	Android_CC_UPDATE_CHECK = 0x0E,
	Android_CC_UPDATE_RESET = 0x0F,
	
	//安卓发起到BOX的命令 0x20--0x7F
	Android_BOX_GET_SW_AND_HW_VER					= 0x20,
	Android_BOX_CONTROL_LED,
	Android_BOX_CONTROL_DOOR,
	Android_BOX_GET_DOOR_STATUS,
	Android_BOX_CONTROL_HEAT,
	Android_BOX_GET_HEAT,
	Android_BOX_CONTROL_DISPLAY,
	Android_BOX_CONTROL_CARD_READER,
	Android_BOX_CONTROL_BEEP,
	Android_BOX_UPDATE,
	Android_BOX_FLASH_UPDATE,
	Android_BOX_DISPLAY_DATA,
	Android_BOX_DISPLAY_FLASH,
	Android_BOX_CHECK,

	//CC发起命令0x80--0xBF
	//CC发起到安卓的命令0x80--0x9F
	CC_Android_UP_CPU_ID							= 0x80,
	CC_Android_UP_ADDR,
	CC_Android_UP_BOX_STATUS,

	//CC发起到BOX的命令0xA0--0xBF
	CC_BOX_HEART									= 0xA0,
	CC_BOX_SCAN_CONTROL,
	CC_BOX_ASSIGNED_ADDR,
	
	//餐格发起命令0xC0--0xFF
	BOX_Android_UP_CARD_INFO						= 0xC0,
	BOX_Android_UP_DOOR_STATUS,
	BOX_Android_UP_CHECK_STATUS,
}_Can_Cmd;

typedef union test_struct
{
	uint64_t data;
	uint8_t buff[8];
}test_struct_t;

void serial_cmd_analay(void * instance);
void can1_frame_parse(void* ret_msg);
void can2_frame_parse(void* ret_msg);

#endif
