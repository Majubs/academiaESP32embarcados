#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/portable.h"

static const char *TAG = "RAM";

int x = 42;
char y[50];

void app_main(void)
{
    printf("x = %d | y = %c\n", x, y[0]);

    int heap = xPortGetFreeHeapSize();
    ESP_LOGI(TAG, "DRAM(xPortGetFreeHeapSize): %d bytes = %d KB", heap, heap / 1024);

    int DRAM = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "DRAM: %d bytes", DRAM);

    int IRAM = heap_caps_get_free_size(MALLOC_CAP_32BIT) - DRAM;
    ESP_LOGI(TAG, "IRAM: %d bytes", IRAM);
}
