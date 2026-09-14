#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "mqtt_client.h"
#include "mqtt_identity.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define WIFI_CONNECTED_BIT BIT0
#define LED_GPIO GPIO_NUM_2

static mqtt_identity_t identity;

static const char *WIFI_LOG_TAG = "wifi";
static const char *MQTT_LOG_TAG = "mqtt";
static const char *PWS_LOG_TAG = "pws";

static EventGroupHandle_t wifi_event_group;

static void log_error_if_nonzero(const char *tag, const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(tag, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(MQTT_LOG_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);

    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_LOG_TAG, "MQTT_EVENT_CONNECTED");

            esp_mqtt_client_subscribe(client, identity.command_topic, 1);
            esp_mqtt_client_publish(client, identity.discovery_topic, identity.discovery_payload, 0, 1, 1);
            esp_mqtt_client_publish(client, identity.availability_topic, "online", 0, 1, 1);
            esp_mqtt_client_publish(client, identity.state_topic, gpio_get_level(LED_GPIO) ? "ON" : "OFF", 0, 1, 1);
            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_LOG_TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_LOG_TAG, "MQTT_EVENT_DATA");
            
            if (event->topic_len == strlen(identity.command_topic) && strncmp(event->topic, identity.command_topic, event->topic_len) ==  0) {
                if (event->data_len == 2 && strncmp(event->data, "ON", 2) == 0) {
                    gpio_set_level(LED_GPIO, 1);

                    esp_mqtt_client_publish(client, identity.state_topic, "ON", 0, 1, 1);
                }
                else if (event->data_len == 3 && strncmp(event->data, "OFF", 3) == 0) {
                    gpio_set_level(LED_GPIO, 0);

                    esp_mqtt_client_publish(client, identity.state_topic, "OFF", 0, 1, 1);
                }
            }
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_LOG_TAG, "MQTT_EVENT_ERROR");

            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero(MQTT_LOG_TAG, "reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero(MQTT_LOG_TAG, "reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero(MQTT_LOG_TAG, "captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(MQTT_LOG_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
            
        default:
            ESP_LOGI(MQTT_LOG_TAG, "Other event id: %d", event->event_id);
            break;
    }
}

void mqtt_start(void) {
    ESP_ERROR_CHECK(mqtt_identity_init(&identity));
    
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_PWS_MQTT_BROKER_URI,
        .credentials = {
            .client_id = identity.device_id,
            .username = CONFIG_PWS_MQTT_USERNAME,
            .authentication = {
                .password = CONFIG_PWS_MQTT_PASSWORD,
            },
        },
        .session.last_will = {
            .topic = identity.availability_topic,
            .msg = "offline",
            .qos = 1,
            .retain = 1,
        },
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(WIFI_LOG_TAG, "Connecting to Wi-Fi...");

        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(WIFI_LOG_TAG, "Wi-Fi disconnected, reconnecting...");

        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_LOG_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_start(void) {
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = CONFIG_PWS_WIFI_SSID,
            .password = CONFIG_PWS_WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

    ESP_ERROR_CHECK(esp_wifi_start());

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(WIFI_LOG_TAG, "Network ready");
}

static void flash_start(void) {
    esp_err_t flash_error = nvs_flash_init();
    if (flash_error == ESP_ERR_NVS_NO_FREE_PAGES || flash_error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        flash_error = nvs_flash_init();
    }

    ESP_ERROR_CHECK(flash_error);
}

void app_main(void) {
    esp_log_level_set("*", ESP_LOG_INFO);

    ESP_LOGI(PWS_LOG_TAG, "Startup..");
    ESP_LOGI(PWS_LOG_TAG, "Free heap: %u KiB", (esp_get_free_heap_size() / 1024));
    ESP_LOGI(PWS_LOG_TAG, "IDF version: %s", esp_get_idf_version());

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    flash_start();
    wifi_start();
    mqtt_start();
}