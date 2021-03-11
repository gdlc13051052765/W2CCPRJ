#include "debug_task.h"
#include "main.h"
#include "def_fifo.h" 

/*===========================�Զ���ӿ��б�==================================*/
 void DEF_NAME_FUNC(SI_INT_STATE)(void *argc, void *argv);
 void DEF_NAME_FUNC(SI_DEV_STATE)(void *argc, void* argv);
 void DEF_NAME_FUNC(SI_NAME_RET)(void *argc, void *argv);
void DEF_NAME_FUNC(SI_RESET)(void *argc, void *argv);

 /*==============================end=======================================*/
 

#define MAX_MIN_CMD_LEN				4			//��С�����ֵĳ���

static _cmd_cache mCmd_Cache = 
{
	.is_value				= 0,	//�Ƿ������Ч����
	.rev_count			= 0,
	.r_point				= 0,
};

//�����ʽ  [�����ַ���]�ո�[����1]�ո�[����2]�ո�[...]\r\n 
static void DEF_NAME_FUNC(help)(void *argc, void* argv);
static void DEF_NAME_FUNC(shell)(void *argc, void* argv);
static void DEF_NAME_FUNC(error)(void *argc, void* argv);
static void DEF_NAME_FUNC(reboot)(void *argc, void* argv);

_cmd_list  mCmd_List_Table[] = 
{ 
	CMD_FUNC_DEF(error),		//������ʾ
	CMD_FUNC_DEF(help),
	CMD_FUNC_DEF(shell),
	CMD_FUNC_DEF(reboot),
	
	/*==========�Զ���==============*/
	CMD_FUNC_DEF(SI_INT_STATE),
	CMD_FUNC_DEF(SI_DEV_STATE),
	CMD_FUNC_DEF(SI_NAME_RET),
	CMD_FUNC_DEF(SI_RESET),
	
};

//help
static void DEF_NAME_FUNC(help)(void *argc, void* argv)
{
	uint8_t step_count = 0;
	
	printf("�������: [�����ַ���] [����1] [����2]\n");
	
	printf("cmd list[sys cmd]:\n");
	
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	printf("\t: %s\n", mCmd_List_Table[step_count++].cmd_name);
	
	printf("cmd list[usr cmd]:\n");
	for(int i=step_count++; i<  MAX_NUM(mCmd_List_Table); i++)
	{ 
	  printf("\t: %s\n", mCmd_List_Table[i].cmd_name);
		
		if(((uint32_t *)argv)[0] == 0xFF)
		{
			mCmd_List_Table[i].cmd_func(argc, argv);	//��ӡ��ϸ��Ϣ
		}
	}
	
}

//entyr shell
static void DEF_NAME_FUNC(error)(void *argc, void* argv)
{
	printf("��֧�� [  %s  ] �����ʹ��help�鿴�����б�\n" , (char*)argc);
}

//entyr shell
static void DEF_NAME_FUNC(shell)(void *argc, void* argv)
{
	printf("cmd frame: [�����ַ���] [����1] [����2]\n");
}

//sys reboot
static void DEF_NAME_FUNC(reboot)(void *argc, void* argv)
{
	printf("systerm reboot success!\n");
	 NVIC_SystemReset();
}

/*==================================================================================
* �� �� ���� push_cmd_cache
* ��    ���� None
* ��������:  �ѽ��յ��������ַ��ŵ�����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-12 172859
==================================================================================*/
void push_cmd_cache(uint8_t ch)
{ 
	mCmd_Cache.rev_buff[mCmd_Cache.rev_count++] = ch; 
	 
	if(ch == '\r')
	{
		mCmd_Cache.is_value++;		//�ӵ�����������
	}
}

/*==================================================================================
* �� �� ���� execute_cmd
* ��    ���� None
* ��������:  �����
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-12 172859
==================================================================================*/
static void execute_cmd(const char *cmd_buf, uint16_t len)
{

}

/*==================================================================================
* �� �� ���� cmd_string_handle
* ��    ���� None
* ��������:  �ַ��������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-12 172859
==================================================================================*/ 
p_func find_func(char *cmd_str)
{ 
	for(int i=0; i<  MAX_NUM(mCmd_List_Table); i++)
	{ 
		if(strstr(cmd_str,mCmd_List_Table[i].cmd_name) != 0)
		{
			return mCmd_List_Table[i].cmd_func;
		}
	}
	 
	return mCmd_List_Table[0].cmd_func;		//��ӡ������ʾ
}

/*==================================================================================
* �� �� ���� sub_cmd_string
* ��    ���� None
* ��������:  ��ȡ����
* �� �� ֵ�� ���ؽ����ַ����ĳ���
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 104515
==================================================================================*/ 
int sub_cmd_string(char *src_str, int src_len, char *dst_str)
{
	uint8_t start_index =0, end_index=0;
	
	//������ʼλ��
	while((src_str[start_index] == '\r') || (src_str[start_index] == '\n'))
	{
		start_index++;
	}
	
	//�޸���ʼ����λ��
	end_index = start_index;
	
	//���ҽ���λ��
	while((src_str[end_index] != '\r') && (src_str[end_index] != '\n'))
	{
		end_index++;
	}
	
	//������ʼ��ַ
	for(int i=start_index; i<end_index; i++)
	{
		dst_str[i] = src_str[i];
	}

	return (end_index+1);
}

/*==================================================================================
* �� �� ���� cmd_handle_func_task
* ��    ���� None
* ��������:  �ַ��������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 105247
==================================================================================*/ 
int cmd_frame_param(const char *cmd_frame, char *cmd_str, uint32_t *param_tab)
{ 	
	//����
	sscanf(cmd_frame, "\r\n%[^ ] %d %d \r", cmd_str, (int*)&param_tab[0], (int*)&param_tab[1]); 	//��������Ͳ���
	 
	printf("cmd_str=%s, param_tab[0]=0x%04x, param_tab[1]=0x%04x\n", cmd_str, param_tab[0], param_tab[1]);
	
	return 0;
}

/*==================================================================================
* �� �� ���� cmd_handle_func_task
* ��    ���� None
* ��������:  �ַ��������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-14 105247
==================================================================================*/ 
void cmd_handle_func_task(void)
{
	p_func pfunc = 0;
	char cmd_frame[16] = {0};			//һ֡����Ļ���
	char cmd_str[16] = {0};				//������
	uint32_t param_tab[8] = {0}; 	//������ʮ�����ƣ� 
	
	if(!mCmd_Cache.is_value)
	{
		return ;
	}
	
	while(1)
	{
		//������һ֡���������\r\n��
		mCmd_Cache.r_point = sub_cmd_string((char*)(mCmd_Cache.rev_buff+mCmd_Cache.r_point), mCmd_Cache.rev_count, cmd_frame);
		
		//��������
		cmd_frame_param(cmd_frame, cmd_str, param_tab);
		
		//���Һ���
		pfunc = find_func(cmd_str);
		 
		//ִ�к���
		pfunc(cmd_str, param_tab);
		 
		//�ж��Ƿ�������
		if(mCmd_Cache.r_point + MAX_MIN_CMD_LEN >= mCmd_Cache.rev_count)
		{
			//��ջ��� 
			mCmd_Cache.is_value = 0;
			mCmd_Cache.rev_count = 0;
			mCmd_Cache.r_point = 0;
			memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
			return ;
		}
	}
}

/*==================================================================================
* �� �� ���� cmd_handle_func_task
* ��    ���� None
* ��������:  �ַ��������
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-12 172859
==================================================================================*/ 
//void cmd_handle_func_task(void)
//{
//	p_func pfunc = 0;
//	char cmd_str[16] = {0};
//	uint8_t param_tab[8] = {0}; 
//	uint8_t point = 0;
//	
//	if(!mCmd_Cache.is_value)
//	{
//		return ;
//	}
//	
//	//�����������
//	do{
//		//ȥ������� \r\n
//		while((mCmd_Cache.rev_buff[mCmd_Cache.r_point] == '\r')||(mCmd_Cache.rev_buff[mCmd_Cache.r_point] == '\n')) 
//		{
//			mCmd_Cache.r_point++; 
//			if(mCmd_Cache.rev_count > 0)
//			{
//				mCmd_Cache.rev_count--;
//			}
//			
//			if(mCmd_Cache.rev_count == 0)
//			{
//				//��ջ���
//				mCmd_Cache.rev_count = 0;
//				mCmd_Cache.r_point = 0;
//				mCmd_Cache.is_value = 0;
//				memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
//				return ;
//			}
//		}
//		
//		//���������
//		do{cmd_str[point++] =  mCmd_Cache.rev_buff[mCmd_Cache.r_point++]; mCmd_Cache.rev_count--;}while((mCmd_Cache.rev_buff[mCmd_Cache.r_point] != '\r')&&(mCmd_Cache.rev_count != 0));
//	
//		//����Ϸ����ж�
//		if((cmd_str[0] != 0) && (cmd_str[0] != '\r') && (cmd_str[0] != '\n'))
//		{
//			//���Һ���
//			pfunc = find_func(cmd_str);
//			
//			//ִ������
//			if(pfunc != NULL)
//			{
//				pfunc(param_tab);
//			}
//			
//			point = 0;
//			memset(cmd_str, 0, sizeof(cmd_str));
//		}

//		//��������
//		(mCmd_Cache.is_value>0)?(mCmd_Cache.is_value--):(mCmd_Cache.is_value=0);
//	}while(mCmd_Cache.is_value);
//	
//	//��ջ��� 
//	mCmd_Cache.is_value = 0;
//	mCmd_Cache.rev_count = 0;
//	mCmd_Cache.r_point = 0;
//	memset(mCmd_Cache.rev_buff, 0, sizeof(mCmd_Cache.rev_buff));
//	 
//}



