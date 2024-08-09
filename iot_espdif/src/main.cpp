#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "env_sens_bme280_drv.hpp"

#define SCL_IO_PIN GPIO_NUM_22
#define SDA_IO_PIN GPIO_NUM_21
#define PORT_NUMBER -1

#ifdef __cplusplus
extern "C"
{
#endif
    void app_main(void)
    {
        i2c_master_bus_config_t i2c_bus_config = {
            .i2c_port = PORT_NUMBER,
            .sda_io_num = SDA_IO_PIN,
            .scl_io_num = SCL_IO_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
        };
        i2c_master_bus_handle_t bus_handle;

        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

        bme280_config_t bme280_config = {
            .standby_time = 0b011,
            .filter_coefficient = 0b000,
            .pressure_oversampling = 0b001,
            .temperature_oversampling = 0b001,
            .humidity_oversampling = 0b001,
        };

        EnvSensBME280Drv bme280(&bme280_config, bus_handle);

        if (bme280.init())
        {
            printf("\nSensor initialized\n");
            if (bme280.startContinuousMeasurements())
            {
                printf("\nStarted Measurement\n\n");
                while (1)
                {
                    bme280.process();
                    int32_t temp = {0};
                    uint32_t pres = {0};
                    uint16_t hum = {0};

                    if (bme280.readTemperature(temp) == ok)
                    {
                        printf("temperature: %.2lf\n", (double)temp / 100);
                    }

                    if (bme280.readPressure(pres) == ok)
                    {
                        printf("pressure: %.3lf\n", (double)pres / 25600);
                    }

                    if (bme280.readHumidity(hum) == ok)
                    {
                        printf("humidity: %.3lf\n\n", (double)hum / 1024);
                    }

                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                }
            }
        }

        // while (1)
        // {
        //     printf("HIGH\n");
        //     vTaskDelay(5000 / portTICK_PERIOD_MS);
        //     printf("LOW\n");
        //     vTaskDelay(5000 / portTICK_PERIOD_MS);
        // }
    }
#ifdef __cplusplus
}
#endif