#ifndef  __MINI_DISPATCH_H
#define  __MINI_DISPATCH_H
#include <stdint.h>

typedef void (*task_hook_func)(void*);

typedef enum 
{
	TASK_ENABLE=1,
	TASK_DISABLE
}TASK_STA;

// ����ṹ
typedef struct _TASK_COMPONENTS
{
	uint8_t 		Run;                 		//�������б�ǣ�0-�����У�1����
	TASK_STA		RunState;								//����״̬��TASK_ENABLE��ʹ�ܼ��    TASK_DISABLE�������м��
	//		uint16_t    Priority;								//���ȼ�
	uint32_t  	run_power;						//���Ǹ�����ÿ����һ�μ�һ
	uint32_t 		Timer;              		//��ʱ��
	uint32_t 		ItvTime;              	//�������м��ʱ�䣬��һ�����ʱ��
	void (*TaskHook)(void*);    		// Ҫ���е�������
} TASK_COMPONENTS;       					// ������


// �����嵥
typedef enum _TASK_LIST
{         
	TASK_CAN_SED = 0,						//CAN��������
	TASK_CAN_SED_HB,						//CAN��������
	TASK_CFG_CAN_ADDR,					//����CAN���ߵ�ַ 
	TASK_CC_RADIO_ANDROID,			//CC�㲥���ݵ���׿
	TASK_TEST_LOOP,							//��������
	
	TASKS_MAX                   // �ܵĿɹ�����Ķ�ʱ������Ŀ
} TASK_LIST;

void TaskSetTimes(TASK_LIST Task , uint32_t Times);
void TaskRefresh(TASK_LIST Task);
uint8_t IsTaskEnable(TASK_LIST Task);
void EnableTask(TASK_LIST Task);
void DisableTask(TASK_LIST Task);
void TaskRemarks(void);
void TaskProcess(void*);
#endif
