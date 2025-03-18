#ifndef __NVS_HANDLER_H__
#define __NVS_HANDLER_H__

#include <stdint.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define DEFAULT_SETPOINT 25U

typedef struct
{
    float kp;
    float ki;
    float kd;
} pid_controller_t;

void nvs_handler_config();

bool nvs_handler_read_pid(pid_controller_t *pid_ctl);

#endif // __NVS_HANDLER_H__
