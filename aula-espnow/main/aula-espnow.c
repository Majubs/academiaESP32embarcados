#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_mac.h"
#include "esp_wifi.h"
#include "hal/gpio_types.h"
#include "nvs_flash.h"
#include "esp_now.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_PIN 11
#define LED_PIN    4

// uint8_t receiver_mac[6] = {0x40, 0x4C, 0xCA, 0xFF, 0xFE, 0x56};
uint8_t receiver_mac[6] = {0x7C, 0x87, 0xCE, 0x2F, 0xDA, 0x84};

const char *TAG = "ESP NOW";

void get_mac(void)
{
    uint8_t mac_address[6];
    // get mac address
    esp_efuse_mac_get_default(mac_address);

    // print mac address
    ESP_LOGI(TAG, "MAC Address: %02x:%02x:%02x:%02x:%02x:%02x", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4],
             mac_address[5]);
}

void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char msg_buffer[50];

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        sprintf(msg_buffer, "Message sent to " MACSTR, MAC2STR(mac_addr));
        ESP_LOGI(TAG, "%s", msg_buffer);
    }
    else
    {
        sprintf(msg_buffer, "Error sending message to " MACSTR, MAC2STR(mac_addr));
        ESP_LOGE(TAG, "%s", msg_buffer);
    }
}

void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
    ESP_LOGI(TAG, "Got message from " MACSTR, MAC2STR(esp_now_info->src_addr));
    printf("Message: %.*s", data_len, data);

    if (strncmp((char *)data, "LED ON", data_len) == 0)
    {
        gpio_set_level(LED_PIN, 1);
    }
    else if (strncmp((char *)data, "LED OFF", data_len) == 0)
    {
        gpio_set_level(LED_PIN, 0);
    }
}

void app_main(void)
{
    // init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    get_mac();

    // init WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // init ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    // add peer
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer.peer_addr, receiver_mac, 6);
    esp_now_add_peer(&peer);

    // GPIOs
    // gpio_reset_pin(BUTTON_PIN);
    // gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    char send_msg_buffer[100];
    uint8_t button_state = 1;
    uint8_t led_status = 0;

    while (1)
    {
        // uint8_t new_state = gpio_get_level(BUTTON_PIN);
        led_status = !led_status;
        sprintf(send_msg_buffer, "LED %s", led_status ? "ON" : "OFF");
        ESP_LOGI(TAG, "Sending ESP-NOW message");
        esp_now_send(receiver_mac, (uint8_t *)send_msg_buffer, strlen(send_msg_buffer));

        // if (new_state != button_state)
        // {
        //     button_state = new_state;
        //     if (button_state == 0)
        //     {
        //         ESP_LOGI(TAG, "Button pressed");
        //     }
        //     else
        //     {
        //         ESP_LOGI(TAG, "Button released");
        //     }
        // }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(esp_now_deinit());
    ESP_ERROR_CHECK(esp_wifi_stop());
}
