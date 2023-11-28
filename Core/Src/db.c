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

struct dbSettings SetSettings = {0};

struct dbCron dbCrontxt[MAXSIZE];

const struct dbPinsInfo PinsInfo[NUMPIN] = {
		// Total 89 pins
		{"PA0", "A", 0, GPIO_PIN_0, GPIOA, 1, 0, 0},
		{"PA3", "A", 3, GPIO_PIN_3, GPIOA, 1, 0, 0},
		{"PA4", "A", 4, GPIO_PIN_4, GPIOA, 1, 0, 0},
		{"PA5", "A", 5, GPIO_PIN_5, GPIOA, 1, 0, 0},
		{"PA6", "A", 6, GPIO_PIN_6, GPIOA, 1, 0, 0},
		{"PA15", "A", 15, GPIO_PIN_15, GPIOA, 0, 0, 0},

		{"PB0", "B", 0, GPIO_PIN_0, GPIOB, 0, 0, 0}, // LD1 [Green] 6
		{"PB1", "B", 1, GPIO_PIN_1, GPIOB, 1, 0, 0},
		{"PB2", "B", 2, GPIO_PIN_2, GPIOB, 0, 0, 0},
		{"PB4", "B", 4, GPIO_PIN_4, GPIOB, 0, 0, 0},
		{"PB5", "B", 5, GPIO_PIN_5, GPIOB, 0, 0, 0},
		{"PB6", "B", 6, GPIO_PIN_6, GPIOB, 0, 0, 1},
		{"PB7", "B", 7, GPIO_PIN_7, GPIOB, 0, 0, 0}, // LD2 [Blue] 12
		{"PB8", "B", 8, GPIO_PIN_8, GPIOB, 0, 0, 0},
		{"PB9", "B", 9, GPIO_PIN_9, GPIOB, 0, 1, 0},
		{"PB10", "B", 10, GPIO_PIN_10, GPIOB, 0, 0, 0},
		{"PB11", "B", 11, GPIO_PIN_11, GPIOB, 0, 0, 0},
		{"PB12", "B", 12, GPIO_PIN_12, GPIOB, 0, 0, 0},
		{"PB14", "B", 14, GPIO_PIN_14, GPIOB, 0, 0, 0}, // LD3 [Red] 18
		{"PB15", "B", 15, GPIO_PIN_15, GPIOB, 0, 0, 0},

		{"PC0", "C", 0, GPIO_PIN_0, GPIOC, 1, 0, 0},
		{"PC2", "C", 2, GPIO_PIN_2, GPIOC, 1, 0, 0},
		{"PC3", "C", 3, GPIO_PIN_3, GPIOC, 1, 0, 0},
		{"PC6", "C", 6, GPIO_PIN_6, GPIOC, 0, 0, 0},
		{"PC7", "C", 7, GPIO_PIN_7, GPIOC, 0, 0, 0},
		{"PC8", "C", 8, GPIO_PIN_8, GPIOC, 0, 0, 0},
		{"PC9", "C", 9, GPIO_PIN_9, GPIOC, 0, 0, 0},
		{"PC10", "C", 10, GPIO_PIN_10, GPIOC, 0, 0, 0},
		{"PC11", "C", 11, GPIO_PIN_11, GPIOC, 0, 0, 0},
		{"PC12", "C", 12, GPIO_PIN_12, GPIOC, 0, 0, 0},
		{"PC13", "C", 13, GPIO_PIN_13, GPIOC, 0, 0, 0}, // USER_Btn [B1]

		{"PD0", "D", 0, GPIO_PIN_0, GPIOD, 0, 0, 0},
		{"PD1", "D", 1, GPIO_PIN_1, GPIOD, 0, 0, 0},
		{"PD2", "D", 2, GPIO_PIN_2, GPIOD, 0, 0, 0},
		{"PD3", "D", 3, GPIO_PIN_3, GPIOD, 0, 0, 0},
		{"PD4", "D", 4, GPIO_PIN_4, GPIOD, 0, 0, 0},
		{"PD5", "D", 5, GPIO_PIN_5, GPIOD, 0, 0, 0},
		{"PD6", "D", 6, GPIO_PIN_6, GPIOD, 0, 0, 0},
		{"PD7", "D", 7, GPIO_PIN_7, GPIOD, 0, 0, 0},
		//{"PD8", "D", 8, GPIO_PIN_8, GPIOD}, //STLK_RX [STM32F103CBT6_PA3] (usart3)
		//{"PD9", "D", 9, GPIO_PIN_9, GPIOD}, //STLK_TX [STM32F103CBT6_PA2] (usart3)
		{"PD10", "D", 10, GPIO_PIN_10, GPIOD, 0, 0, 0},
		{"PD11", "D", 11, GPIO_PIN_11, GPIOD, 0, 0, 0},
		{"PD12", "D", 12, GPIO_PIN_12, GPIOD, 0, 0, 0},
		{"PD13", "D", 13, GPIO_PIN_13, GPIOD, 0, 0, 0},
		{"PD14", "D", 14, GPIO_PIN_14, GPIOD, 0, 0, 0},
		{"PD15", "D", 15, GPIO_PIN_15, GPIOD, 0, 0, 0},

		{"PE0", "E", 0, GPIO_PIN_0, GPIOE, 0, 0, 0},
		{"PE1", "E", 1, GPIO_PIN_1, GPIOE, 0, 0, 0},
		{"PE2", "E", 2, GPIO_PIN_2, GPIOE, 0, 0, 0},
		{"PE3", "E", 3, GPIO_PIN_3, GPIOE, 0, 0, 0},
		{"PE4", "E", 4, GPIO_PIN_4, GPIOE, 0, 0, 0},
		{"PE5", "E", 5, GPIO_PIN_5, GPIOE, 0, 0, 0},
		{"PE6", "E", 6, GPIO_PIN_6, GPIOE, 0, 0, 0},
		{"PE7", "E", 7, GPIO_PIN_7, GPIOE, 0, 0, 0},
		{"PE8", "E", 8, GPIO_PIN_8, GPIOE, 0, 0, 0},
		{"PE9", "E", 9, GPIO_PIN_9, GPIOE, 0, 0, 0},
		{"PE10", "E", 10, GPIO_PIN_10, GPIOE, 0, 0, 0},
		{"PE11", "E", 11, GPIO_PIN_11, GPIOE, 0, 0, 0},
		{"PE12", "E", 12, GPIO_PIN_12, GPIOE, 0, 0, 0},
		{"PE13", "E", 13, GPIO_PIN_13, GPIOE, 0, 0, 0},
		{"PE14", "E", 14, GPIO_PIN_14, GPIOE, 0, 0, 0},
		{"PE15", "E", 15, GPIO_PIN_15, GPIOE, 0, 0, 0},

		{"PF0", "F", 0, GPIO_PIN_0, GPIOF, 0, 1, 0},
		{"PF1", "F", 1, GPIO_PIN_1, GPIOF, 0, 0, 1},
		{"PF2", "F", 2, GPIO_PIN_2, GPIOF, 0, 0, 0},
		{"PF3", "F", 3, GPIO_PIN_3, GPIOF, 1, 0, 0},
		{"PF4", "F", 4, GPIO_PIN_4, GPIOF, 1, 0, 0},
		{"PF5", "F", 5, GPIO_PIN_5, GPIOF, 1, 0, 0},
		{"PF6", "F", 6, GPIO_PIN_6, GPIOF, 1, 0, 0},
		{"PF7", "F", 7, GPIO_PIN_7, GPIOF, 1, 0, 0},
		{"PF8", "F", 8, GPIO_PIN_8, GPIOF, 1, 0, 0},
		{"PF9", "F", 9, GPIO_PIN_9, GPIOF, 1, 0, 0},
		{"PF10", "F", 10, GPIO_PIN_10, GPIOF, 1, 0, 0},
		{"PF11", "F", 11, GPIO_PIN_11, GPIOF, 0, 0, 0},
		{"PF12", "F", 12, GPIO_PIN_12, GPIOF, 0, 0, 0},
		{"PF13", "F", 13, GPIO_PIN_13, GPIOF, 0, 0, 0},
		{"PF14", "F", 14, GPIO_PIN_14, GPIOF, 0, 0, 1},
		{"PF15", "F", 15, GPIO_PIN_15, GPIOF, 0, 1, 0},

		{"PG0", "G", 0, GPIO_PIN_0, GPIOG, 0, 0, 0},
		{"PG1", "G", 1, GPIO_PIN_1, GPIOG, 0, 0, 0},
		{"PG2", "G", 2, GPIO_PIN_2, GPIOG, 0, 0, 0},
		{"PG3", "G", 3, GPIO_PIN_3, GPIOG, 0, 0, 0},
		{"PG4", "G", 4, GPIO_PIN_4, GPIOG, 0, 0, 0},
		{"PG5", "G", 5, GPIO_PIN_5, GPIOG, 0, 0, 0},
		{"PG8", "G", 8, GPIO_PIN_8, GPIOG, 0, 0, 0},
		{"PG9", "G", 9, GPIO_PIN_9, GPIOG, 0, 0, 0},
		{"PG10", "G", 10, GPIO_PIN_10, GPIOG, 0, 0, 0},
		{"PG12", "G", 12, GPIO_PIN_12, GPIOG, 0, 0, 0},
		{"PG14", "G", 14, GPIO_PIN_14, GPIOG, 0, 0, 0},
		{"PG15", "G", 15, GPIO_PIN_15, GPIOG, 0, 0, 0}
};
