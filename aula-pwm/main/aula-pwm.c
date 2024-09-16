#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"

// ESP32 includes
#include "driver/ledc.h"

void app_main(void)
{
    uint16_t duty = 0;

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t timer_config = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .timer_num = LEDC_TIMER_0,
                                        .freq_hz = 5000,
                                        .duty_resolution = LEDC_TIMER_10_BIT,
                                        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_config);

    // Prepare and then apply the LEDC PWM channel0 configuration
    ledc_channel_config_t channel_config = {.channel = LEDC_CHANNEL_0,
                                            .speed_mode = LEDC_LOW_SPEED_MODE,
                                            .timer_sel = LEDC_TIMER_0,
                                            .intr_type = LEDC_INTR_DISABLE,
                                            .gpio_num = GPIO_NUM_2,
                                            .duty = 0};
    ledc_channel_config(&channel_config);

    // initialize duty cycle value
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    while (true)
    {
        // fade routine
        if (duty < 1024)
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            duty++;
        }
        else
        {
            duty = 0;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
