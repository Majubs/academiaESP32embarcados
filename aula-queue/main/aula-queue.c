#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED 14

QueueHandle_t xQueue; // Queue handle

/*Tasks Handles*/
TaskHandle_t xTaskProducerHandle = NULL;
TaskHandle_t xTaskConsumerHandle = NULL;

/*Tasks prototipes*/
void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);

static const char *TAG = "main: ";

void app_main(void)
{
    bool status = false; // variable to store the LED status

    xQueue = xQueueCreate(5, sizeof(int)); // Create a queue with 5 elements of 4 bytes

    xTaskCreate(vTaskProducer, "Producer", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTaskProducerHandle); // create task
    xTaskCreate(vTaskConsumer, "Consumer", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTaskConsumerHandle); // create task

    gpio_set_direction(LED, GPIO_MODE_OUTPUT); // set the pin as output
    // loop
    while (1)
    {
        gpio_set_level(LED, status ^= 1); // LED status
        vTaskDelay(pdMS_TO_TICKS(1000));  // delay of 1 second
    }
}

void vTaskProducer(void *pvParameters)
{
    int count = 0; // counter variable
    while (1)
    {
        if (count < 10)
        {                                              // if counter < 10
            xQueueSend(xQueue, &count, portMAX_DELAY); // send data to queue
            count++;                                   // increment counter
        }
        else
        {
            count = 0;                       // reset counter
            vTaskDelay(pdMS_TO_TICKS(5000)); // delay of 5 seconds
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // delay of 500ms
    }
}

void vTaskConsumer(void *pvParameters)
{
    int received_count = 0;
    while (1)
    {
        if (xQueueReceive(xQueue, &received_count, pdMS_TO_TICKS(1000)) == pdTRUE) // receive data from queue with 1 second timeout
        {
            ESP_LOGI(TAG, "Received: %d", received_count); // log message
        }
        else
        {
            ESP_LOGI(TAG, "Timeout"); // log message
        }
    }
}
