/*
 * webserver.c
 *
 *  Created on: Dec 14, 2022
 *      Author: Anton & Denis
 */

#include <stdlib.h>
#include <webserver.h>
#include "string.h"
#include "stdio.h"
#include <ctype.h>
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "stm32f7xx_hal.h"
#include "math.h"
#include "time.h"
#include "cJSON.h"
#include "db.h"
#include "cmsis_os.h"

//#include "data_pin_t"

static void *current_connection;
static int variable = 0;

uint8_t verifyNum = 0;
uint8_t tab = 0;
uint8_t id = 0;
uint8_t numTabLine = 0;
uint8_t countJson = 0;

char randomSSID[27] = {0};
char ssid[27] = {0};
char url[30] = {0};
char tempbufer[1024] = {0};
extern unsigned long Ti;


extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];
extern struct dbPinToPin PinsLinks[NUMPINLINKS];
extern struct dbSettings SetSettings;
extern struct dbCron dbCrontxt[MAXSIZE];

///////////////////////////

extern osMessageQId usbQueueHandle;
extern osMessageQId myQueueHandle;
extern data_pin_t data_pin;

// Generation SSID
char *randomSSIDGeneration(char *rSSID, int num)
{
    // Initialize counter
    int i = 0;

    // Array of symbols
    static const char symbols[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";

    //  the random SSID
    for (i = 0; i < num; i++) {
      rSSID[i] = symbols[rand() % 61];
    }
    rSSID[num]='\0';

    return rSSID;
}


// Restart SSID
void restartSSID(void){
		Ti = HAL_GetTick();
}

// Clear pin
void clearPin(int pinnum){
	int i = 0;

	PinsConf[pinnum].ptype = 0;
	PinsConf[pinnum].binter = 0;
	PinsConf[pinnum].hinter = 0;
	PinsConf[pinnum].repeat = 0;
	PinsConf[pinnum].rinter = 0;
	PinsConf[pinnum].dcinter = 0;
	PinsConf[pinnum].pclick = 0;
	PinsConf[pinnum].info[0] = '\0';
	PinsConf[pinnum].onoff = 0;

	while (i <= NUMPINLINKS - 1) {
		if (PinsLinks[i].idin == pinnum) {
			//printf("flag %d \n", i);
			PinsLinks[i].idin = 0;
			PinsLinks[i].idout = 0;
			PinsLinks[i].flag = 0;
		}
		i++;
	}

}

//////////////////////////////  SSI MULTIPART Function  ///////////////////////


// Counting the number of lines JSON
int MultiPartTabCount(int num, int pinnum, int count)
{
	count = 0;
	for (int i = 0; i <= pinnum; i++){
		if(num == PinsConf[i].topin && num == 1){
			count++;
		}
		if(num == PinsConf[i].topin && num == 2){
			count++;
		}
		if(num == PinsConf[i].topin && num == 3){
			count++;
		}
	}
	return count;
}
//////////////////////////////  SSI HANDLER  //////////////////////////////////

char const *TAGCHAR[] = { "tabjson", "ssid", "check", "menu", "lang", "formjson", "cronjson"};
char const **TAGS = TAGCHAR;


const uint8_t SSI_TAG_NUM = (sizeof(TAGCHAR) / sizeof(TAGCHAR[0]));

static u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen,
		u16_t current_tag_part, u16_t *next_tag_part) {

	char* str = NULL;
	char macStr[34] = {0};
	cJSON *root = NULL;
	cJSON *fld = NULL;
	int idplus = 0;
	int i = 0;

	switch (iIndex) {
	// ssi tag <!--#tabjson-->
	case 0:
	#if LWIP_HTTPD_SSI_MULTIPART
		if (variable == NUMPIN) {
			variable = 0;
			countJson = 0;
			break;
		} else {
			///////
			if(tab == 1){

				sprintf(pcInsert,"{\"id\":%d,\"pins\":\"%s\",\"topin\":%d,\"pwm\":%d,\"i2cdata\":%d,\"i2cclok\":%d},",
						variable, PinsInfo[variable].pins, PinsConf[variable].topin, PinsInfo[variable].pwm, PinsInfo[variable].i2cdata, PinsInfo[variable].i2cclok);

				if(variable == (NUMPIN-1)){
					pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON
				}
			}
			if(tab == 2){
				if(PinsConf[variable].topin == 1){
					// buttoms json
					idplus = variable + 1;

				    root = cJSON_CreateObject();
				    while (i <= NUMPINLINKS - 1) {
				    	if(PinsLinks[i].idin == variable && PinsLinks[i].flag == 1){
				    		cJSON_AddNumberToObject(root, PinsInfo[PinsLinks[i].idout].pins,i + 1);
				    	}
				    	i++;
				    }

					str = cJSON_Print(root);
					cJSON_Delete(root);

					sprintf(pcInsert,
							"{\"topin\":%d,\"id\":%d,\"pins\":\"%s\",\"ptype\":%d,\"binter\":%d,\"hinter\":%d,\"repeat\":%d,\"rinter\":%d,\"dcinter\":%d,\"pclick\":%d,\"pinact\":%s,\"info\":\"%s\",\"onoff\":%d},",
							PinsConf[variable].topin, idplus, PinsInfo[variable].pins,
							PinsConf[variable].ptype, PinsConf[variable].binter,
							PinsConf[variable].hinter, PinsConf[variable].repeat,
							PinsConf[variable].rinter, PinsConf[variable].dcinter,
							PinsConf[variable].pclick, str, PinsConf[variable].info,
							PinsConf[variable].onoff);

					free(str);

					////////////////
					countJson++;

					if(countJson == numTabLine){
						printf("DELLL \n");
						pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON в конце
					}
				} else {
					pcInsert = "";
				}
			}
			if(tab == 3){
				if(PinsConf[variable].topin == 2){
					// relay json
					idplus = variable + 1;

					sprintf(pcInsert,
							"{\"topin\":%d,\"id\":%d,\"pins\":\"%s\",\"ptype\":%d,\"pwm\":%d,\"on\":%d,\"istate\":%d,\"dvalue\":%d,\"ponr\":%d,\"info\":\"%s\",\"onoff\":%d},",
							PinsConf[variable].topin, idplus, PinsInfo[variable].pins,
							PinsConf[variable].ptype, PinsConf[variable].pwm, PinsConf[variable].on,
							PinsConf[variable].istate, PinsConf[variable].dvalue,
							PinsConf[variable].ponr, PinsConf[variable].info,
							PinsConf[variable].onoff);
					////////////////

					countJson++;

					if(countJson == numTabLine){
						printf("DELLL \n");
						pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON в конце
					}

				} else {
					pcInsert = "";
				}
			}
			if(tab == 4){// tabswitch json
				if(PinsConf[variable].topin == 3){
					idplus = variable + 1;

				    root = cJSON_CreateObject();
				    while (i <= NUMPINLINKS - 1) {
				    	if(PinsLinks[i].idin == variable && PinsLinks[i].flag == 1){
				    		cJSON_AddNumberToObject(root, PinsInfo[PinsLinks[i].idout].pins,i + 1);
				    	}
				    	i++;
				    }

					str = cJSON_Print(root);
					cJSON_Delete(root);

					sprintf(pcInsert,
							"{\"topin\":%d,\"id\":%d,\"pins\":\"%s\",\"ptype\":%d,\"binter\":%d,\"hinter\":%d,\"repeat\":%d,\"rinter\":%d,\"dcinter\":%d,\"pclick\":%d,\"pinact\":%s,\"info\":\"%s\",\"onoff\":%d},",
							PinsConf[variable].topin, idplus, PinsInfo[variable].pins,
							PinsConf[variable].ptype, PinsConf[variable].binter,
							PinsConf[variable].hinter, PinsConf[variable].repeat,
							PinsConf[variable].rinter, PinsConf[variable].dcinter,
							PinsConf[variable].pclick, str, PinsConf[variable].info,
							PinsConf[variable].onoff);

					free(str);

					////////////////
					countJson++;

					if(countJson == numTabLine){
						printf("DELLL \n");
						pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON в конце
					}

				} else {
					pcInsert = "";
				}
			}

			*next_tag_part = variable;
			variable++;
			return strlen(pcInsert);
			break;
		}
	#else
		printf("LWIP_HTTPD_SSI_MULTIPART disabled /n");
	#endif
		break;
		// ssi tag  <!--#ssid-->
		case 1:
			sprintf(pcInsert, "%s", randomSSID);
			return strlen(pcInsert);
			break;

		// ssi tag <!--#check-->
		case 2:
			root = cJSON_CreateArray();
			fld = cJSON_CreateObject();
			cJSON_AddItemToArray(root, fld = cJSON_CreateObject());

			if(verifyNum == 2){
				memset(randomSSID, '\0', sizeof(randomSSID));
				cJSON_AddNumberToObject(fld, "login", 1);
				cJSON_AddStringToObject(fld, "ssid", randomSSIDGeneration(randomSSID, 27));
			} else {
				cJSON_AddNumberToObject(fld, "login", 0);
				cJSON_AddStringToObject(fld, "ssid", "");
			}

			str = cJSON_Print(root);
			cJSON_Delete(root);
			sprintf(pcInsert, "%s", str);
			free(str);
			verifyNum = 0;

			return strlen(pcInsert);
			break;

		// ssi tag <!--#menu-->
		case 3:
			sprintf(pcInsert,"<a href=\"index.shtml?ssid=%s\">Home</a> | <a href=\"select.shtml?ssid=%s\">Select pin</a> | <a href=\"tabswitch.shtml?ssid=%s\">Switch pin</a> | <a href=\"tabbuttom.shtml?ssid=%s\">Buttom pin</a> | <a href=\"tabrelay.shtml?ssid=%s\">Relay pin</a> | <a href=\"tabcron.shtml?ssid=%s\">Timers (crone)</a> | <a href=\"settings.shtml?ssid=%s\">Settings</a> | <a href=\"logout.shtml\">Logout</a> ", randomSSID,randomSSID,randomSSID,randomSSID,randomSSID,randomSSID,randomSSID);
			return strlen(pcInsert);
			break;

		// ssi tag <!--#lang-->
		case 4:
			sprintf(pcInsert,"/js/%slang.js", SetSettings.lang);
			return strlen(pcInsert);
			break;

		// ssi tag <!--#formjson-->
		case 5:
			if(tab != 0){
				//root = cJSON_CreateArray();
				//root = cJSON_CreateObject();
				//cJSON_AddItemToArray(root, fld = cJSON_CreateObject());

				if(tab == 1){
					root = cJSON_CreateObject();
					cJSON_AddNumberToObject(root, "topin", PinsConf[id].topin);
					cJSON_AddNumberToObject(root, "id", id + 1); // id numbering from 1
					cJSON_AddStringToObject(root, "pins", PinsInfo[id].pins);
					cJSON_AddNumberToObject(root, "ptype", PinsConf[id].ptype);
					cJSON_AddNumberToObject(root, "binter", PinsConf[id].binter);
					cJSON_AddNumberToObject(root, "hinter", PinsConf[id].hinter);
					cJSON_AddNumberToObject(root, "repeat", PinsConf[id].repeat);
					cJSON_AddNumberToObject(root, "rinter", PinsConf[id].rinter);
					cJSON_AddNumberToObject(root, "dcinter", PinsConf[id].dcinter);
					cJSON_AddNumberToObject(root, "pclick", PinsConf[id].pclick);
					cJSON_AddStringToObject(root, "info", PinsConf[id].info);
					cJSON_AddNumberToObject(root, "onoff", PinsConf[id].onoff);
					str = cJSON_PrintUnformatted(root);
					cJSON_Delete(root);
				}

				if(tab == 2){
					root = cJSON_CreateObject();
					cJSON_AddNumberToObject(root, "topin", PinsConf[id].topin);
					cJSON_AddNumberToObject(root, "id", id + 1);  // id numbering from 1
					cJSON_AddStringToObject(root, "pins", PinsInfo[id].pins);
					cJSON_AddNumberToObject(root, "ptype", PinsConf[id].ptype);
					cJSON_AddNumberToObject(root, "pwm", PinsConf[id].pwm);
					cJSON_AddNumberToObject(root, "on", PinsConf[id].on);
					cJSON_AddNumberToObject(root, "istate", PinsConf[id].istate);
					cJSON_AddNumberToObject(root, "dvalue", PinsConf[id].dvalue);
					cJSON_AddNumberToObject(root, "ponr", PinsConf[id].ponr);
					cJSON_AddStringToObject(root, "info", PinsConf[id].info);
					cJSON_AddNumberToObject(root, "onoff", PinsConf[id].onoff);
					str = cJSON_PrintUnformatted(root);
					cJSON_Delete(root);
				}

				if(tab == 3){
					root = cJSON_CreateObject();

					cJSON_AddNumberToObject(root, "id",id + 1);
					cJSON_AddStringToObject(root, "pins", PinsInfo[id].pins);
					fld = cJSON_CreateObject();

					while (variable <=  NUMPIN - 1) {
						if (PinsConf[variable].topin == 2) {
							cJSON_AddNumberToObject(fld, PinsInfo[variable].pins, variable + 1);
						}
						variable++;
					}
					variable = 0;

					cJSON_AddItemToObject(root, "rpins", fld);

					str = cJSON_PrintUnformatted(root);
					cJSON_Delete(root);
				}
				if(tab == 4){
					root = cJSON_CreateObject();
					cJSON_AddStringToObject(root, "lang", SetSettings.lang);
					cJSON_AddNumberToObject(root, "lon_de", SetSettings.lon_de);
					cJSON_AddNumberToObject(root, "lat_de", SetSettings.lat_de);
					cJSON_AddNumberToObject(root, "check_mqtt", SetSettings.check_mqtt);
					cJSON_AddNumberToObject(root, "mqtt_prt", SetSettings.mqtt_prt);
					cJSON_AddStringToObject(root, "mqtt_clt", SetSettings.mqtt_clt);
					cJSON_AddStringToObject(root, "mqtt_usr", SetSettings.mqtt_usr);
					cJSON_AddStringToObject(root, "mqtt_pswd", SetSettings.mqtt_pswd);
					cJSON_AddStringToObject(root, "mqtt_tpc", SetSettings.mqtt_tpc);
					cJSON_AddStringToObject(root, "mqtt_ftpc", SetSettings.mqtt_ftpc);
					cJSON_AddNumberToObject(root, "mqtt_hst0", SetSettings.mqtt_hst0);
					cJSON_AddNumberToObject(root, "mqtt_hst1", SetSettings.mqtt_hst1);
					cJSON_AddNumberToObject(root, "mqtt_hst2", SetSettings.mqtt_hst2);
					cJSON_AddNumberToObject(root, "mqtt_hst3", SetSettings.mqtt_hst3);
					cJSON_AddNumberToObject(root, "check_ip", SetSettings.check_ip);
					cJSON_AddNumberToObject(root, "ip_addr0", SetSettings.ip_addr0);
					cJSON_AddNumberToObject(root, "ip_addr1", SetSettings.ip_addr1);
					cJSON_AddNumberToObject(root, "ip_addr2", SetSettings.ip_addr2);
					cJSON_AddNumberToObject(root, "ip_addr3", SetSettings.ip_addr3);
					cJSON_AddNumberToObject(root, "sb_mask0", SetSettings.sb_mask0);
					cJSON_AddNumberToObject(root, "sb_mask1", SetSettings.sb_mask1);
					cJSON_AddNumberToObject(root, "sb_mask2", SetSettings.sb_mask2);
					cJSON_AddNumberToObject(root, "sb_mask3", SetSettings.sb_mask3);
					cJSON_AddNumberToObject(root, "gateway0", SetSettings.gateway0);
					cJSON_AddNumberToObject(root, "gateway1", SetSettings.gateway1);
					cJSON_AddNumberToObject(root, "gateway2", SetSettings.gateway2);
					cJSON_AddNumberToObject(root, "gateway3", SetSettings.gateway3);
					snprintf(macStr, sizeof(macStr), "%02X-%02X-%02X-%02X-%02X-%02X",
							SetSettings.macaddr0, SetSettings.macaddr1, SetSettings.macaddr2, SetSettings.macaddr3, SetSettings.macaddr4, SetSettings.macaddr5);
					cJSON_AddStringToObject(root, "macaddr", macStr);
					cJSON_AddStringToObject(root, "adm_name", SetSettings.adm_name);
					cJSON_AddStringToObject(root, "adm_pswd", SetSettings.adm_pswd);
					cJSON_AddStringToObject(root, "token", SetSettings.token);
					cJSON_AddNumberToObject(root, "timezone", SetSettings.timezone);


					str = cJSON_PrintUnformatted(root);
					cJSON_Delete(root);
				}
				if(tab == 5){
					root = cJSON_CreateObject();

					cJSON_AddNumberToObject(root, "id", id); // в JS доваляеме +1
					cJSON_AddStringToObject(root, "cron", dbCrontxt[id].cron);
					cJSON_AddStringToObject(root, "activ", dbCrontxt[id].activ);
					cJSON_AddStringToObject(root, "info", dbCrontxt[id].info);
					str = cJSON_PrintUnformatted(root);
					cJSON_Delete(root);
				}
			}

			sprintf(pcInsert, "%s", str);
			free(str);
			return strlen(pcInsert);
			break;
			// ssi tag <!--#cronjson-->
			case 6:

				root = cJSON_CreateArray();
				i = 0;
				fld = cJSON_CreateObject();
				while (i <= MAXSIZE - 1) {
					cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
					cJSON_AddNumberToObject(fld, "id", i);
					cJSON_AddStringToObject(fld, "cron", dbCrontxt[i].cron);
					cJSON_AddStringToObject(fld, "activ", dbCrontxt[i].activ);
					cJSON_AddStringToObject(fld, "info", dbCrontxt[i].info);

					i++;
				}

				str = cJSON_Print(root);

				cJSON_Delete(root);
				sprintf(pcInsert, "%s", str);
				free(str);

				return strlen(pcInsert);
				break;
		default:
			break;
	}
	return 0;
}

//////////////////////////////  CGI HANDLER  //////////////////////////////////

const char* FormCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* LoginCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* SelectCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* RelayCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* ButtonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* SettingCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* TimerCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* LogoutCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* TabjsonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* SelectSetCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* FormRelayCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* FormButtonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* FormPinToPinCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* OnOffSetCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* FormjsonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* SettingsCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* FormcronCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* CronCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* RebootCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);
const char* ApiCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);

static const tCGI URL_TABLES[] = {
		{"/index.shtml", (tCGIHandler) FormCGI_Handler },
		{"/logon.shtml", (tCGIHandler) LoginCGI_Handler },
		{"/select.shtml", (tCGIHandler) SelectCGI_Handler },
		{"/tabrelay.shtml", (tCGIHandler) RelayCGI_Handler },
		{"/tabbuttom.shtml", (tCGIHandler) ButtonCGI_Handler },
		{"/settings.shtml", (tCGIHandler) SettingCGI_Handler },
		{"/timers.shtml", (tCGIHandler) TimerCGI_Handler },
		{"/logout.shtml", (tCGIHandler) LogoutCGI_Handler },
		{"/tabjson.shtml", (tCGIHandler) TabjsonCGI_Handler },
		{"/selectset.shtml", (tCGIHandler) SelectSetCGI_Handler },
		{"/formrelay.shtml", (tCGIHandler) FormRelayCGI_Handler },
		{"/formbuttom.shtml", (tCGIHandler) FormButtonCGI_Handler },
		{"/formtopin.shtml", (tCGIHandler) FormPinToPinCGI_Handler },
		{"/onoffset.shtml", (tCGIHandler) OnOffSetCGI_Handler },
		{"/formjson.shtml", (tCGIHandler) FormjsonCGI_Handler },
		{"/settings.shtml", (tCGIHandler) SettingsCGI_Handler },
		{"/formcron.shtml", (tCGIHandler) FormcronCGI_Handler },
		{"/tabcron.shtml", (tCGIHandler) CronCGI_Handler },
		{"/reboot.shtml", (tCGIHandler) RebootCGI_Handler },
		{"/api.shtml", (tCGIHandler) ApiCGI_Handler }
};

const uint8_t CGI_URL_NUM = (sizeof(URL_TABLES) / sizeof(tCGI));


// index.shtml Handler (Index 0)
const char* FormCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 0) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}	}

	//printf("URL %s \n", URL_TABLES[iIndex].pcCGIName);

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return URL_TABLES[iIndex].pcCGIName;
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}


// Login Handler (Index 1)
const char* LoginCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 1) {
		for (int i = 0; i < iNumParams; i++) {
			if(strcmp(pcParam[i], "username") == 0){
				printf("OK Get 1 \n");
				if(strcmp(pcValue[i], SetSettings.adm_name) == 0){
					verifyNum++;
				}

			} else if(strcmp(pcParam[i], "password") == 0) {
				printf("OK Get 2 \n");
				if(strcmp(pcValue[i], SetSettings.adm_pswd) == 0){
					verifyNum++;
				}
			}
		}
	}

	return "/logon.shtml";
}


// select.shtml Handler (Index 2)
const char* SelectCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {


	if (iIndex == 2) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return URL_TABLES[iIndex].pcCGIName;  //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


// tabrelay.shtml Handler (Index 3)
const char* RelayCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {


	if (iIndex == 3) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if(1){
		//printf("SSID OK \n");
		restartSSID();
		return "/tabrelay.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// tabbuttom.shtml Handler (Index 4)
const char* ButtonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	int idin = 0;
	int idout = 0;
	int i = 0;
	int del = 0;
	uint16_t usbdata = 0;

	if (iIndex == 4) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "idin") == 0)
			{
				idin = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "idout") == 0)
			{
				idout = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "del") == 0)
			{
				del = atoi(pcValue[i]);
			}
		}
		if(idin != 0 && idout != 0){
			// @todo проверка передзаписью превязан ли этот пин уже или нет
			while (i <= NUMPINLINKS - 1) {
				if (PinsLinks[i].flag == 0) {
					//printf("flag %d \n", i);
					PinsLinks[i].idin = idin - 1;
					PinsLinks[i].idout = idout - 1;
					PinsLinks[i].flag = 1;
					usbdata = 4;
					break;
				}
				i++;
			}
			if(usbdata != 0){
				xQueueSend(usbQueueHandle, &usbdata, 0);
				printf("usbdata = 4 \n");
			}
			usbdata = 0;

		}
		if(del != 0){
			PinsLinks[del-1].idin = 0;
			PinsLinks[del-1].idout = 0;
			PinsLinks[del-1].flag = 0;
			usbdata = 4;
			if(usbdata != 0){
				xQueueSend(usbQueueHandle, &usbdata, 0);
				printf("usbdata = 4 \n");
			}
			usbdata = 0;
		}
	}

	/* login succeeded */
	if(1){
		//printf("SSID OK \n");
		restartSSID();
		return "/tabbuttom.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// settings.shtml Handler (Index 5)
const char* SettingCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {


	if (iIndex == 5) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if(1){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
		return "/settings.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}

// timers.shtml Handler (Index 6)
const char* TimerCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {


	if (iIndex == 6) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if(1){
		//printf("SSID OK \n");
		restartSSID();
		return "/timers.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// logout.shtml Handler logout (Index 7)
const char* LogoutCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 7) {
		memset(randomSSID, '\0', sizeof(randomSSID));
	}

	return "/login.shtml";
}


// tabjson.shtml Handler table json (Index 8)
const char* TabjsonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 8) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
				if(tab == 2)
				{
					numTabLine = MultiPartTabCount(1,NUMPIN-1, numTabLine);
				}
				if(tab == 3)
				{
					numTabLine = MultiPartTabCount(2,NUMPIN-1, numTabLine);
				}
				if(tab == 4)
				{
					numTabLine = MultiPartTabCount(3,NUMPIN-1, numTabLine);
				}
			}
		}
	}

	/* login succeeded */
	if(1){
		//printf("SSID OK \n");
		restartSSID();
		return "/tabjson.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


// selectset.shtml Handler (Index 9)
const char* SelectSetCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	int varid;
	int val;

	if (iIndex == 9) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				varid = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "val") == 0)
			{
				val = atoi(pcValue[i]);
			}
		}
		PinsConf[varid].topin = val;
		if (val == 1 || val == 2 || val == 3){
			PinsConf[varid].onoff = 1;
		}else{
			PinsConf[varid].onoff = 0;
			clearPin(varid);
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		restartSSID();
		return "/selectset.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}


// formbuttom.shtml Handler (Index 10)
const char* FormRelayCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	id = 0;
	tab = 0;

	if (iIndex == 10) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]) - 1;
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/formrelay.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// formbuttom.shtml Handler (Index 11)
const char* FormButtonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	id = 0;
	tab = 0;

	if (iIndex == 11) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]) - 1;
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/formbuttom.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// formtopin.shtml Handler (Index 12)
const char* FormPinToPinCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {


	if (iIndex == 12) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]) - 1;
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/formtopin.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


// onoffset.shtml Handler (Index 13)
const char* OnOffSetCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	int varid;
	int val;

	if (iIndex == 13) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				varid = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "val") == 0)
			{
				val = atoi(pcValue[i]);
			}
		}
		PinsConf[varid-1].onoff = val;
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/selectset.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}


// formjson.shtml Handler table json (Index 14)
const char*  FormjsonCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 14) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/formjson.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// settings.shtml Handler table json (Index 15)
const char*  SettingsCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 15) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/settings.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


// formcron.shtml Handler table json (Index 16)
const char*  FormcronCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	if (iIndex == 16) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]) - 1;
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/formcron.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// tabcron.shtml Handler (Index 17)
const char* CronCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {
	int del = 0;
	uint16_t usbdata = 3;

	if (iIndex == 17) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0) {
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "del") == 0) {
				del = atoi(pcValue[i])-1;
				memset(dbCrontxt[del].cron, '\0', sizeof(dbCrontxt[del].cron));
				memset(dbCrontxt[del].activ, '\0', sizeof(dbCrontxt[del].activ));
				memset(dbCrontxt[del].info, '\0', sizeof(dbCrontxt[del].info));

				xQueueSend(usbQueueHandle, &usbdata, 0);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/tabcron.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}

// Reboot.shtml Handler (Index 18)
const char* RebootCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]) {
	int rb = 0;

	if (iIndex == 18) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				memset(ssid, '\0', sizeof(ssid));
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "rb") == 0)
			{
				rb = atoi(pcValue[i]);
				if(rb == 1){
					NVIC_SystemReset(); // REBOOT
				}
			}
		}
	}


	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		//printf("SSID OK \n");
		restartSSID();
		return "/reboot.shtml";
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}

// api.shtml Handler (Index 19)
const char* ApiCGI_Handler(int iIndex, int iNumParams, char *pcParam[],char *pcValue[]) {
	int pinid = 0;
	int action = 0;
	char token[11] = {0};



	if (iIndex == 19) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "token") == 0)
			{
				memset(token, '\0', sizeof(token));
				strcpy(token, pcValue[i]);

			}
			if (strcmp(pcParam[i], "pinid") == 0)
			{
				pinid = atoi(pcValue[i]);

			}
			if (strcmp(pcParam[i], "action") == 0)
			{
				action = atoi(pcValue[i]);

			}
		}
	}
	if(strcmp(token, SetSettings.token) == 0 && pinid != 0){
		printf("token OK \n");
		if(PinsConf[pinid-1].topin == 2){
			printf("relay OK \n");
			data_pin.pin = pinid-1;
			data_pin.action = action;
			xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
		}
	}


	return "/api.shtml";

}
////////////////////////////// POST START //////////////////////////////////


PostBufer_t v_PostBufer;
static void *current_connection;


// POST request Relay
void setPinRelay(int idpin, char *name, char *token) {

	idpin = idpin - 1;
	if (strcmp(name, "pwm") == 0) {
		PinsConf[idpin].pwm = atoi(token);
	} else if (strcmp(name, "on") == 0) {
		PinsConf[idpin].on = atoi(token);
	} else if (strcmp(name, "istate") == 0) {
		PinsConf[idpin].istate = atoi(token);
	} else if (strcmp(name, "dvalue") == 0) {
		PinsConf[idpin].dvalue = atoi(token);
	} else if (strcmp(name, "ponr") == 0) {
		PinsConf[idpin].ponr = atoi(token);
	} else if (strcmp(name, "info") == 0) {
		strcpy(PinsConf[idpin].info, token);
	} else if (strcmp(name, "onoff") == 0) {
		PinsConf[idpin].onoff = atoi(token);
	} else {

	}
}


// POST request Buttom
void setPinButtom(int idpin, char *name, char *token) {

	idpin = idpin - 1;
	if (strcmp(name, "ptype") == 0) {
		PinsConf[idpin].ptype = atoi(token);
	} else if (strcmp(name, "binter") == 0) {
		PinsConf[idpin].binter = atoi(token);
	} else if (strcmp(name, "hinter") == 0) {
		PinsConf[idpin].hinter = atoi(token);
	} else if (strcmp(name, "repeat") == 0) {
		PinsConf[idpin].repeat = atoi(token);
	} else if (strcmp(name, "rinter") == 0) {
		PinsConf[idpin].rinter = atoi(token);
	} else if (strcmp(name, "dcinter") == 0) {
		PinsConf[idpin].dcinter = atoi(token);
	} else if (strcmp(name, "pclick") == 0) {
		PinsConf[idpin].pclick = atoi(token);
	} else if (strcmp(name, "info") == 0) {
		strcpy(PinsConf[idpin].info, token);
	} else if (strcmp(name, "onoff") == 0) {
		PinsConf[idpin].onoff = atoi(token);
	} else {

	}
}

// POST request Cron
void setCron(int idpin, char *name, char *token) {

    char decoded_url[50] = {0};

	idpin = idpin - 1;
	if (strcmp(name, "cron") == 0) {
		url_decode(token, decoded_url);
		strcpy(dbCrontxt[idpin].cron, decoded_url);
	} else if (strcmp(name, "activ") == 0) {
		url_decode(token, decoded_url);
		strcpy(dbCrontxt[idpin].activ, decoded_url);
	} else if (strcmp(name, "info") == 0) {
		strcpy(dbCrontxt[idpin].info, token);
	}  else {

	}
}
// Функция декодирования URL
void url_decode(char* url, char* decoded)
{
    int i = 0, j = 0;
    while (url[i] != '\0') {
        if (url[i] == '%') {
            int num;
            sscanf(&url[i+1], "%2x", &num);
            decoded[j] = (char)num;
            i += 3;  // Пропускаем 3 символа: %XY
        } else if (url[i] == '+') {
            decoded[j] = ' ';
            i++;
        } else {
            decoded[j] = url[i];
            i++;
        }
        j++;
    }
    decoded[j] = '\0';
}

// Parser IP address
void parserIP(char *data, unsigned char *value)
{
    int index = 0;
        while (*data) {
        if (isdigit((unsigned char)*data)) {
            value[index] *= 10;
            value[index] += *data - '0';
        } else {
            index++;
        }
        data++;
    }
}


// POST request Settings
void setSettings(char *name, char *token) {
	char ipstr[34] = {0};
	unsigned char value[4] = {0};
	int values[6];

	if (strcmp(name, "check_mqtt") == 0) {
		SetSettings.check_mqtt = atoi(token);
	} else if (strcmp(name, "check_ip") == 0) {
		SetSettings.check_ip = atoi(token);
	} else if (strcmp(name, "ip_addr") == 0) {
		strcpy(ipstr, token);
		parserIP(ipstr, value);
		SetSettings.ip_addr0 = value[0];
		SetSettings.ip_addr1 = value[1];
		SetSettings.ip_addr2 = value[2];
		SetSettings.ip_addr3 = value[3];
	} else if (strcmp(name, "sb_mask") == 0) {
		strcpy(ipstr, token);
		parserIP(ipstr, value);
		SetSettings.sb_mask0 = value[0];
		SetSettings.sb_mask1 = value[1];
		SetSettings.sb_mask2 = value[2];
		SetSettings.sb_mask3 = value[3];
	} else if (strcmp(name, "gateway") == 0) {
		strcpy(ipstr, token);
		parserIP(ipstr, value);
		SetSettings.gateway0 = value[0];
		SetSettings.gateway1 = value[1];
		SetSettings.gateway2 = value[2];
		SetSettings.gateway3 = value[3];
	} else if (strcmp(name, "macaddr") == 0) {
		strcpy(ipstr, token);
		//uint8_t bytes[6];
		if( 6 == sscanf(ipstr, "%x-%x-%x-%x-%x-%x%*c", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])){
		    /* convert to uint8_t */
		    SetSettings.macaddr0 = (uint8_t) values[0];
		    SetSettings.macaddr1 = (uint8_t) values[1];
		    SetSettings.macaddr2 = (uint8_t) values[2];
		    SetSettings.macaddr3 = (uint8_t) values[3];
		    SetSettings.macaddr4 = (uint8_t) values[4];
		    SetSettings.macaddr5 = (uint8_t) values[5];
		}
	} else if (strcmp(name, "mqtt_hst") == 0) {
		strcpy(ipstr, token);
		parserIP(ipstr, value);
		SetSettings.mqtt_hst0 = value[0];
		SetSettings.mqtt_hst1 = value[1];
		SetSettings.mqtt_hst2 = value[2];
		SetSettings.mqtt_hst3 = value[3];
	} else if (strcmp(name, "mqtt_prt") == 0) {
		SetSettings.mqtt_prt = atoi(token);
	} else if (strcmp(name, "mqtt_clt") == 0) {
		strcpy(SetSettings.mqtt_clt, token);
	} else if (strcmp(name, "mqtt_usr") == 0) {
		strcpy(SetSettings.mqtt_usr, token);
	} else if (strcmp(name, "mqtt_pswd") == 0) {
		strcpy(SetSettings.mqtt_pswd, token);
	} else if (strcmp(name, "mqtt_tpc") == 0) {
		strcpy(SetSettings.mqtt_tpc, token);
	} else if (strcmp(name, "mqtt_ftpc") == 0) {
		strcpy(SetSettings.mqtt_ftpc, token);
	} else if (strcmp(name, "lang") == 0) {
		strcpy(SetSettings.lang, token);
	} else if (strcmp(name, "timezone") == 0) {
		SetSettings.timezone = atoi(token);
	} else if (strcmp(name, "adm_name") == 0) {
		strcpy(SetSettings.adm_name, token);
	} else if (strcmp(name, "adm_pswd") == 0) {
		strcpy(SetSettings.adm_pswd, token);
	} else if (strcmp(name, "token") == 0) {
		strcpy(SetSettings.token, token);
	} else {

	}
}



err_t httpd_post_begin(void *connection, const char *uri,
		const char *http_request, uint16_t http_request_len, int content_len,
		char *response_uri, uint16_t response_uri_len, uint8_t *post_auto_wnd) {

	  LWIP_UNUSED_ARG(connection);
	  LWIP_UNUSED_ARG(http_request);
	  LWIP_UNUSED_ARG(http_request_len);
	  LWIP_UNUSED_ARG(content_len);
	  LWIP_UNUSED_ARG(post_auto_wnd);


		printf("response_uri: %s \n", response_uri);

	  v_PostBufer.len = 0;
	  memset(v_PostBufer.buf, '\0', sizeof(v_PostBufer.buf));
	  memset(v_PostBufer.uri, '\0', sizeof(v_PostBufer.uri));

	  // parse URI to "?"
	  int c = strchr(uri, '?') - uri;
	  strncpy(v_PostBufer.uri, uri, c);
	  v_PostBufer.uri[c] = '\0';


	  if (current_connection != connection) {
	      current_connection = connection;
	      *post_auto_wnd = 1;

	      return ERR_OK;
	  }
	  return ERR_VAL;
}



err_t httpd_post_receive_data(void *connection, struct pbuf *p) {

	memset(tempbufer, '\0', sizeof(tempbufer));

	if (current_connection == connection && p != NULL) {
	  if(strcmp("upgrade.shtml", v_PostBufer.uri) == 0){

		  // @todo  Firmware update in flash

		  return ERR_OK;
	  } else {
		  strncpy(tempbufer, p->payload, p->len);
		  strcat(v_PostBufer.buf, tempbufer);
		  pbuf_free(p);
		  printf("POST 1 \n");

		  return ERR_OK;
	  }

	    if (p != NULL) {
	        pbuf_free(p);
	    }
		return ERR_OK;
	}

	if (p != NULL) {
		pbuf_free(p);
	}
	return ERR_VAL;
}


void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) {

    int count = 0;
    int id = 0;
	char *end_str;
	char *name;
	uint16_t usbdata = 0;
	//printf("POST %s \n", v_PostBufer.buf);

    char *token = strtok_r(v_PostBufer.buf, "&", &end_str);
    while (token != NULL)
    {
        char *end_token;
        //printf("---- %s \n", token);
        char *token2 = strtok_r(token, "=", &end_token);
        while (token2 != NULL)
        {
        	count++;
        	if(count == 1){
        	    name = token2;
        	}
        	if(count == 2){
        		// SET id
        		if (strcmp(name, "id") == 0){
        			id = atoi(token2);
        		}
        		// POST request Relay
        		if (strcmp(v_PostBufer.uri, "/tabrelay.shtml") == 0 && id != 0){
        			if(token2 != NULL){
        				setPinRelay(id, name, token2);
        				usbdata = 1;
        			}
        		}
        		// POST request Buttom
        		if (strcmp(v_PostBufer.uri, "/tabbuttom.shtml") == 0 && id != 0){
        			if(token2 != NULL){
        				setPinButtom(id, name, token2);
        				usbdata = 1;
        			}
        		}
        		// POST request Settings
        		if (strcmp(v_PostBufer.uri, "/settings.shtml") == 0){
        			if(token2 != NULL){
        				setSettings(name, token2);
        				usbdata = 2;
        			}
        		}
        		// POST request Cron
				if (strcmp(v_PostBufer.uri, "/tabcron.shtml") == 0){
					if(token2 != NULL){
						setCron(id, name, token2);
						usbdata = 3;
					}
				}

        	}
            token2 = strtok_r(NULL, "=", &end_token);
        }
        token = strtok_r(NULL, "&", &end_str);
        count = 0;
    }


	if (current_connection == connection) {
	    /* login succeeded */

		//printf("URL %s \n", v_PostBufer.uri);

/******************************************************************************************/
		// Отправка числа в очередь
		if(usbdata != 0){
			xQueueSend(usbQueueHandle, &usbdata, 0);
		}
		usbdata = 0;

/******************************************************************************************/

		restartSSID();
		snprintf(response_uri, response_uri_len, v_PostBufer.uri);

		current_connection = NULL;

	}
}

//////////////////// POST  END ////////////////////////



void http_server_init(void) {
	httpd_init();

	http_set_cgi_handlers(URL_TABLES, CGI_URL_NUM); //
	http_set_ssi_handler((tSSIHandler) ssi_handler, (char const**) TAGS, SSI_TAG_NUM); //
}
