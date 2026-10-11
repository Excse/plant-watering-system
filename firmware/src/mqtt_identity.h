#pragma once

#include <stdbool.h>

#include "esp_err.h"

#define MQTT_DEVICE_ID_SIZE \
    (sizeof("pws_") - 1 + 12 + 1)

#define MQTT_LEGACY_MOISTURE_DISCOVERY_TOPIC_SIZE \
    (sizeof("homeassistant/sensor/") + MQTT_DEVICE_ID_SIZE + sizeof("/moisture/config") - 2)

#define MQTT_MOISTURE_DISCOVERY_TOPIC_SIZE \
    (sizeof("homeassistant/device/") + MQTT_DEVICE_ID_SIZE + sizeof("/config") - 2)

#define MQTT_MOISTURE_STATE_TOPIC_SIZE \
    (MQTT_DEVICE_ID_SIZE + sizeof("/moisture/state") - 1)

#define MQTT_AVAILABILITY_TOPIC_SIZE \
    (MQTT_DEVICE_ID_SIZE + sizeof("/status") - 1)

typedef struct {
    char device_id[MQTT_DEVICE_ID_SIZE];
    
    char moisture_discovery_topic[MQTT_MOISTURE_DISCOVERY_TOPIC_SIZE];
    char legacy_moisture_discovery_topic[MQTT_LEGACY_MOISTURE_DISCOVERY_TOPIC_SIZE];
    char moisture_state_topic[MQTT_MOISTURE_STATE_TOPIC_SIZE];
    char availability_topic[MQTT_AVAILABILITY_TOPIC_SIZE];

    char *moisture_discovery_payload;
    int moisture_discovery_qos;
    bool moisture_discovery_retain;
} mqtt_identity_t;

esp_err_t mqtt_identity_init(mqtt_identity_t *identity);

void mqtt_identity_destroy(mqtt_identity_t *identity);
