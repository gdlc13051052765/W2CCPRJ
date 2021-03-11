#include "iap_protocols.h"
#include "board_info.h"
#include "frame_parse.h"
#include "crc.h" 

_Iap_Param mIap_Param = 
{
	.boot_msg_addr	= IAP_FLAG_ADDR,
	.app_run_addr		= APP_START_ADDR,
	.app_cache_addr	= APP_CACHE_ADDR,		//stFlash/exFlash
	
	.is_valid_fw_msg	= 0,
	.frame_step			= 0, 
	.update_state		= iap_null,
	 
	.w_flash = &mEx_Flash_Func,
	.r_flash = &mEx_Flash_Func,
	.msg_flash = &stFlash_Func,
};

static void iap_init(void);
static uint8_t iap_frame_info(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_frame_data(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_frame_patch(const uint8_t* src_buff, uint16_t *ret_result);
static uint8_t iap_firmware_complete_check(uint16_t *ret_result);
static uint8_t iap_reset_system(void);

_Iap_Func mIap_Func = 
{
	.init 			= iap_init,
	.info_opt		= iap_frame_info,
	.data_opt		= iap_frame_data,
	.patch_opt	= iap_frame_patch,
	.check_opt	= iap_firmware_complete_check,
	.reset_opt	= iap_reset_system,
};


_pIap_Func pIap_Func = &mIap_Func;
/*==================================================================================
* �� �� ���� write_firware
* ��    ���� None
* ��������:  д�̼�
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-07 025540
==================================================================================*/
static uint8_t write_firware(uint32_t addr, const uint8_t* buff, uint32_t length)
{ 
	uint8_t r_buff[FRAME_LENGTH] = {0};
	uint8_t try_count =3;
	
	do{
			if(mIap_Param.w_flash->write(addr, buff, length) == 0x01)
			{
				if(mIap_Param.r_flash->read(addr,r_buff, FRAME_LENGTH) == 0x01)
				{
					if(msg_frame_compare(r_buff, buff, FRAME_LENGTH) == 0x01)
					{
						return ret_ok;
					}
				}
			}
			
	}while(try_count--);
	
	return ret_flash_lost;
}

 
 
/*==================================================================================
* �� �� ���� iap_init
* ��    ���� None
* ��������:  ��ʼ��
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-06 042608
==================================================================================*/
static void iap_init(void)
{ 
	
}
 
/*==================================================================================
* �� �� ���� check_firware_is_complete
* ��    ���� None
* ��������:  У��̼��Ƿ�����
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-08 120232
==================================================================================*/
static uint8_t  check_firware_is_complete(uint16_t *fw_id)
{
	uint8_t ret = 0; 
	uint8_t r_buff[106+2] = {0};
	
	//���³�ʼ������ 
	mIap_Param.frame_step = 0;
	
	//�����Ϣ֡�Ƿ����
	if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, r_buff, FRAME_MAX_LENS) == 0x01)
	{
		//У�������Ϣ֡
		if(msg_frame_check(&mIap_Param))
		{
			//��Ч��Ϣ֡
			mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
			
			//��鵱ǰ���յ���֡ID
			if((ret=firmware_data_check(&mIap_Param, fw_id)) == ret_ok)
			{
				return 0x01;
			}
			else
			{
				return ret;
			}
		}
		else
		{
			return ret_check_error;
		}
	}
	else
	{
		return ret_flash_lost;
	}
}



/*==================================================================================
* �� �� ���� check_firware_is_complete
* ��    ���� None
* ��������:  ���ݵ�ǰ�汾ֵȥУ�黺��̼��Ƿ�һ��
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-08 034506
==================================================================================*/
static uint8_t  check_firware_complete_ver(uint8_t soft_ver, uint16_t *fw_id)
{
	uint8_t ret = 0; 
	uint8_t r_buff[106+2] = {0}; 
	 
	//���³�ʼ������ 
	mIap_Param.frame_step = 0;
	mIap_Param.update_state = iap_null;

	//�����Ϣ֡�Ƿ����
	if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, r_buff, FRAME_MAX_LENS) == 0x01)
	{
		//У�������Ϣ֡
		if(msg_frame_check(&mIap_Param))
		{
			//�жϰ汾�Ƿ�Ϸ�
			if(soft_ver != mIap_Param.fw_msg.MsgFrame.AppVer)
			{
				return ret_ver_error;
			}

			//��Ч��Ϣ֡
			mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡

			//��鵱ǰ���յ���֡ID
			ret = find_firmware_lost_step(&mIap_Param, fw_id);		//У�鵱ǰ���յĵİ�ID 

			if(ret == ret_need_pkg )
			{
				mIap_Param.frame_step = *fw_id -1;
				return ret_rev_pkg_id;
			}
			 
			return ret;
		}
		else
		{
			return ret_check_error;
		}
	}
	else
	{
		return ret_flash_lost;
	}
}

/*==================================================================================
* �� �� ���� iap_frame_info
* ��    ���� None
* ��������:  ��Ϣ֡����
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_info(const uint8_t* src_buff, uint16_t *ret_result)
{
	uint8_t ret = 0;  

	//У����Ϣ֡�Ƿ�Ϸ� 
	if(msg_frame_check(&mIap_Param) == 0)
	{
		//���Ϸ�
		return ret_info_error;
	}
	
	if(mIap_Param.update_state != iap_null)
	{
		//�����Ѿ���������
		if(msg_frame_compare(mIap_Param.fw_msg.FrameBuff, src_buff,  FRAME_MAX_LENS) == 0x01)	//��Ϣ֡�Ƚ�
		{
			//��ͬ��Ϣ֡ 
			mIap_Param.update_state = iap_info_state;
			mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
			mIap_Param.frame_step = 0;
			
			ret = find_firmware_lost_step(&mIap_Param, ret_result);		//У�鵱ǰ���յĵİ�ID 
			printf("iap_frame_info()-> ret:%04x; fw_id=%04x\r\n",ret_rev_pkg_id,*ret_result);
			if(ret == ret_need_pkg )
			{
				mIap_Param.frame_step = *ret_result-1;
				return ret_rev_pkg_id;
			}
			 
			return ret;
		}
		else
		{
			//У����Ϣ֡�Ƿ�Ϸ�
			mIap_Param.update_state = iap_info_state;
			mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
			mIap_Param.frame_step = 0;
			
			memcpy(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_LENGTH);	//ʹ���µ���Ϣ֡


			//��ͬ��Ϣ֡�������洢�еĻ��������������¿�ʼ
			//�Ƚ��д洢������
			if(mIap_Param.w_flash->erase(mIap_Param.app_cache_addr, (uint32_t)(mIap_Param.fw_msg.MsgFrame.AppTotalPackage*FRAME_LENGTH)) == 0x01)
			{
				if(write_firware(mIap_Param.app_cache_addr, src_buff, FRAME_LENGTH) == ret_ok) //д����Ϣ֡
				{
				
					return ret_ok;
				}
				else
				{
					return ret_flash_lost;
				}
			}
			else
			{
				return ret_flash_lost;
			}
		}
	}
	else
	{
		//����������
		if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, mIap_Param.fw_msg.FrameBuff, FRAME_MAX_LENS) == 0x01)	//�Ӵ洢��ͬ����Ϣ֡
		{
			if(msg_frame_compare(mIap_Param.fw_msg.FrameBuff, src_buff,  FRAME_MAX_LENS) == 0x01)	//��Ϣ֡
			{
				//��Ч��Ϣ֡
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
				mIap_Param.frame_step = 0;
				
				ret = find_firmware_lost_step(&mIap_Param, ret_result);		//У�鵱ǰ���յĵİ�ID 
				printf("iap_frame_info()-> ret:%04x; fw_id=%04x\r\n",ret_rev_pkg_id,*ret_result);
				if(ret == ret_need_pkg )
				{ 
					mIap_Param.frame_step = *ret_result-1;
					return ret_rev_pkg_id;
				}
				
				return ret;
			}
			else
			{
				//��Ч��Ϣ֡
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
				mIap_Param.frame_step = 0;
				
				memcpy(mIap_Param.fw_msg.FrameBuff, src_buff, FRAME_LENGTH);	
				//ʹ���µ���Ϣ֡
				//�Ƚ��д洢������
				if(mIap_Param.w_flash->erase(mIap_Param.app_cache_addr, (uint32_t)(mIap_Param.fw_msg.MsgFrame.AppTotalPackage*FRAME_LENGTH)) == 0x01)
				{
					if(write_firware(mIap_Param.app_cache_addr, src_buff, FRAME_LENGTH) == ret_ok) //д����Ϣ֡
					{
						return ret_ok;
					}
					else
					{
						return ret_flash_lost;
					}
				}
				else
				{
					return ret_flash_lost;
				}
			}
		}
		else
		{
			return ret_flash_lost;
		}
	}
	
	
	/*
	//����Ƿ���յ���Ч����Ϣ֡ 
	if(mIap_Param.r_flash->read(mIap_Param.app_cache_addr, mIap_Param.fw_msg.FrameBuff, FRAME_MAX_LENS) == 0x01)
	{
		if(msg_frame_compare(mIap_Param.fw_msg.FrameBuff, src_buff,  FRAME_MAX_LENS) == 0x01)	//��Ϣ֡
		{
			if(mIap_Param.update_state == iap_null)
			{
				//У�������Ϣ֡
				if(msg_frame_check(&mIap_Param))
				{
					//��Ч��Ϣ֡
					mIap_Param.update_state = iap_info_state;
					mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
					
					ret = firmware_data_check(&mIap_Param, ret_result);		//У�鵱ǰ���յĵİ�ID 

					return ret;
				}
				else
				{
					//��Ϣ֡У��ʧ��
					return ret_info_error;
				}
			}
			else
			{
				//��Ч��Ϣ֡,�һ������Ѿ����չ�������
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
				
				*ret_result = mIap_Param.frame_step+1;
				return ret_rev_pkg_id;
			}
		}
		else
		{
			//������Ϣ֡�洢
			//У�������Ϣ֡
			if(msg_frame_check(&mIap_Param))
			{
				//��Ч��Ϣ֡
				mIap_Param.update_state = iap_info_state;
				mIap_Param.is_valid_fw_msg = 0x01;	//���յ���Ч��Ϣ֡
			}
			else
			{
				//��Ϣ֡У��ʧ��
				return ret_info_error;
			}
	
			//�Ƚ��д洢������
			if(mIap_Param.w_flash->erase(mIap_Param.app_cache_addr, (uint32_t)(mIap_Param.fw_msg.MsgFrame.AppTotalPackage*FRAME_LENGTH)) == 0x01)
			{
				if(write_firware(mIap_Param.app_cache_addr, src_buff, FRAME_LENGTH) == ret_ok) //д����Ϣ֡
				{
					return ret_ok;
				}
				else
				{
					return ret_flash_lost;
				}
			}
			else
			{
				return ret_flash_lost;
			}
		}
	}
	else
	{
		return ret_flash_lost;
	} 
  */
}

/*==================================================================================
* �� �� ���� iap_frame_data
* ��    ���� None
* ��������:  ����֡����
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_data(const uint8_t* src_buff, uint16_t *ret_result)
{ 
	uint32_t w_addr = 0; 
	
	//У������֡�Ƿ�Ϸ�
	pBootAppFrame pmsg = (pBootAppFrame)src_buff;
	 
	//�ж��Ƿ�Ϊ�ϵ�����
	if( mIap_Param.update_state == iap_null) //û�н��յ���Ϣ֡
	{
		//���ⲿ�洢��У��̼���Ϣ
		 return check_firware_complete_ver(pmsg->DataFrame.SoftVer, ret_result);
	}
	else if( mIap_Param.update_state == iap_info_state) //û�н��յ���Ϣ֡
	{
		mIap_Param.update_state = iap_data_state;
	}
	 
	w_addr = pmsg->DataFrame.FrameStep * FRAME_LENGTH;	//����д���ַ
	
	printf("iap_frame_data()-> FrameStep:%04x; frame_step=%04x\r\n",pmsg->DataFrame.FrameStep,mIap_Param.frame_step);
	if((pmsg->DataFrame.FrameStep == (mIap_Param.frame_step+1))&&
		 pmsg->DataFrame.CRC16 == crc16_ccitt(src_buff, FRAME_LENGTH-2)&&
		 pmsg->DataFrame.SoftVer == mIap_Param.fw_msg.MsgFrame.AppVer)
	{
			mIap_Param.frame_step++;	//�Լ�����֡ID��
//				mIap_Param.frame_step = pmsg->DataFrame.FrameStep;	//����֡ID
		    return write_firware(w_addr, src_buff, FRAME_LENGTH);
	}
	else
	{
		return  ret_data_error; 
	} 
}

/*==================================================================================
* �� �� ���� iap_firmware_check
* ��    ���� None
* ��������:  ����֡����
* �� �� ֵ�� None
* ��    ע��  
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-06 042608
==================================================================================*/
static uint8_t iap_firmware_complete_check(uint16_t *ret_result)
{
//	uint8_t ret = 0;
	
	//�ж��Ƿ���������ݰ�
//	if(mIap_Param.update_state == iap_data_state)
	{
		//У���Ƿ�ȱ����������ȱ�ٵİ�����
		return check_firmware_lost(&mIap_Param, ret_result);
	}
//	return ret;
}

/*==================================================================================
* �� �� ���� iap_frame_patch
* ��    ���� None
* ��������:  �������̴���
* �� �� ֵ�� None
* ��    ע�� ����Ҫע��flash��������Ҫ������������ȡ�����������룬��д
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-06 042608
==================================================================================*/
static uint8_t iap_frame_patch(const uint8_t* src_buff, uint16_t *ret_result)
{
	uint8_t ret = 0;
	uint16_t needPkg = 0;
	uint32_t w_addr = 0;
	pBootAppFrame pmsg = (pBootAppFrame)src_buff;
	
	printf("d fram step=%04x, param step=%04x\r\n", pmsg->DataFrame.FrameStep, mIap_Param.frame_step+1);
	//У�鵱ǰ���Ƿ�Ϸ�
	if((pmsg->DataFrame.FrameStep == (mIap_Param.frame_step+1))&&
	 pmsg->DataFrame.CRC16 == crc16_ccitt(src_buff, FRAME_LENGTH-2)&&
	 pmsg->DataFrame.SoftVer == mIap_Param.fw_msg.MsgFrame.AppVer)
	{
			w_addr = pmsg->DataFrame.FrameStep * pmsg->DataFrame.FrameStep; 
			if((ret=write_firware(w_addr, src_buff, FRAME_LENGTH)) == ret_ok) //���뷽ʽ�����ô�ҳУ���
			{
				//��ȡ��һ��
				ret = find_firmware_lost_step(&mIap_Param, &needPkg);
				
				mIap_Param.frame_step++;
				if(ret == 0)
				{
					return ret_ok;
				}
				else
				{
					*ret_result = needPkg;	//��Ҫ�İ�ID
					printf("needPkg=%04x\r\n", needPkg);
					return ret_need_pkg;
				}
				//������Ҫ����һ������
				
			}
			else
			{
				return ret;
			}
	}
	else
	{
		return  ret_data_error; 
	}  
}

/*==================================================================================
* �� �� ���� write_flag_to_iap
* ��    ���� None
* ��������:	 д��������ת��־
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-11 105718
==================================================================================*/
static void write_flag_to_iap(void)
{ 
	uint32_t jump_flag_value =  TO_IAP_FLAG; 
	//д���־ 
	mIap_Param.msg_flash->write(mIap_Param.boot_msg_addr, (uint8_t*)&jump_flag_value, 4);
}

/*==================================================================================
* �� �� ���� iap_reset_system
* ��    ���� None
* ��������:  ϵͳ��λ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2021-01-11 105718
==================================================================================*/
static uint8_t iap_reset_system(void)
{ 
	//��д��־λ
	write_flag_to_iap();

	HAL_NVIC_SystemReset();
	

	
	while(1);	//�ȴ���λ���
} 

