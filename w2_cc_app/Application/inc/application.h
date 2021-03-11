#ifndef  __APPLICATION_H
#define  __APPLICATION_H
#include <stdint.h>
#include "board_info.h"
 

#define MAX_FRAME_LENGTH			160			//�������֡���ȣ�������


#define  MAX_FLASH_SIZE				256			//flash�洢����
#define  MAX_USE_FLASH_SIZE		64			//ʵ�ʲ���ʹ�õĿռ�
#define  CONFIG_DATA_LEN      12      //�������ݳ���
 

#pragma pack(1)
typedef union
{
	struct
	{
		//�̶���ʽ
		uint8_t crc_data_len;	//crcУ�����ݵĳ���
		uint8_t crc_data_len_n;	//crcУ�����ݵĳ���
		
		//ʣ�����ݺ�У��ռ64�ֽ�
		uint8_t can_addr;			//�Ͱ�׿���CANͨ�ŵ�ַ
		uint8_t can_addr_n;		//�Ͱ�׿���CANͨ�ŵ�ַ  ȡ��
		  
		uint8_t soft_ver;					//����汾
		uint8_t soft_ver_n;				//����汾  ȡ��
 
		uint8_t hard_ver;					//Ӳ����ַ
		uint8_t hard_ver_n;				//Ӳ����ַ  ȡ��
		 
 
		//����ռ��8byte
		
		//����ӵ�����
		
		uint32_t crc32;				//У��
		
		uint8_t res_buff[MAX_USE_FLASH_SIZE-12];		//�����ռ�, 20ָ�Ѿ�ռ�õĿռ�
	}_Use_Param;
	
	struct
	{
		uint8_t null_buff[MAX_USE_FLASH_SIZE];	//�洢��
		uint8_t copy_param[MAX_USE_FLASH_SIZE];	//���ݲ�����
	}_copy_Param;
	
	uint8_t flash_buff[MAX_FLASH_SIZE];
}_App_Config_Param,*_pApp_Config_Param;
#pragma pack()



typedef struct
{
		uint8_t can_addr;					//�Ͱ�׿���ͨ�ŵ�ַ
		uint8_t soft_ver;					//����汾
		uint8_t hard_ver;					//Ӳ����ַ	
		
		_App_Config_Param config_param;	//���ò���
	
}_App_Param, *_pApp_Param;

 extern _App_Param mApp_Param;

#define 	EN_DEBUG                (0)   //0:����          1:��������
#define 	EN_WWDG                 (0)   //0:�رտ��Ź�     1:ʹ�ܿ��Ź�
#define   EN_AUTO_CAN_ADDR_CFG		(1)		//0:������can��ַ�Զ�����  1��ʹ��CAN��ַ�Զ�����

#if EN_DEBUG == 0
	#define debug_print		printf
	#define debug_print_hex  print_hex
#else
	#define debug_print	
	#define debug_print_hex  	
#endif



void systerm_init(void);
void si_sed_loop_task(void); 
void app_dispatch(void);

uint8_t si_sed_buff_to(uint8_t to_addr, uint8_t cmd, uint8_t *buff, uint8_t len, uint8_t try_time);
uint8_t si_sed_buff_to_master(uint8_t cmd, uint8_t *buff, uint8_t len, uint8_t try_time);
#endif

