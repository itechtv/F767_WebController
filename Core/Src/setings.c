/*
 * setings.c
 *
 *  Created on: 29 мая 2023 г.
 *      Author: anton
 */

#include "db.h"
#include "fatfs.h"
#include "usb_host.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include "db.h"
#include "setings.h"


char fsbuffer[25500] = { 0 };//2000

extern struct dbSettings SetSettings;
extern struct dbCron dbCrontxt[MAXSIZE];
extern struct dbPinsInfo PinsInfo[NUMPIN];
extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinToPin PinsLinks[NUMPINLINKS];

/**************************************************************************/
// Функция включает тактирование на указанном порту.
int enablePort(char *portName) {

	switch (*portName) {
	case 'A':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		break;
	case 'B':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		break;
	case 'C':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		break;
	case 'D':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		break;
	case 'E':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
		break;
	case 'F':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
		break;
	case 'G':
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
		break;
	default:
		printf("Invalid port '%s'!\n", portName);
		return 0; // Вернем '0' если ошибка!
	}
	return 1;// Вернем '1' если все OK!
}

// Функция для проверки, включено ли тактирование порта
//void checkPortClockStatus(char *portName, int isClockEnabled) {
//    if (isClockEnabled) {
//        printf("port '%s' is ON!\n", portName);
//    } else {
//        printf("port '%s' is OFF!\n", portName);
//        enablePort(portName); // Подаем тактирование на не активный порт.
//        printf("port '%s' is ON!\n", portName);
//    }
//}
/**************************************************************************/

// Когда форму сохраняем
void SetSetingsConfig() {
	cJSON *root_obj = NULL;
	char *out_str = NULL;
	FRESULT fresult;
	UINT Byteswritten; // File read/write count

	if (f_open(&USBHFile, (const TCHAR*) "setings.ini", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

		root_obj = cJSON_CreateObject();
		cJSON_AddStringToObject(root_obj, "adm_name", SetSettings.adm_name);
		cJSON_AddStringToObject(root_obj, "adm_pswd", SetSettings.adm_pswd);
		cJSON_AddStringToObject(root_obj, "token", SetSettings.token);
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
		//cJSON_AddNumberToObject(root_obj, "mqtt_qos", SetSettings.mqtt_qos);  // (QOS)
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
		free(out_str);

		if(fresult == FR_OK){

		}

		cJSON_Delete(root_obj);
		memset(fsbuffer, '\0', sizeof(fsbuffer));
		f_close(&USBHFile);
	}
}

// Первый запуск
void StartSetingsConfig() {
	cJSON *root_obj = NULL;
	char *out_str = NULL;
	FRESULT fresult;
	UINT Byteswritten; // File read/write count

	if (f_open(&USBHFile, (const TCHAR*) "setings.ini", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
		printf("f_open! create setings.ini \r\n");
		root_obj = cJSON_CreateObject();

		cJSON_AddStringToObject(root_obj, "adm_name", ADM_NAME);
		cJSON_AddStringToObject(root_obj, "adm_pswd", ADM_PASS); // Пароль для авторизации
		cJSON_AddStringToObject(root_obj, "token", "");
		cJSON_AddStringToObject(root_obj, "lang", LANG); //
		cJSON_AddNumberToObject(root_obj, "timezone", 0); // UTC
		cJSON_AddNumberToObject(root_obj, "lon_de", 0); // Longitude / Долгота
		cJSON_AddNumberToObject(root_obj, "lat_de", 0); // Latitude / Широта
		cJSON_AddNumberToObject(root_obj, "ip1_sntp0", 0); // SMTP Server primary
		cJSON_AddNumberToObject(root_obj, "ip1_sntp1", 0); // SMTP Server primary
		cJSON_AddNumberToObject(root_obj, "ip1_sntp2", 0); // SMTP Server primary
		cJSON_AddNumberToObject(root_obj, "ip1_sntp3", 0); // SMTP Server primary
		cJSON_AddNumberToObject(root_obj, "ip2_sntp0", 0); // SMTP Server secondary
		cJSON_AddNumberToObject(root_obj, "ip2_sntp1", 0); // SMTP Server secondary
		cJSON_AddNumberToObject(root_obj, "ip2_sntp2", 0); // SMTP Server secondary
		cJSON_AddNumberToObject(root_obj, "ip2_sntp3", 0); // SMTP Server secondary
		cJSON_AddNumberToObject(root_obj, "ip3_sntp0", 0); // SMTP Server teriary
		cJSON_AddNumberToObject(root_obj, "ip3_sntp1", 0); // SMTP Server teriary
		cJSON_AddNumberToObject(root_obj, "ip3_sntp2", 0); // SMTP Server teriary
		cJSON_AddNumberToObject(root_obj, "ip3_sntp3", 0); // SMTP Server teriary
		cJSON_AddNumberToObject(root_obj, "check_mqtt", 0); // check MQTT on/off
		cJSON_AddNumberToObject(root_obj, "mqtt_prt", MQTT_PRT); // Your MQTT broker port (default port is set to 1883)
		cJSON_AddNumberToObject(root_obj, "mqtt_qos", MQTT_QOS); // Your MQTT QOS (default QOS is set to 0)
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
		cJSON_AddNumberToObject(root_obj, "ip_addr0", IP_ADDR0); // IP адрес
		cJSON_AddNumberToObject(root_obj, "ip_addr1", IP_ADDR1); // IP адрес
		cJSON_AddNumberToObject(root_obj, "ip_addr2", IP_ADDR2); // IP адрес
		cJSON_AddNumberToObject(root_obj, "ip_addr3", IP_ADDR3); // IP адрес
		cJSON_AddNumberToObject(root_obj, "sb_mask0", SB_MASK0);	// Маска сети
		cJSON_AddNumberToObject(root_obj, "sb_mask1", SB_MASK1);	// Маска сети
		cJSON_AddNumberToObject(root_obj, "sb_mask2", SB_MASK2);	// Маска сети
		cJSON_AddNumberToObject(root_obj, "sb_mask3", SB_MASK3);	// Маска сети
		cJSON_AddNumberToObject(root_obj, "gateway0", GATEWAY0); // Шлюз
		cJSON_AddNumberToObject(root_obj, "gateway1", GATEWAY1); // Шлюз
		cJSON_AddNumberToObject(root_obj, "gateway2", GATEWAY2); // Шлюз
		cJSON_AddNumberToObject(root_obj, "gateway3", GATEWAY3); // Шлюз
		cJSON_AddNumberToObject(root_obj, "macaddr0", 0); // MAC address
		cJSON_AddNumberToObject(root_obj, "macaddr1", 0); // MAC address
		cJSON_AddNumberToObject(root_obj, "macaddr2", 0); // MAC address
		cJSON_AddNumberToObject(root_obj, "macaddr3", 0); // MAC address
		cJSON_AddNumberToObject(root_obj, "macaddr4", 0); // MAC address
		cJSON_AddNumberToObject(root_obj, "macaddr5", 0); // MAC address

		out_str = cJSON_PrintUnformatted(root_obj);
		fresult = f_write(&USBHFile, (const void*) out_str, strlen(out_str), &Byteswritten);
		free(out_str);

		if(fresult == FR_OK){

		}

		printf("f_open! setings.ini \r\n");

		cJSON_Delete(root_obj);
		memset(fsbuffer, '\0', sizeof(fsbuffer));
		f_close(&USBHFile);

		strcpy(SetSettings.lang, LANG);
		strcpy(SetSettings.adm_name, ADM_NAME);
		strcpy(SetSettings.adm_pswd, ADM_PASS);
		SetSettings.ip_addr0 = IP_ADDR0;
		SetSettings.ip_addr1 = IP_ADDR1;
		SetSettings.ip_addr2 = IP_ADDR2;
		SetSettings.ip_addr3 = IP_ADDR3;
		SetSettings.sb_mask0 = SB_MASK0;
		SetSettings.sb_mask1 = SB_MASK1;
		SetSettings.sb_mask2 = SB_MASK2;
		SetSettings.sb_mask3 = SB_MASK3;
		SetSettings.gateway0 = GATEWAY0;
		SetSettings.gateway1 = GATEWAY1;
		SetSettings.gateway2 = GATEWAY2;
		SetSettings.gateway3 = GATEWAY3;
		SetSettings.mqtt_prt = MQTT_PRT;
		SetSettings.mqtt_prt = MQTT_QOS;

	}
}

// если файл существует, открываем его и перезаписываем
void GetSetingsConfig() {
	FILINFO finfo;
	FRESULT fresult = f_stat("setings.ini", &finfo);

	if (fresult == FR_OK) {
		if (f_open(&USBHFile, (const TCHAR*) "setings.ini", FA_READ) == FR_OK) {
			char fsbuffer[1024];
			UINT Byteswritten = 0;
			fresult = f_read(&USBHFile, fsbuffer, sizeof(fsbuffer), &Byteswritten);

			cJSON *root_obj = cJSON_Parse(fsbuffer);

			strcpy(SetSettings.adm_name, cJSON_GetObjectItem(root_obj, "adm_name")->valuestring);
			strcpy(SetSettings.adm_pswd, cJSON_GetObjectItem(root_obj, "adm_pswd")->valuestring);
			strcpy(SetSettings.token, cJSON_GetObjectItem(root_obj, "token")->valuestring);
			strcpy(SetSettings.lang, cJSON_GetObjectItem(root_obj, "lang")->valuestring);
			SetSettings.timezone = cJSON_GetObjectItem(root_obj, "timezone")->valueint;
			SetSettings.lon_de = cJSON_GetObjectItem(root_obj, "lon_de")->valueint;
			SetSettings.lat_de = cJSON_GetObjectItem(root_obj, "lat_de")->valueint;
			SetSettings.ip1_sntp0 = cJSON_GetObjectItem(root_obj, "ip1_sntp0")->valueint;
			SetSettings.ip1_sntp1 = cJSON_GetObjectItem(root_obj, "ip1_sntp1")->valueint;
			SetSettings.ip1_sntp2 = cJSON_GetObjectItem(root_obj, "ip1_sntp2")->valueint;
			SetSettings.ip1_sntp3 = cJSON_GetObjectItem(root_obj, "ip1_sntp3")->valueint;
			SetSettings.ip2_sntp0 = cJSON_GetObjectItem(root_obj, "ip2_sntp0")->valueint;
			SetSettings.ip2_sntp1 = cJSON_GetObjectItem(root_obj, "ip2_sntp1")->valueint;
			SetSettings.ip2_sntp2 = cJSON_GetObjectItem(root_obj, "ip2_sntp2")->valueint;
			SetSettings.ip2_sntp3 = cJSON_GetObjectItem(root_obj, "ip2_sntp3")->valueint;
			SetSettings.ip3_sntp0 = cJSON_GetObjectItem(root_obj, "ip3_sntp0")->valueint;
			SetSettings.ip3_sntp1 = cJSON_GetObjectItem(root_obj, "ip3_sntp1")->valueint;
			SetSettings.ip3_sntp2 = cJSON_GetObjectItem(root_obj, "ip3_sntp2")->valueint;
			SetSettings.ip3_sntp3 = cJSON_GetObjectItem(root_obj, "ip3_sntp3")->valueint;
			// Настройки MQTT
			SetSettings.check_mqtt = cJSON_GetObjectItem(root_obj, "check_mqtt")->valueint;
			SetSettings.mqtt_prt = cJSON_GetObjectItem(root_obj, "mqtt_prt")->valueint;
			//SetSettings.mqtt_qos = cJSON_GetObjectItem(root_obj, "mqtt_qos")->valueint;
			strcpy(SetSettings.mqtt_clt, cJSON_GetObjectItem(root_obj, "mqtt_clt")->valuestring);
			strcpy(SetSettings.mqtt_usr, cJSON_GetObjectItem(root_obj, "mqtt_usr")->valuestring);
			strcpy(SetSettings.mqtt_pswd, cJSON_GetObjectItem(root_obj, "mqtt_pswd")->valuestring);
			strcpy(SetSettings.mqtt_tpc, cJSON_GetObjectItem(root_obj, "mqtt_tpc")->valuestring);
			strcpy(SetSettings.mqtt_ftpc, cJSON_GetObjectItem(root_obj, "mqtt_ftpc")->valuestring);
			SetSettings.mqtt_hst0 = cJSON_GetObjectItem(root_obj, "mqtt_hst0")->valueint;
			SetSettings.mqtt_hst1 = cJSON_GetObjectItem(root_obj, "mqtt_hst1")->valueint;
			SetSettings.mqtt_hst2 = cJSON_GetObjectItem(root_obj, "mqtt_hst2")->valueint;
			SetSettings.mqtt_hst3 = cJSON_GetObjectItem(root_obj, "mqtt_hst3")->valueint;

			// Настройки IP адреса
			SetSettings.check_ip = cJSON_GetObjectItem(root_obj, "check_ip")->valueint;
			SetSettings.ip_addr0 = cJSON_GetObjectItem(root_obj, "ip_addr0")->valueint;
			SetSettings.ip_addr1 = cJSON_GetObjectItem(root_obj, "ip_addr1")->valueint;
			SetSettings.ip_addr2 = cJSON_GetObjectItem(root_obj, "ip_addr2")->valueint;
			SetSettings.ip_addr3 = cJSON_GetObjectItem(root_obj, "ip_addr3")->valueint;
			SetSettings.sb_mask0 = cJSON_GetObjectItem(root_obj, "sb_mask0")->valueint;
			SetSettings.sb_mask1 = cJSON_GetObjectItem(root_obj, "sb_mask1")->valueint;
			SetSettings.sb_mask2 = cJSON_GetObjectItem(root_obj, "sb_mask2")->valueint;
			SetSettings.sb_mask3 = cJSON_GetObjectItem(root_obj, "sb_mask3")->valueint;
			SetSettings.gateway0 = cJSON_GetObjectItem(root_obj, "gateway0")->valueint;
			SetSettings.gateway1 = cJSON_GetObjectItem(root_obj, "gateway1")->valueint;
			SetSettings.gateway2 = cJSON_GetObjectItem(root_obj, "gateway2")->valueint;
			SetSettings.gateway3 = cJSON_GetObjectItem(root_obj, "gateway3")->valueint;
			SetSettings.macaddr0 = cJSON_GetObjectItem(root_obj, "macaddr0")->valueint;
			SetSettings.macaddr1 = cJSON_GetObjectItem(root_obj, "macaddr1")->valueint;
			SetSettings.macaddr2 = cJSON_GetObjectItem(root_obj, "macaddr2")->valueint;
			SetSettings.macaddr3 = cJSON_GetObjectItem(root_obj, "macaddr3")->valueint;
			SetSettings.macaddr4 = cJSON_GetObjectItem(root_obj, "macaddr4")->valueint;
			SetSettings.macaddr5 = cJSON_GetObjectItem(root_obj, "macaddr5")->valueint;

			cJSON_Delete(root_obj);
			memset(fsbuffer, '\0', sizeof(fsbuffer));
			f_close(&USBHFile);
		}
	}
}
// если файл существует, открываем для чтения.
void GetCronConfig() {
	FILINFO finfo;
	cJSON *root_obj = NULL;
	FRESULT fresult;
	UINT Byteswritten; // File read/write count

	fresult = f_stat("cron.ini", &finfo);
	if (fresult == FR_OK) {
		// если файл существует, открываем его
		if (f_open(&USBHFile, (const TCHAR*) "cron.ini", FA_READ) == FR_OK) {

			fresult = f_read(&USBHFile, fsbuffer, sizeof(fsbuffer), &Byteswritten);
			printf("CRON file EXISTS! \r\n");
			root_obj = cJSON_Parse(fsbuffer);

			for (int i = 0; i < cJSON_GetArraySize(root_obj); i++) {
				cJSON *cron_item = cJSON_GetArrayItem(root_obj, i);

				strcpy(dbCrontxt[i].cron, cJSON_GetObjectItem(cron_item, "cron")->valuestring);
				strcpy(dbCrontxt[i].activ, cJSON_GetObjectItem(cron_item, "activ")->valuestring);
				dbCrontxt[i].ptime = cJSON_GetObjectItem(cron_item, "ptime")->valueint;
				strcpy(dbCrontxt[i].info, cJSON_GetObjectItem(cron_item, "info")->valuestring);

				//SetSettings.check_mqtt = cJSON_GetObjectItem(root_obj, "check_mqtt")->valueint;

			}
			cJSON_Delete(root_obj);
			memset(fsbuffer, '\0', sizeof(fsbuffer));
			f_close(&USBHFile);
		}
	}
}

// Если файл не существует, создаем его и записываем данные
void SetCronConfig() {
	FILINFO finfo;
	cJSON *root_obj = NULL;
	cJSON *fld = NULL;
	UINT Byteswritten; // File read/write count
	FRESULT fresult;

	fresult = f_stat("cron.ini", &finfo);
	char *out_str = NULL;
	int i = 0;
	if (f_open(&USBHFile, (const TCHAR*) "cron.ini",
	FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
		// Запись JSON в файл
		printf("Write CRON in to file. \r\n");

		root_obj = cJSON_CreateArray();
		fld = cJSON_CreateObject();

		for (i = 0; i < MAXSIZE; i++) {
			cJSON_AddItemToArray(root_obj, fld = cJSON_CreateObject());

			cJSON_AddStringToObject(fld, "cron", dbCrontxt[i].cron);
			cJSON_AddStringToObject(fld, "activ", dbCrontxt[i].activ);
			cJSON_AddNumberToObject(fld, "ptime", 0); //????????????????????????????????
			cJSON_AddStringToObject(fld, "info", dbCrontxt[i].info);

		}
		out_str = cJSON_PrintUnformatted(root_obj);
		fresult = f_write(&USBHFile, (const void*) out_str, strlen(out_str), &Byteswritten);
		free(out_str);

		if(fresult == FR_OK){

		}
		printf("f_open! cron.ini \r\n");

		cJSON_Delete(root_obj);
		memset(fsbuffer, '\0', sizeof(fsbuffer));
		f_close(&USBHFile);
	}



}
// если файл "pins.ini" существует, открываем для чтения.
void GetPinConfig() {

	FILINFO finfo;
	cJSON *root_obj = NULL;
	FRESULT fresult;
	UINT Byteswritten; // File read/write count

	fresult = f_stat("pins.ini", &finfo);
	if (fresult == FR_OK) {
		// если файл существует, открываем его
		if (f_open(&USBHFile, (const TCHAR*) "pins.ini", FA_READ) == FR_OK) {

			fresult = f_read(&USBHFile, fsbuffer, sizeof(fsbuffer), &Byteswritten);
			printf("PINS file EXISTS! \r\n");
			root_obj = cJSON_Parse(fsbuffer);

			for (int i = 0; i < cJSON_GetArraySize(root_obj); i++) {
				cJSON *pins_item = cJSON_GetArrayItem(root_obj, i);

				cJSON *topin = cJSON_GetObjectItem(pins_item, "topin");
				PinsConf[i].topin = topin->valueint;

				PinsConf[i].pwm = cJSON_GetObjectItem(pins_item, "pwm")->valueint;
				PinsConf[i].on = cJSON_GetObjectItem(pins_item, "on")->valueint;
				PinsConf[i].istate = cJSON_GetObjectItem(pins_item, "istate")->valueint;
				PinsConf[i].dvalue = cJSON_GetObjectItem(pins_item, "dvalue")->valueint;
				PinsConf[i].ponr = cJSON_GetObjectItem(pins_item, "ponr")->valueint;
				strcpy(PinsConf[i].ptype, cJSON_GetObjectItem(pins_item, "ptype")->valuestring);
				PinsConf[i].binter = cJSON_GetObjectItem(pins_item, "binter")->valueint;
				PinsConf[i].hinter = cJSON_GetObjectItem(pins_item, "hinter")->valueint;
				PinsConf[i].repeat = cJSON_GetObjectItem(pins_item, "repeat")->valueint;
				PinsConf[i].rinter = cJSON_GetObjectItem(pins_item, "rinter")->valueint;
				PinsConf[i].dcinter = cJSON_GetObjectItem(pins_item, "dcinter")->valueint;
				PinsConf[i].pclick = cJSON_GetObjectItem(pins_item, "pclick")->valueint;
				strcpy(PinsConf[i].info, cJSON_GetObjectItem(pins_item, "info")->valuestring);
				PinsConf[i].onoff = cJSON_GetObjectItem(pins_item, "onoff")->valueint;
				PinsConf[i].event = cJSON_GetObjectItem(pins_item, "event")->valueint;
				PinsConf[i].act = cJSON_GetObjectItem(pins_item, "act")->valueint;
				PinsConf[i].parametr = cJSON_GetObjectItem(pins_item, "parametr")->valueint;
				PinsConf[i].timeout = cJSON_GetObjectItem(pins_item, "timeout")->valueint;
				strcpy(PinsConf[i].condit, cJSON_GetObjectItem(pins_item, "condit")->valuestring);
			}

			cJSON_Delete(root_obj);
			memset(fsbuffer, '\0', sizeof(fsbuffer));
			f_close(&USBHFile);
		}
	}

}

// Если файл "pins.ini" не существует, создаем его и записываем данные
void SetPinConfig() {
//	FILINFO finfo;
	cJSON *root_obj = NULL;
	cJSON *fld = NULL;
	UINT Byteswritten; // File read/write count
	FRESULT fresult;
	char *out_str = NULL;
	int i = 0;
	if (f_open(&USBHFile, (const TCHAR*) "pins.ini",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
		// Запись JSON в файл

		root_obj = cJSON_CreateArray();
		fld = cJSON_CreateObject();

		for (i = 0; i < NUMPIN; i++)
		{
			cJSON_AddItemToArray(root_obj, fld = cJSON_CreateObject());

			cJSON_AddNumberToObject(fld, "topin", PinsConf[i].topin);
			cJSON_AddNumberToObject(fld, "pwm", PinsConf[i].pwm);
			cJSON_AddNumberToObject(fld, "on", PinsConf[i].on);
			cJSON_AddNumberToObject(fld, "istate", PinsConf[i].istate);
			cJSON_AddNumberToObject(fld, "dvalue", PinsConf[i].dvalue);
			cJSON_AddNumberToObject(fld, "ponr", PinsConf[i].ponr);
			cJSON_AddStringToObject(fld, "ptype", PinsConf[i].ptype);
			cJSON_AddNumberToObject(fld, "binter", PinsConf[i].binter);
			cJSON_AddNumberToObject(fld, "hinter", PinsConf[i].hinter);
			cJSON_AddNumberToObject(fld, "repeat", PinsConf[i].repeat);
			cJSON_AddNumberToObject(fld, "rinter", PinsConf[i].rinter);
			cJSON_AddNumberToObject(fld, "dcinter", PinsConf[i].dcinter);
			cJSON_AddNumberToObject(fld, "pclick", PinsConf[i].pclick);
			cJSON_AddStringToObject(fld, "info", PinsConf[i].info);
			cJSON_AddNumberToObject(fld, "onoff", PinsConf[i].onoff);
			cJSON_AddNumberToObject(fld, "event", PinsConf[i].event);
			cJSON_AddNumberToObject(fld, "act", PinsConf[i].act);
			cJSON_AddNumberToObject(fld, "parametr", PinsConf[i].parametr);
			cJSON_AddNumberToObject(fld, "timeout", PinsConf[i].timeout);
			cJSON_AddStringToObject(fld, "condit", PinsConf[i].condit);
		}
		out_str = cJSON_PrintUnformatted(root_obj);
		fresult = f_write(&USBHFile, (const void*) out_str, strlen(out_str), &Byteswritten);
		free(out_str);

		if(fresult == FR_OK){

		}
		cJSON_Delete(root_obj);
		memset(fsbuffer, '\0', sizeof(fsbuffer));
		f_close(&USBHFile);
	}
}


void GetPinToPin() {
	FILINFO finfo;
	cJSON *root_obj = NULL;
	FRESULT fresult;
	UINT Byteswritten; // File read/write count

	fresult = f_stat("pintopin.ini", &finfo);
	if (fresult == FR_OK) {
		// если файл существует, открываем его
		if (f_open(&USBHFile, (const TCHAR*) "pintopin.ini", FA_READ) == FR_OK) {

			fresult = f_read(&USBHFile, fsbuffer, sizeof(fsbuffer), &Byteswritten);

			root_obj = cJSON_Parse(fsbuffer);

			for (int i = 0; i < cJSON_GetArraySize(root_obj); i++) {
				cJSON *pins_item = cJSON_GetArrayItem(root_obj, i);

				PinsLinks[i].idin = cJSON_GetObjectItem(pins_item, "idin")->valueint;
				PinsLinks[i].idout = cJSON_GetObjectItem(pins_item, "idout")->valueint;
				PinsLinks[i].flag = cJSON_GetObjectItem(pins_item, "flag")->valueint;

			}

			cJSON_Delete(root_obj);
			memset(fsbuffer, '\0', sizeof(fsbuffer));
			f_close(&USBHFile);
		}
	}


}

// Если файл "pintopin.ini" не существует, создаем его и записываем данные
void SetPinToPin() {
	cJSON *root_obj = NULL;
	cJSON *fld = NULL;
	UINT Byteswritten; // File read/write count
	FRESULT fresult;
	char *out_str = NULL;
	int i = 0;
	if (f_open(&USBHFile, (const TCHAR*) "pintopin.ini",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
		// Запись JSON в файл

		root_obj = cJSON_CreateArray();
		fld = cJSON_CreateObject();
		for (i = 0; i < NUMPINLINKS; i++)
		{
			cJSON_AddItemToArray(root_obj, fld = cJSON_CreateObject());

			cJSON_AddNumberToObject(fld, "idin", PinsLinks[i].idin);
			cJSON_AddNumberToObject(fld, "idout", PinsLinks[i].idout);
			cJSON_AddNumberToObject(fld, "flag", PinsLinks[i].flag);

		}
		out_str = cJSON_PrintUnformatted(root_obj);
		fresult = f_write(&USBHFile, (const void*) out_str, strlen(out_str), &Byteswritten);
		free(out_str);

		if(fresult == FR_OK){

		}
		cJSON_Delete(root_obj);
		memset(fsbuffer, '\0', sizeof(fsbuffer));
		f_close(&USBHFile);
	}

}




void InitPin() {
	int i = 0;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    for (i = 0; i < NUMPIN; i++){
    	if(PinsConf[i].topin == 2){

    		// проверяем тактирование порта
			//checkPortClockStatus(PinsInfo[i].port, __HAL_RCC_GPIOA_IS_CLK_ENABLED());
    		HAL_GPIO_DeInit(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);

			// инициализация пина OUTPUT
    		GPIO_InitStruct.Pin = PinsInfo[i].hal_pin; // вывод
    		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // режим – выход
    		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW ; //
    		HAL_GPIO_Init(PinsInfo[i].gpio_name, &GPIO_InitStruct);
    	}
    	if(PinsConf[i].topin == 1){

    		// проверяем тактирование порта
			//checkPortClockStatus(PinsInfo[i].port, __HAL_RCC_GPIOA_IS_CLK_ENABLED());

            // сбрасываем биты для данного пина
            HAL_GPIO_DeInit(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);


			// инициализация пина  INPUT
    	    GPIO_InitStruct.Pin = PinsInfo[i].hal_pin; // вход
    	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // устанавливаем режим работы порта на вход

    	    // @todo поменять на int
    	    if (strcmp(PinsConf[i].ptype, "GPIO_PULLUP") == 0) {
    	    	GPIO_InitStruct.Pull = GPIO_PULLUP;
    	    }
    	    else if (strcmp(PinsConf[i].ptype, "GPIO_PULLDOWN") == 0) {
    	    	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    	    }
    	    else if (strcmp(PinsConf[i].ptype, "None") == 0) {
    	    	GPIO_InitStruct.Pull = GPIO_NOPULL;
    	    } else {
    	    	GPIO_InitStruct.Pull = GPIO_NOPULL;
    	    }


    	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // устанавливаем максимальную скорость порта
    	    HAL_GPIO_Init(PinsInfo[i].gpio_name, &GPIO_InitStruct); // инициализируем порт B
    	}
    }
	//PinsInfo[i].hal_pin
	//PinsInfo[i].gpio_name

}
