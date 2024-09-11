#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "env_sens_bme280_drv.hpp"
#include "led_strip_ws2812b_drv.hpp"
#include "led_strip_base.hpp"

#include "env_sens_task.hpp"
#include "mqtt_task.hpp"
#include "led_strip_task.hpp"

#define SCL_IO_PIN GPIO_NUM_22
#define SDA_IO_PIN GPIO_NUM_21
#define PORT_NUMBER -1

extern "C"
{
    int app_main(void)
    {
        i2c_master_bus_config_t i2c_bus_config = {
            .i2c_port = PORT_NUMBER,
            .sda_io_num = SDA_IO_PIN,
            .scl_io_num = SCL_IO_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
        };
        i2c_master_bus_handle_t bus_handle;

        I2cMasterWrapper i2cmasterwrapper;

        ESP_ERROR_CHECK(i2cmasterwrapper.new_bus(&i2c_bus_config, &bus_handle));

        bme280_config_t bme280_config = {
            .standby_time = 0b011,
            .filter_coefficient = 0b000,
            .pressure_oversampling = 0b001,
            .temperature_oversampling = 0b001,
            .humidity_oversampling = 0b001,
        };

        static EnvSensBME280Drv bme280(&bme280_config, bus_handle, i2cmasterwrapper);

        static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
        static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

        static struct led_strip_t led_strip = {
            .led_strip_length = LED_STRIP_LENGTH,
            .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
            .gpio = GPIO_NUM_13,
            .led_strip_buf_1 = led_strip_buf_1,
            .led_strip_buf_2 = led_strip_buf_2,
        };
        led_strip.access_semaphore = xSemaphoreCreateBinary();

        RmtWrapper rmtwrapper;

        static LedStripWS2812BDrv ws2812b1;
        static LedStrip ledStrip1(ws2812b1, &led_strip, rmtwrapper);

        bool led_init_ok = ledStrip1.init();

        if (led_init_ok)
        {
            printf("LED STRIP INITIALIZED\n");
            xTaskCreate(&led_strip_task, "LedStripTask", LED_STRIP_TASK_SIZE, &ledStrip1, LED_STRIP_TASK_PRIORITY, NULL);
            printf("LED STRIP TASK STARTED\n");
            ledStrip1.clear();
            ledStrip1.show();
        }

        static params p;
        p.bme = &bme280;
        p.led = &ledStrip1;

        xTaskCreate(&myEnvSensTask, "EnvSensTask", 2048, &bme280, 5, NULL);
        printf("ENV SENS TASK STARTED\n");
        xTaskCreate(&myMQTTtask, "TaskMQTT", 4096, &p, 5, NULL);
        printf("MQTT TASK STARTED\n");

        return 0;
    }
}
