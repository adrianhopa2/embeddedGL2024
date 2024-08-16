#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <string>

#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "env_sens_bme280_drv.hpp"
// #include "env_sens_bmp280_drv.hpp"
#include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT_EXAMPLE";

esp_mqtt_client_handle_t client;
bool isMQTTconnected = false;

#define SCL_IO_PIN GPIO_NUM_22
#define SDA_IO_PIN GPIO_NUM_21
#define PORT_NUMBER -1

i2c_master_bus_handle_t bus_handle;
EnvSensBME280Drv *bmp;

void myBMP280Task(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        bmp->process();

        int32_t temp = {0};
        uint32_t pres = {0};
        uint32_t hum = {0};

        if (bmp->readTemperature(temp) == ok)
        {
            printf("\ntemperature: %.2lf\n", (double)temp / 100);
        }

        if (bmp->readPressure(pres) == ok)
        {
            printf("pressure: %.2lf\n", (double)pres / 25600);
        }

        if (bmp->readHumidity(hum) == ok)
        {
            printf("humidity: %.2lf\n", (double)hum / 1024);
        }
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_t *>(event_data);

    switch (static_cast<esp_mqtt_event_id_t>(event_id))
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        isMQTTconnected = true;
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        isMQTTconnected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void myMQTTapp()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = "mqtt://public.mqtthq.com",
            }}};

#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.broker.address.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.broker.address.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);

    /*Let's enqueue a few messages to the outbox to see the allocations*/
    // int msg_id;
    //  msg_id = esp_mqtt_client_enqueue(client, "/topic_esp/qos1", "data_3", 0, 1, 0, true);
    //  ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);
    //  msg_id = esp_mqtt_client_enqueue(client, "/topic_esp/qos2", "QoS2 message", 0, 2, 0, true);
    //  ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);

    /* Now we start the client and it's possible to see the memory usage for the operations in the outbox. */

    esp_mqtt_client_start(client);
}

void myMQTTtask(void *pvParameters)
{
    myMQTTapp();

    int msg_id = {0};

    while (1)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        if (isMQTTconnected)
        {
            bmp->process();

            int32_t temp = {0};
            uint32_t pres = {0};
            uint32_t hum = {0};

            std::string payload = {"{ \"temperature\": "};

            if (bmp->readTemperature(temp) == ok)
            {
                payload += std::to_string(static_cast<double>(temp) / 100);
            }

            payload += ", \"pressure\": ";

            if (bmp->readPressure(pres) == ok)
            {
                payload += std::to_string(static_cast<double>(pres) / 25600);
            }

            if (bmp->readHumidity(hum) == ok)
            {
                payload += ", \"humidity\": ";
                payload += std::to_string(static_cast<double>(hum) / 1024);
            }

            payload += " }";

            msg_id = esp_mqtt_client_publish(client, "/topic/iot_env_data", payload.c_str(), 0, 0, 0);
            ESP_LOGI("MQTT_TASK", "sent publish successful, msg_id=%d", msg_id);
        }
    }
}

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

        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

        bme280_config_t bme280_config = {
            .standby_time = 0b011,
            .filter_coefficient = 0b000,
            .pressure_oversampling = 0b001,
            .temperature_oversampling = 0b001,
            .humidity_oversampling = 0b001,
        };

        static EnvSensBME280Drv bme280(&bme280_config, bus_handle);

        bmp = &bme280;

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        ESP_ERROR_CHECK(example_connect());

        if (bme280.init() == idle)
        {
            ESP_LOGI("SENSOR", "Sensor initialized");
            if (bme280.startContinuousMeasurements() == ok)
            {
                ESP_LOGI("SENSOR", "Started Measurement");

                xTaskCreate(&myMQTTtask, "TaskMQTT", 4096, NULL, 5, NULL);

                //xTaskCreatePinnedToCore(&myBMP280Task, "readTask", 2048, NULL, 5, NULL, 1);
            }
        }
    }
}
