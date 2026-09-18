#pragma once

#include "esp_err.h"

#define MQTT_DEVICE_ID_SIZE \
    (sizeof("pws_") - 1 + 12 + 1)

#define MQTT_DISCOVERY_TOPIC_SIZE \
    (sizeof("homeassistant/device/") + MQTT_DEVICE_ID_SIZE + sizeof("/config") - 2)

#define MQTT_COMMAND_TOPIC_SIZE \
    (MQTT_DEVICE_ID_SIZE + sizeof("/led/set") - 1)

#define MQTT_STATE_TOPIC_SIZE \
    (MQTT_DEVICE_ID_SIZE + sizeof("/led/state") - 1)

#define MQTT_AVAILABILITY_TOPIC_SIZE \
    (MQTT_DEVICE_ID_SIZE + sizeof("/status") - 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char device_id[MQTT_DEVICE_ID_SIZE];
    
    char discovery_topic[MQTT_DISCOVERY_TOPIC_SIZE];
    char command_topic[MQTT_COMMAND_TOPIC_SIZE];
    char state_topic[MQTT_STATE_TOPIC_SIZE];
    char availability_topic[MQTT_AVAILABILITY_TOPIC_SIZE];

    char *discovery_payload;
} mqtt_identity_t;

esp_err_t mqtt_identity_init(mqtt_identity_t *identity);

void mqtt_identity_destroy(mqtt_identity_t *identity);

#ifdef __cplusplus
}
#endif
