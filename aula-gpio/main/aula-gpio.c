#include <stdint.h>
#include <stdio.h>

#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN_ONBOARD 2
#define BUTTON_PIN1     21
#define BUTTON_PIN2     22

static const char *TAG = "GPIO";

// queue handler
static QueueHandle_t gpio_evt_queue = NULL;

void poll_two_buttons(bool button_state1, bool button_state2);

// ISR handler
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;                  // get the GPIO number
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL); // send the GPIO number to the queue
}

// button task
void buttonTask(void *pvparameters)
{
    uint32_t gpio_num;                // gpio number variable
    uint8_t led1 = 0, led2 = 0;       // led state variables
    TickType_t last_button_press = 0; // last button press time variable

    while (true)
    {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY); // wait for gpio event from ISR
        ESP_LOGI(TAG, "GPIO[%li] intr \n", gpio_num);            // LOG the GPIO number

        TickType_t current_time = xTaskGetTickCount(); // get current time

        if (current_time - last_button_press >= pdMS_TO_TICKS(250)) // check if 250ms has passed since last button press
        {
            last_button_press = current_time; // update last button press time

            if (gpio_num == BUTTON_PIN1) // check if button1 was pressed
            {
                gpio_set_level(LED_PIN_ONBOARD, led1 ^= 1); // toggle led1
            }
            else if (gpio_num == BUTTON_PIN2) // check if button2 was pressed
            {
                gpio_set_level(LED_PIN_ONBOARD, led2 ^= 1); // toggle led2
            }
        }
    }
}

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

    gpio_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 2, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BUTTON_PIN1, gpio_isr_handler, (void *)BUTTON_PIN1);
    gpio_isr_handler_add(BUTTON_PIN2, gpio_isr_handler, (void *)BUTTON_PIN2);

    while (true)
    {
        // poll_two_buttons(button_state1, button_state2);

        // toggle a LED
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Done!");
}

void poll_two_buttons(bool button_state1, bool button_state2)
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
