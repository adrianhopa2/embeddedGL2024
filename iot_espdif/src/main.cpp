#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "env_sens_bme280_drv.hpp"

#include "env_sens_task.hpp"
#include "mqtt_task.hpp"

#define SCL_IO_PIN GPIO_NUM_22
#define SDA_IO_PIN GPIO_NUM_21
#define PORT_NUMBER -1

extern "C"
{
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

        static EnvSensBME280Drv bme280(&bme280_config, bus_handle);

        xTaskCreate(&myEnvSensTask, "EnvSensTask", 2048, &bme280, 5, NULL);
        xTaskCreate(&myMQTTtask, "TaskMQTT", 4096, &bme280, 5, NULL);
    }
}
