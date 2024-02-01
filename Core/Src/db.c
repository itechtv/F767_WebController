/*
 * db.c
 *
 *  Created on: Dec 14, 2022
 *      Author: Anton & Denis
 */

#include <db.h>
#include "stdio.h"
#include "stm32f7xx_hal.h"


struct dbPinsConf PinsConf[NUMPIN];

struct dbPinToPin PinsLinks[NUMPINLINKS];

struct dbCron CronTask[NUMTASK];

struct dbSettings SetSettings;

struct dbCron dbCrontxt[MAXSIZE];

const struct dbPinsInfo PinsInfo[NUMPIN] = {
		// Total 89 pins
<<<<<<< HEAD
		{"PA0", "A", 0,		 GPIO_PIN_0, GPIOA, 1, 1, 0, 0, TIM2, TIM_CHANNEL_1, GPIO_AF1_TIM2},
		{"PA3", "A", 3,		 GPIO_PIN_3, GPIOA, 0, 1, 0, 0, TIM2, TIM_CHANNEL_4, GPIO_AF1_TIM2},
		{"PA4", "A", 4,		 GPIO_PIN_4, GPIOA, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PA5", "A", 5,		 GPIO_PIN_5, GPIOA, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PA6", "A", 6,		 GPIO_PIN_6, GPIOA, 0, 1, 0, 0, TIM3, TIM_CHANNEL_1, GPIO_AF2_TIM3},
		{"PA15", "A", 15,	 GPIO_PIN_15, GPIOA, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
=======
		{"PA0", "A", 0,		 GPIO_PIN_0, GPIOA, 1, 1, 0, 0, TIM2, TIM_CHANNEL_1},
		{"PA3", "A", 3,		 GPIO_PIN_3, GPIOA, 0, 1, 0, 0, TIM2, TIM_CHANNEL_4},
		{"PA4", "A", 4,		 GPIO_PIN_4, GPIOA, 0, 0, 0, 0, NULL, 0},
		{"PA5", "A", 5,		 GPIO_PIN_5, GPIOA, 0, 0, 0, 0, NULL, 0},
		{"PA6", "A", 6,		 GPIO_PIN_6, GPIOA, 0, 1, 0, 0, TIM3, TIM_CHANNEL_1},
		{"PA15", "A", 15,	 GPIO_PIN_15, GPIOA, 0, 0, 0, 0, NULL, 0},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PB0", "B", 0,		 GPIO_PIN_0, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1}, // LD1 [Green] 6
		{"PB1", "B", 1,		 GPIO_PIN_1, GPIOB, 0, 1, 0, 0, TIM3, TIM_CHANNEL_4, GPIO_AF2_TIM3},
		{"PB2", "B", 2,		 GPIO_PIN_2, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB4", "B", 4,		 GPIO_PIN_4, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB5", "B", 5,		 GPIO_PIN_5, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB6", "B", 6,		 GPIO_PIN_6, GPIOB, 1, 0, 0, 1, NULL, 0, GPIO_AF1_TIM1},
		{"PB7", "B", 7,		 GPIO_PIN_7, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1}, // LD2 [Blue] 12
		{"PB8", "B", 8,		 GPIO_PIN_8, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB9", "B", 9,		 GPIO_PIN_9, GPIOB, 0, 0, 1, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB10", "B", 10,	 GPIO_PIN_10, GPIOB, 0, 1, 0, 0, TIM2, TIM_CHANNEL_3, GPIO_AF1_TIM2},
		{"PB11", "B", 11,	 GPIO_PIN_11, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB12", "B", 12,	 GPIO_PIN_12, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PB14", "B", 14,	 GPIO_PIN_14, GPIOB, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1}, // LD3 [Red] 18
		{"PB15", "B", 15,	 GPIO_PIN_15, GPIOB, 0, 1, 0, 0, TIM12, TIM_CHANNEL_2, GPIO_AF9_TIM12},
=======
		{"PB0", "B", 0,		 GPIO_PIN_0, GPIOB, 0, 0, 0, 0, NULL, 0}, // LD1 [Green] 6
		{"PB1", "B", 1,		 GPIO_PIN_1, GPIOB, 0, 1, 0, 0, TIM3, TIM_CHANNEL_4},
		{"PB2", "B", 2,		 GPIO_PIN_2, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB4", "B", 4,		 GPIO_PIN_4, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB5", "B", 5,		 GPIO_PIN_5, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB6", "B", 6,		 GPIO_PIN_6, GPIOB, 1, 0, 0, 1, NULL, 0},
		{"PB7", "B", 7,		 GPIO_PIN_7, GPIOB, 0, 0, 0, 0, NULL, 0}, // LD2 [Blue] 12
		{"PB8", "B", 8,		 GPIO_PIN_8, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB9", "B", 9,		 GPIO_PIN_9, GPIOB, 0, 0, 1, 0, NULL, 0},
		{"PB10", "B", 10,	 GPIO_PIN_10, GPIOB, 0, 1, 0, 0, TIM2, TIM_CHANNEL_2},
		{"PB11", "B", 11,	 GPIO_PIN_11, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB12", "B", 12,	 GPIO_PIN_12, GPIOB, 0, 0, 0, 0, NULL, 0},
		{"PB14", "B", 14,	 GPIO_PIN_14, GPIOB, 0, 0, 0, 0, NULL, 0}, // LD3 [Red] 18
		{"PB15", "B", 15,	 GPIO_PIN_15, GPIOB, 0, 1, 0, 0, TIM12, TIM_CHANNEL_2},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PC0", "C", 0,		 GPIO_PIN_0, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC2", "C", 2,		 GPIO_PIN_2, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC3", "C", 3,		 GPIO_PIN_3, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC6", "C", 6,		 GPIO_PIN_6, GPIOC, 1, 1, 0, 0, TIM8, TIM_CHANNEL_1, GPIO_AF3_TIM8},
		{"PC7", "C", 7,		 GPIO_PIN_7, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_2, GPIO_AF3_TIM8},
		{"PC8", "C", 8,		 GPIO_PIN_8, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_3, GPIO_AF3_TIM8},
		{"PC9", "C", 9,		 GPIO_PIN_9, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_4, GPIO_AF3_TIM8},
		{"PC10", "C", 10,	 GPIO_PIN_10, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC11", "C", 11,	 GPIO_PIN_11, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC12", "C", 12,	 GPIO_PIN_12, GPIOC, 1, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PC13", "C", 13,	 GPIO_PIN_13, GPIOC, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1}, // USER_Btn [B1]
=======
		{"PC0", "C", 0,		 GPIO_PIN_0, GPIOC, 0, 0, 0, 0, NULL, 0},
		{"PC2", "C", 2,		 GPIO_PIN_2, GPIOC, 0, 0, 0, 0, NULL, 0},
		{"PC3", "C", 3,		 GPIO_PIN_3, GPIOC, 0, 0, 0, 0, NULL, 0},
		{"PC6", "C", 6,		 GPIO_PIN_6, GPIOC, 1, 1, 0, 0, TIM8, TIM_CHANNEL_1},//???
		{"PC7", "C", 7,		 GPIO_PIN_7, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_2},//???
		{"PC8", "C", 8,		 GPIO_PIN_8, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_3},//???
		{"PC9", "C", 9,		 GPIO_PIN_9, GPIOC, 0, 1, 0, 0, TIM8, TIM_CHANNEL_4},//???
		{"PC10", "C", 10,	 GPIO_PIN_10, GPIOC, 0, 0, 0, 0, NULL, 0},
		{"PC11", "C", 11,	 GPIO_PIN_11, GPIOC, 0, 0, 0, 0, NULL, 0},
		{"PC12", "C", 12,	 GPIO_PIN_12, GPIOC, 1, 0, 0, 0, NULL, 0},
		{"PC13", "C", 13,	 GPIO_PIN_13, GPIOC, 0, 0, 0, 0, NULL, 0}, // USER_Btn [B1]
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PD0", "D", 0,		 GPIO_PIN_0, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD1", "D", 1,		 GPIO_PIN_1, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD2", "D", 2,		 GPIO_PIN_2, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD3", "D", 3,		 GPIO_PIN_3, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD4", "D", 4,		 GPIO_PIN_4, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD5", "D", 5,		 GPIO_PIN_5, GPIOD, 1, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD6", "D", 6,		 GPIO_PIN_6, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD7", "D", 7,		 GPIO_PIN_7, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
=======
		{"PD0", "D", 0,		 GPIO_PIN_0, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD1", "D", 1,		 GPIO_PIN_1, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD2", "D", 2,		 GPIO_PIN_2, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD3", "D", 3,		 GPIO_PIN_3, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD4", "D", 4,		 GPIO_PIN_4, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD5", "D", 5,		 GPIO_PIN_5, GPIOD, 1, 0, 0, 0, NULL, 0},
		{"PD6", "D", 6,		 GPIO_PIN_6, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD7", "D", 7,		 GPIO_PIN_7, GPIOD, 0, 0, 0, 0, NULL, 0},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git
		//{"PD8", "D", 8, GPIO_PIN_8, GPIOD}, //STLK_RX [STM32F103CBT6_PA3] (usart3)
		//{"PD9", "D", 9, GPIO_PIN_9, GPIOD}, //STLK_TX [STM32F103CBT6_PA2] (usart3)
<<<<<<< HEAD
		{"PD10", "D", 10,	 GPIO_PIN_10, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD11", "D", 11,	 GPIO_PIN_11, GPIOD, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PD12", "D", 12,	 GPIO_PIN_12, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_1, GPIO_AF2_TIM4},
		{"PD13", "D", 13,	 GPIO_PIN_13, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_2, GPIO_AF2_TIM4},
		{"PD14", "D", 14,	 GPIO_PIN_14, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_3, GPIO_AF2_TIM4},
		{"PD15", "D", 15,	 GPIO_PIN_15, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_4, GPIO_AF2_TIM4},
=======
		{"PD10", "D", 10,	 GPIO_PIN_10, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD11", "D", 11,	 GPIO_PIN_11, GPIOD, 0, 0, 0, 0, NULL, 0},
		{"PD12", "D", 12,	 GPIO_PIN_12, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_1},
		{"PD13", "D", 13,	 GPIO_PIN_13, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_2},
		{"PD14", "D", 14,	 GPIO_PIN_14, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_3},
		{"PD15", "D", 15,	 GPIO_PIN_15, GPIOD, 0, 1, 0, 0, TIM4, TIM_CHANNEL_4},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PE0", "E", 0,		 GPIO_PIN_0, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE1", "E", 1,		 GPIO_PIN_1, GPIOE, 1, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE2", "E", 2,		 GPIO_PIN_2, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE3", "E", 3,		 GPIO_PIN_3, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE4", "E", 4,		 GPIO_PIN_4, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE5", "E", 5,		 GPIO_PIN_5, GPIOE, 0, 1, 0, 0, TIM9, TIM_CHANNEL_1, GPIO_AF3_TIM9},
		{"PE6", "E", 6,		 GPIO_PIN_6, GPIOE, 0, 1, 0, 0, TIM9, TIM_CHANNEL_2, GPIO_AF3_TIM9},
		{"PE7", "E", 7,		 GPIO_PIN_7, GPIOE, 1, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE8", "E", 8,		 GPIO_PIN_8, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE9", "E", 9,		 GPIO_PIN_9, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_1, GPIO_AF1_TIM1},
		{"PE10", "E", 10,	 GPIO_PIN_10, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE11", "E", 11,	 GPIO_PIN_11, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_2, GPIO_AF1_TIM1},
		{"PE12", "E", 12,	 GPIO_PIN_12, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PE13", "E", 13,	 GPIO_PIN_13, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_3, GPIO_AF1_TIM1},
		{"PE14", "E", 14,	 GPIO_PIN_14, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_4, GPIO_AF1_TIM1},
		{"PE15", "E", 15,	 GPIO_PIN_15, GPIOE, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
=======
		{"PE0", "E", 0,		 GPIO_PIN_0, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE1", "E", 1,		 GPIO_PIN_1, GPIOE, 1, 0, 0, 0, NULL, 0},
		{"PE2", "E", 2,		 GPIO_PIN_2, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE3", "E", 3,		 GPIO_PIN_3, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE4", "E", 4,		 GPIO_PIN_4, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE5", "E", 5,		 GPIO_PIN_5, GPIOE, 0, 1, 0, 0, TIM9, TIM_CHANNEL_1},
		{"PE6", "E", 6,		 GPIO_PIN_6, GPIOE, 0, 1, 0, 0, TIM9, TIM_CHANNEL_2},
		{"PE7", "E", 7,		 GPIO_PIN_7, GPIOE, 1, 0, 0, 0, NULL, 0},
		{"PE8", "E", 8,		 GPIO_PIN_8, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE9", "E", 9,		 GPIO_PIN_9, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_1},
		{"PE10", "E", 10,	 GPIO_PIN_10, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE11", "E", 11,	 GPIO_PIN_11, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_2},
		{"PE12", "E", 12,	 GPIO_PIN_12, GPIOE, 0, 0, 0, 0, NULL, 0},
		{"PE13", "E", 13,	 GPIO_PIN_13, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_3},
		{"PE14", "E", 14,	 GPIO_PIN_14, GPIOE, 0, 1, 0, 0, TIM1, TIM_CHANNEL_4},
		{"PE15", "E", 15,	 GPIO_PIN_15, GPIOE, 0, 0, 0, 0, NULL, 0},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PF0", "F", 0,		 GPIO_PIN_0, GPIOF, 0, 0, 1, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF1", "F", 1,		 GPIO_PIN_1, GPIOF, 0, 0, 0, 1, NULL, 0, GPIO_AF1_TIM1},
		{"PF2", "F", 2,		 GPIO_PIN_2, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF3", "F", 3,		 GPIO_PIN_3, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF4", "F", 4,		 GPIO_PIN_4, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF5", "F", 5,		 GPIO_PIN_5, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF6", "F", 6,		 GPIO_PIN_6, GPIOF, 0, 1, 0, 0, TIM10, TIM_CHANNEL_1, GPIO_AF3_TIM10},
		{"PF7", "F", 7,		 GPIO_PIN_7, GPIOF, 0, 1, 0, 0, TIM11, TIM_CHANNEL_1, GPIO_AF3_TIM11},
		{"PF8", "F", 8,		 GPIO_PIN_8, GPIOF, 0, 1, 0, 0, TIM13, TIM_CHANNEL_1, GPIO_AF9_TIM13},
		{"PF9", "F", 9,		 GPIO_PIN_9, GPIOF, 0, 1, 0, 0, TIM14, TIM_CHANNEL_1, GPIO_AF9_TIM14},
		{"PF10", "F", 10,	 GPIO_PIN_10, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF11", "F", 11,	 GPIO_PIN_11, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF12", "F", 12,	 GPIO_PIN_12, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF13", "F", 13,	 GPIO_PIN_13, GPIOF, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PF14", "F", 14, 	 GPIO_PIN_14, GPIOF, 0, 0, 0, 1, NULL, 0, GPIO_AF1_TIM1},
		{"PF15", "F", 15,	 GPIO_PIN_15, GPIOF, 0, 0, 1, 0, NULL, 0, GPIO_AF1_TIM1},
=======
		{"PF0", "F", 0,		 GPIO_PIN_0, GPIOF, 0, 0, 1, 0, NULL, 0},
		{"PF1", "F", 1,		 GPIO_PIN_1, GPIOF, 0, 0, 0, 1, NULL, 0},
		{"PF2", "F", 2,		 GPIO_PIN_2, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF3", "F", 3,		 GPIO_PIN_3, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF4", "F", 4,		 GPIO_PIN_4, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF5", "F", 5,		 GPIO_PIN_5, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF6", "F", 6,		 GPIO_PIN_6, GPIOF, 0, 1, 0, 0, TIM10, TIM_CHANNEL_1},
		{"PF7", "F", 7,		 GPIO_PIN_7, GPIOF, 0, 1, 0, 0, TIM11, TIM_CHANNEL_1},
		{"PF8", "F", 8,		 GPIO_PIN_8, GPIOF, 0, 1, 0, 0, TIM13, TIM_CHANNEL_1},
		{"PF9", "F", 9,		 GPIO_PIN_9, GPIOF, 0, 1, 0, 0, TIM14, TIM_CHANNEL_1},
		{"PF10", "F", 10,	 GPIO_PIN_10, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF11", "F", 11,	 GPIO_PIN_11, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF12", "F", 12,	 GPIO_PIN_12, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF13", "F", 13,	 GPIO_PIN_13, GPIOF, 0, 0, 0, 0, NULL, 0},
		{"PF14", "F", 14, 	 GPIO_PIN_14, GPIOF, 0, 0, 0, 1, NULL, 0},
		{"PF15", "F", 15,	 GPIO_PIN_15, GPIOF, 0, 0, 1, 0, NULL, 0},
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git

<<<<<<< HEAD
		{"PG0", "G", 0,		 GPIO_PIN_0, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG1", "G", 1,		 GPIO_PIN_1, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG2", "G", 2,		 GPIO_PIN_2, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG3", "G", 3,		 GPIO_PIN_3, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG4", "G", 4,		 GPIO_PIN_4, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG5", "G", 5,		 GPIO_PIN_5, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG8", "G", 8,		 GPIO_PIN_8, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG9", "G", 9,		 GPIO_PIN_9, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG10", "G", 10,	 GPIO_PIN_10, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG12", "G", 12,	 GPIO_PIN_12, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG14", "G", 14,	 GPIO_PIN_14, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1},
		{"PG15", "G", 15,	 GPIO_PIN_15, GPIOG, 0, 0, 0, 0, NULL, 0, GPIO_AF1_TIM1}
=======
		{"PG0", "G", 0,		 GPIO_PIN_0, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG1", "G", 1,		 GPIO_PIN_1, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG2", "G", 2,		 GPIO_PIN_2, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG3", "G", 3,		 GPIO_PIN_3, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG4", "G", 4,		 GPIO_PIN_4, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG5", "G", 5,		 GPIO_PIN_5, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG8", "G", 8,		 GPIO_PIN_8, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG9", "G", 9,		 GPIO_PIN_9, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG10", "G", 10,	 GPIO_PIN_10, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG12", "G", 12,	 GPIO_PIN_12, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG14", "G", 14,	 GPIO_PIN_14, GPIOG, 0, 0, 0, 0, NULL, 0},
		{"PG15", "G", 15,	 GPIO_PIN_15, GPIOG, 0, 0, 0, 0, NULL, 0}
>>>>>>> branch 'Test' of https://github.com/itechtv/F767_WebController.git
};
