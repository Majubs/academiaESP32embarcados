#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_oneshot.h"
#include "esp_log.h"

const static char *TAG = "[DAC]";

void app_main(void)
{
    // DAC oneshot
    dac_oneshot_handle_t chann0_handle;
    dac_oneshot_config_t chann0_cfg = {
        .chan_id = DAC_CHAN_0,
    };

    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chann0_cfg, &chann0_handle));

    ESP_LOGI(TAG, "DAC oneshot start");

    while (true)
    {
        for (int wave = 0; wave < 255; wave++)
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chann0_handle, wave));
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        for (int wave = 255; wave < 0; wave--)
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chann0_handle, wave));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}
