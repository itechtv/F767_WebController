/*
 * webserver.h
 *
 *  Created on: Dec 14, 2022
 *      Author: Anton & Denis
 */

#ifndef INC_WEBSERVER_H_
#define INC_WEBSERVER_H_
#include <stdint.h>
#include "main.h"


void http_server_init (void);
void restartSSID (void);
void url_decode(char* url, char* decoded);



typedef struct tagPostBufer_t
{
	uint32_t len;
	char buf[2048];
	char uri[25];
}PostBufer_t;

extern PostBufer_t v_PostBufer;



#endif /* INC_WEBSERVER_H_ */
