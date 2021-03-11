#ifndef  __STRUCT_TYPE_H
#define  __STRUCT_TYPE_H
#include <stdint.h>

#define RADIO_MAX_LEN					256
#define SERIAL_MAX_LEN	1028

//typedef union
//{
//	struct
//	{
//		uint8_t w_addr;
//		uint8_t w_len;
//		uint8_t w_cmd;
//		uint8_t w_data[1];
//	}_Tx_Msg;
//	
//	struct
//	{
//		uint8_t r_addr;
//		uint8_t r_len;
//		uint8_t r_cmd;
//		uint8_t r_status;
//		uint8_t r_data[1];
//	}_Rx_Msg;
//	
//	struct
//	{
//		uint8_t s_addr;
//		uint8_t s_len;
//		uint8_t s_cmd;
//		uint8_t s_crc;
//	}_Simply;
//	
//	uimt8_t buff[MAX_LEN];
//}_Comm_Msg,*_pComm_Msg;


typedef union
{
	struct
	{
		uint8_t match;
		uint8_t d_addr;
		uint8_t s_addr;
		uint8_t node_id;	//boxid
		uint8_t cmd_index;
		uint8_t payload[1];
	}_Comm;
	 
	struct
	{
		uint8_t d_addr;
		uint8_t s_addr;
		uint8_t node_id;	//boxid
		uint8_t cmd_index;
		uint32_t crc; 
	}_Simply;
	
	uint8_t buff[RADIO_MAX_LEN];
}_Radio_Msg,*_pRadio_Msg;			//����ͨ�Žṹ

typedef union
{
	struct
	{
		uint8_t sof_0;
		uint8_t sof_1;
		uint8_t first_id; 
		uint8_t len;
		uint8_t sl_id;
		uint8_t cmd;
		uint8_t node_id;		//����uart-si4463 node �൱�� ������
		uint8_t payload[1];
	}_Cmd_Msg;
	
	struct
	{
		uint8_t sof_0;
		uint8_t sof_1;
		uint8_t first_id; 
		uint8_t len;
		uint8_t sl_id;
		uint8_t cmd; 		
		uint8_t payload[1]; //����uart-si4463 node �൱�� ������
	}_Comm;
	  
	uint8_t buff[SERIAL_MAX_LEN];
}_Serial_Msg,*_pSerial_Msg; //����ͨ�Ž��


//radio siͨ�Ű�

typedef struct
{ 
	uint8_t match;
	uint8_t channel;
}_IP_Typedef;


typedef 	uint8_t				_Serial_Item;
typedef  	uint8_t*			_pSerial_Item;


typedef struct
{ 
	uint8_t pointer;	//ָ��
	uint8_t lenght;
	_IP_Typedef ip; 
	uint8_t buff[RADIO_MAX_LEN];
}_Si_Pkt,*_pSi_Pkt;

typedef struct
{
	uint8_t is_use;	//�Ƿ�ʹ��  0:����  1��ʹ����
	uint8_t try_count;	//�����ط����� 
	_Si_Pkt pkt_buff;
}_Fifo_Item,*_pFifo_Item;


typedef struct
{
	uint8_t w_point;					//д��ĳ���
	uint8_t pp_buf[256];			//ping����   ��ѯ֡����ֵ+box_id+��ǩ������1B����Ҫ���±�ǩ������+����ǩ����(1B 1����  0:ɾ��)+��ǩ��Ϣ(8+8)����17*��ǩ������
}_PP_item,*_pPP_Item;

typedef struct
{
	uint8_t step_count;				//�Զ���֡����ֵ
	_pPP_Item p_cur;					//��ǰָ��
	_PP_item ping_item;			//ping����
	_PP_item pang_item;			//ping����
}_PP_Cache,*_pPP_Cache;		//ƹ�����ݽṹ

#endif
