/*
 * ds18b20.h
 *
 *	The MIT License.
 *  Created on: 20.09.2018
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *      mateusz@msalamon.pl
 *
 */
#ifndef	_DS18B20_H
#define	_DS18B20_H

#include "main.h"
#include "stm32f7xx.h"
#include "onewire.h"
#include <db.h>
//extern TIM_HandleTypeDef htim14;

//
//	CONFIGURATION
//

//	Remember to configure a timer on CubeMX 1us per tick
//	example 72 MHz cpu - Prescaler=(72-1), Counter period=65000
#define _DS18B20_MAX_SENSORS		    10 // Укажи количество ds18b20! ???zerg???
#define	_DS18B20_GPIO					DS18B20_GPIO_Port
#define	_DS18B20_PIN					DS18B20_Pin
//#define	_DS18B20_TIMER					htim14

//#define _DS18B20_USE_CRC

//
//	Sensor structure
//
typedef struct
{
	int         idpin;
	uint8_t 	Address[8];
	float 		Temperature;
	uint8_t		ValidDataFlag;
	int         uppert;         // Верхний предел температуры (ds18b20 -55°C to +125°C)
	char        textupt[30];    // Поле ввода для действия при достижении верхнего предела температуры. (ds18b20 -55°C to +125°C)
	int         lowert;         // Нижний предел температуры  (ds18b20 -55°C to +125°C)
	char        textlowt[30];   // Поле ввода для действия при достижении нижнего предела температуры. (ds18b20 -55°C to +125°C)
	char        info[30];	    // Info
} Ds18b20Sensor_t;

//struct dbOneWire { // эту структуру подставь вместо Ds18b20Sensor_t	ds18b20[SIZE][_DS18B20_MAX_SENSORS]; в ds18b20.c
//	int         idpin;
//	uint8_t 	Address[8];     // Если ds18b20 то ROM
//	float 		Temperature;
//	uint8_t		ValidDataFlag;
//	int         uppert;         // Верхний предел температуры (ds18b20 -55°C to +125°C)
//	char        textupt[30];    // Поле ввода для действия при достижении верхнего предела температуры. (ds18b20 -55°C to +125°C)
//	int         lowert;         // Нижний предел температуры  (ds18b20 -55°C to +125°C)
//	char        textlowt[30];   // Поле ввода для действия при достижении нижнего предела температуры. (ds18b20 -55°C to +125°C)
//	char        info[30];	    // Info
//};
//
//	DEFINES
//
//#define SIZE (sizeof(ds18b20) / sizeof(ds18b20[0][0]))// Общее количество пинов для OneWire
//#define SIZE 2 // zerg ???
//size_t SIZE = sizeof(ds18b20) / sizeof(ds18b20[0]);

#define DS18B20_FAMILY_CODE				0x28

#define DS18B20_CMD_ALARMSEARCH			0xEC
#define DS18B20_CMD_CONVERTTEMP			0x44 //Измеряем температуру на каждом устройстве

#define DS18B20_STEP_12BIT		0.0625
#define DS18B20_STEP_11BIT		0.125
#define DS18B20_STEP_10BIT		0.25
#define DS18B20_STEP_9BIT		0.5

#define DS18B20_RESOLUTION_R1	6 // Resolution bit R1
#define DS18B20_RESOLUTION_R0	5 // Resolution bit R0

#ifdef _DS18B20_USE_CRC
#define DS18B20_DATA_LEN	9
#else
#define DS18B20_DATA_LEN	5
#endif

typedef enum {
	DS18B20_Resolution_9bits = 9,
	DS18B20_Resolution_10bits = 10,
	DS18B20_Resolution_11bits = 11,
	DS18B20_Resolution_12bits = 12
} DS18B20_Resolution_t;
//
//	FUNCTIONS
//
/************************ Zerg *****************************/
//extern Ds18b20Sensor_t ds18b20[][_DS18B20_MAX_SENSORS];
extern Ds18b20Sensor_t ds18b20[_DS18B20_MAX_SENSORS];
extern OneWire_t OneWire_instances[];
//void myfunction(uint8_t quentity, char (*ROM)[SIZE][17]);

//struct dbPinsInfo {
//	char pins[5];
//	char port[2];
//	int number;
//	uint16_t hal_pin;//15
//	GPIO_TypeDef* gpio_name;//A
//	short onewire; // 0 - Not able; 1 - Able
//	short pwm; // 0 - Not able; 1 - Able
//	short i2cdata; // 0 - Not able; 1 - Able
//	short i2cclok; // 0 - Not able; 1 - Able
//	TIM_TypeDef* tim;
//	uint32_t tim_channel;
//	uint16_t af; // Alternate function
//};


void configureGPIOpin(struct dbPinsInfo* sensor);
// 	Init
void		DS18B20_Init(DS18B20_Resolution_t resolution,struct dbPinsInfo* mysensor);
//	Settings
uint8_t 	DS18B20_GetResolution(uint8_t number); // Get the sensor resolution
//uint8_t 	DS18B20_SetResolution(uint8_t number, DS18B20_Resolution_t resolution);	// Set the sensor resolution
uint8_t DS18B20_SetResolution(uint8_t pin_index, uint8_t sensor_index, DS18B20_Resolution_t resolution, Ds18b20Sensor_t *sensor, OneWire_t *onewire);//*onewire!!!!!!!!!!!!!!!!
// Control
uint8_t 	DS18B20_Start(uint8_t number, uint8_t i); // Start conversion of one sensor
//void 		DS18B20_StartAll(void);	// Start conversion for all sensors
void        DS18B20_StartAll(OneWire_t* onewire);// Start conversion for all sensors
//uint8_t		DS18B20_Read(uint8_t number, float* destination); // Read one sensor
//uint8_t DS18B20_Read(uint8_t number, float *destination, uint8_t i);
uint8_t     DS18B20_Read(uint8_t pin_index, uint8_t sensor_index, uint8_t* destination);
//void 		DS18B20_ReadAll(void);	// Read all connected sensors
void 		DS18B20_ReadAll(OneWire_t* onewire,uint8_t pin_index);// Read all connected sensors
uint8_t 	DS18B20_Is(uint8_t* ROM); // Check if ROM address is DS18B20 family
//uint8_t 	DS18B20_AllDone(void);	// Check if all sensor's conversion is done
uint8_t 	DS18B20_AllDone(OneWire_t* OneWire);	// Check if all sensor's conversion is done
//	ROMs
void        DS18B20_GetROM(uint8_t sensor_index, uint8_t *ROM);// Get sensor's ROM from 'number' position
void		DS18B20_WriteROM(uint8_t pin_index, uint8_t sensor_index, uint8_t *ROM); // Write a ROM to 'number' position in sensors table
// Return functions
uint8_t 	DS18B20_Quantity(void);	// Returns quantity of connected sensors
//uint8_t	DS18B20_GetTemperature(uint8_t number, float* destination); // Returns 0 if read data is invalid
uint8_t     DS18B20_GetTemperature(uint8_t sensor_index, float* destination, uint8_t pin_index); // Returns 0 if read data is invalid
#endif
