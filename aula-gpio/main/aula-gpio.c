#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN_ONBOARD 2
#define BUTTON_PIN 21

static const char *TAG = "GPIO";

void app_main(void)
{
    bool button_state = 1;
    bool i = 0;

    gpio_reset_pin(LED_PIN_ONBOARD);
    gpio_set_direction(LED_PIN_ONBOARD, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BUTTON_PIN); // reset pin and set as GPIO
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);  // set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY); // set pull-up resistor

    while (true)
    {
        int new_state = gpio_get_level(
            BUTTON_PIN); // read button state and save in new_state variable

        // if button state is different from previous state
        if (new_state != button_state)
        {
            button_state = new_state; // update button state
            if (button_state == 0)    // if button is pressed
            {
                gpio_set_level(LED_PIN_ONBOARD, i ^= 1); // toggle LED state
                ESP_LOGI(TAG, "Button pressed"); // print message on console
            }
            else // if button is not pressed
            {
                ESP_LOGI(TAG, "Button released"); // print message on console
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // delay for 100 ms
    }
}
