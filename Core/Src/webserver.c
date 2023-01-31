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
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "stm32f7xx_hal.h"
#include "math.h"
#include "time.h"
#include "cJSON.h"
#include "db.h"




static void *current_connection;
static int variable = 0;

uint8_t verifyNum = 0;
uint8_t tab = 0;
uint8_t id = 0;
uint8_t numTabLine = 0;
uint8_t countJson = 0;

char randomSSID[27] = {0};
char url[30] = {0};
char bufervar[1500] = {0};
extern unsigned long Ti;



extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];
extern struct dbPinToPin PinsLinks[NUMPINLINKS];
extern struct dbSettings SetSettings;



//////////////////////////

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

//////////////////////////////  SSI MULTIPART Function  ///////////////////////


// Counting the number of lines JSON
int MultiPartTabCount(int num, int pinnum)
{
	int count = 0;
	for (int i = 0; i <= pinnum; i++){
		if(num == PinsConf[i].topin && num == 1){
			count++;
		}
		if(num == PinsConf[i].topin && num == 2){
			count++;
		}
	}
	return count;
}
//////////////////////////////  SSI HANDLER  //////////////////////////////////

char const *TAGCHAR[] = { "tabjson", "ssid", "check", "menu", "lang", "formjson"};
char const **TAGS = TAGCHAR;


const uint8_t SSI_TAG_NUM = (sizeof(TAGCHAR) / sizeof(TAGCHAR[0]));

static u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen,
		u16_t current_tag_part, u16_t *next_tag_part) {

	char* str = NULL;
	cJSON *root = NULL;
	cJSON *fld = NULL;

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
				sprintf(pcInsert,"{\"id\":%d,\"pins\":\"%s\",\"topin\":%d},",
						variable, PinsInfo[variable].pins, PinsConf[variable].topin);

				if(variable == (NUMPIN-1)){
					pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON
				}
			}
			if(tab == 2){
				if(PinsConf[variable].topin == 1){
					// buttoms json
					sprintf(pcInsert,
							"{\"topin\":%d,\"id\":%d,\"pins\":\"%s\",\"ptype\":\"%s\",\"binter\":%d,\"hinter\":%d,\"repeat\":%d,\"rinter\":%d,\"dcinter\":%d,\"pclick\":%d,\"pinact\":%s,\"info\":\"%s\",\"onoff\":%d},",
							PinsConf[variable].topin, variable, PinsInfo[variable].pins,
							PinsConf[variable].ptype, PinsConf[variable].binter,
							PinsConf[variable].hinter, PinsConf[variable].repeat,
							PinsConf[variable].rinter, PinsConf[variable].dcinter,
							PinsConf[variable].pclick, "[{}]", PinsConf[variable].info,
							PinsConf[variable].onoff);
					countJson++;
					////////////////
				} else {
					pcInsert = "";
				}

				if(countJson == numTabLine){
					pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON
				}
			}
			if(tab == 3){
				if(PinsConf[variable].topin == 2){
					// relay json
					sprintf(pcInsert,
							"{\"topin\":%d,\"id\":%d,\"pins\":\"%s\",\"ptype\":\"%s\",\"pwm\":%d,\"on\":%d,\"istate\":%d,\"dvalue\":%d,\"ponr\":%d,\"info\":\"%s\",\"onoff\":%d},",
							PinsConf[variable].topin, variable, PinsInfo[variable].pins,
							PinsConf[variable].ptype, PinsConf[variable].pwm, PinsConf[variable].on,
							PinsConf[variable].istate, PinsConf[variable].dvalue,
							PinsConf[variable].ponr, PinsConf[variable].info,
							PinsConf[variable].onoff);
					////////////////
					countJson++;
				} else {
					pcInsert = "";
				}

				if(countJson == numTabLine){
					pcInsert[strlen(pcInsert) - 1] = '\0'; // Удаляем "," из JSON
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
			verifyNum = 0;

			return strlen(pcInsert);
			break;

		// ssi tag <!--#menu-->
		case 3:
			sprintf(pcInsert,"<a href=\"index.shtml?ssid=%s\">Home</a> | <a href=\"select.shtml?ssid=%s\">Select pin</a> | <a href=\"tabbuttom.shtml?ssid=%s\">Buttom pin</a> | <a href=\"tabrelay.shtml?ssid=%s\">Relay pin</a> | <a href=\"timers.shtml?ssid=%s\">Timers</a> | <a href=\"settings.shtml?ssid=%s\">Settings</a> | <a href=\"logout.shtml\">Logout</a> ", randomSSID,randomSSID,randomSSID,randomSSID,randomSSID,randomSSID);
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
				root = cJSON_CreateObject();
				//cJSON_AddItemToArray(root, fld = cJSON_CreateObject());

				if(tab == 1){
					cJSON_AddNumberToObject(root, "topin", PinsConf[id].topin);
					cJSON_AddNumberToObject(root, "id", id);
					cJSON_AddStringToObject(root, "pins", PinsInfo[id].pins);
					cJSON_AddStringToObject(root, "ptype", PinsConf[id].ptype);
					cJSON_AddNumberToObject(root, "binter", PinsConf[id].binter);
					cJSON_AddNumberToObject(root, "hinter", PinsConf[id].hinter);
					cJSON_AddNumberToObject(root, "repeat", PinsConf[id].repeat);
					cJSON_AddNumberToObject(root, "rinter", PinsConf[id].rinter);
					cJSON_AddNumberToObject(root, "dcinter", PinsConf[id].dcinter);
					cJSON_AddNumberToObject(root, "pclick", PinsConf[id].pclick);
					cJSON_AddStringToObject(root, "info", PinsConf[id].info);
					cJSON_AddNumberToObject(root, "onoff", PinsConf[id].onoff);
				}

				if(tab == 2){
					cJSON_AddNumberToObject(root, "topin", PinsConf[id].topin);
					cJSON_AddNumberToObject(root, "id", id);
					cJSON_AddStringToObject(root, "pins", PinsInfo[id].pins);
					cJSON_AddStringToObject(root, "ptype", PinsConf[id].ptype);
					cJSON_AddNumberToObject(root, "pwm", PinsConf[id].binter);
					cJSON_AddNumberToObject(root, "on", PinsConf[id].hinter);
					cJSON_AddNumberToObject(root, "istate", PinsConf[id].repeat);
					cJSON_AddNumberToObject(root, "dvalue", PinsConf[id].rinter);
					cJSON_AddNumberToObject(root, "ponr", PinsConf[id].dcinter);
					cJSON_AddStringToObject(root, "info", PinsConf[id].info);
					cJSON_AddNumberToObject(root, "onoff", PinsConf[id].onoff);
				}

				//str = cJSON_Print(root);
				str = cJSON_PrintUnformatted(root);
				cJSON_Delete(root);
			}

			sprintf(pcInsert, "%s", str);
			return strlen(pcInsert);
			break;

		default:
			break;
	}
	return 0;
}

//////////////////////////////  CGI HANDLER  //////////////////////////////////
// @todo  объединить iIndexы избавиться от дублирования кода,для этого надо получить url

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
		{"/formbuttom.shtml", (tCGIHandler) FormButtonCGI_Handler }
};

const uint8_t CGI_URL_NUM = (sizeof(URL_TABLES) / sizeof(tCGI));


// index.shtml Handler (Index 0)
const char* FormCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	char ssid[27] = {0};

	if (iIndex == 0) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	//printf("URL %s \n", URL_TABLES[iIndex].pcCGIName);

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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
				if(strcmp(pcValue[i], "admin") == 0){
					verifyNum++;
				}

			} else if(strcmp(pcParam[i], "password") == 0) {
				printf("OK Get 2 \n");
				if(strcmp(pcValue[i], "12345678") == 0){
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

	char ssid[27] = {0};

	if (iIndex == 2) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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

	char ssid[27] = {0};

	if (iIndex == 3) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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

	char ssid[27] = {0};

	if (iIndex == 4) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
		return "/tabbuttom.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}

}

// tabbuttom.shtml Handler (Index 5)
const char* SettingCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	char ssid[27] = {0};

	if (iIndex == 5) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
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

// tabbuttom.shtml Handler (Index 6)
const char* TimerCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	char ssid[27] = {0};

	if (iIndex == 6) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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

	char ssid[27] = {0};

	if (iIndex == 8) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
				if(tab == 2)
				{
					numTabLine = MultiPartTabCount(1,NUMPIN);
					printf("count  %d \n", numTabLine);
				}
				if(tab == 3)
				{
					numTabLine = MultiPartTabCount(2,NUMPIN);
					printf("count  %d \n", numTabLine);
				}
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
		return "/tabjson.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


// @todo проверить на пустое значение id и val перед записью
// selectset.shtml Handler (Index 9)
const char* SelectSetCGI_Handler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	char ssid[27] = {0};
	int varid;
	int val;

	if (iIndex == 9) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
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
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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

	char ssid[27] = {0};
	id = 0;
	tab = 0;

	if (iIndex == 10) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
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

	char ssid[27] = {0};
	id = 0;
	tab = 0;

	if (iIndex == 11) {
		for (int i = 0; i < iNumParams; i++) {
			if (strcmp(pcParam[i], "ssid") == 0)
			{
				strcpy(ssid, pcValue[i]);
			}
			if (strcmp(pcParam[i], "id") == 0)
			{
				id = atoi(pcValue[i]);
			}
			if (strcmp(pcParam[i], "tab") == 0)
			{
				tab = atoi(pcValue[i]);
			}
		}
	}

	/* login succeeded */
	if (strcmp (ssid, randomSSID) == 0 && strlen(randomSSID) != 0){
		printf("SSID OK \n");
		restartSSID();
		memset(ssid, '\0', sizeof(ssid));
		return "/formbuttom.shtml"; //
	} else {
		printf("SSID Failed \n");
		memset(randomSSID, '\0', sizeof(randomSSID));
		return "/login.shtml";
	}
}


////////////////////////////// POST START //////////////////////////////////



int http_post_content_len;


err_t httpd_post_begin(void *connection, const char *uri,
		const char *http_request, uint16_t http_request_len, int content_len,
		char *response_uri, uint16_t response_uri_len, uint8_t *post_auto_wnd) {

	  LWIP_UNUSED_ARG(connection);
	  LWIP_UNUSED_ARG(http_request);
	  LWIP_UNUSED_ARG(http_request_len);
	  LWIP_UNUSED_ARG(content_len);
	  LWIP_UNUSED_ARG(post_auto_wnd);

	  strncpy(url, uri, 15);

	  memset(bufervar, '\0', sizeof(bufervar));

	  if (!memcmp(uri, "/login.shtml", 13)) {
	    if (current_connection != connection) {
	      current_connection = connection;

	      snprintf(response_uri, response_uri_len, "/loginfail.shtml");


	      *post_auto_wnd = 1;
	      return ERR_OK;
	    }
	  }
	  return ERR_VAL;

}




err_t httpd_post_receive_data(void *connection, struct pbuf *p) {

	char poststr[1000];

	if (current_connection == connection && p != NULL) {




			   memcpy(poststr, p->payload, p->len);
			   strcat(bufervar, poststr);
			   pbuf_free(p);



		pbuf_free(p);
		return ERR_OK;
	}


	if (p != NULL) {
		pbuf_free(p);
	}
	return ERR_VAL;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) {

	  /* default page is "login failed" */
	  snprintf(response_uri, response_uri_len, "/loginfail.shtml");


	  if (current_connection == connection) {

	    /* login succeeded */
		if (strlen(randomSSID) != 0){
			snprintf(response_uri, response_uri_len, "/session.shtml");
		}

	    current_connection = NULL;

	  }
}

//////////////////// POST  END ////////////////////////



void http_server_init(void) {
	httpd_init();
	http_set_ssi_handler((tSSIHandler) ssi_handler, (char const**) TAGS, SSI_TAG_NUM); //

	http_set_cgi_handlers(URL_TABLES, CGI_URL_NUM); //
}
