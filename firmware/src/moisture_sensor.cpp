#include "moisture_sensor.h"

#include <atomic>

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"

static const char *TAG = "moisture";

static constexpr adc_channel_t ADC_CHANNEL = ADC_CHANNEL_6;
static constexpr adc_unit_t ADC_UNIT = ADC_UNIT_1;
static adc_oneshot_unit_handle_t ADC_HANDLE;

static constexpr int SAMPLE_COUNT = 32;
static constexpr int SAMPLE_FREQ = 10;
static constexpr int READ_FREQ = 1000;

static std::atomic<int> LATEST_MOISTURE_PERCENTAGE{-1};

static constexpr float FILTER_ALPHA = 0.25f;
static float FILTERED_MOISTURE = -1.0f;

static int moisture_filter(int measurement, float alpha) {
    if (FILTERED_MOISTURE < 0.0f) {
        FILTERED_MOISTURE = measurement;
    } else {
        FILTERED_MOISTURE = alpha * measurement + (1.0f - alpha) * FILTERED_MOISTURE;
    }

    return static_cast<int>(FILTERED_MOISTURE + 0.5f);
}

static int moisture_percent(int raw, int dry_raw, int wet_raw) {
    if (dry_raw < 0 || dry_raw > 4095) {
        return -1;
    }

    if (wet_raw < 0 || wet_raw > 4095) {
        return -1;
    }

    if (dry_raw == wet_raw) {
        return -1;
    }
 
    const float percent = 100.0f * (raw - dry_raw) / (wet_raw - dry_raw);
    if (percent <= 0.0f) {
        return 0;
    }

    if (percent >= 100.0f) {
        return 100;
    }

    return static_cast<int>(percent + 0.5f);
}

static void moisture_task(void *) {
    // Let the sensor settle after power-up before collecting samples
    vTaskDelay(pdMS_TO_TICKS(1000));

    TickType_t prev_wake_time = xTaskGetTickCount();
    while (true) {
        int sum = 0;

        esp_err_t error = ESP_OK;
        for (int index = 0; index < SAMPLE_COUNT; ++index) {
            int sample;

            error = adc_oneshot_read(ADC_HANDLE, ADC_CHANNEL, &sample);
            if (error != ESP_OK) {
                break;
            }

            sum += sample;

            // A delay between each sub-sample defined by SUB_SAMPLE_FREQ in ms
            vTaskDelay(pdMS_TO_TICKS(SAMPLE_FREQ));
        }

        if (error != ESP_OK) {
            // Do not convert a failed or incomplete reading into a moisture value
            ESP_LOGW(TAG, "ADC read failed: %s", esp_err_to_name(error));
        } else {
            // Take the rounded mean value of all samples as the raw reading 
            const int raw = (sum + SAMPLE_COUNT / 2) / SAMPLE_COUNT;
            const int filtered = moisture_filter(raw, FILTER_ALPHA);

            const int percent = moisture_percent(
                filtered,
                CONFIG_PWS_MOISTURE_DRY_RAW,
                CONFIG_PWS_MOISTURE_WET_RAW
            );
            LATEST_MOISTURE_PERCENTAGE.store(percent);

            if (percent < 0) {
                ESP_LOGI(TAG, "GPIO34 raw=%d filtered=%d (uncalibrated)", raw, filtered);
            } else {
                ESP_LOGI(TAG, "GPIO34 raw=%d filtered=%d moisture=%d%%", raw, filtered, percent);
            }
        }

        // Only every READ_FREQ ms the sensor should take a mean-sampled ADC reading
        vTaskDelayUntil(&prev_wake_time, pdMS_TO_TICKS(READ_FREQ));
    }
}

esp_err_t moisture_sensor_start(void) {
    if (ADC_HANDLE != nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    if (CONFIG_PWS_MOISTURE_DRY_RAW == -1) {
        ESP_LOGW(TAG, "The dry (0%%) raw reading in menuconfig has not been set yet");
    }

    if (CONFIG_PWS_MOISTURE_WET_RAW == -1) {
        ESP_LOGW(TAG, "The wet (100%%) raw reading in menuconfig has not been set yet");
    }

    adc_oneshot_unit_init_cfg_t unit_config = {};
    unit_config.unit_id = ADC_UNIT;

    const esp_err_t handle_error = adc_oneshot_new_unit(&unit_config, &ADC_HANDLE);
    if (handle_error != ESP_OK) {
        return handle_error;
    }

    adc_oneshot_chan_cfg_t channel_config = {};
    channel_config.atten = ADC_ATTEN_DB_12;
    channel_config.bitwidth = ADC_BITWIDTH_12;

    const esp_err_t channel_error = adc_oneshot_config_channel(ADC_HANDLE, ADC_CHANNEL, &channel_config);
    if (channel_error != ESP_OK) {
        adc_oneshot_del_unit(ADC_HANDLE);
        ADC_HANDLE = nullptr;
        return channel_error;
    }

    if (xTaskCreate(moisture_task, "moisture", 3072, nullptr, 5, nullptr) != pdPASS) {
        adc_oneshot_del_unit(ADC_HANDLE);
        ADC_HANDLE = nullptr;
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

int moisture_sensor_percentage(void) {
    return LATEST_MOISTURE_PERCENTAGE.load();
}