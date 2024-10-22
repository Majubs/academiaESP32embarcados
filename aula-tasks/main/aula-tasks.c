#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define LED_PIN 2

static const char *TAG_COUNTER = "[COUNTER]";
static const char *TAG_MONITOR = "[MONITOR]";
static const char *TAG_MAIN = "[MAIN]";

// tasks functions prototypes
void vTaskCounter(void *pvParameters);
void vTaskMonitor(void *pvParameters);

// tasks hadlers
TaskHandle_t xTaskCounterHandle = NULL;
TaskHandle_t xTaskMonitorHandle = NULL;

SemaphoreHandle_t xSemaphore = NULL;

void app_main(void)
{
    int programCounter = 0;
    ESP_LOGI(TAG_MAIN, "Program start");

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    xSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(vTaskCounter, "Counter", 2048, (void *)&programCounter, tskIDLE_PRIORITY, &xTaskCounterHandle);
    xTaskCreate(vTaskMonitor, "Monitor", 2048, (void *)&programCounter, tskIDLE_PRIORITY, &xTaskMonitorHandle);

    int i = 0;
    while (1)
    {
        ESP_LOGI(TAG_MAIN, "Main ticks: %d", i++); // log message
        vTaskDelay(pdMS_TO_TICKS(1000));           // delay of 1 second
        // logic to handle button to delete Monitor ask here
    }
}

void vTaskCounter(void *pvParameters)
{
    int *counter = (int *)pvParameters;
    *counter = 0;

    ESP_LOGI(TAG_COUNTER, "Initializing Counter task");

    while (1)
    {
        ESP_LOGI(TAG_COUNTER, "Counter: %d", *counter); // log message

        (*counter)++;
        xSemaphoreGive(xSemaphore);

        vTaskDelay(pdMS_TO_TICKS(100)); // delay of 1 second
    }

    ESP_LOGI(TAG_COUNTER, "Ending Counter task");
}

void vTaskMonitor(void *pvParameters)
{
    int counterValue = 0;

    ESP_LOGI(TAG_MONITOR, "Initializing Monitor task");

    while (1)
    {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY))
        {
            counterValue = (*(int *)(pvParameters));
            ESP_LOGI(TAG_MONITOR, "Counter value: %d", counterValue); // log message
        }
        else
        {
            counterValue = 0;
            ESP_LOGD(TAG_MONITOR, "Semaphore timeout");
        }

        if (counterValue > 100)
        {
            gpio_set_level(LED_PIN, 1);
            ESP_LOGI(TAG_MONITOR, "Reseting Counter task!");
            vTaskDelete(xTaskCounterHandle);
            vTaskDelay(pdMS_TO_TICKS(1000)); // delay of 1 second
            xTaskCreate(vTaskCounter, "Counter", 2048, pvParameters, 1, &xTaskCounterHandle);
            gpio_set_level(LED_PIN, 0);
        }
    }

    ESP_LOGI(TAG_MONITOR, "Ending Monitor task");
}
