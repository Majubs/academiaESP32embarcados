#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_log.h"

const static char *TAG = "[ADC]";

void app_main(void)
{
    int adc_raw = 0;
    int voltage = 0;

    // ADC oneshot
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12, // Changing this changes the interval of values that can be read
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));

    while (true)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw));
        ESP_LOGI(TAG, "ADC%d Channel[%d] raw data: %d\n", ADC_UNIT_1 + 1, ADC_CHANNEL_0, adc_raw);
        // Convert to voltge, max value is 2500
        voltage = (adc_raw * 2500) / 8192;
        ESP_LOGI(TAG, "ADC%d Channel[%d] voltage: %d\n", ADC_UNIT_1 + 1, ADC_CHANNEL_0, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
