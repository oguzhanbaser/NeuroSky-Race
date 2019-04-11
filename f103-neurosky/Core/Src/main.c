/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
GPIO_TypeDef *LED_PORT2_NAMES[10] = {LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port, LED5_GPIO_Port,
										LED6_GPIO_Port, LED7_GPIO_Port, LED8_GPIO_Port, LED9_GPIO_Port, LED10_GPIO_Port};

//GPIO_TypeDef *LED_PORT1_NAMES[10] = {LED11_GPIO_Port, LED12_GPIO_Port, LED13_GPIO_Port, LED14_GPIO_Port, LED15_GPIO_Port,
//										LED16_GPIO_Port, LED17_GPIO_Port, LED18_GPIO_Port, LED19_GPIO_Port, LED20_GPIO_Port};

uint16_t LED_PINS_2[10] = {LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin, LED5_Pin,
		LED6_Pin, LED7_Pin, LED8_Pin, LED9_Pin, LED10_Pin};

//uint16_t LED_PINS_1[10] = {LED11_Pin, LED12_Pin, LED13_Pin, LED14_Pin, LED15_Pin,
//		LED16_Pin, LED17_Pin, LED18_Pin, LED19_Pin, LED20_Pin};

GPIO_TypeDef *LED_PORT1_NAMES[10] = {LED20_GPIO_Port, LED19_GPIO_Port, LED18_GPIO_Port, LED17_GPIO_Port, LED16_GPIO_Port,
										LED15_GPIO_Port, LED14_GPIO_Port, LED13_GPIO_Port, LED12_GPIO_Port, LED11_GPIO_Port};

uint16_t LED_PINS_1[10] = {LED20_Pin, LED19_Pin, LED18_Pin, LED17_Pin, LED16_Pin,
		LED15_Pin, LED14_Pin, LED13_Pin, LED12_Pin, LED11_Pin};


uint8_t pwm_lvl[10] = {0, 0, 100, 105, 110, 115, 120, 125, 130, 140};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
#define BUFF_SIZE 256
#define PAYLOAD_LEN 256

uint16_t buffCnt1 = 0, buffCnt2 = 0;
uint8_t payloadData1[BUFF_SIZE], payloadData2[BUFF_SIZE];
uint8_t pData1[BUFF_SIZE], pBuff1[BUFF_SIZE], pData2[BUFF_SIZE], pBuff2[BUFF_SIZE];
uint8_t poorQuality1 = 0, attention1 = 0, meditation1 = 0, bigPacket1 = 0, isDataRead1 = 0;
uint8_t poorQuality2 = 0, attention2 = 0, meditation2 = 0, bigPacket2 = 0, isDataRead2 = 0;

uint8_t llen;
char tt[128];
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)
	{
		if (isDataRead1 == 0)
		{
			for(int i = 0; i < BUFF_SIZE; i++)
			{
				pBuff1[i] = pData1[i];
			}

			isDataRead1 = 1;
		}
	}else if(huart == &huart3)
	{
		if (isDataRead2 == 0)
		{
			for(int i = 0; i < BUFF_SIZE; i++)
			{
				pBuff2[i] = pData2[i];
			}

			isDataRead2 = 1;
		}
	}
}


uint8_t ReadOneByteFromBuff1()
{
	uint8_t retVal = pBuff1[buffCnt1];
	buffCnt1++;
	if(buffCnt1 == PAYLOAD_LEN)
	{
		//HAL_UART_Receive_DMA(&huart4, pData1, BUFF_SIZE);
		buffCnt1 = 0;
		isDataRead1 = 0;
	}
	return retVal;
}

uint8_t ReadOneByteFromBuff2()
{
	uint8_t retVal = pBuff2[buffCnt2];
	buffCnt2++;
	if(buffCnt2 == PAYLOAD_LEN)
	{
		//HAL_UART_Receive_DMA(&huart5, pData2, BUFF_SIZE);
		buffCnt2 = 0;
		isDataRead2 = 0;
	}
	return retVal;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart2, pData1, BUFF_SIZE);
  HAL_UART_Receive_DMA(&huart3, pData2, BUFF_SIZE);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /*
	  while(1)
	  {
		  poorQuality1 = poorQuality2 = rand() % 100;
		  attention1 = attention2 = rand() % 100;
		  meditation1 = meditation2 = rand() % 100;
		  llen = sprintf(tt, "#|1|%u|%u|%u|\n\r", poorQuality1, attention1, meditation1);
		  CDC_Transmit_FS((uint8_t *)tt, llen);
		  HAL_Delay(1000);
		  llen = sprintf(tt, "#|2|%u|%u|%u|\n\r", poorQuality2, attention2, meditation2);
		  CDC_Transmit_FS((uint8_t *)tt, llen);
		  HAL_Delay(1000);
	  }*/

	  while(isDataRead1 == 0 && isDataRead2 == 0);

	  if(isDataRead1 == 1)
	  {
		  if(ReadOneByteFromBuff1() == 170)
		  {
			  if(ReadOneByteFromBuff1() == 170)
			  {
				  uint8_t payLen = ReadOneByteFromBuff1();
				  uint8_t mCheckSum = 0, genCheckSum = 0;
				  if(payLen > 169)
					  break;

				  for(int i = 0; i < payLen; i++)
				  {
					  payloadData1[i] = ReadOneByteFromBuff1();
					  genCheckSum += payloadData1[i];
				  }

				  mCheckSum = ReadOneByteFromBuff1();
				  genCheckSum = 255 - genCheckSum;

				  if(mCheckSum == genCheckSum)
				  {
					  poorQuality1 = 200;
					  attention1 = 0;
					  meditation1 = 0;
					  bigPacket1 = 0;

					  for(int i = 0; i < payLen; i++)
					  {
						  switch(payloadData1[i])
						  {
						  case 2:
							  i++;
							  poorQuality1 = payloadData1[i];
							  bigPacket1 = 1;
							  break;
						  case 3:
							  i++;
							  break;
						  case 4:
							  i++;
							  attention1 = payloadData1[i];
							  break;
						  case 5:
							  i++;
							  meditation1 = payloadData1[i];
							  break;
						  case 0x80:
							  i = i + 23;
							  break;
						  case 0x83:
							  i = i + 25;
							  break;
						  default:
							  break;
						  }
					  }

					  if(bigPacket1 == 1)
					  {
						  //isDataRead = 0;
						  //llen = sprintf(tt, "1 Poor: %u, Att: %u, Med: %u\n\r", poorQuality1, attention1, meditation1);
						  llen = sprintf(tt, "#|1|%u|%u|%u|\n\r", poorQuality1, attention1, meditation1);
						  CDC_Transmit_FS((uint8_t *)tt, llen);

						  if(poorQuality1 == 0)
						  {
							  uint8_t max_lvl = attention1 / 10;
							  max_lvl = max_lvl > 10 ? 10 : max_lvl;
							  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_lvl[max_lvl]);

							  for(int i = 0; i < max_lvl; i++)
							  {
								  LED_PORT1_NAMES[i]->BSRR = LED_PINS_1[i];
							  }

							  for(int i = max_lvl; i < LED_COUNT; i++)
							  {
								  LED_PORT1_NAMES[i]->BRR = LED_PINS_1[i];
							  }

						  }else
						  {
							  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

							  for(int i = 0; i < LED_COUNT; i++)
							  {
								  LED_PORT1_NAMES[i]->BRR = LED_PINS_1[i];
							  }
						  }
					  }else{

					  }

				  }

			  }
		  }

	  }

	  if(isDataRead2 == 1)
	  {
		  if(ReadOneByteFromBuff2() == 170)
		  {
			  if(ReadOneByteFromBuff2() == 170)
			  {
				  uint8_t payLen = ReadOneByteFromBuff2();
				  uint8_t mCheckSum = 0, genCheckSum = 0;
				  if(payLen > 169)
					  break;

				  for(int i = 0; i < payLen; i++)
				  {
					  payloadData2[i] = ReadOneByteFromBuff2();
					  genCheckSum += payloadData2[i];
				  }

				  mCheckSum = ReadOneByteFromBuff2();
				  genCheckSum = 255 - genCheckSum;

				  if(mCheckSum == genCheckSum)
				  {
					  poorQuality2 = 200;
					  attention2 = 0;
					  meditation2 = 0;
					  bigPacket2 = 0;

					  for(int i = 0; i < payLen; i++)
					  {
						  switch(payloadData2[i])
						  {
						  case 2:
							  i++;
							  poorQuality2 = payloadData2[i];
							  bigPacket2 = 1;
							  break;
						  case 3:
							  i++;
							  break;
						  case 4:
							  i++;
							  attention2 = payloadData2[i];
							  break;
						  case 5:
							  i++;
							  meditation2 = payloadData2[i];
							  break;
						  case 0x80:
							  i = i + 23;
							  break;
						  case 0x83:
							  i = i + 25;
							  break;
						  default:
							  break;
						  }
					  }

					  if(bigPacket2 == 1)
					  {
						  //isDataRead = 0;
						  //llen = sprintf(tt, "2 Poor: %u, Att: %u, Med: %u\n\r", poorQuality2, attention2, meditation2);
						  llen = sprintf(tt, "#|2|%u|%u|%u|\n\r", poorQuality2, attention2, meditation2);
						  CDC_Transmit_FS((uint8_t *)tt, llen);

						  if(poorQuality2 == 0)
						  {
							  uint8_t max_lvl = attention2 / 10;
							  max_lvl = max_lvl > 10 ? 10 : max_lvl;
							  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm_lvl[max_lvl]);

							  for(int i = 0; i < max_lvl; i++)
							  {
								  LED_PORT2_NAMES[i]->BSRR = LED_PINS_2[i];
							  }

							  for(int i = max_lvl; i < LED_COUNT; i++)
							  {
								  LED_PORT2_NAMES[i]->BRR = LED_PINS_2[i];
							  }
						  }else{
							  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

							  for(int i = 0; i < LED_COUNT; i++)
							  {
								  LED_PORT2_NAMES[i]->BRR = LED_PINS_2[i];
							  }
						  }

						  bigPacket2 = 0;
					  }else{

					  }

				  }

			  }
		  }
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 281;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 255;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 255;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 281;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 255;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 255;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

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
  HAL_GPIO_WritePin(GPIOC, LED20_Pin|LED19_Pin|LED18_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED17_Pin|LED16_Pin|LED15_Pin|LED14_Pin 
                          |LED13_Pin|LED12_Pin|LED6_Pin|LED5_Pin 
                          |LED4_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED11_Pin|LED10_Pin|LED9_Pin|LED8_Pin 
                          |LED7_Pin|LED2_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED20_Pin LED19_Pin LED18_Pin */
  GPIO_InitStruct.Pin = LED20_Pin|LED19_Pin|LED18_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED17_Pin LED16_Pin LED15_Pin LED14_Pin 
                           LED13_Pin LED12_Pin LED6_Pin LED5_Pin 
                           LED4_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED17_Pin|LED16_Pin|LED15_Pin|LED14_Pin 
                          |LED13_Pin|LED12_Pin|LED6_Pin|LED5_Pin 
                          |LED4_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED11_Pin LED10_Pin LED9_Pin LED8_Pin 
                           LED7_Pin LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED11_Pin|LED10_Pin|LED9_Pin|LED8_Pin 
                          |LED7_Pin|LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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
