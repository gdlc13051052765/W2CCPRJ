#include "can_drv.h" 

//extern CAN_HandleTypeDef hcan; 

 
/*==================================================================================
* �� �� ���� can_sed_pkg_without_cache
* ��    ���� None
* ��������:  ֱ�ӷ���
* �� �� ֵ�� 0:�����ɹ�  0x81��ʧ��
* ��    ע�� 
* ��    �ߣ�  xiaozh
* ����ʱ�䣺 2019-09-23 180101
==================================================================================*/
uint8_t  can_sed_pkg_without_cache(CAN_HandleTypeDef hcan, void* can_msg)
{
	uint8_t res = 0;
	uint32_t mTxMailbox = 0; 
	_pCan_Msg pmsg = can_msg;
	CAN_TxHeaderTypeDef mCan_header;
	uint8_t i =0;
	
	mCan_header.StdId  = CAN_STD_ID;		//��׼��ʶ��
	mCan_header.ExtId  = pmsg->ex_id.EX_ID;		//��չ��ʶ��(29)
	mCan_header.IDE		 = CAN_ID_EXT;	//ʹ�ñ�׼֡
	mCan_header.RTR 	 = CAN_RTR_DATA;  //����֡
	mCan_header.DLC		 = pmsg->byte_count;//���ݳ���
	
	do
	{
		res = HAL_CAN_AddTxMessage(&hcan, &mCan_header, (uint8_t*)pmsg->data, &mTxMailbox);
		if(res == HAL_ERROR)
		{
			res = 0x81;
			i ++;
			HAL_Delay(3);
		} else if (res == HAL_OK)
		{
			res = 0;
			i = 3;
		}
	} while (i < 3);//������3��
	
	return res;
}


