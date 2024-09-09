#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN_ONBOARD 2
#define BUTTON_PIN1     21
#define BUTTON_PIN2     22

static const char *TAG = "GPIO";

void app_main(void)
{
    bool button_state1 = 1;
    bool button_state2 = 1;
    // bool i = 0;

    gpio_reset_pin(LED_PIN_ONBOARD);
    gpio_set_direction(LED_PIN_ONBOARD, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BUTTON_PIN1);                       // reset pin and set as GPIO
    gpio_set_direction(BUTTON_PIN1, GPIO_MODE_INPUT);  // set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN1, GPIO_PULLUP_ONLY); // set pull-up resistor

    gpio_reset_pin(BUTTON_PIN2);                       // reset pin and set as GPIO
    gpio_set_direction(BUTTON_PIN2, GPIO_MODE_INPUT);  // set BUTTON_PIN as input
    gpio_set_pull_mode(BUTTON_PIN2, GPIO_PULLUP_ONLY); // set pull-up resistor

    while (true)
    {
        int new_state1 = gpio_get_level(BUTTON_PIN1); // read button state and save in new_state variable
        int new_state2 = gpio_get_level(BUTTON_PIN2); // read button state and save in new_state variable

        // if button state is different from previous state
        if (new_state1 != button_state1)
        {
            button_state1 = new_state1; // update button state
            if (button_state1 == 0)     // if button is pressed
            {
                gpio_set_level(LED_PIN_ONBOARD, 1); // toggle LED state
                ESP_LOGI(TAG, "Button 1 pressed");  // print message on console
            }
            else // if button is not pressed
            {
                ESP_LOGI(TAG, "Button 1 released"); // print message on console
            }
        }

        if (new_state2 != button_state1)
        {
            button_state2 = new_state2; // update button state
            if (button_state2 == 0)     // if button is pressed
            {
                gpio_set_level(LED_PIN_ONBOARD, 0); // toggle LED state
                ESP_LOGI(TAG, "Button 0 pressed");  // print message on console
            }
            else // if button is not pressed
            {
                ESP_LOGI(TAG, "Button 0 released"); // print message on console
            }
            vTaskDelay(100 / portTICK_PERIOD_MS); // delay for 100 ms
        }
    }

    ESP_LOGI(TAG, "Done!");
}
