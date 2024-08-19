#include "env_sens_task.hpp"

void myEnvSensTask(void *pvParameters)
{
    EnvSensBME280Drv *bme = static_cast<EnvSensBME280Drv *>(pvParameters);

    bool meas = false;

    if (bme->init() == idle)
    {
        printf("SENSOR: Sensor initialized\n");

        if (bme->startContinuousMeasurements() == ok)
        {
            printf("SENSOR: Started Continuous Measurements\n");
            meas = true;
        }
    }

    while (1)
    {
        if (meas)
        {
            vTaskDelay(5000 / portTICK_PERIOD_MS);

            bme->process();
            ESP_LOGI("ENV_SENS_TASK", "sensor process successful");

            int32_t temp = {0};
            uint32_t pres = {0};
            uint32_t hum = {0};

            if (bme->readTemperature(temp) == ok)
            {
                printf("temperature: %.2lf\n", (double)temp / 100);
            }

            if (bme->readPressure(pres) == ok)
            {
                printf("pressure: %.3lf\n", (double)pres / 25600);
            }

            if (bme->readHumidity(hum) == ok)
            {
                printf("humidity: %.3lf\n", (double)hum / 1024);
            }
        }
    }
}