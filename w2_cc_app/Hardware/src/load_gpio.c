#include "load_gpio.h"
/*==================================================================================
* Copyright(C), 2019-2020, Red Hat Inc. 
* File name: load_gpio.c
* Author:      xiaozh
* Version:    V0.01
* Date: 	2019-10-11
* Description: ����GPIO����
* Function List: // ��Ҫ�������б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
* History:    // �޸���ʷ������ÿ���޸ĵ����ڡ��޸��ߺ��޸����ݼ���
==================================================================================*/
uint16_t g_box_power_pin[BOX_POWER_PIN_NUM] = {
	LOAD_ACT0_Pin,
	LOAD_ACT1_Pin,
	LOAD_ACT2_Pin,
	LOAD_ACT3_Pin,
	LOAD_ACT4_Pin,
	LOAD_ACT5_Pin,
	LOAD_ACT6_Pin,
	LOAD_ACT7_Pin,
	LOAD_ACT8_Pin,
	LOAD_ACT9_Pin,
	LOAD_ACT10_Pin,
	LOAD_ACT11_Pin,  
};

void * g_box_power_port[BOX_POWER_PIN_NUM] = {
	LOAD_ACT0_GPIO_Port,
	LOAD_ACT1_GPIO_Port,
	LOAD_ACT2_GPIO_Port,
	LOAD_ACT3_GPIO_Port,
	LOAD_ACT4_GPIO_Port,
	LOAD_ACT5_GPIO_Port,
	LOAD_ACT6_GPIO_Port,
	LOAD_ACT7_GPIO_Port,
	LOAD_ACT8_GPIO_Port,
	LOAD_ACT9_GPIO_Port,
	LOAD_ACT10_GPIO_Port,
	LOAD_ACT11_GPIO_Port,  
};

/*==================================================================================
* �� �� ���� box_power_open
* ��    ���� None
* ��������:  ��box��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-07 174224
==================================================================================*/
void box_power_open(uint8_t index)
{
	//HAL_GPIO_WritePin(LOAD_ACT0_GPIO_Port, g_box_power_pin[index], GPIO_PIN_SET);
	if (index >= 0 && index < BOX_POWER_PIN_NUM)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)g_box_power_port[index], g_box_power_pin[index], GPIO_PIN_SET);

	}
}

/*==================================================================================
* �� �� ���� box_power_all_open
* ��    ���� None
* ��������:  ������box��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-08 141724
==================================================================================*/
void box_power_all_open(void)
{ 
	 int i = 0; 
	
	for(i=0; i<BOX_POWER_PIN_NUM; i++)
	{
		box_power_open(i); 
		HAL_Delay(50);	//��һ����ʱ��ֹ��Դ������
	} 
}

/*==================================================================================
* �� �� ���� box_power_close
* ��    ���� None
* ��������:  �ر�box��Դ
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-07 173607
==================================================================================*/
void box_power_close(uint8_t index)
{ 
	if (index >= 0 && index < BOX_POWER_PIN_NUM)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)g_box_power_port[index], g_box_power_pin[index], GPIO_PIN_RESET);
	}
}
									
/*==================================================================================
* �� �� ���� set_box_state
* ��    ���� index : 0~13:���õ���box״̬�� 0xFF����������
* ��������:  ����box��Դ״̬
* �� �� ֵ�� None
* ��    ע�� None
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-11-07 174937
==================================================================================*/
void set_box_power(uint8_t index, _Operate_Type opt_type)
{  
	uint32_t gpio_temp = 0;
	 if(index == 0xFF)		//����
	 {
		 switch(opt_type)
		 {
			 case POWER_OFF:		//�ر�����
			 case CLOSE_OTHERS:	//�ر�
			 {
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					box_power_close(i);
				}				
				break;
			 }
			 
			 case POWER_ON:
			 case OPEN_OTHERS:
			 {
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					box_power_open(i);
				}				
				break;
			 }
			 
			 default:
			 {
				 break;
			 }
		 }
	 }
	 else if(index <= 0xFF)
	 { 
		 switch(opt_type)
		 {
			 case POWER_OFF:		//�ر�����
			 {
				 box_power_close(index);
				 break;
			 }
			 
			 case CLOSE_OTHERS:	//�ر�
			 {
				 
				for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					if (i == index)
					{
					  	box_power_open(i);
					} else {
						box_power_close(i);
					}
				}
				break;
			 }
			 
			 case POWER_ON:
			 {
				 box_power_open( index);
				 break;
			 }
			 
			 case OPEN_OTHERS:
			 { 
			 	for (int i = 0; i < BOX_POWER_PIN_NUM; ++i)
				{
					if (i == index)
					{
					  	box_power_close(i);
					} else {
						box_power_open(i);
					}
				}
				break;
			 }
			 
			 default:
			 {
				 break;
			 }
		 }
	 }
}

/*==================================================================================
* �� �� ���� box_power_test
* ��    ���� None
* ��������:  ���Ժ���
* �� �� ֵ�� None
* ��    ע�� ����������ֵ�����Զ����зְ�����
* ��    �ߣ� xiaozh
* ����ʱ�䣺 2019-09-17 122635
==================================================================================*/
void box_power_test_func(void)
{
	int i = 0; 
	
	for(i=0; i<12; i++)
	{
		box_power_open(i); 
		HAL_Delay(50);
	} 
}
 
