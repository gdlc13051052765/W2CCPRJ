#include "application.h"
#include "includes.h"
#include "stdbool.h"
_App_Param mApp_Param = 
{ 
	.can_addr 				= 0x00,
	.soft_ver			= 0x00,  
	.hard_ver					= 0xFF,
};
 
_Serial_Info mSerial_Info;
 
/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/*==================================================================================
* 函 数 名： systerm_init
* 参    数： None
* 功能描述:  系统初始
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-17 121318
==================================================================================*/
void systerm_init(void)
{
	//中断向量表初始化
#if  EN_DEBUG
	SCB->VTOR = FLASH_BASE | 0X03000; /* Vector Table Relocation in Internal FLASH. */
#else
  
#endif
	
	HAL_Init();
	
	SystemClock_Config();
	/*===========板级初始化==========*/ 
	//读取配置参数
	read_config_param(&mApp_Param);
	//mApp_Param.can_addr = 0x2;
	
	
	/*===========板级初始化==========*/
	bsp_init();
	st_crc_init(); 
	debug_print("can addr = %d\n",mApp_Param.can_addr);
	/*============是能系统中断========*/
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	
	box_info_init();
	
	//can协议初始化
	can_instance_init(hcan1);
	can_instance_init(hcan2);

	can_power_and_addr_init();//can 地址和power对应数据初始化

	/*============外设初始化========*/
//	si446x_regiser_init(&si446x_tx, (void*)&mSpi_Class[0], CHANNEL_1, &si444x_1_rev, &si444x_1_sed);	
	//si446x_register_init(DEFAULT_SED_SI, (void*)&mSpi_Class[1], ADDR_TO_CHANNEL(mApp_Param.si_rx_addr), &si444x_2_rev, &si444x_2_sed);

	//进入接收模式
	//si446x_rev_long_message(DEFAULT_SED_SI,  ADDR_TO_CHANNEL(mApp_Param.si_rx_addr), ADDR_TO_MATCH(mApp_Param.si_rx_addr));
 
	printf("systerm init is complete!\n");
	
//	printf("rev channel=%2x, match=%2x, ms channle=%2x, match=%2x\n", 
//			    ADDR_TO_CHANNEL(mApp_Param.si_rx_addr),ADDR_TO_MATCH(mApp_Param.si_rx_addr),
//				ADDR_TO_CHANNEL(mApp_Param.ms_si_rx_addr), ADDR_TO_MATCH(mApp_Param.ms_si_rx_addr));
}

/*==================================================================================
* 函 数 名： app_loop_task
* 参    数： None
* 功能描述:  运行
* 返 回 值： None
* 备    注： None
* 作    者： xiaozh
* 创建时间： 2019-09-17 141735
==================================================================================*/ 
void app_dispatch(void)
{
	//定时任务调度
	TaskProcess(0);  
	
	//无线业务处理
	//si_radio_event_handle(DEFAULT_SED_SI);
	  
	//解析无线协议
	//radio_frame_param(DEFAULT_SED_SI); 
	 
	//CAN协议解析
	can1_rev_decode();
	can2_rev_decode();
	//can_sed_loop_check(hcan1);
	//debug 功能
	//cmd_handle_func_task();
}

/*==================================================================================
* 函 数 名： USART1_IRQHandler
* 参    数： None
* 功能描述:  串口中断
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-06 115746
==================================================================================*/
/*__weak void USART1_IRQHandler(void)
{
	uint8_t value;
	if((USART1->SR & UART_IT_RXNE) != 0)
	{ 
		 value = USART1->DR;
		 serial_push_byte(&mSerial_Info, value);
	}
	
	if((USART1->SR & UART_IT_IDLE) != 0)
	{
		//清中断标志
		value = USART1->DR;
		value = USART1->SR;
		//写完成标志
		mSerial_Info.rev_complete = 0x01;
	}
}*/

/*==================================================================================
* 函 数 名： HAL_SysTick_Callback
* 参    数： None
* 功能描述:  系统滴答回调
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-06 115746
==================================================================================*/
void HAL_SysTick_Callback(void)
{
	//标记任务
	TaskRemarks();
}

/*==================================================================================
* 函 数 名： box_heartbeat_loop
* 参    数： None
* 功能描述:  餐格心跳查询
* 返 回 值： None
* 备    注： 默认为200ms查询一次，查询所有节点
* 作    者： xiaozh
* 创建时间： 2019-11-06 110749
==================================================================================*/
void box_heartbeat_loop_task(void* argv)
{
	send_heart_pkg_to_box();
}

typedef struct
{
    uint16_t Head;
    uint16_t len;
    uint8_t instruction;
    uint8_t data[150];
    uint32_t crc;
}SlaveTestCmdTypeDef;


uint8_t Slave_SendTestCmd(void)
{
    SlaveTestCmdTypeDef TestCmd;
    TestCmd.Head = 0xAA55;
    TestCmd.len = sizeof(SlaveTestCmdTypeDef) - 9;
    TestCmd.instruction = 0x04;
    memset(TestCmd.data, 0xff, 150);
    TestCmd.crc = st_crc32(((uint8_t *) &TestCmd ) + 5,sizeof(SlaveTestCmdTypeDef) - 9);

//    Slave_AddTranceiverMessage((uint8_t *) &TestCmd,sizeof(SlaveTestCmdTypeDef),0);
	//si_sed_buff_to(0x0E, 0X55, (uint8_t *) &TestCmd, sizeof(SlaveTestCmdTypeDef), 1);  
	
	return 0;
}



void test_send(void)
{ 
	Slave_SendTestCmd();
	
//	uint8_t test_buff[256] = {0};
//	uint32_t frame_step = 0;
//	uint8_t s_count = 0;
//	uint32_t crc32_t = 0;
//	for(int i=0; i<sizeof(test_buff);i++)
//	{
//		test_buff[i+5] = i+1;
//	}

// frame_step++;
//	test_buff[s_count++] = 0x55;	//命令
//	test_buff[s_count++] = 0x01;	//命令
//	test_buff[s_count++] = 0x0E;	//命令
//	
//	test_buff[s_count++] = frame_step>>24;
//	test_buff[s_count++] = frame_step>>16;
//	test_buff[s_count++] = frame_step>>8;
//	test_buff[s_count++] = frame_step>>0;

//	crc32_t = st_crc32(test_buff,  160-4);
//	
//	test_buff[160-4] = crc32_t>>24;
//	test_buff[160-3] = crc32_t>>16;
//	test_buff[160-2] = crc32_t>>8;
//	test_buff[160-1] = crc32_t>>0;
//	
////	si446x_sed_long_message(DEFAULT_SED_SI, 0, 0x0E, test_buff, 160);
//	si_sed_buff_to(0x0E, 0X55, test_buff, 160, 1);  
}

/*==================================================================================
* 函 数 名： test_loop_task
* 参    数： None
* 功能描述:  测试任务
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-29 170658
==================================================================================*/
bool ledstatus = false;
void test_loop_task(void* argv)
{
	if(ledstatus==false){
		ledstatus = true;
		set_box_power(0,1);
	}else{
		ledstatus = false;
		set_box_power(0,0);
	}
	
//  test_send();
//	uint8_t sed_buff[255] = {0};
//	for(int i=0;i<sizeof(sed_buff); i++)
//	{
//		sed_buff[i] = i;
//	}
//		printf("\r\n");
//	debug_print_hex(sed_buff, 128);
//	printf("\r\n");
//	can_sed_link_pkg(0x03, 0x02,  sed_buff,  7);
}












