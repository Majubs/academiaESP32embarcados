#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "esp_log.h"
#include "freertos/portable.h"

#include "nvs_handler.h"

static const char *TAG = "RAM";

// Get the string name of type enum values used in this example
static const char *get_type_str(esp_partition_type_t type)
{
    switch (type)
    {
    case ESP_PARTITION_TYPE_APP:
        return "ESP_PARTITION_TYPE_APP";
    case ESP_PARTITION_TYPE_DATA:
        return "ESP_PARTITION_TYPE_DATA";
    default:
        return "UNKNOWN_PARTITION_TYPE"; // type not used in this example
    }
}

// Get the string name of subtype enum values used in this example
static const char *get_subtype_str(esp_partition_subtype_t subtype)
{
    switch (subtype)
    {
    case ESP_PARTITION_SUBTYPE_DATA_NVS:
        return "ESP_PARTITION_SUBTYPE_DATA_NVS";
    case ESP_PARTITION_SUBTYPE_DATA_PHY:
        return "ESP_PARTITION_SUBTYPE_DATA_PHY";
    case ESP_PARTITION_SUBTYPE_APP_FACTORY:
        return "ESP_PARTITION_SUBTYPE_APP_FACTORY";
    case ESP_PARTITION_SUBTYPE_DATA_FAT:
        return "ESP_PARTITION_SUBTYPE_DATA_FAT";
    default:
        return "UNKNOWN_PARTITION_SUBTYPE"; // subtype not used in this example
    }
}

// Find the partition using given parameters
static void find_partition(esp_partition_type_t type, esp_partition_subtype_t subtype, const char *name)
{
    ESP_LOGI(TAG, "Find partition with type %s, subtype %s, label %s...", get_type_str(type), get_subtype_str(subtype),
             name == NULL ? "NULL (unspecified)" : name);

    const esp_partition_t *part = esp_partition_find_first(type, subtype, name);

    if (part != NULL)
    {
        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%" PRIx32 " with size 0x%" PRIx32, part->label, part->address, part->size);
    }
    else
    {
        ESP_LOGE(TAG, "\tpartition not found!");
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "RAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM");

    int heap = xPortGetFreeHeapSize();
    ESP_LOGI(TAG, "DRAM(xPortGetFreeHeapSize): %d bytes = %d KB", heap, heap / 1024);

    int DRAM = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "DRAM: %d bytes", DRAM);

    int IRAM = heap_caps_get_free_size(MALLOC_CAP_32BIT) - DRAM;
    ESP_LOGI(TAG, "IRAM: %d bytes", IRAM);

    ESP_LOGI(TAG, "FLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASH");

    extern const char csv_start[] asm("_binary_partitions_csv_start");
    extern const char csv_end[] asm("_binary_partitions_csv_end");

    ESP_LOGI(TAG, "Printing partition table csv file contents for reference...\n\n%.*s", csv_end - csv_start + 1, csv_start);

    /* First Part - Finding partitions using esp_partition_find_first. */

    ESP_LOGI(TAG, "----------------Find partitions---------------");

    // Find partitions using esp_partition_find_first(). This returns the first partition matching the passed constraints.
    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);
    find_partition(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);

    ESP_LOGI(TAG, "Find second FAT partition by specifying the label");
    // In case of multiple matches, `esp_partition_find_first` returns the first match.
    find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage2");

    /* Second Part - Iterating over partitions */

    ESP_LOGI(TAG, "----------------Iterate through partitions---------------");

    esp_partition_iterator_t it;

    ESP_LOGI(TAG, "Iterating through app partitions...");
    it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);

    // Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
    // label is found. Verify if its the same instance as the one found before.
    for (; it != NULL; it = esp_partition_next(it))
    {
        const esp_partition_t *part = esp_partition_get(it);
        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%" PRIx32 " with size 0x%" PRIx32, part->label, part->address, part->size);
    }
    // Release the partition iterator to release memory allocated for it
    esp_partition_iterator_release(it);

    ESP_LOGI(TAG, "Iterating through data partitions...");
    it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

    // Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
    // label is found. Verify if its the same instance as the one found before.
    for (; it != NULL; it = esp_partition_next(it))
    {
        const esp_partition_t *part = esp_partition_get(it);
        ESP_LOGI(TAG, "\tfound partition '%s' at offset 0x%" PRIx32 " with size 0x%" PRIx32, part->label, part->address, part->size);
    }

    // Release the partition iterator to release memory allocated for it
    esp_partition_iterator_release(it);

    ESP_LOGI(TAG, "HTMLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");

    extern const uint8_t page_html_start[] asm("_binary_page_html_start");
    extern const uint8_t page_html_end[] asm("_binary_page_html_end");

    ESP_LOGI(TAG, "Start address: %p | End address: %p", page_html_start, page_html_end);
    ESP_LOGI(TAG, "Size: %d", page_html_end - page_html_start);
    ESP_LOGI(TAG, "CONTENT:\n\n%s\n", page_html_start);

    ESP_LOGI(TAG, "NVSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
    nvs_handler_config();
    pid_controller_t pid;
    nvs_handler_read_pid(&pid);

    ESP_LOGI(TAG, "Example end");
}
