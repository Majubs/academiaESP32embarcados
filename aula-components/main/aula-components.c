#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// componente craido especificamente para essa aplicação
#include "calculadora.h"

void app_main(void)
{
    int res = soma(10, 20);

    printf("Resultado: %d\n", res);

    vTaskDelay(pdMS_TO_TICKS(500));

    res = subtrai(100, 20);

    printf("Resultado: %d\n", res);

    vTaskDelay(pdMS_TO_TICKS(500));

    res = multiplica(100, 200);

    printf("Resultado: %d\n", res);

    vTaskDelay(pdMS_TO_TICKS(500));

    res = divide(1000, 200);

    printf("Resultado: %d\n", res);

    vTaskDelay(pdMS_TO_TICKS(500));
}
