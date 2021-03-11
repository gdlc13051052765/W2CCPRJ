#include "bsp_init.h"
#include "can_drv.h"
#include "application.h"

#include "main.h"

CAN_HandleTypeDef hcan1;//外接安卓
CAN_HandleTypeDef hcan2;//接餐格

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart4;


static uint32_t get_filter_by_s1(uint32_t filter, uint8_t s1_addr)
{
  return (( filter & 0xFFE1FFFF ) | ((s1_addr & 0xF) << 17));
}

static uint32_t get_filter_by_s2(uint32_t filter, uint8_t s2_addr)
{
  return (( filter & 0xfffE1fff ) | ((s2_addr & 0xF) << 13));
}
  
/*==================================================================================
* 函 数 名： get_mcu_id
* 参    数： 
* 功能描述:  获取mcu四个字节ID
* 返 回 值： None
* 备    注： 
* 作    者： lc
* 创建时间： 2021-02-05 044124
==================================================================================*/
uint32_t get_mcu_id(void)
{
	return *(__IO uint32_t*)(0X1FFFF7F0); 
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
	
void MX_CAN_Init(uint8_t s1_addr, uint8_t s2_addr)
{
	uint32_t filter_value = SIGNAL_MASTER_FILTERID;
	uint32_t filter_mask = SIGNAL_MASTER_MASK;

	/* USER CODE BEGIN CAN_Init 0 */

	CAN_FilterTypeDef CAN_FilterInitStructure;
	/* USER CODE END CAN_Init 0 */

	/* USER CODE BEGIN CAN_Init 1 */

	/* USER CODE END CAN_Init 1 */
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 12;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CAN_Init 2 */
	hcan2.Instance = CAN2;
	hcan2.Init.Prescaler = 12;
	hcan2.Init.Mode = CAN_MODE_NORMAL;
	hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan2.Init.TimeSeg1 = CAN_BS1_3TQ;
	hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan2.Init.TimeTriggeredMode = DISABLE;
	hcan2.Init.AutoBusOff = ENABLE;
	hcan2.Init.AutoWakeUp = DISABLE;
	hcan2.Init.AutoRetransmission = DISABLE;
	hcan2.Init.ReceiveFifoLocked = DISABLE;
	hcan2.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan2) != HAL_OK)
	{
		Error_Handler();
	}

	/*===========================安卓-->cc 过滤器fifo0 过滤=============================*/
	//广播包过滤、bit120~bit13  0xf-0x0  bit12:1   0x00F08000
	filter_value = 0x00F08000 >> 3;
	filter_mask =  0x00FFFC00 >> 3;

	CAN_FilterInitStructure.FilterBank  = 0;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//列表模式
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	CAN_FilterInitStructure.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);


	//cc自身数据过滤,bit20~bit17 本cc的地址，定为cc_addr为0-8之间 bit16~bit13  0x8
	filter_value = get_filter_by_s1(0x00F08000 >> 3, s1_addr);
	filter_mask =  0x00FFFC00 >> 3;

	CAN_FilterInitStructure.FilterBank  = 1;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//列表模式
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);


	//安卓->cc多包接收，设置为全部忽略bit12 为0
	filter_value = get_filter_by_s1(0x00F00000 >> 3, s1_addr);
	filter_mask = 0x00FF0000 >> 3;//GET_FILTER_BY_S1(0x00FF8000, 0xF);

	CAN_FilterInitStructure.FilterBank  = 2;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);
	//

	//安卓->餐格数据过滤 bit20~bit17 本cc的地址，定为cc_addr为0-8之间 bit16~bit13  0x0-0x8
	filter_value = get_filter_by_s1(0x00F00000 >> 3, s1_addr);//调试时固定本餐格的地址为8
	filter_mask =  0x00F00000 >> 3;

	CAN_FilterInitStructure.FilterBank  = 3;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);

	//安卓->餐格广播数据过滤 bit20~bit17 0xF，bit16~bit13  0xF
	filter_value = get_filter_by_s1(0x00FF8000 >> 3, s1_addr);
	filter_mask = 0x00FFFC00 >> 3;//GET_FILTER_BY_S1(0x00FF8000, 0xF);

	CAN_FilterInitStructure.FilterBank  = 4;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);
	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);

	// 安卓广播升级数据到餐格 cc地址==0xf box地址0xf ad by lc
	filter_value = 0x001FE000;
	filter_mask = 0x001FE000;//GET_FILTER_BY_S1(0x00FF8000, 0xF);

	CAN_FilterInitStructure.FilterBank  = 5;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);
	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO0;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);


	/*===========================餐格->安卓 过滤器fifo1 过滤=============================*/
	//餐格->安卓数据过滤  bit20~bit17 0xE，bit16~bit13  0x0-0x8
	filter_value = get_filter_by_s1(0x00F08000 >> 3, s1_addr);//MUTIL_MASTER_FILTERID;
	filter_mask =  0x00F0FC00 >> 3;//MUTIL_MASTER_MASK0;

	CAN_FilterInitStructure.FilterBank  = 14;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO1;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterInitStructure);

	//餐格->cc数据过滤  bit20~bit17 本cc的地址，定为cc_addr为0-8之间，调试时使用8
	filter_value = get_filter_by_s1(0x00F08000 >> 3, 0xD);//MUTIL_MASTER_FILTERID;
	filter_mask =  0x00F0FC00 >> 3;//MUTIL_MASTER_MASK0;

	CAN_FilterInitStructure.FilterBank  = 15;     /* 过滤器组 */
	CAN_FilterInitStructure.FilterMode    = CAN_FILTERMODE_IDMASK;//CAN_FilterMode_IdList;//CAN_FilterMode_IdMask;  /* 屏敝模式 */
	CAN_FilterInitStructure.FilterScale   = CAN_FILTERSCALE_32BIT; ///* 32位 */

	CAN_FilterInitStructure.FilterIdHigh      = FILTER_H(filter_value);  
	CAN_FilterInitStructure.FilterIdLow       = FILTER_L(filter_value);
	CAN_FilterInitStructure.FilterMaskIdHigh  = FILTER_MASK_H(filter_mask);
	CAN_FilterInitStructure.FilterMaskIdLow   = FILTER_MASK_L(filter_mask);

	CAN_FilterInitStructure.FilterFIFOAssignment  = CAN_FILTER_FIFO1;  ///* 能够通过该过滤器的报文存到fifo0中 */
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterInitStructure);

	//CAN1->IER |= CAN_IT_BOF|CAN_IT_FMP0|CAN_IT_ERR;
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);	//FIFO0消息挂起中断允许.	 
	//__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);	//FIFO0消息挂起中断允许.	

	//__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);  //FIFO0消息挂起中断允许.   
	__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);  //FIFO0消息挂起中断允许.  

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	//HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);

	HAL_CAN_Start(&hcan2);
	//HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);

	/* USER CODE END CAN_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */
	__HAL_SPI_ENABLE(&hspi1);
  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
	__HAL_SPI_ENABLE(&hspi3);
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	__HAL_UART_ENABLE(&huart1);
  /* USER CODE END USART1_Init 2 */
}

static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  __HAL_UART_ENABLE(&huart4);
  /* USER CODE END USART1_Init 2 */
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BEEP_Pin|LOAD_ACT9_Pin|LOAD_ACT10_Pin|LOAD_ACT11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LOAD_ACT0_Pin|LOAD_ACT1_Pin|LOAD_ACT2_Pin|LOAD_ACT3_Pin 
                          |CS_FLASH_Pin|SPI3_NSS2_Pin|SPI3_NSS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LOAD_ACT4_Pin|LOAD_ACT5_Pin|LOAD_ACT6_Pin|LOAD_ACT7_Pin 
                          |LOAD_ACT8_Pin|RUN_SIGNAL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BEEP_Pin LOAD_ACT9_Pin LOAD_ACT10_Pin LOAD_ACT11_Pin */
  GPIO_InitStruct.Pin = BEEP_Pin|LOAD_ACT9_Pin|LOAD_ACT10_Pin|LOAD_ACT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LOAD_ACT0_Pin LOAD_ACT1_Pin LOAD_ACT2_Pin LOAD_ACT3_Pin 
                           CS_FLASH_Pin SPI3_NSS2_Pin SPI3_NSS1_Pin */
  GPIO_InitStruct.Pin = LOAD_ACT0_Pin|LOAD_ACT1_Pin|LOAD_ACT2_Pin|LOAD_ACT3_Pin 
                          |CS_FLASH_Pin|SPI3_NSS2_Pin|SPI3_NSS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LOAD_ACT4_Pin LOAD_ACT5_Pin LOAD_ACT6_Pin LOAD_ACT7_Pin 
                           LOAD_ACT8_Pin RUN_SIGNAL_Pin */
  GPIO_InitStruct.Pin = LOAD_ACT4_Pin|LOAD_ACT5_Pin|LOAD_ACT6_Pin|LOAD_ACT7_Pin 
                          |LOAD_ACT8_Pin|RUN_SIGNAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/*==================================================================================
* 函 数 名： bsp_init
* 参    数： None
* 功能描述:  板级初始化
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2019-09-17 121426
==================================================================================*/
void bsp_init()
{
	MX_GPIO_Init();
	MX_CAN_Init(mApp_Param.can_addr, 0);
	MX_SPI1_Init();
	MX_SPI3_Init();
	MX_USART4_UART_Init();
	//__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);    //使能接收中断
}
