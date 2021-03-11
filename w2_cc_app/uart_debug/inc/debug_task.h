#ifndef  __DEBUG_TASK_H
#define  __DEBUG_TASK_H
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_CMD_CACHE_LEN		256

#define MAX_NUM(NAME)				(sizeof(NAME)/sizeof(NAME[0]))


typedef void (*p_func)(void*, void*);	//cmd param

typedef struct
{
	uint8_t is_value;			//�ӵ���������  �ж� \r
	uint16_t rev_count;		//��¼�洢�ĸ���
	uint8_t r_point;			//�洢ָ��
	uint8_t rev_buff[MAX_CMD_CACHE_LEN];
}_cmd_cache;


typedef struct
{
	char *cmd_name;		//�ַ�������
	void (*cmd_func)(void *, void*);	//����ص�
}_cmd_list;

#define TO_STRING(x) #x
#define CMD_FUNC_DEF(NAME)				{TO_STRING(NAME), NAME##_func_callback}
#define CMD_NAME_FUNC_DEF(NAME)		{TO_STRING(NAME##_CMD), NAME##_func_callback}

#define DEF_NAME_FUNC(NAME)		NAME##_func_callback

void push_cmd_cache(uint8_t ch);
void cmd_handle_func_task(void);

#endif
