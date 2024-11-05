#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN 2
// #define LED1_PIN 21          //GPIO21
// #define LED2_PIN 33          //GPIO33
// #define LED3_PIN 14          //GPIO14
// #define LED4_PIN 13          //GPIO15
// #define BUTTON1_PIN 2        //GPIO2
// #define BUTTON2_PIN 3        //GPIO3

QueueHandle_t xGPIOQueue; // Queue handle

/*Tasks Handles*/
TaskHandle_t xTaskLedHandle = NULL;
TaskHandle_t xTaskConsumerHandle = NULL;

/*Tasks prototipes*/
void vTaskLed(void *pvParameters);
void vTaskConsumer(void *pvParameters);

static const char *TAG = "QISR";

// Interrupt handler
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg; // get the GPIO number
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    xQueueSendFromISR(xGPIOQueue, &gpio_num, NULL); // send the GPIO number that generated the interrupt to the queue

    if (xHigherPriorityTaskWoken == pdTRUE) // check if the interrupt raised the task priority
    {
        portYIELD_FROM_ISR(); // force context switch
    }
}

void app_main(void)
{
    bool status = false; // variable to store the LED status

    gpio_config_t io_conf = {}; // GPIO config structure

    // Configure LEDs
    // io_conf.pin_bit_mask =
    //     (1ULL << LED1_PIN) | (1ULL << LED2_PIN) | (1ULL << LED3_PIN) | (1ULL << LED4_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_OUTPUT;       // Set the GPIO mode
    io_conf.pull_up_en = 0;                // Disable pull-up mode
    io_conf.pull_down_en = 0;              // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&io_conf);                 // Configure GPIO with the given settings

    // Configure buttons
    // LAB: add more buttons
    // io_conf.pin_bit_mask = (1ULL << BUTTON1_PIN) | (1ULL << BUTTON2_PIN); // Bitmask to select the GPIOs you want to configure
    io_conf.mode = GPIO_MODE_INPUT;        // Set the GPIO mode
    io_conf.pull_up_en = 1;                // Enable pull-up mode
    io_conf.pull_down_en = 0;              // Disable pull-down mode
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Interrupt on negative edge
    gpio_config(&io_conf);                 // Configure GPIO with the given settings

    // install gpio isr service and add isr handler for button1 and button2
    // LAB: add ISR handler for all buttons
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                           // install gpio isr service
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void *)BUTTON1_PIN); // add isr handler for button1
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void *)BUTTON2_PIN); // add isr handler for button2

    xGPIOQueue = xQueueCreate(5, sizeof(uint32_t)); // Create a queue with 5 elements of 4 bytes

    xTaskCreate(vTaskLed, "LED", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTaskLedHandle);                // create task
    xTaskCreate(vTaskConsumer, "Consumer", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTaskConsumerHandle); // create task

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT); // set the pin as output
    // loop
    while (1)
    {
        gpio_set_level(LED_PIN, status ^= 1); // toggle LED status
        vTaskDelay(pdMS_TO_TICKS(1000));      // delay of 1 second
    }
}

void vTaskLed(void *pvParameters)
{
    uint8_t led = 0; // led state variable

    while (true)
    {
        led ^= 1; // toggle led3
        // gpio_set_level(LED4_PIN, led); //set led3 state
        vTaskDelay(pdMS_TO_TICKS(100)); // delay 100ms
    }
}

void vTaskConsumer(void *pvParameters)
{
    uint32_t gpio_num;                // gpio number variable
    uint8_t led1 = 0, led2 = 0;       // led state variables
    TickType_t last_button_press = 0; // last button press time variable

    while (true)
    {
        xQueueReceive(xGPIOQueue, &gpio_num, portMAX_DELAY); // wait for gpio event from ISR
        ESP_LOGI(TAG, "GPIO[%u] intr \n", gpio_num);         // LOG the GPIO number

        TickType_t current_time = xTaskGetTickCount(); // get current time

        if (current_time - last_button_press >= pdMS_TO_TICKS(250)) // check if 250ms has passed since last button press
        {
            last_button_press = current_time; // update last button press time

            // LAB: add more cases to switch, one for each button
            switch (gpio_num)
            {
            case BUTTON1_PIN:
                gpio_set_level(LED1_PIN, led1 ^= 1); // toggle led1
            case BUTTON2_PIN:
                gpio_set_level(LED2_PIN, led2 ^= 1); // toggle led1
                break;
            default:
                ESP_LOGW(TAG, "Unkown button!\n");
            }
        }
    }
}
