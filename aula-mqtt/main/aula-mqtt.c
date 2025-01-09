#include <stdio.h>

#include "esp_err.h"
#include "esp_event_base.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi.h"
#include "mqtt_app.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(wifi_init_sta());
    ESP_LOGI(TAG, "WiFi connected");

    mqtt_app_start();
    ESP_LOGI(TAG, "MQTT connected");

    mqtt_app_subscribe("esp32c5/var", 0);

    mqtt_app_publish("esp32c5/temp", "69", 0, 0);
    ESP_LOGI(TAG, "Message published");

    mqtt_app_unsubscribe("esp32c5/var");
}
