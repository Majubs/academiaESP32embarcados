#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "dht.h"
#include "sdkconfig.h"
#include "ssd1306.h"

static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t sensor_gpio = 15;

static const char *TAG = "DHT11";

void app_main(void)
{
    int16_t temperature = 0;
    int16_t humidity = 0;
    char line_char[20] = {0};

    SSD1306_t oled;

    ESP_LOGI(TAG, "Panel is 128x64");
    ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);

    i2c_master_init(&oled, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ssd1306_init(&oled, 128, 64);
    ssd1306_clear_screen(&oled, false);
    ssd1306_contrast(&oled, 0xFF);
    ssd1306_display_text_x3(&oled, 0, "DHT11", 5, false);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ssd1306_clear_screen(&oled, false);

    while (1)
    {
        if (dht_read_data(sensor_type, sensor_gpio, &humidity, &temperature) == ESP_OK)
        {
            ESP_LOGI(TAG, "Humidity:    %d%%", humidity / 10);
            ESP_LOGI(TAG, "Temperature: %dC", temperature / 10);

            sprintf(line_char, "Temp: %dC", temperature / 10);
            ssd1306_display_text(&oled, 0, line_char, sizeof(line_char), false);
            sprintf(line_char, "Hum: %d%%", humidity / 10);
            ssd1306_display_text(&oled, 1, line_char, sizeof(line_char), false);
        }
        else
        {
            ESP_LOGI(TAG, "Could not read data from sensor");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
