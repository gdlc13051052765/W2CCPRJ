#ifndef  __COMM_CAN_H
#define  __COMM_CAN_H
#include <stdint.h>
#include <stdio.h>
#include "can_drv.h" 
#include "can_def_fifo.h"
#include "can_fifo.h"

#define MAX_MUTIL_LEN   256 
#define MAX_MUTIL_ITEM_NUM	10

#define MAX_SED_SIGNAL_LEN		8	//����ܷ��͵ĵ�֡

#define RETRY_TIME				3000 //3sû�յ��ظ��ط�

#define SAME_ID					1
#define SAME_PKG_ID				2


typedef struct
{
	uint8_t is_complete;	//�Ƿ������� 
	uint8_t in_use;			//�Ƿ�����ʹ��  0������ 1��ʹ����
	uint8_t recv_pkg_num;		//��¼��������֡������
	_Ex_id  ex_id;		//��չ֡ID	
	uint8_t r_len;
	uint8_t r_data[MAX_MUTIL_LEN];
}_Mutil_Ring,*_pMutil_Ring;
  
typedef _pCan_Msg _pSignal_Ring;

typedef struct
{ 
	_Ex_id  ex_id;		//��չ֡ID	
	
	uint8_t  byte_count;		//ʵ�ʻ����еĳ���
	uint8_t  data[MAX_MUTIL_LEN];
}_Ret_Msg,*_pRet_Msg;		//���������͵���

typedef struct
{
	_Ex_id  ex_id;		//���յ�id
	uint8_t  tbale_index;	//������ֵ����Ӧ����
}_Mutil_Table,*_pMutil_Table;

typedef struct
{
	//����fifo
	void*     		Sed_Fifo;		//����fifo
	//����fifo
	void* 				Rcv_Fifo;	//�̰�fifo
	
	//fifo�����ӿ�
  _pCan_Fifo_Func  p_ffunc;			//�����ӿ�
	
	//��֡������
	_pMutil_Ring 	pMutil_Fifo;		//����fifo
}_Can_Instance, *_pCan_Instance;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

void can_instance_init(CAN_HandleTypeDef hcan);
uint8_t can_recv_mutil_frame(CAN_HandleTypeDef hcan, void *can_msg);
uint8_t can_recv_signal_frame(CAN_HandleTypeDef hcan, void *can_msg);
uint8_t can_pop_one_frame(CAN_HandleTypeDef hcan, void *ret_msg);
void can_sed_loop_check(CAN_HandleTypeDef hcan);
void can1_msg_queue_pop(uint32_t msg_id);

uint8_t can_send_ka_to_Box(uint8_t s2_addr);
uint8_t can_send_one_pkg_to_Android_by_radio(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len);
uint8_t can_send_one_pkg_to_Android_by_link(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len);
uint8_t can_send_one_pkg_to_Box_by_radio(uint8_t host_cmd, uint8_t msg_id, uint8_t* buff, uint8_t len);
uint8_t can_send_one_pkg_to_Box_by_link(uint8_t host_cmd, uint8_t s2_addr, uint8_t msg_id, uint8_t* buff, uint8_t len);

#endif
