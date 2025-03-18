#include "nvs_handler.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stddef.h>

// Global variables
const char *TAGNVS = "NVS Test";

nvs_handle_t nvs_hand;
uint8_t setpoint = DEFAULT_SETPOINT;

void nvs_handler_config()
{
    ESP_LOGI(TAGNVS, "Initializing NVS");

    esp_err_t err = nvs_flash_init_partition("nvs_ext");
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase_partition("nvs_ext"));
        err = nvs_flash_init_partition("nvs_ext");
    }
    // ESP_ERROR_CHECK(err);

    // NVS statistics
    nvs_stats_t statistics;
    nvs_get_stats("nvs_ext", &statistics);
    ESP_LOGI(TAGNVS, "Used: %d | Free: %d | Namespace count: %d", statistics.used_entries, statistics.free_entries, statistics.namespace_count);

    err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_hand);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGNVS, "Error (%s) opening NVS handle", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAGNVS, "Reading setpoint from NVS");
        err = nvs_get_u8(nvs_hand, "setpoint", &setpoint);

        switch (err)
        {
        case ESP_OK:
            ESP_LOGI(TAGNVS, "Done");
            ESP_LOGI(TAGNVS, "Setpoint = %d" PRIu8, setpoint);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAGNVS, "Value not found!");
            ESP_LOGI(TAGNVS, "Setting setpoint to default value: %d", DEFAULT_SETPOINT);

            setpoint = DEFAULT_SETPOINT;
            ESP_LOGI(TAGNVS, "Writing value to NVS");
            err = nvs_set_u8(nvs_hand, "setpoint", setpoint);
            ESP_LOGI(TAGNVS, "Write status: %s", (err != ESP_OK) ? "Failed!" : "Done");
            err = nvs_commit(nvs_hand);
            ESP_LOGI(TAGNVS, "Commit status: %s", (err != ESP_OK) ? "Failed!" : "Done");
        default:
            break;
        }
        nvs_close(nvs_hand);
    }
}

bool nvs_handler_read_pid(pid_controller_t *pid_ctl)
{
    esp_err_t err = nvs_open_from_partition("nvs_ext", "storage", NVS_READWRITE, &nvs_hand);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGNVS, "Error (%s) opening NVS handle", esp_err_to_name(err));
        return false;
    }
    else
    {
        ESP_LOGI(TAGNVS, "Reading PID from NVS");
        size_t required_size = sizeof(pid_controller_t);
        err = nvs_get_blob(nvs_hand, "pid", pid_ctl, &required_size);

        switch (err)
        {
        case ESP_OK:
            ESP_LOGI(TAGNVS, "Done");
            ESP_LOGI(TAGNVS, "PID: kp = %f | ki = %f | kd = %f", pid_ctl->kp, pid_ctl->ki, pid_ctl->kd);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAGNVS, "Value not found!");
            ESP_LOGI(TAGNVS, "Setting PID to default values");
            pid_ctl->kp = 1.0;
            pid_ctl->ki = 0.8;
            pid_ctl->kd = 0.2;
            ESP_LOGI(TAGNVS, "Writing values to NVS");
            err = nvs_set_blob(nvs_hand, "pid", pid_ctl, required_size);
            ESP_LOGI(TAGNVS, "Write status: %s", (err != ESP_OK) ? "Failed!" : "Done");
            err = nvs_commit(nvs_hand);
            ESP_LOGI(TAGNVS, "Commit status: %s", (err != ESP_OK) ? "Failed!" : "Done");
        default:
            break;
        }
        nvs_close(nvs_hand);
    }

    return true;
}
