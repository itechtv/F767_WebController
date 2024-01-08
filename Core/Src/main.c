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
#include "setings.h"

#include "multi_button.h"
extern struct Button button[NUMPIN];
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
data_pin_t data_pin;
uint16_t usbnum = 0;

//const uint32_t DEBOUNCE_DELAY_MS = 150;     // Задержка для защиты от дребезга multi_button
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
osThreadId OutputTaskHandle;
uint32_t OutputTaskBuffer[ 512 ];
osStaticThreadDef_t OutputTaskControlBlock;
osThreadId ConfigTaskHandle;
uint32_t ConfigTaskBuffer[ 512 ];
osStaticThreadDef_t ConfigTaskControlBlock;
osThreadId InputTaskHandle;
uint32_t InputTaskBuffer[ 512 ];
osStaticThreadDef_t InputTaskControlBlock;
osMessageQId myQueueHandle;
uint8_t myQueueBuffer[ 16 * sizeof( struct data_pin_t ) ];
osStaticMessageQDef_t myQueueControlBlock;
osMessageQId usbQueueHandle;
uint8_t usbQueueBuffer[ 16 * sizeof( uint16_t ) ];
osStaticMessageQDef_t usbQueueControlBlock;
/* USER CODE BEGIN PV */

extern struct dbSettings SetSettings;
extern struct dbCron dbCrontxt[MAXSIZE];
extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];
extern struct dbPinToPin PinsLinks[NUMPINLINKS];
extern uint8_t IP_ADDRESS[4];
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
void StartOutputTask(void const * argument);
void StartConfigTask(void const * argument);
void StartInputTask(void const * argument);

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

//////////////////////////////////////????????
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
//  MX_FATFS_Init();
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
  osMessageQStaticDef(usbQueue, 16, uint16_t, usbQueueBuffer, &usbQueueControlBlock);
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

  /* definition and creation of OutputTask */
  osThreadStaticDef(OutputTask, StartOutputTask, osPriorityNormal, 0, 512, OutputTaskBuffer, &OutputTaskControlBlock);
  OutputTaskHandle = osThreadCreate(osThread(OutputTask), NULL);

  /* definition and creation of ConfigTask */
  osThreadStaticDef(ConfigTask, StartConfigTask, osPriorityNormal, 0, 512, ConfigTaskBuffer, &ConfigTaskControlBlock);
  ConfigTaskHandle = osThreadCreate(osThread(ConfigTask), NULL);

  /* definition and creation of InputTask */
  osThreadStaticDef(InputTask, StartInputTask, osPriorityNormal, 0, 512, InputTaskBuffer, &InputTaskControlBlock);
  InputTaskHandle = osThreadCreate(osThread(InputTask), NULL);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
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

//    sntp_time += (2 * 60 * 60); ///Beijing time is 8 hours in East 8 District
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

//    printf("rtc_get_time: c03, test get = %lu\n", get_timestamp());

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
	int pin = 0;
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
					pin = atoi(token2);
					if(pin != 0){
						data_pin.pin = pin-1;
					}
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

			if(k == 2) {
				xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
			}
			k = 0;
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
  ulTaskNotifyTake(0, portMAX_DELAY);  //
  MX_LWIP_Init();

  /* init code for USB_HOST */
//  MX_USB_HOST_Init();
  /* USER CODE BEGIN 5 */

	http_server_init();
	osDelay(1000);

	printf("My IP adress is - %d.%d.%d.%d \r\n",IP_ADDRESS[0],IP_ADDRESS[1],IP_ADDRESS[2],IP_ADDRESS[3]);

	client = mqtt_client_new();
	example_do_connect(client, SetSettings.mqtt_tpc); // Подписались на топик"Zagotovka"
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
		if (sDate.Year != 0) {

			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

			stm.tm_year = sDate.Year; //RTC_Year rang 0-99,but tm_year since 1900
			stm.tm_mon = sDate.Month; //RTC_Month rang 1-12,but tm_mon rang 0-11
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

/* USER CODE BEGIN Header_StartOutputTask */
/**
* @brief Function implementing the OutputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOutputTask */
void StartOutputTask(void const * argument)
{
  /* USER CODE BEGIN StartOutputTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
  /* Infinite loop */
  for(;;)
  {
		if (xQueueReceive(myQueueHandle, &data_pin, portMAX_DELAY) == pdTRUE) {
			if (data_pin.action == 0) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_RESET);
				//printf("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action);
			}
			if (data_pin.action == 1) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_SET);
				//printf("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action);
			}
			if (data_pin.action == 2) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_TogglePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin);
				//printf("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action);
			}
		}
    osDelay(1);
  }
  /* USER CODE END StartOutputTask */
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
	uint8_t  usbflag = 1;
	//FRESULT fresult;
	FILINFO finfo;
	//UINT Byteswritten; // File read/write count



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

				FRESULT fresult = f_stat("setings.ini", &finfo);
				if (fresult == FR_OK) {
					GetSetingsConfig();
					GetCronConfig();
					GetPinConfig();
					GetPinToPin();

					InitPin();

					InitMultibutton();

					xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
					xTaskNotifyGive(SSIDTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ SSIDTask
					xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
					xTaskNotifyGive(OutputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OutputTask
					xTaskNotifyGive(InputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ InputTask

				} else {
					StartSetingsConfig();
					xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
					xTaskNotifyGive(SSIDTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ SSIDTask
					xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
					xTaskNotifyGive(OutputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OutputTask
					xTaskNotifyGive(InputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ InputTask
				}
				usbflag = 0;
			}
			/******************************************************************************************/
			// Функция для чтения целых чисел из очереди
			if (xQueueReceive(usbQueueHandle, &usbnum, portMAX_DELAY) == pdTRUE) {
				switch (usbnum) {
				case 1:
					SetPinConfig();
					break;
				case 2:
					SetSetingsConfig();
					break;
				case 3:
					SetCronConfig();
					break;
				case 4:
					SetPinToPin();
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

/* USER CODE BEGIN Header_StartInputTask */
/**
* @brief Function implementing the InputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInputTask */
void StartInputTask(void const *argument) {
	/* USER CODE BEGIN StartInputTask */
	ulTaskNotifyTake(0, portMAX_DELAY);

	uint8_t pinStates[NUMPIN] = { 0 };
	uint32_t pinTimes[NUMPIN] = { 0 };
	uint32_t millis;
	uint8_t pinLevel[NUMPIN] = { 0 };

	/* Infinite loop */
	for (;;) {
		millis = HAL_GetTick();
		for (uint8_t i = 0; i < NUMPIN; i++) {
			if (PinsConf[i].topin == 1) { // Для 'button'
				button_ticks(&button[i]);
			}
			if (PinsConf[i].topin == 3 && strcmp(PinsConf[i].ptype, "GPIO_PULLDOWN") == 0) { // Для 'switch'
				pinStates[i] = HAL_GPIO_ReadPin(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);
				//printf(" STpin %d \r\n", pinStates[i]);
				if (pinStates[i] == 1 && (millis - pinTimes[i]) >= PinsConf[i].binter && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;
					//printf(" clicks 1 %lu pin %d \r\n", (unsigned long)pinTimes[i], i);

					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						//printf(" IN %d OUT %d \r\n", PinsLinks[a].idin, PinsLinks[a].idout);
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 1;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
				if (pinStates[i] == 0 && (millis - pinTimes[i]) >= PinsConf[i].binter && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;
					//printf(" clicks 1 %lu pin %d \r\n", (unsigned long)pinTimes[i], i);

					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						//printf(" IN %d OUT %d \r\n", PinsLinks[a].idin, PinsLinks[a].idout);
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 0;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
			}
			if (PinsConf[i].topin == 3 && strcmp(PinsConf[i].ptype, "GPIO_PULLUP") == 0) { // Для 'switch'
				pinStates[i] = HAL_GPIO_ReadPin(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);
				//printf(" STpin %d \r\n", pinStates[i]);
				if (pinStates[i] == 1 && (millis - pinTimes[i]) >= PinsConf[i].binter && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;
					//printf(" clicks 1 %lu pin %d \r\n", (unsigned long)pinTimes[i], i);

					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						//printf(" IN %d OUT %d \r\n", PinsLinks[a].idin, PinsLinks[a].idout);
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 1;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
				if (pinStates[i] == 0 && (millis - pinTimes[i]) >= PinsConf[i].binter && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;
					//printf(" clicks 1 %lu pin %d \r\n", (unsigned long)pinTimes[i], i);

					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						//printf(" IN %d OUT %d \r\n", PinsLinks[a].idin, PinsLinks[a].idout);
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 0;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
			}
		}
		//osDelay(5);
	}
	/* USER CODE END StartInputTask */
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
