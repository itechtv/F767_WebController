/*
 * setengs.h
 *
 *  Created on: 29 мая 2023 г.
 *      Author: anton
 */

#ifndef INC_SETINGS_H_
#define INC_SETINGS_H_

void SetSetingsConfig();
void StartSetingsConfig();
void GetSetingsConfig();
void GetCronConfig();
void SetCronConfig();
void GetPinConfig();
void SetPinConfig();
void GetPinToPin();
void SetPinToPin();
void InitPin();


#define ADM_NAME "admin"
#define ADM_PASS "12345678"
#define LANG "ru"

#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 11
#define IP_ADDR3 80
#define SB_MASK0 255
#define SB_MASK1 255
#define SB_MASK2 255
#define SB_MASK3 0
#define GATEWAY0 192
#define GATEWAY1 168
#define GATEWAY2 11
#define GATEWAY3 1
#define MQTT_PRT 1883
#define MQTT_QOS 0

#endif /* INC_SETINGS_H_ */
