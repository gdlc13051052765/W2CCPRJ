#ifndef __FRAME_PARSE_H
#define __FRAME_PARSE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_MASS_LOST_LEVEL			5			//5%
#define LOST_MAX_LEN		10			//����޸Ĵ�ֵ����Ҫ�޸ĳ�ʼ��
#define LOST_INIT() {0, 0}


typedef struct
{
	uint8_t state;
	uint16_t frame_id;
}_Lost_Pkg, *_pLost_Pkg;

typedef struct
{
	uint8_t lost_pkg_num;		//��ʧ�ܰ�����
	_pLost_Pkg precord_lost;	//��¼��ʧ����Ϣ
}_Record_Lost,_pRecord_Lost;




#define FRAME_LENGTH 				106//һ֡������󳤶�


#define BigLittle16_t(Value)    (uint16_t)((Value>>8)|(Value<<8))
#define BigLittle32_t(Value)    ((Value&0xFF000000)>>24)|((Value&0x00FF0000)>>8)|((Value&0x0000FF00)<<8)|((Value&0x000000FF)<<24)

#pragma pack(1)
typedef  union
{
  struct
  {
    uint16_t FrameStep;         //֡����ֵ
    uint16_t AppTotalPackage;   //�ܰ���
    uint8_t  FrameType;         //֡����������  0x01��MS������   0x02:SL������  0x03:CC������
    uint8_t  AppVer;            //APP�汾
    uint8_t  nAppVer;           //APP�汾ȡ��
		uint8_t  HardVer;           //Ӳ���汾
    uint8_t  nHardVer;          //Ӳ���汾ȡ��
    uint32_t AppStartAddr;      //app��ʼ��ַ
    uint32_t AppCrc32;          //appԴ��CRC32У��
    uint16_t AppYear;           //app������
    uint8_t  AppMomth;          //app������
    uint8_t  AppDay;            //app������
    uint8_t  AppHour;           //app����ʱ
    uint8_t  AppMinute;         //app�����
    uint8_t  AppSolvePrb;       //app��������
    uint8_t  AppIsEncry;        //֡���ܱ�־λ
    uint8_t  FrameEncryKey[64]; //֡�ӽ�����Կ
		uint16_t  machine_type;			//����������
		uint8_t  board_type;				//��������
    uint8_t  FrameRet[12];  		//֡����
    uint16_t CRC16;             //֡CRC32У��
  }MsgFrame;
  
  struct
  {
    uint16_t FrameStep;         //֡����ֵ
    uint8_t  FrameType;         //֡���� 
		uint8_t  SoftVer;						//����汾
    uint8_t  AppBuff[FRAME_LENGTH-6];    	//APP�̼�
    uint16_t CRC16;             //CRC16
  }DataFrame;
  
  uint8_t FrameBuff[FRAME_LENGTH];
}BootAppFrame,*pBootAppFrame;
#pragma pack ()
 
uint8_t find_firmware_lost_step(void *instance, uint16_t *fw_id);
uint8_t msg_frame_check(void *instance);
uint8_t msg_frame_compare(const uint8_t *src_buff, const uint8_t *rev_buff, uint8_t len);
uint8_t firmware_data_check(void *instance, uint16_t *fw_id);
uint8_t check_firmware_lost(void *instance, uint16_t *fw_id);
#endif
