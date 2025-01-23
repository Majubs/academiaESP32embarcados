#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_bit_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "soc/uart_pins.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_check.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "hal/gpio_types.h"

#define GPIO_WAKEUP_PIN               39
#define TIMER_WAKEP_US                (5000000)

#define EXAMPLE_UART_NUM              0
#define EXAMPLE_UART_TX_IO_NUM        U0TXD_GPIO_NUM
#define EXAMPLE_UART_RX_IO_NUM        U0RXD_GPIO_NUM

#define EXAMPLE_UART_WAKEUP_THRESHOLD 3 // the threshold of the number of edges that RX pin received to wakeup the chip up from light sleep

#define EXAMPLE_READ_BUF_SIZE         1024
#define EXAMPLE_UART_BUF_SIZE         (EXAMPLE_READ_BUF_SIZE * 2)

static QueueHandle_t uart_evt_que = NULL;

static const char *TAG = "LightSleep";

static esp_err_t uart_initialization(void);
static esp_err_t uart_wakeup_config(void);
esp_err_t example_register_uart_wakeup(void);

static void uart_wakeup_task(void *arg)
{
    uart_event_t event;
    if (uart_evt_que == NULL)
    {
        ESP_LOGE(TAG, "uart_evt_que is NULL");
        abort();
    }

    uint8_t *dtmp = (uint8_t *)malloc(EXAMPLE_READ_BUF_SIZE);

    while (1)
    {
        // Waiting for UART event.
        if (xQueueReceive(uart_evt_que, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            ESP_LOGI(TAG, "uart%d recved event:%d", EXAMPLE_UART_NUM, event.type);
            switch (event.type)
            {
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(EXAMPLE_UART_NUM, dtmp, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[DATA EVT]:");
                uart_write_bytes(EXAMPLE_UART_NUM, (const char *)dtmp, event.size);
                break;
            // Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EXAMPLE_UART_NUM);
                xQueueReset(uart_evt_que);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(EXAMPLE_UART_NUM);
                xQueueReset(uart_evt_que);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
                // ESP32 can wakeup by uart but there is no wake up interrupt
#if SOC_UART_SUPPORT_WAKEUP_INT
            // Event of waking up by UART
            case UART_WAKEUP:
                ESP_LOGI(TAG, "uart wakeup");
                break;
#endif
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_config_t config = {
        .pin_bit_mask = BIT64(GPIO_WAKEUP_PIN), .mode = GPIO_MODE_INPUT, .pull_down_en = false, .pull_up_en = false, .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&config);

    ESP_LOGI(TAG, "Enabling GPIO wakeup");
    gpio_wakeup_enable(GPIO_WAKEUP_PIN, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    ESP_LOGI(TAG, "Enabling timer wakeup after %dus", TIMER_WAKEP_US);
    esp_sleep_enable_timer_wakeup(TIMER_WAKEP_US);

    ESP_LOGI(TAG, "Enabling timer wakeup after %dus", TIMER_WAKEP_US);
    example_register_uart_wakeup();

    ESP_LOGI(TAG, "GPIO wakeup source is ready");
    vTaskDelay(pdMS_TO_TICKS(100));
    const char *wakeup_reason;
    int64_t t_before_us, t_after_us;

    while (1)
    {
        ESP_LOGI(TAG, "Entering light sleep");
        vTaskDelay(pdMS_TO_TICKS(100));
        uart_wait_tx_idle_polling(CONFIG_CONSOLE_UART_NUM);
        t_before_us = esp_timer_get_time();
        esp_light_sleep_start();
        t_after_us = esp_timer_get_time();

        switch (esp_sleep_get_wakeup_cause())
        {
        case ESP_SLEEP_WAKEUP_UART:
            wakeup_reason = "UART";
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            wakeup_reason = "Timer";
            break;
        case ESP_SLEEP_WAKEUP_GPIO:
            wakeup_reason = "GPIO";
            break;
        default:
            wakeup_reason = "Other";
            break;
        }
    }

    // log wakeup reason and sleep duration
    ESP_LOGI(TAG, "Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n", wakeup_reason, t_after_us / 1000,
             (t_after_us - t_before_us) / 1000);

    switch (esp_sleep_get_wakeup_cause())
    {
    case ESP_SLEEP_WAKEUP_TIMER:
        ESP_LOGI(TAG, "Waiting a little");
        vTaskDelay(pdMS_TO_TICKS(5000));
        break;
    case ESP_SLEEP_WAKEUP_GPIO:
        ESP_LOGI(TAG, "Waiting for GPIO%d to go high", GPIO_WAKEUP_PIN);
        while (gpio_get_level(GPIO_WAKEUP_PIN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        break;
    default:
        ESP_LOGI(TAG, "NOP");
        break;
    }
}

static esp_err_t uart_initialization(void)
{
    uart_config_t uart_cfg = {
        .baud_rate = 115200, // CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    ESP_RETURN_ON_ERROR(uart_driver_install(EXAMPLE_UART_NUM, EXAMPLE_UART_BUF_SIZE, EXAMPLE_UART_BUF_SIZE, 20, &uart_evt_que, 0), TAG,
                        "Install uart failed");
    if (EXAMPLE_UART_NUM == CONFIG_ESP_CONSOLE_UART_NUM)
    {
        /* temp fix for uart garbled output, can be removed when IDF-5683 done */
        ESP_RETURN_ON_ERROR(uart_wait_tx_idle_polling(EXAMPLE_UART_NUM), TAG, "Wait uart tx done failed");
    }
    ESP_RETURN_ON_ERROR(uart_param_config(EXAMPLE_UART_NUM, &uart_cfg), TAG, "Configure uart param failed");
    ESP_RETURN_ON_ERROR(uart_set_pin(EXAMPLE_UART_NUM, EXAMPLE_UART_TX_IO_NUM, EXAMPLE_UART_RX_IO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE), TAG,
                        "Configure uart gpio pins failed");
    return ESP_OK;
}

static esp_err_t uart_wakeup_config(void)
{
    /* UART will wakeup the chip up from light sleep if the edges that RX pin received has reached the threshold
     * Besides, the Rx pin need extra configuration to enable it can work during light sleep */
    ESP_RETURN_ON_ERROR(gpio_sleep_set_direction(EXAMPLE_UART_RX_IO_NUM, GPIO_MODE_INPUT), TAG, "Set uart sleep gpio failed");
    ESP_RETURN_ON_ERROR(gpio_sleep_set_pull_mode(EXAMPLE_UART_RX_IO_NUM, GPIO_PULLUP_ONLY), TAG, "Set uart sleep gpio failed");
    ESP_RETURN_ON_ERROR(uart_set_wakeup_threshold(EXAMPLE_UART_NUM, EXAMPLE_UART_WAKEUP_THRESHOLD), TAG, "Set uart wakeup threshold failed");
    /* Only uart0 and uart1 (if has) support to be configured as wakeup source */
    ESP_RETURN_ON_ERROR(esp_sleep_enable_uart_wakeup(EXAMPLE_UART_NUM), TAG, "Configure uart as wakeup source failed");
    return ESP_OK;
}

esp_err_t example_register_uart_wakeup(void)
{
    /* Initialize uart1 */
    ESP_RETURN_ON_ERROR(uart_initialization(), TAG, "Initialize uart%d failed", EXAMPLE_UART_NUM);
    /* Enable wakeup from uart */
    ESP_RETURN_ON_ERROR(uart_wakeup_config(), TAG, "Configure uart as wakeup source failed");

    xTaskCreate(uart_wakeup_task, "uart_wakeup_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "uart wakeup source is ready");
    return ESP_OK;
}
