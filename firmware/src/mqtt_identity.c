#include "mqtt_identity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"

static const char *TAG = "mqtt_identity";

extern const char DISCOVERY_TEMPLATE_START[] asm("_binary_discovery_json_start");
extern const char DISCOVERY_TEMPLATE_END[] asm("_binary_discovery_json_end");

static char *replace_token(const char *source, const char *token, const char *replacement) {
    if (source == NULL || token == NULL || replacement == NULL) {
        return NULL;
    }

    const size_t replacement_length = strlen(replacement);
    const size_t token_length = strlen(token);

    // An empty token would cause the search loop to never advance
    if (token_length == 0) {
        return NULL;
    }

    // First pass: determine how much memory the rendered string will require
    size_t result_length = strlen(source);

    const char *input_cursor = source;
    const char *match;

    while ((match = strstr(input_cursor, token)) != NULL) {
        if (replacement_length >= token_length) {
            result_length += replacement_length - token_length;
        } else {
            result_length -= token_length - replacement_length;
        }

        input_cursor = match + token_length;
    }

    // Allocate the rendered string, including the null terminator
    char *result = malloc(result_length + 1);
    if (result == NULL) {
        return NULL;
    }

    // Second pass: copy the original string while replacing every occurrence of the token
    input_cursor = source;
    char *output_cursor = result;

    while ((match = strstr(input_cursor, token)) != NULL) {
        const size_t segment_length = (size_t)(match - input_cursor);

        memcpy(output_cursor, input_cursor, segment_length);
        output_cursor += segment_length;

        memcpy(output_cursor, replacement, replacement_length);
        output_cursor += replacement_length;

        input_cursor = match + token_length;
    }

    // Copy everything remaining after the final token
    strcpy(output_cursor, input_cursor);

    return result;
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
        identity->discovery_topic,
        sizeof(identity->discovery_topic),
        "homeassistant/light/%s/config",
        identity->device_id
    );

    snprintf(
        identity->command_topic,
        sizeof(identity->command_topic),
        "%s/led/set",
        identity->device_id
    );

    snprintf(
        identity->state_topic,
        sizeof(identity->state_topic),
        "%s/led/state",
        identity->device_id
    );

    snprintf(
        identity->availability_topic,
        sizeof(identity->availability_topic),
        "%s/status",
        identity->device_id
    );

    identity->discovery_payload = replace_token(
        DISCOVERY_TEMPLATE_START,
        "{{device_id}}",
        identity->device_id
    );

    if (identity->discovery_payload == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Device identifier: %s", identity->device_id);
    ESP_LOGD(TAG, "Discovery topic: %s", identity->discovery_topic);

    return ESP_OK;
}

void mqtt_identity_destroy(mqtt_identity_t *identity) {
    if (identity == NULL) {
        return;
    }

    free(identity->discovery_payload);
    identity->discovery_payload = NULL;
}