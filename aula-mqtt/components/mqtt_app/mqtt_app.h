#ifndef MQTT_APP_H
#define MQTT_APP_H

#include "freertos/event_groups.h"
#include "esp_log.h"

#define MQTT_CONNECTED BIT0

void mqtt_app_start(void);
void mqtt_app_stop(void);

void mqtt_app_subscribe(char *topic, int qos);
void mqtt_app_unsubscribe(char *topic);

void mqtt_app_publish(char *topic, char *payload, int qos, int retain);

int mqtt_app_is_connected(void);

#endif // !MQTT_APP_H
