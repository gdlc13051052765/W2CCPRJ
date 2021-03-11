#ifndef __IAP_PROTOCOLS_H
#define __IAP_PROTOCOLS_H


#include "ex_flash.h"
#include "st_flash.h"
#include "frame_parse.h"


#define FRAME_MAX_LENS						106	//һ֡������󳤶�
#define   TO_IAP_FLAG		 	 				0x12345678 
 
  
typedef enum
{
	ret_ok = 0,		//ִ�гɹ�
	 
	ret_rev_pkg_id = 0x10,	//�ϵ��������ϵ�λ�ð�ID
	ret_need_pkg = 0x20,	//��Ҫ�İ����
	ret_flash_lost = 0xFE,	//�洢����ʧ���߲��ɶ�д
	ret_info_error = 0x11,	//��Ϣ֡У�����
	ret_data_error = 0x12,	//����֡У�����
	ret_ver_error = 0xEB,		//�汾��Ϣ��һ�£����¿�ʼ
	ret_check_error = 0xEE,	//У����յĹ̼�����
	ret_mass_lost = 0xFB,		//�����ʹ��ߣ����¿�ʼ
	ret_app_lost = 0xFF,		//���й̼�������������ָ����APP����
	
}_Iap_Ret;

typedef enum
{
	iap_null = 0,	//δ��ʼ����
	iap_info_state,	//��Ϣ֡�������
	iap_data_state,	//����֡����
	iap_check_complete, //У�����
}_Update_State;

	
typedef struct
{
		//ȫ�ֱ�����ַ
 const  uint32_t boot_msg_addr;		//boot��Ϣ��
 const	uint32_t app_run_addr;		//app���е�ַ
 const	uint32_t app_cache_addr;	//app�����ַ
	_Update_State update_state;				//����״̬  0��δ��ʼ����   1����Ϣ֡�������  2������֡�������  3��У�����
	uint8_t is_valid_fw_msg;		//�����е���Ϣ֡�Ƿ���Ч  0:û�н��յ���Ч����Ϣ֡  1���Ѿ����յ���Ч����Ϣ֡
	
	uint16_t frame_step;				//��ǰ����֡��ID��
	BootAppFrame fw_msg;	//��Ϣ֡����
	
	const _Flash_Func *w_flash;	//дflash�ӿ�
	const _Flash_Func *r_flash;	//��ȡflash�ӿ�
	const _Flash_Func *msg_flash;	//��ȡflash�ӿ�
}_Iap_Param, *_pIap_Param;


typedef struct
{
	void (*init)(void);
	uint8_t (*info_opt)(const uint8_t*, uint16_t *);		//��Ϣ֡���� �̶� 106�ֽ�
	uint8_t (*data_opt)(const uint8_t*, uint16_t *);	//����֡���� �̶� 106�ֽ�
	uint8_t (*patch_opt)(const uint8_t*, uint16_t *);	//�������� �̶� 106�ֽ�
	uint8_t (*check_opt)(uint16_t *);								//У���������
	uint8_t (*reset_opt)(void);		//ϵͳ��λ
}_Iap_Func, *_pIap_Func;


extern _pIap_Func pIap_Func;

#endif
