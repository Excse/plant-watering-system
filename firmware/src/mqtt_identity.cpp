#include "mqtt_identity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "rhadar.h"

static const char *TAG = "mqtt_identity";

static esp_err_t discovery_validation_error(const rhadar::ValidationError& error) {
    ESP_LOGE(TAG, "Invalid discovery field %s: %s", error.field.c_str(), error.message.c_str());
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t moisture_discovery_init(mqtt_identity_t *identity) {
    const std::string device_id = identity->device_id;
    auto device = rhadar::DeviceBuilder{device_id}
        .name("PWS " + device_id)
        .manufacturer("DIY")
        .model("ESP32")
        .build();
    if (!device) {
        return discovery_validation_error(device.error());
    }

    auto origin = rhadar::OriginBuilder{"esp32-idf"}
        .sw_version("1.0.0")
        .build();
    if (!origin) {
        return discovery_validation_error(origin.error());
    }

    auto sensor = rhadar::SensorBuilder{device_id + "_moisture"}
        .name("Soil moisture")
        .state_topic(identity->moisture_state_topic)
        .availability_topic(identity->availability_topic)
        .device_class(rhadar::SensorDeviceClass::Moisture)
        .state_class(rhadar::SensorStateClass::Measurement)
        .unit_of_measurement("%")
        .suggested_display_precision(0)
        .build();
    if (!sensor) {
        return discovery_validation_error(sensor.error());
    }

    auto message = rhadar::MessageBuilder{*device, *origin, device_id}
        .add_component("moisture", *sensor)
        .qos(1)
        .build();
    if (!message) {
        return discovery_validation_error(message.error());
    }
    if (message->topic().size() >= sizeof(identity->moisture_discovery_topic)) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Copy the built message into the identity's lifetime-owned buffers.
    identity->moisture_discovery_payload = strdup(message->payload().c_str());
    if (identity->moisture_discovery_payload == nullptr) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(identity->moisture_discovery_topic, message->topic().c_str(), message->topic().size() + 1);
    identity->moisture_discovery_qos = message->qos();
    identity->moisture_discovery_retain = message->retain();
    return ESP_OK;
}

esp_err_t mqtt_identity_init(mqtt_identity_t *identity) {
    if (identity == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(identity, 0, sizeof(*identity));

    // Use the factory-programmed base MAC address as a stable, device-specific identifier
    uint8_t mac[6];

    esp_err_t mac_error = esp_efuse_mac_get_default(mac);
    if (mac_error != ESP_OK) {
        return mac_error;
    }

    snprintf(
        identity->device_id,
        sizeof(identity->device_id),
        "pws_%02x%02x%02x%02x%02x%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );

    snprintf(
        identity->legacy_moisture_discovery_topic,
        sizeof(identity->legacy_moisture_discovery_topic),
        "homeassistant/sensor/%s/moisture/config",
        identity->device_id
    );

    snprintf(
        identity->moisture_state_topic,
        sizeof(identity->moisture_state_topic),
        "%s/moisture/state",
        identity->device_id
    );

    snprintf(
        identity->availability_topic,
        sizeof(identity->availability_topic),
        "%s/status",
        identity->device_id
    );

    const esp_err_t discovery_error = moisture_discovery_init(identity);
    if (discovery_error != ESP_OK) {
        mqtt_identity_destroy(identity);
        return discovery_error;
    }

    ESP_LOGI(TAG, "Device identifier: %s", identity->device_id);
    ESP_LOGD(TAG, "Discovery topic: %s", identity->moisture_discovery_topic);

    return ESP_OK;
}

void mqtt_identity_destroy(mqtt_identity_t *identity) {
    if (identity == NULL) {
        return;
    }

    free(identity->moisture_discovery_payload);
    identity->moisture_discovery_payload = NULL;
}
