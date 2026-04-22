/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define RX_BUFFER_SIZE 10
//#define TIMER_TIMEOUT 7000  // Timeout in millisecondi (5 secondi)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */

int32_t ic_val1 = 0;
uint32_t ic_val2 = 0;
uint8_t is_first_captured = 0;
uint32_t difference = 0;
float distance = 50;
int cont_a = 0;
char codice_digitato[MAX_CODICE];
uint8_t indice = 0;
//char msg[] ="Put 1";
//char msg1[] ="Put 2";//const char msg[] = "ALARM \r\n";
const char codice_corretto[] = "1926";
//const char* error_msg = "Fault\r\n";
//suint8_t rx_buffer[RX_BUFFER_SIZE];
uint32_t last_message_time = 0;  // Tempo dell'ultimo messaggio "tutto_ok"
uint8_t timer_started = 0;       // Flag per indicare se il timer è stato avviato
uint8_t timeout_flag = 0;        // Flag per indicare se è scaduto il timeout
StatoSistema stato_corrente = STATO_DISARMATO;
char tasto;
uint32_t Alarm = 0x42;
uint32_t Stop = 0x44;
uint32_t Wake = 0x46;
uint32_t rx_buffer;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */
void HCSR04_Trigger(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t verifica_codice(void) {
    return (strcmp(codice_digitato, codice_corretto) == 0);
}

#define KEYPAD_PORT GPIOD

const char tasti[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

uint16_t righe[4] = {r1_Pin, r2_Pin, r3_Pin, r4_Pin}; // R1-R4
uint16_t colonne[4] = {c1_Pin, c2_Pin, c3_Pin, c4_Pin}; // C1-C4
GPIO_TypeDef* colonne_port[4] = {GPIOD, GPIOB, GPIOB, GPIOB};
char leggi_tasto(void) {
    for (int r = 0; r < 4; r++) {
        // Imposta tutte le righe alte
        for (int i = 0; i < 4; i++) {
            HAL_GPIO_WritePin(GPIOD, righe[i], GPIO_PIN_SET);
        }

        // Porta a 0 solo la riga corrente
        HAL_GPIO_WritePin(GPIOD, righe[r], GPIO_PIN_RESET);
        HAL_Delay(1);

        // Scansiona le colonne
        for (int c = 0; c < 4; c++) {
            if (HAL_GPIO_ReadPin(colonne_port[c], colonne[c]) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(colonne_port[c], colonne[c]) == GPIO_PIN_RESET); // attesa rilascio
                HAL_Delay(10); // debounce
                gestisci_tasto(tasti[r][c]);
                return tasti[r][c];
            }
        }
    }
    return '\0'; // Nessun tasto premuto
}

void HCSR04_Trigger(void) {
    HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);  // 1 ms
    HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, GPIO_PIN_SET);
    HAL_Delay(0.01);  // 10 µs
    HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, GPIO_PIN_RESET);
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
  MX_I2C1_Init();
  MX_USB_PCD_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_UART4_Init();
  MX_UART5_Init();

  /* USER CODE BEGIN 2 */
  LCD_Init();
   HAL_GPIO_WritePin(GPIOE, LD6_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); // PC0 ON
   HAL_GPIO_WritePin(GPIOE, LD4_Pin, GPIO_PIN_RESET);


   HAL_UART_Receive_IT(&huart4, (uint8_t*)&rx_buffer, sizeof(uint32_t));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  /* USER CODE END WHILE */
	  	  tasto = leggi_tasto();

	  	        if (tasto != '\0') {

	  	            if (tasto == '#') {
	  	                codice_digitato[indice] = '\0';

	  	                if (verifica_codice()) {
	  	                	LCD_Set_Cursor(1, 0);
	  	                	LCD_Send_String("                ");

	  	                    if (stato_corrente == STATO_DISARMATO) {
	  	                        stato_corrente = STATO_ARMATO;
	  	                        LCD_Set_Cursor(1, 0);
	  	                        LCD_Send_String("ARMATO");
	  	                        HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_SET);   // LED rosso ON
	  	                        HAL_GPIO_WritePin(GPIOE, LD6_Pin, GPIO_PIN_RESET); // LED verde OFF
	  	                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // PC0 OFF
	  	                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // PC1 ON

	  	                    } else {
	  	                        stato_corrente = STATO_DISARMATO;
	  	                        cont_a = 0;
	  	                        LCD_Set_Cursor(1, 0);
	  	                        LCD_Send_String("DISARMATO");
	  	                        timer_started = 0;
	  	                        HAL_TIM_Base_Stop_IT(&htim4);  // Ferma il timer

	  	                        //HAL_TIM_Base_DeInit(&htim4);
	  	                        HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_RESET); // LED blu OFF
	  	                        HAL_GPIO_WritePin(GPIOE, LD6_Pin, GPIO_PIN_SET);   // LED rosso ON
	  	                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); // PC0 ON
	  	                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // PC1 OFF
	  	                        HAL_UART_Transmit(&huart4, (uint8_t*)&Stop, sizeof(uint32_t), HAL_MAX_DELAY);//stop a B
	  	                        HAL_UART_Transmit(&huart5, (uint8_t*)&Stop, sizeof(uint32_t), HAL_MAX_DELAY);//stop a C

	  	                    }
	  	                } //else {
	  	                    //HAL_UART_Transmit(&huart2, (uint8_t*)"Codice errato\r\n", 16, HAL_MAX_DELAY);


	  	                indice = 0;

	  	            } else if (tasto == '*') {
	  	                indice = 0; // reset
	  	            } else {
	  	                if (indice < MAX_CODICE - 1) {
	  	                    codice_digitato[indice++] = tasto;

	  	                }
	  	            }
	  	        }

	  	        // LED di stato
	  	        if (stato_corrente == STATO_ARMATO) {
	  	            HCSR04_Trigger();
	  	                                   HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);



	  	                                  HAL_Delay(10);

	  	                                  if (distance < 30.0f) {

	  	                                      HAL_GPIO_WritePin(GPIOE, LD4_Pin, GPIO_PIN_SET);
	  	                                      cont_a++;
	  	  									if(cont_a == 1){
	  	  										LCD_Set_Cursor(1, 0);
	  	  										LCD_Send_String("ALLARME");

	  	  				                        HAL_UART_Transmit(&huart4, (uint8_t*)&Alarm, sizeof(uint32_t), HAL_MAX_DELAY);
	  	  					                   // 2 è alarm
	  	  					                   //HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_RESET);
	  	  									}
	  	                                  } else {

	  	                                      HAL_GPIO_WritePin(GPIOE, LD4_Pin, GPIO_PIN_RESET);

	  	                                  }

	  	        }
	  	        /*
	  	        if (timeout_flag) {
	  	                    // Timeout scaduto, invia il messaggio di errore a C tramite USART3
	  	                    HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);

	  	                    // Resetta il flag per il prossimo controllo
	  	                    timeout_flag = 0;
	  	                }
	  			*/
	  	        HAL_Delay(50);  // debounce

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 9599;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 44999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin
                          |LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|E_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, r1_Pin|r2_Pin|r3_Pin|r4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DRDY_Pin MEMS_INT3_Pin MEMS_INT4_Pin MEMS_INT1_Pin
                           MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = DRDY_Pin|MEMS_INT3_Pin|MEMS_INT4_Pin|MEMS_INT1_Pin
                          |MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_I2C_SPI_Pin LD4_Pin LD3_Pin LD5_Pin
                           LD7_Pin LD9_Pin LD10_Pin LD8_Pin
                           LD6_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin
                          |LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RS_Pin E_Pin D4_Pin D5_Pin
                           D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = RS_Pin|E_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_MISO_Pin SPI1_MISOA7_Pin */
  GPIO_InitStruct.Pin = SPI1_MISO_Pin|SPI1_MISOA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : trigger_Pin */
  GPIO_InitStruct.Pin = trigger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(trigger_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : r1_Pin r2_Pin r3_Pin r4_Pin */
  GPIO_InitStruct.Pin = r1_Pin|r2_Pin|r3_Pin|r4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : c1_Pin */
  GPIO_InitStruct.Pin = c1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(c1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : c2_Pin c3_Pin c4_Pin */
  GPIO_InitStruct.Pin = c2_Pin|c3_Pin|c4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
        if (is_first_captured == 0) {
            ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            is_first_captured = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
        } else {
            ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
            __HAL_TIM_SET_COUNTER(htim, 0);

            if (ic_val2 > ic_val1)
                difference = ic_val2 - ic_val1;
            else
                difference = (0xFFFF - ic_val1 + ic_val2);

            distance = (float)difference * 0.0343 / 2.0;
            is_first_captured = 0;

            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
            HAL_TIM_IC_Stop_IT(htim, TIM_CHANNEL_4);
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
			if(rx_buffer == 0x21){
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);

            if (!timer_started) {
            	__HAL_TIM_SET_COUNTER(&htim4, 0);
                __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);       //  Azzera anche il flag UIF
                HAL_TIM_Base_Start_IT(&htim4);   // Avvia il timer solo una volta
                timer_started = 1;
            }

            __HAL_TIM_SET_COUNTER(&htim4, 0);    // Azzera il timer ad ogni ricezione


			}
            HAL_UART_Receive_IT(&huart4, (uint8_t*)&rx_buffer, sizeof(uint32_t));

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        if (timer_started) {
            timer_started = 0;
            HAL_TIM_Base_Stop_IT(&htim4);  // Ferma il timer
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);


              HAL_UART_Transmit(&huart5, (uint8_t*)&Wake, sizeof(uint32_t), HAL_MAX_DELAY);
        }
    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
