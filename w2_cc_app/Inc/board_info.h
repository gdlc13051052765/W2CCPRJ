#ifndef  __BOARD_INFO_H
#define  __BOARD_INFO_H

#define  ST_FLASH_CACHE_ADDR					0x08008C00		//�ڲ�FLASH�����ַ
#define  EX_FLASH_CACHE_ADDR					0							//�ⲿ�����ַ

/*==================================================================================
* ����˵���� ���ļ�Ϊmcu_tools.exe����������������
* ��    ע�� ÿһ��궨���ֵ������Ҫ��()��������Ϊ�˷��������������ò�Ҫ�䶯��������Ҫ�޸Ĺ���
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-05 113517
==================================================================================*/
#define  SOFT_VERSION					(0x02)		//����汾 1���ֽ�
#define  HARD_VERSION					(0x02)		//Ӳ���汾 1���ֽ�
#define  MACHINE_TYPE					(0x06)		//���������� 2���ֽ�
#define  BOARD_TYPE						(0x05)		//�忨���� 1���ֽ�

/*==========================FLASH��ַ�����=========================*/
#define   APP_START_ADDR			0x08003000					//�û�������ʼ��ַ
#define   APP_CACHE_ADDR			EX_FLASH_CACHE_ADDR					//�������򻺴��ַ
#define   IAP_FLAG_ADDR				0x08002000					//�������ò�����ַ


#endif
