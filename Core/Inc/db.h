/*
 * db.h
 *
 *  Created on: Dec 14, 2022
 *      Author: Anton & Denis
 */

#ifndef INC_DB_H_
#define INC_DB_H_

#define NUMPIN 89 // количество пинов
#define NUMPINLINKS 100 // количество pin to pin
#define NUMTASK 30 // кол-во CRON task

#include "stdio.h"
#include "stm32f7xx_hal.h"

#define MAXSIZE 10


struct dbCron {
	char cron[35]; // 30-45 30-45 1-22 15-17 5-12 1-7 23 max возможное кол-во символов.
	char activ[255]; // max 534
	uint32_t ptime; // для паузы в Unix time sec.
	char info[30];
};

struct dbPinsConf {    // Создали структуру с необходимым набором типов элиментов.
	uint8_t topin;		// Type of pins: 0 - NONE; 1 - BUTTON; 2 - RELAY; 3 - SWITCH;  4 - 1-WIRE; 5 - PWM; 6,7 - I2C;   8,9 - Encoder;
	int pwm;		// PWM frequency
	uint8_t on;			// ON
	uint8_t istate;		// Invert state
	int dvalue;		// Dimmer value
	uint8_t ponr;		// Power on restore
	uint8_t ptype; 		// Pullup type 0 - NONE; 1 - GPIO_PULLUP; 2 - GPIO_PULLDOWN
	/////
	uint8_t sclick; 	// SINGLE CLICK
	char dclick[255]; 	// DOUBLE CLICK
	char lpress[255]; 	// LONG PRESS
	////
	uint8_t encoderb; 	// Encoder pin B
	uint8_t hinter; 	// Hold interval
	uint8_t repeat; 	// Repeat
	uint8_t rinter; 	// Repeat interval
	uint8_t dcinter;	// Double-click interval
	uint8_t pclick; 	// Prevent Click
	char info[30];	// Info
	uint8_t onoff;		// On | Off
	uint8_t event;     // Event (StateChanged - 0, Pressed -1, Released - 2, Repeat - 3, Hold - 4, LongClick - 5, Click - 6, DoubleClick - 7)
	uint8_t act;       // Action (No action - 0, On - 1,  Off - 2, Toggle - 3, Value - 4, IncValue - 5)
	short parametr;  // Parameter value for dimmer (0-255), value for IncValue (-255 to 255).
	int timeout;     // Timeout (ms)
	char condit[30]; // Condition

};

struct dbPinsInfo { // Создали структуру с необходимым набором типов элиментов для PIN's.
	char pins[5];
	char port[2];
	int number;
	uint16_t hal_pin;
	GPIO_TypeDef* gpio_name;
	short onewire; // 0 - Not able; 1 - Able
	short pwm; // 0 - Not able; 1 - Able
	short i2cdata; // 0 - Not able; 1 - Able
	short i2cclok; // 0 - Not able; 1 - Able
	TIM_TypeDef* tim;
	uint32_t tim_channel;
	uint16_t af; // Alternate function
};


struct dbPinToPin { // привязка кнопок к реле
	int idin;
	int idout;
	int flag;
};

struct dbSettings { 	// Cтруктура для setting
	char adm_name[10];  // Имя пользователя для авторизации
	char adm_pswd[15]; // Пароль для авторизации
	char token[11]; // Token для API
	char lang[3]; //
	int timezone;		// UTC
	double lon_de;    	// Longitude / Долгота
	double lat_de;    	// Latitude / Широта
	short ip1_sntp0; 	// SMTP Server primary
	short ip1_sntp1; 	// SMTP Server primary
	short ip1_sntp2; 	// SMTP Server primary
	short ip1_sntp3; 	// SMTP Server primary
	short ip2_sntp0; 	// SMTP Server secondary
	short ip2_sntp1; 	// SMTP Server secondary
	short ip2_sntp2; 	// SMTP Server secondary
	short ip2_sntp3; 	// SMTP Server secondary
	short ip3_sntp0; 	// SMTP Server teriary
	short ip3_sntp1; 	// SMTP Server teriary
	short ip3_sntp2; 	// SMTP Server teriary
	short ip3_sntp3; 	// SMTP Server teriary
	// Настройки MQTT
	short check_mqtt; 	// check MQTT on/off
	int mqtt_prt;       // Your MQTT broker port (default port is set to 1883)
	char mqtt_clt[10];  // Device's unique identifier.
	char mqtt_usr[10];  // MQTT Имя пользователя для авторизации
	char mqtt_pswd[15]; // MQTT Пароль для авторизации
	char mqtt_tpc[15];  // Unique identifying topic for your device (kitchen-light) It is recommended to use a single word for the topic.
	char mqtt_ftpc[15]; // Полный топик for example lights/%prefix%/%topic%/
	short mqtt_hst0; 	// Your MQTT broker address or IP
	short mqtt_hst1; 	// Your MQTT broker address or IP
	short mqtt_hst2; 	// Your MQTT broker address or IP
	short mqtt_hst3; 	// Your MQTT broker address or IP
	// Настройки IP адреса
	short check_ip; 	// check DHCP on/off
	short ip_addr0; 	// IP адрес
	short ip_addr1; 	// IP адрес
	short ip_addr2; 	// IP адрес
	short ip_addr3; 	// IP адрес
	short sb_mask0;		// Маска сети
	short sb_mask1;		// Маска сети
	short sb_mask2;		// Маска сети
	short sb_mask3;		// Маска сети
	short gateway0; 	// Шлюз
	short gateway1; 	// Шлюз
	short gateway2; 	// Шлюз
	short gateway3; 	// Шлюз
	uint8_t macaddr0;	// MAC address
	uint8_t macaddr1;	// MAC address
	uint8_t macaddr2;	// MAC address
	uint8_t macaddr3;	// MAC address
	uint8_t macaddr4;	// MAC address
	uint8_t macaddr5;	// MAC address
};



#endif /* INC_DB_H_ */
