/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"
#include "fatfs.h"
#include "lwip.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
///////////////////////////////////////////
#include "webserver.h"
#include "lwip/apps/httpd.h"
#include <stdio.h>
#include <string.h>
#include <lwip_mqtt.h>
#include "db.h"
#include "lwdtc.h"
#include "cJSON.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct data_pin_t {
	uint8_t pin;
	uint8_t action;
} data_pin_t;

data_pin_t data_pin;

uint16_t usbnum = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// Cron variable
struct tm *timez;
time_t cronetime;
time_t cronetime_old;

/////////////////////////////////
int i = 0;
char str[40] = { 0 };
char fsbuffer[2000] = { 0 };
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart3;

osThreadId WebServerTaskHandle;
uint32_t WebServerTaskBuffer[ 2048 ];
osStaticThreadDef_t WebServerTaskControlBlock;
osThreadId SSIDTaskHandle;
uint32_t SSIDTaskBuffer[ 256 ];
osStaticThreadDef_t SSIDTaskControlBlock;
osThreadId CronTaskHandle;
uint32_t CronTaskBuffer[ 512 ];
osStaticThreadDef_t CronTaskControlBlock;
osThreadId ActionTaskHandle;
uint32_t ActionTaskBuffer[ 512 ];
osStaticThreadDef_t ActionTaskControlBlock;
osThreadId ConfigTaskHandle;
uint32_t ConfigTaskBuffer[ 512 ];
osStaticThreadDef_t ConfigTaskControlBlock;
osMessageQId myQueueHandle;
uint8_t myQueueBuffer[ 16 * sizeof( struct data_pin_t ) ];
osStaticMessageQDef_t myQueueControlBlock;
osMessageQId usbQueueHandle;
uint8_t myQueue02Buffer[ 16 * sizeof( uint16_t ) ];
osStaticMessageQDef_t myQueue02ControlBlock;
/* USER CODE BEGIN PV */
extern struct dbSettings SetSettings;
extern struct dbCron dbCrontxt[MAXSIZE];
extern struct dbPinsInfo PinsInfo[NUMPIN];

extern ApplicationTypeDef Appli_state;

RTC_TimeTypeDef sTime = { 0 };
RTC_DateTypeDef sDate = { 0 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
void StartWebServerTask(void const * argument);
void StartSSIDTask(void const * argument);
void StartCronTask(void const * argument);
void StartActionTask(void const * argument);
void StartConfigTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//////////
extern struct netif gnetif;
extern char randomSSID[27];

unsigned long Ti;

mqtt_client_t *client;
char pacote[50];
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
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueue */
  osMessageQStaticDef(myQueue, 16, struct data_pin_t, myQueueBuffer, &myQueueControlBlock);
  myQueueHandle = osMessageCreate(osMessageQ(myQueue), NULL);

  /* definition and creation of usbQueue */
  osMessageQStaticDef(usbQueue, 16, uint16_t, myQueue02Buffer, &myQueue02ControlBlock);
  usbQueueHandle = osMessageCreate(osMessageQ(usbQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of WebServerTask */
  osThreadStaticDef(WebServerTask, StartWebServerTask, osPriorityNormal, 0, 2048, WebServerTaskBuffer, &WebServerTaskControlBlock);
  WebServerTaskHandle = osThreadCreate(osThread(WebServerTask), NULL);

  /* definition and creation of SSIDTask */
  osThreadStaticDef(SSIDTask, StartSSIDTask, osPriorityNormal, 0, 256, SSIDTaskBuffer, &SSIDTaskControlBlock);
  SSIDTaskHandle = osThreadCreate(osThread(SSIDTask), NULL);

  /* definition and creation of CronTask */
  osThreadStaticDef(CronTask, StartCronTask, osPriorityNormal, 0, 512, CronTaskBuffer, &CronTaskControlBlock);
  CronTaskHandle = osThreadCreate(osThread(CronTask), NULL);

  /* definition and creation of ActionTask */
  osThreadStaticDef(ActionTask, StartActionTask, osPriorityNormal, 0, 512, ActionTaskBuffer, &ActionTaskControlBlock);
  ActionTaskHandle = osThreadCreate(osThread(ActionTask), NULL);

  /* definition and creation of ConfigTask */
  osThreadStaticDef(ConfigTask, StartConfigTask, osPriorityIdle, 0, 512, ConfigTaskBuffer, &ConfigTaskControlBlock);
  ConfigTaskHandle = osThreadCreate(osThread(ConfigTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

//  RTC_TimeTypeDef sTime = {0};
//  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*************************** Processing functions of SNTP **********************************/
/*!
 * @brief Get the current timestamp
 *
 * @param [in] none
 *
 * @retval current timestamp
 */
uint32_t get_timestamp(void) {
	struct tm stm;
	///The acquisition time must be before the acquisition date

	stm.tm_year = sDate.Year + 100;  //RTC_Year rang 0-99,but tm_year since 1900
	stm.tm_mon = sDate.Month - 1;     //RTC_Month rang 1-12,but tm_mon rang 0-11
	stm.tm_mday = sDate.Date;         //RTC_Date rang 1-31 and tm_mday rang 1-31
	stm.tm_hour = sTime.Hours;       //RTC_Hours rang 0-23 and tm_hour rang 0-23
	stm.tm_min = sTime.Minutes;     //RTC_Minutes rang 0-59 and tm_min rang 0-59
	stm.tm_sec = sTime.Seconds;
	return (mktime(&stm));
}
/*!
 * @brief SNTP Get the processing function of the timestamp
 * Execution conditions: none
 *
 * @param [in]: timestamp obtained by sntp
 *
 * @retval: None
 */
void sntp_set_time(uint32_t sntp_time) {
	char buf[80];

	if (sntp_time == 0) {
		printf("sntp_set_time: wrong!\n");
		return;
	}
	time_t rawtime = sntp_time;

//	sntp_time += (2 * 60 * 60); ///Beijing time is 8 hours in East 8 District
	timez = localtime(&rawtime);

	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", timez);
	printf("%s \n", buf);
	/*
	 * Set the time of RTC
	 */
	sTime.Hours = timez->tm_hour;
	sTime.Minutes = timez->tm_min;
	sTime.Seconds = timez->tm_sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	/*
	 * Set the date of RTC
	 */
	sDate.WeekDay = timez->tm_wday;
	sDate.Month = (timez->tm_mon) + 1;
	sDate.Date = timez->tm_mday;
	sDate.Year = (timez->tm_year) - 100;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	printf("RTC time: 20%d-%02d-%02d %d:%d:%d\n", sDate.Year, sDate.Month,
			sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);

//	printf("rtc_get_time: c03, test get = %lu\n", get_timestamp());

}
/*************************** END OF SNTP **********************************/

PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART1 and Loop until the end of transmission */

	HAL_UART_Transmit(&huart3, (uint8_t*) &ch, 1, 0xFFFF);

	return ch;
}

// int pause  0 - до паузы 1 - после паузы
void parse_string(char *str, time_t cronetime_olds, int i, int pause) {
	char *token;
	char *saveptr;
	int flag = 0;
	int k = 0;
	char delim[] = ";";

	// Разбиваем строку на элементы, разделенные точкой с запятой
	token = strtok_r(str, delim, &saveptr);
	while (token != NULL) {
		char *end_token;
		// Если нашли элемент "p", устанавливаем флаг

		if (token[0] == 'p') {
			char *newstring = token + 1;
			//printf("Pause %d seconds\n", atoi(newstring));
			dbCrontxt[i].ptime = cronetime_olds + atoi(newstring);
			flag = 1;
		}
		// в зависимости от флага отправляем в очередь до или после паузы
		if (flag == pause) {
			//printf("%s\n", token);

			//strcpy(data_pin.message, pch);

			char *token2 = strtok_r(token, ":", &end_token);
			//printf("pin = %d\n", atoi(token2));

			while (token2 != NULL) {
				// тут отправляем в очередь
				if (k == 0) {
					data_pin.pin = atoi(token2);
					//printf("pin = %s\n", token2);
				}
				if (k == 1) {
					data_pin.action = atoi(token2);
					//printf("action = %s\n", token2);
				}

				token2 = strtok_r(NULL, ":", &end_token);
				k++;
				// printf("action = %d\n", atoi(token2));
			}
			k = 0;

			xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
		}
		token = strtok_r(NULL, delim, &saveptr);
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartWebServerTask */
/**
 * @brief  Function implementing the WebServerTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartWebServerTask */
void StartWebServerTask(void const * argument)
{
  /* init code for LWIP */
	ulTaskNotifyTake(0, portMAX_DELAY);
	MX_LWIP_Init();

  /* init code for USB_HOST */
  /* USER CODE BEGIN 5 */
	http_server_init();
	osDelay(1000);

	client = mqtt_client_new();
	example_do_connect(client, "test"); // Подписались на топик"Zagotovka"
	//sprintf(pacote, "Cool, MQTT-client is working!"); // Cобщение на 'MQTT' сервер.
	//example_publish(client, pacote); // Публикуем сообщение.

	osDelay(1000);
	bsp_sntp_init();

	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartSSIDTask */
/**
 * @brief Function implementing the SSIDTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSSIDTask */
void StartSSIDTask(void const * argument)
{
  /* USER CODE BEGIN StartSSIDTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
	//
	/* Infinite loop */
	for (;;) {
		if (strlen(randomSSID) != 0) {
			if (HAL_GetTick() - Ti >= 500000) {
				Ti = HAL_GetTick();
				memset(&randomSSID, '\0', sizeof(randomSSID));
				//printf("StartTaskToken1 \n");
			}
		}
		osDelay(1);
	}
  /* USER CODE END StartSSIDTask */
}

/* USER CODE BEGIN Header_StartCronTask */
/**
 * @brief Function implementing the CronTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartCronTask */
void StartCronTask(void const * argument)
{
  /* USER CODE BEGIN StartCronTask */
	ulTaskNotifyTake(0, portMAX_DELAY);

	static lwdtc_cron_ctx_t cron_ctxs[MAXSIZE];

	/* Define context for CRON, used to parse data to */
	size_t fail_index;
	printf("Count task %d\r\n", LWDTC_ARRAYSIZE(dbCrontxt));
	/* Parse all cron strings */
	if (lwdtc_cron_parse_multi(cron_ctxs, dbCrontxt, MAXSIZE, &fail_index)
			!= lwdtcOK) {
		printf("Failed to parse cron at index %d\r\n", (int) fail_index);
	}
	printf("CRONs parsed and ready to go\r\n");

	struct tm stm;
	/* Infinite loop */
	for (;;) {
		if (stm.tm_year != 0) {

			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

			stm.tm_year = sDate.Year + 100; //RTC_Year rang 0-99,but tm_year since 1900
			stm.tm_mon = sDate.Month - 1; //RTC_Month rang 1-12,but tm_mon rang 0-11
			stm.tm_mday = sDate.Date; //RTC_Date rang 1-31 and tm_mday rang 1-31
			stm.tm_hour = sTime.Hours; //RTC_Hours rang 0-23 and tm_hour rang 0-23
			stm.tm_min = sTime.Minutes; //RTC_Minutes rang 0-59 and tm_min rang 0-59
			stm.tm_sec = sTime.Seconds;

			cronetime = mktime(&stm);

			if (cronetime != cronetime_old) {
				cronetime_old = cronetime;
				timez = localtime(&cronetime);
				i = 0;

				while (i < LWDTC_ARRAYSIZE(dbCrontxt)) {
					if (cronetime >= dbCrontxt[i].ptime
							&& dbCrontxt[i].ptime != 0) {

						strcpy(str, dbCrontxt[i].activ);
						parse_string(str, cronetime_old, i, 1);
						dbCrontxt[i].ptime = 0;
					}
					i++;
				}
				i = 0;

				/* Check if CRON should execute */
				while (i < LWDTC_ARRAYSIZE(cron_ctxs)) {
					if (lwdtc_cron_is_valid_for_time(timez, cron_ctxs, &i)
							== lwdtcOK) {

						strcpy(str, dbCrontxt[i].activ);
						parse_string(str, cronetime_old, i, 0);
						//xQueueSend(myQueueHandle, &i, 0);
					}
					i++;
				}
			}
			osDelay(1);
		}
	}
  /* USER CODE END StartCronTask */
}

/* USER CODE BEGIN Header_StartActionTask */
/**
 * @brief Function implementing the ActionTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartActionTask */
void StartActionTask(void const * argument)
{
  /* USER CODE BEGIN StartActionTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
	//

	/* Infinite loop */
	for (;;) {
		if (xQueueReceive(myQueueHandle, &data_pin, portMAX_DELAY) == pdTRUE) {
			if (data_pin.action == 0) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name,
						PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_RESET);
			}
			if (data_pin.action == 1) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name,
						PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_SET);
			}
			if (data_pin.action == 2) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_TogglePin(PinsInfo[data_pin.pin].gpio_name,
						PinsInfo[data_pin.pin].hal_pin);
			}
		}
		osDelay(1);
	}
  /* USER CODE END StartActionTask */
}

/* USER CODE BEGIN Header_StartConfigTask */
/**
 * @brief Function implementing the ConfigTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartConfigTask */
void StartConfigTask(void const * argument)
{
  /* USER CODE BEGIN StartConfigTask */
	int usbflag = 1;
	FRESULT fresult;
	FILINFO finfo;
	UINT Byteswritten; // File read/write count

	cJSON *root_obj = NULL;
	char *out_str = NULL;

	MX_FATFS_Init();
	/* init code for USB_HOST */

	MX_USB_HOST_Init();
	/* Infinite loop */
	for (;;) {
		switch (Appli_state) {
		case APPLICATION_READY:
			if (usbflag == 1) {
				osDelay(1000);
				printf("APPLICATION_READY! \r\n");

				fresult = f_stat("setings.ini", &finfo);
				if (fresult == FR_OK) {
					// если файл существует, открываем его и перезаписываем
					if (f_open(&USBHFile, (const TCHAR*) "setings.ini", FA_READ)
							== FR_OK) {

						fresult = f_read(&USBHFile, fsbuffer, sizeof(fsbuffer),
								&Byteswritten);

						cJSON *root_obj = cJSON_Parse(fsbuffer);
						cJSON *adm_name = cJSON_GetObjectItem(root_obj,
								"adm_name");
						cJSON *adm_pswd = cJSON_GetObjectItem(root_obj,
								"adm_pswd");
						cJSON *lang = cJSON_GetObjectItem(root_obj, "lang");
						cJSON *timezone = cJSON_GetObjectItem(root_obj,
								"timezone");
						cJSON *lon_de = cJSON_GetObjectItem(root_obj, "lon_de");
						cJSON *lat_de = cJSON_GetObjectItem(root_obj, "lat_de");
						cJSON *ip1_sntp0 = cJSON_GetObjectItem(root_obj,
								"ip1_sntp0");
						cJSON *ip1_sntp1 = cJSON_GetObjectItem(root_obj,
								"ip1_sntp1");
						cJSON *ip1_sntp2 = cJSON_GetObjectItem(root_obj,
								"ip1_sntp2");
						cJSON *ip1_sntp3 = cJSON_GetObjectItem(root_obj,
								"ip1_sntp3");
						cJSON *ip2_sntp0 = cJSON_GetObjectItem(root_obj,
								"ip2_sntp0");
						cJSON *ip2_sntp1 = cJSON_GetObjectItem(root_obj,
								"ip2_sntp1");
						cJSON *ip2_sntp2 = cJSON_GetObjectItem(root_obj,
								"ip2_sntp2");
						cJSON *ip2_sntp3 = cJSON_GetObjectItem(root_obj,
								"ip2_sntp3");
						cJSON *ip3_sntp0 = cJSON_GetObjectItem(root_obj,
								"ip3_sntp0");
						cJSON *ip3_sntp1 = cJSON_GetObjectItem(root_obj,
								"ip3_sntp1");
						cJSON *ip3_sntp2 = cJSON_GetObjectItem(root_obj,
								"ip3_sntp2");
						cJSON *ip3_sntp3 = cJSON_GetObjectItem(root_obj,
								"ip3_sntp3");
						// Настройки MQTT
						cJSON *check_mqtt = cJSON_GetObjectItem(root_obj,
								"check_mqtt");
						cJSON *mqtt_prt = cJSON_GetObjectItem(root_obj,
								"mqtt_prt");
						cJSON *mqtt_clt = cJSON_GetObjectItem(root_obj,
								"mqtt_clt");
						cJSON *mqtt_usr = cJSON_GetObjectItem(root_obj,
								"mqtt_usr");
						cJSON *mqtt_pswd = cJSON_GetObjectItem(root_obj,
								"mqtt_pswd");
						cJSON *mqtt_tpc = cJSON_GetObjectItem(root_obj,
								"mqtt_tpc");
						cJSON *mqtt_ftpc = cJSON_GetObjectItem(root_obj,
								"mqtt_ftpc");
						cJSON *mqtt_hst0 = cJSON_GetObjectItem(root_obj,
								"mqtt_hst0");
						cJSON *mqtt_hst1 = cJSON_GetObjectItem(root_obj,
								"mqtt_hst1");
						cJSON *mqtt_hst2 = cJSON_GetObjectItem(root_obj,
								"mqtt_hst2");
						cJSON *mqtt_hst3 = cJSON_GetObjectItem(root_obj,
								"mqtt_hst3");
						// Настройки IP адреса
						cJSON *check_ip = cJSON_GetObjectItem(root_obj,
								"check_ip");
						cJSON *ip_addr0 = cJSON_GetObjectItem(root_obj,
								"ip_addr0");
						cJSON *ip_addr1 = cJSON_GetObjectItem(root_obj,
								"ip_addr1");
						cJSON *ip_addr2 = cJSON_GetObjectItem(root_obj,
								"ip_addr2");
						cJSON *ip_addr3 = cJSON_GetObjectItem(root_obj,
								"ip_addr3");
						cJSON *sb_mask0 = cJSON_GetObjectItem(root_obj,
								"sb_mask0");
						cJSON *sb_mask1 = cJSON_GetObjectItem(root_obj,
								"sb_mask1");
						cJSON *sb_mask2 = cJSON_GetObjectItem(root_obj,
								"sb_mask2");
						cJSON *sb_mask3 = cJSON_GetObjectItem(root_obj,
								"sb_mask3");
						cJSON *gateway0 = cJSON_GetObjectItem(root_obj,
								"gateway0");
						cJSON *gateway1 = cJSON_GetObjectItem(root_obj,
								"gateway1");
						cJSON *gateway2 = cJSON_GetObjectItem(root_obj,
								"gateway2");
						cJSON *gateway3 = cJSON_GetObjectItem(root_obj,
								"gateway3");
						cJSON *macaddr0 = cJSON_GetObjectItem(root_obj,
								"macaddr0");
						cJSON *macaddr1 = cJSON_GetObjectItem(root_obj,
								"macaddr1");
						cJSON *macaddr2 = cJSON_GetObjectItem(root_obj,
								"macaddr2");
						cJSON *macaddr3 = cJSON_GetObjectItem(root_obj,
								"macaddr3");
						cJSON *macaddr4 = cJSON_GetObjectItem(root_obj,
								"macaddr4");
						cJSON *macaddr5 = cJSON_GetObjectItem(root_obj,
								"macaddr5");

						strcpy(SetSettings.adm_name, adm_name->valuestring);
						strcpy(SetSettings.adm_pswd, adm_pswd->valuestring);
						strcpy(SetSettings.lang, lang->valuestring);

						SetSettings.timezone = timezone->valueint;
						SetSettings.lon_de = lon_de->valueint;
						SetSettings.lat_de = lat_de->valueint;

						SetSettings.ip1_sntp0 = ip1_sntp0->valueint;
						SetSettings.ip1_sntp1 = ip1_sntp1->valueint;
						SetSettings.ip1_sntp2 = ip1_sntp2->valueint;
						SetSettings.ip1_sntp3 = ip1_sntp3->valueint;
						SetSettings.ip2_sntp0 = ip2_sntp0->valueint;
						SetSettings.ip2_sntp1 = ip2_sntp1->valueint;
						SetSettings.ip2_sntp2 = ip2_sntp2->valueint;
						SetSettings.ip2_sntp3 = ip2_sntp3->valueint;
						SetSettings.ip3_sntp0 = ip3_sntp0->valueint;
						SetSettings.ip3_sntp1 = ip3_sntp1->valueint;
						SetSettings.ip3_sntp2 = ip3_sntp2->valueint;
						SetSettings.ip3_sntp3 = ip3_sntp3->valueint;

						SetSettings.check_mqtt = check_mqtt->valueint;
						SetSettings.mqtt_prt = mqtt_prt->valueint;

						strcpy(SetSettings.mqtt_clt, mqtt_clt->valuestring);
						strcpy(SetSettings.mqtt_usr, mqtt_usr->valuestring);
						strcpy(SetSettings.mqtt_pswd, mqtt_pswd->valuestring);
						strcpy(SetSettings.mqtt_tpc, mqtt_tpc->valuestring);
						strcpy(SetSettings.mqtt_ftpc, mqtt_ftpc->valuestring);

						SetSettings.mqtt_hst0 = mqtt_hst0->valueint;
						SetSettings.mqtt_hst1 = mqtt_hst1->valueint;
						SetSettings.mqtt_hst2 = mqtt_hst2->valueint;
						SetSettings.mqtt_hst3 = mqtt_hst3->valueint;

						SetSettings.check_ip = check_ip->valueint;
						SetSettings.ip_addr0 = ip_addr0->valueint;
						SetSettings.ip_addr1 = ip_addr1->valueint;
						SetSettings.ip_addr2 = ip_addr2->valueint;
						SetSettings.ip_addr3 = ip_addr3->valueint;
						SetSettings.sb_mask0 = sb_mask0->valueint;
						SetSettings.sb_mask1 = sb_mask1->valueint;
						SetSettings.sb_mask2 = sb_mask2->valueint;
						SetSettings.sb_mask3 = sb_mask3->valueint;
						SetSettings.gateway0 = gateway0->valueint;
						SetSettings.gateway1 = gateway1->valueint;
						SetSettings.gateway2 = gateway2->valueint;
						SetSettings.gateway3 = gateway3->valueint;
						SetSettings.macaddr0 = macaddr0->valueint;
						SetSettings.macaddr1 = macaddr1->valueint;
						SetSettings.macaddr2 = macaddr2->valueint;
						SetSettings.macaddr3 = macaddr3->valueint;
						SetSettings.macaddr4 = macaddr4->valueint;
						SetSettings.macaddr5 = macaddr5->valueint;

						cJSON_Delete(root_obj);

						f_close(&USBHFile);

						xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
						xTaskNotifyGive(SSIDTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ SSIDTask
						xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
						xTaskNotifyGive(ActionTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ ActionTask

					}
				} else {
					// если файл не существует, создаем его и записываем данные
					if (f_open(&USBHFile, (const TCHAR*) "setings.ini",
							FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
						printf("f_open! create setings.ini \r\n");
						root_obj = cJSON_CreateObject();

						cJSON_AddStringToObject(root_obj, "adm_name", "admin");
						cJSON_AddStringToObject(root_obj, "adm_pswd",
								"12345678"); // Пароль для авторизации
						cJSON_AddStringToObject(root_obj, "lang", "ru"); //
						cJSON_AddNumberToObject(root_obj, "timezone", 0);// UTC
						cJSON_AddNumberToObject(root_obj, "lon_de", 0); // Longitude / Долгота
						cJSON_AddNumberToObject(root_obj, "lat_de", 0); // Latitude / Широта
						cJSON_AddNumberToObject(root_obj, "ip1_sntp0", 0);// SMTP Server primary
						cJSON_AddNumberToObject(root_obj, "ip1_sntp1", 0); // SMTP Server primary
						cJSON_AddNumberToObject(root_obj, "ip1_sntp2", 0); // SMTP Server primary
						cJSON_AddNumberToObject(root_obj, "ip1_sntp3", 0);// SMTP Server primary
						cJSON_AddNumberToObject(root_obj, "ip2_sntp0", 0); // SMTP Server secondary
						cJSON_AddNumberToObject(root_obj, "ip2_sntp1", 0);// SMTP Server secondary
						cJSON_AddNumberToObject(root_obj, "ip2_sntp2", 0); // SMTP Server secondary
						cJSON_AddNumberToObject(root_obj, "ip2_sntp3", 0);// SMTP Server secondary
						cJSON_AddNumberToObject(root_obj, "ip3_sntp0", 0);// SMTP Server teriary
						cJSON_AddNumberToObject(root_obj, "ip3_sntp1", 0); // SMTP Server teriary
						cJSON_AddNumberToObject(root_obj, "ip3_sntp2", 0); // SMTP Server teriary
						cJSON_AddNumberToObject(root_obj, "ip3_sntp3", 0); // SMTP Server teriary
						cJSON_AddNumberToObject(root_obj, "check_mqtt", 0); // check MQTT on/off
						cJSON_AddNumberToObject(root_obj, "mqtt_prt", 0); // Your MQTT broker port (default port is set to 1883)
						cJSON_AddStringToObject(root_obj, "mqtt_clt", ""); // Device's unique identifier.
						cJSON_AddStringToObject(root_obj, "mqtt_usr", ""); // MQTT Имя пользователя для авторизации
						cJSON_AddStringToObject(root_obj, "mqtt_pswd", ""); // MQTT Пароль для авторизации
						cJSON_AddStringToObject(root_obj, "mqtt_tpc", ""); // Unique identifying topic for your device (kitchen-light) It is recommended to use a single word for the topic.
						cJSON_AddStringToObject(root_obj, "mqtt_ftpc", ""); // Полный топик for example lights/%prefix%/%topic%/
						cJSON_AddNumberToObject(root_obj, "mqtt_hst0", 0); // Your MQTT broker address or IP
						cJSON_AddNumberToObject(root_obj, "mqtt_hst1", 0); // Your MQTT broker address or IP
						cJSON_AddNumberToObject(root_obj, "mqtt_hst2", 0); // Your MQTT broker address or IP
						cJSON_AddNumberToObject(root_obj, "mqtt_hst3", 0); // Your MQTT broker address or IP
						// Настройки IP адреса
						cJSON_AddNumberToObject(root_obj, "check_ip", 0); // check DHCP on/off
						cJSON_AddNumberToObject(root_obj, "ip_addr0", 192); // IP адрес
						cJSON_AddNumberToObject(root_obj, "ip_addr1", 168); // IP адрес
						cJSON_AddNumberToObject(root_obj, "ip_addr2", 18); // IP адрес
						cJSON_AddNumberToObject(root_obj, "ip_addr3", 88); // IP адрес
						cJSON_AddNumberToObject(root_obj, "sb_mask0", 255);	// Маска сети
						cJSON_AddNumberToObject(root_obj, "sb_mask1", 255);	// Маска сети
						cJSON_AddNumberToObject(root_obj, "sb_mask2", 255);	// Маска сети
						cJSON_AddNumberToObject(root_obj, "sb_mask3", 0);// Маска сети
						cJSON_AddNumberToObject(root_obj, "gateway0", 192); // Шлюз
						cJSON_AddNumberToObject(root_obj, "gateway1", 168); // Шлюз
						cJSON_AddNumberToObject(root_obj, "gateway2", 18); // Шлюз
						cJSON_AddNumberToObject(root_obj, "gateway3", 1); // Шлюз
						cJSON_AddNumberToObject(root_obj, "macaddr0", 0);// MAC address
						cJSON_AddNumberToObject(root_obj, "macaddr1", 0);// MAC address
						cJSON_AddNumberToObject(root_obj, "macaddr2", 0);// MAC address
						cJSON_AddNumberToObject(root_obj, "macaddr3", 0);// MAC address
						cJSON_AddNumberToObject(root_obj, "macaddr4", 0);// MAC address
						cJSON_AddNumberToObject(root_obj, "macaddr5", 0);// MAC address

						out_str = cJSON_PrintUnformatted(root_obj);

						//sprintf(tbuf,"LAN zagotovka, USB task added information to Relays - score000000\r\n"); // данные которые запишем в файл!
						fresult = f_write(&USBHFile, (const void*) out_str,
								strlen(out_str), &Byteswritten);

						printf("f_open! setings.ini \r\n");

						cJSON_Delete(root_obj);
						f_close(&USBHFile);

						strcpy(SetSettings.lang, "ru");
						strcpy(SetSettings.adm_name, "admin");
						strcpy(SetSettings.adm_pswd, "12345678");
						SetSettings.ip_addr0 = 192;
						SetSettings.ip_addr1 = 168;
						SetSettings.ip_addr2 = 18;
						SetSettings.ip_addr3 = 80;
						SetSettings.sb_mask0 = 255;
						SetSettings.sb_mask1 = 255;
						SetSettings.sb_mask2 = 255;
						SetSettings.sb_mask3 = 0;
						SetSettings.gateway0 = 192;
						SetSettings.gateway1 = 168;
						SetSettings.gateway2 = 18;
						SetSettings.gateway3 = 1;
						SetSettings.mqtt_prt = 1883;

						xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
						xTaskNotifyGive(SSIDTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ SSIDTask
						xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
						xTaskNotifyGive(ActionTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ ActionTask
					}
				}

				usbflag = 0;
			}

/******************************************************************************************/
			// Функция для чтения целых чисел из очереди
				if (xQueueReceive(usbQueueHandle, &usbnum,portMAX_DELAY) == pdTRUE) {
					switch (Appli_state) {
					case 1:
						break;
					case 2:
						if (f_open(&USBHFile, (const TCHAR*) "setings.ini", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

							root_obj = cJSON_CreateObject();
							cJSON_AddStringToObject(root_obj, "adm_name", SetSettings.adm_name);
							cJSON_AddStringToObject(root_obj, "adm_pswd", SetSettings.adm_pswd);
							cJSON_AddStringToObject(root_obj, "lang", SetSettings.lang);
							cJSON_AddNumberToObject(root_obj, "timezone", SetSettings.timezone);
							cJSON_AddNumberToObject(root_obj, "lon_de", SetSettings.lon_de);
							cJSON_AddNumberToObject(root_obj, "lat_de", SetSettings.lat_de);
							cJSON_AddNumberToObject(root_obj, "ip1_sntp0", SetSettings.ip1_sntp0);
							cJSON_AddNumberToObject(root_obj, "ip1_sntp1", SetSettings.ip1_sntp1);
							cJSON_AddNumberToObject(root_obj, "ip1_sntp2", SetSettings.ip1_sntp2);
							cJSON_AddNumberToObject(root_obj, "ip1_sntp3", SetSettings.ip1_sntp3);
							cJSON_AddNumberToObject(root_obj, "ip2_sntp0", SetSettings.ip2_sntp0);
							cJSON_AddNumberToObject(root_obj, "ip2_sntp1", SetSettings.ip2_sntp1);
							cJSON_AddNumberToObject(root_obj, "ip2_sntp2", SetSettings.ip2_sntp2);
							cJSON_AddNumberToObject(root_obj, "ip2_sntp3", SetSettings.ip2_sntp3);
							cJSON_AddNumberToObject(root_obj, "ip3_sntp0", SetSettings.ip3_sntp0);
							cJSON_AddNumberToObject(root_obj, "ip3_sntp1", SetSettings.ip3_sntp1);
							cJSON_AddNumberToObject(root_obj, "ip3_sntp2", SetSettings.ip3_sntp2);
							cJSON_AddNumberToObject(root_obj, "ip3_sntp3", SetSettings.ip3_sntp3);
							cJSON_AddNumberToObject(root_obj, "check_mqtt", SetSettings.check_mqtt);
							cJSON_AddNumberToObject(root_obj, "mqtt_prt", SetSettings.mqtt_prt);
							cJSON_AddStringToObject(root_obj, "mqtt_clt", SetSettings.mqtt_clt);
							cJSON_AddStringToObject(root_obj, "mqtt_usr", SetSettings.mqtt_usr);
							cJSON_AddStringToObject(root_obj, "mqtt_pswd", SetSettings.mqtt_pswd);
							cJSON_AddStringToObject(root_obj, "mqtt_tpc", SetSettings.mqtt_tpc);
							cJSON_AddStringToObject(root_obj, "mqtt_ftpc", SetSettings.mqtt_ftpc);
							cJSON_AddNumberToObject(root_obj, "mqtt_hst0", SetSettings.mqtt_hst0);
							cJSON_AddNumberToObject(root_obj, "mqtt_hst1", SetSettings.mqtt_hst1);
							cJSON_AddNumberToObject(root_obj, "mqtt_hst2", SetSettings.mqtt_hst2);
							cJSON_AddNumberToObject(root_obj, "mqtt_hst3", SetSettings.mqtt_hst3);
							cJSON_AddNumberToObject(root_obj, "check_ip", SetSettings.check_ip);
							cJSON_AddNumberToObject(root_obj, "ip_addr0", SetSettings.ip_addr0);
							cJSON_AddNumberToObject(root_obj, "ip_addr1", SetSettings.ip_addr1);
							cJSON_AddNumberToObject(root_obj, "ip_addr2", SetSettings.ip_addr2);
							cJSON_AddNumberToObject(root_obj, "ip_addr3", SetSettings.ip_addr3);
							cJSON_AddNumberToObject(root_obj, "sb_mask0", SetSettings.sb_mask0);
							cJSON_AddNumberToObject(root_obj, "sb_mask1", SetSettings.sb_mask1);
							cJSON_AddNumberToObject(root_obj, "sb_mask2", SetSettings.sb_mask2);
							cJSON_AddNumberToObject(root_obj, "sb_mask3", SetSettings.sb_mask3);
							cJSON_AddNumberToObject(root_obj, "gateway0", SetSettings.gateway0);
							cJSON_AddNumberToObject(root_obj, "gateway1", SetSettings.gateway1);
							cJSON_AddNumberToObject(root_obj, "gateway2", SetSettings.gateway2);
							cJSON_AddNumberToObject(root_obj, "gateway3", SetSettings.gateway3);
							cJSON_AddNumberToObject(root_obj, "macaddr0", SetSettings.macaddr0);
							cJSON_AddNumberToObject(root_obj, "macaddr1", SetSettings.macaddr1);
							cJSON_AddNumberToObject(root_obj, "macaddr2", SetSettings.macaddr2);
							cJSON_AddNumberToObject(root_obj, "macaddr3", SetSettings.macaddr3);
							cJSON_AddNumberToObject(root_obj, "macaddr4", SetSettings.macaddr4);
							cJSON_AddNumberToObject(root_obj, "macaddr5", SetSettings.macaddr5);

							out_str = cJSON_PrintUnformatted(root_obj);
							fresult = f_write(&USBHFile, (const void*) out_str, strlen(out_str), &Byteswritten);

							cJSON_Delete(root_obj);
							f_close(&USBHFile);

						}
						break;
					case 3:
						break;
					default:
						//printf("Wrong data! \r\n");
						break;
					}
					printf("+++ Received number: %u\n", usbnum);
				}
/******************************************************************************************/

			break;
		default:
			//printf("Wrong data! \r\n");
			break;
		}
		osDelay(1);
	}
  /* USER CODE END StartConfigTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
