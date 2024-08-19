#ifndef _ENV_SENS_TASK_H
#define _ENV_SENS_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "env_sens_bme280_drv.hpp"

#include <esp_log.h>

void myEnvSensTask(void *pvParameters);

#endif