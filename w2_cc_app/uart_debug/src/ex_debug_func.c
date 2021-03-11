#include "ex_debug_func.h"
#include "includes.h"
#include "debug_task.h"

extern _App_Param mApp_Param;
/*==================================================================================
* �� �� ���� print_si446x_irq
* ��    ���� argv[0] == 0xFF ��ӡ��ϸ��Ϣ
* ��������:  ʹ��si��Ϣ��ӡ
* �� �� ֵ�� None
* ��    ע�� si_irq_print
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_INT_STATE)(void *argc, void *argv)
{
	if(strstr(argc, "help") != 0)
	{
		printf("����:[SI_INT_STATE]\t����:[NONE]\t(ע����ӡ SI4463->GET_INT_STATUS)\n");
		return ;
	}
}
 
/*==================================================================================
* �� �� ���� print_si446x_irq
* ��    ���� argv[0] == 0xFF ��ӡ��ϸ��Ϣ
* ��������:  ʹ��si��Ϣ��ӡ
* �� �� ֵ�� None
* ��    ע�� si_irq_print
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_RESET)(void *argc, void *argv)
{
	if(strstr(argc, "help") != 0)
	{
		printf("����:[SI_RESET]\t����:[NONE]\t(ע����ӡ SI4463->RESET)\n");
		return ;
	}
	printf(" si446x reset complete state :");
}

/*==================================================================================
* �� �� ���� print_si446x_irq
* ��    ���� argv[0] == 0xFF ��ӡ��ϸ��Ϣ
* ��������:  ʹ��si��Ϣ��ӡ
* �� �� ֵ�� None
* ��    ע�� si_dev_state
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 113231
==================================================================================*/ 
void DEF_NAME_FUNC(SI_DEV_STATE)(void *argc, void* argv) 
{ 
 	if(strstr(argc, "help") != 0)
	{
		printf("����:[SI_DEV_STATE]\t����:[NONE]\t(ע����ӡ SI4463->REQUEST_DEVICE_STATE)\n");
		return ;
	}
	
	//��������
	uint8_t ret_buff[16] = {0};
 
	printf(" device state :");
	print_hex(ret_buff, 3);
	printf("\n");
}

/*==================================================================================
* �� �� ���� print_si446x_name_info
* ��    ���� argv[0] == 0xFF ��ӡ��ϸ��Ϣ
* ��������:  ��ӡsi4463�����б���ֵ
* �� �� ֵ�� None
* ��    ע�� si_dev_state
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 113231
==================================================================================*/
void DEF_NAME_FUNC(SI_NAME_RET)(void *argc, void *argv)
{ 
	uint8_t argv0=0, argv1=0;
	uint8_t ret_buff[16] = {0}; 
	
 	if(strstr(argc, "help") != 0)
	{
		printf("����:[SI_NAME_RET]\t����:[SI4463 NAME] [����ֵ����] \t(ע����ӡ����ִ�з���ֵ)\n");
		return ;
	}
	argv0 = (((uint32_t*)argv)[0]&0x00FF);
	argv1 = (((uint32_t*)argv)[1]&0x00FF);
	
	if(argv1 > 16)
	{
		printf("�������Ϸ�, ���ճ���ҪС��16\n");
		return ;
	}
	printf("read info:");
	print_hex(ret_buff, argv1);
	printf("\n");
}
