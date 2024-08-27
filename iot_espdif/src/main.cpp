#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "env_sens_bme280_drv.hpp"
#include "led_ws2812b_drv.hpp"

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

        static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
        static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

        static struct led_strip_t led_strip = {
            .rgb_led_type = RGB_LED_TYPE_WS2812,
            .led_strip_length = LED_STRIP_LENGTH,
            .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
            .gpio = GPIO_NUM_13,
            .led_strip_buf_1 = led_strip_buf_1,
            .led_strip_buf_2 = led_strip_buf_2,
        };
        led_strip.access_semaphore = xSemaphoreCreateBinary();

        static LedWS2812BDrv LedStrip(&led_strip);

        bool led_init_ok = LedStrip.led_strip_init();

        if (led_init_ok)
        {
            LedStrip.led_strip_clear();
            LedStrip.led_strip_set_pixel_rgb(7, 0, 0, 10);
            LedStrip.led_strip_show();

            vTaskDelay(2000 / portTICK_PERIOD_MS);

            LedStrip.led_strip_clear();
            LedStrip.led_strip_show();
        }

        struct params
        {
            EnvSensBME280Drv *bme;
            LedWS2812BDrv *led;
        };

        static params p;
        p.bme = &bme280;
        p.led = &LedStrip;

        xTaskCreate(&myEnvSensTask, "EnvSensTask", 2048, &bme280, 5, NULL);
        xTaskCreate(&myMQTTtask, "TaskMQTT", 4096, &p, 5, NULL);
    }
}
