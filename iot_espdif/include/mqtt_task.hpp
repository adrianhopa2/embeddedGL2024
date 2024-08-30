#ifndef _MQTT_TASK_H
#define _MQTT_TASK_H

// #include <stdio.h>
// #include <stdint.h>
// #include <stddef.h>
// #include <string.h>
// #include "esp_system.h"
// #include "nvs_flash.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "protocol_examples_common.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "env_sens_bme280_drv.hpp"
#include "led_strip_base.hpp"

#include <string>
#include <cJSON.h>

#define EXAMPLE_ESP_WIFI_SSID "SA52_S"
#define EXAMPLE_ESP_WIFI_PASS "nokia123"
#define EXAMPLE_ESP_MAXIMUM_RETRY 7

#define CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK 0
#define CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT 0
#define CONFIG_ESP_WPA3_SAE_PWE_BOTH 1

#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENTs
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER ""
#endif

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1


struct params
{
    EnvSensBME280Drv *bme;
    LedStrip *led;
};


void myMQTTtask(void *pvParameters);

#endif