#ifndef  __CAN_DRV_H
#define  __CAN_DRV_H
#include <stdint.h>
#include "main.h"

#define  	CAN_GLOBAL_ADDR				0xFF		//can����ȫ�ֵ�ַ
#define  	CAN_MASTER_ADDR				0xAE		//���ڵ��ַ
#define 	CAN_ADDR_NULL				  0xFE					//�յ�ַ 
#define 	CAN_STD_ID					  0x12					//��׼֡ID
#define  	CAN_FRAME_TYPE			  CAN_ID_EXT		//��������ͨ�Ŷ�ʹ����չ֡��������250kbps

typedef volatile unsigned char  VU08;   /*unsigned 8 bit definition */
typedef volatile unsigned int   VU16;   /*unsigned 16 bit definition*/
typedef volatile unsigned long  VU32;   /*unsigned 32 bit definition*/

#define MAX_CAN_NUM							8+1	//�����򳤶�
#define MAX_CACHE_NUM						32	//��󻺴��

//����CAN��������
//FIFO0
#define SIGNAL_MASTER_FILTERID			0x00000000		//���ù��ˣ������մӻ�����,ֻ����������ַ
#define SIGNAL_MASTER_MASK					0x03000000		//�������룬�����մӻ����ݣ�ֻ���յ�֡����

//FIFO1
#define MUTIL_MASTER_FILTERID				0x03000000		//���ù��ˣ������մӻ�����,ֻ����������ַ 
#define MUTIL_MASTER_MASK0					0x01000000		//�������룬�����մӻ����ݣ�ֻ���յ�֡����
#define MUTIL_MASTER_MASK1					0x02000000		//�������룬�����մӻ����ݣ�ֻ���յ�֡����

typedef union
{
	struct
	{
		uint32_t res:5;			//����λ
		uint32_t msg_id:2;		//��ϢID������ֹ����ָ���λظ���Ϊ0-3��ͨ�ŷ������ɲ�����ͨ�Ž��շ�ʹ�ý��յ���msg_id��			
		uint32_t pkg_id:5;		//����ID������ʹ��0
		uint32_t is_end:1;		//�Ƿ�Ϊ������������Ϊ1
		uint32_t s2_addr:4;		  //��ַ2
		uint32_t s1_addr:4;			//��ַ1
		uint32_t png_cmd:8;			//������ 
		uint32_t no_se:3;		//����ʹ��
	}_bit;
	
	uint32_t EX_ID;			//29λ
}_Ex_id,*_pEx_id;

typedef struct
{ 
	_Ex_id 	 ex_id;		//��չ֡ID
	uint8_t  byte_count;
	uint8_t  data[MAX_CAN_NUM];
}_Can_Msg,*_pCan_Msg;


void can_drv_init(void *fifo, uint8_t type);
uint8_t  can_sed_pkg_without_cache(CAN_HandleTypeDef can, void* can_msg);
#endif
