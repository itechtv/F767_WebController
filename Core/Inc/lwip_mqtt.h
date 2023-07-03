#ifndef LWIP_MQTT_H
#define LWIP_MQTT_H
#include "lwip/apps/mqtt.h"

void example_do_connect(mqtt_client_t *client, const char * topic);
void example_publish(mqtt_client_t *client, void *arg);


#endif  /* LWIP_MQTT_H */
