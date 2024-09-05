#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "HWLog";

void app_main(void)
{
    ESP_LOGI(TAG, "Inicio do Programa\n");

    printf("\n LOG Level para Default = INFO\n");
    ESP_LOGE(TAG, "Erro...");    // Será exibido
    ESP_LOGW(TAG, "Warning");    // Será exibido
    ESP_LOGI(TAG, "Informação"); // Será exibido
    ESP_LOGD(TAG, "Debug");      // Não será exibido
    ESP_LOGV(TAG, "Verbose");    // Não será exibido

    printf("\n LOG Level para Erro\n");
    esp_log_level_set(TAG, ESP_LOG_ERROR);

    ESP_LOGE(TAG, "Erro...");    // Será exibido
    ESP_LOGW(TAG, "Warning");    // Não será exibido
    ESP_LOGI(TAG, "Informação"); // Não será exibido
    ESP_LOGD(TAG, "Debug");      // Não será exibido
    ESP_LOGV(TAG, "Verbose");    // Não será exibido

    printf("\n LOG Level para Erro\n");
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    ESP_LOGE(TAG, "Erro...");    // Será exibido
    ESP_LOGW(TAG, "Warning");    // Será exibido
    ESP_LOGI(TAG, "Informação"); // Será exibido
    ESP_LOGD(TAG, "Debug");      // Será exibido
    ESP_LOGV(TAG, "Verbose");    // Será exibido

    vTaskDelay(500 / portTICK_PERIOD_MS);

    while (1)
    {
        printf("Hello world!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
