#include <stdio.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_oneshot.h"
#include "driver/dac_cosine.h"
#include "esp_log.h"
#include "hal/dac_types.h"
#include "soc/clk_tree_defs.h"

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

#ifdef CONFIG_WAVE_SIN
    dac_cosine_handle_t chan0_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_1,                 // channel 0
        .freq_hz = 1000,                       // frequency 1kHz
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT, // default clock source
        .offset = 0,                           // 0 V ("corta" onda, -255 até 255)
        .phase = DAC_COSINE_PHASE_0,           // 0 degrees
        .atten = DAC_COSINE_ATTEN_DEFAULT,     // default attenuation
        .flags.force_set_freq = false,         // do not force frequency
    };

    ESP_LOGI(TAG, "Initializing DAC cosine wave generator");

    ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &chan0_handle));
    ESP_ERROR_CHECK(dac_cosine_start(chan0_handle));

    ESP_LOGI(TAG, "DAC cosine wave generator initialized");
#endif

#if defined(CONFIG_WAVE_SERRATED) || defined(CONFIG_WAVE_TRIANGLE)
    while (true)
    {
        // DAC oneshot wave
        for (int wave = 0; wave < 255; wave++)
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chann0_handle, wave));
            vTaskDelay(pdMS_TO_TICKS(1));
        }

#ifdef CONFIG_WAVE_TRIANGLE
        for (int wave = 255; wave < 0; wave--)
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chann0_handle, wave));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
#endif
    }
#endif
}
