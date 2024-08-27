#ifndef _LED_WS2812B_DRV_H
#define _LED_WS2812B_DRV_H

#include "led_drv_interface.hpp"

#include <driver/gpio.h>
#include <driver/rmt.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>

enum rgb_led_type_t {
    RGB_LED_TYPE_WS2812 = 0,
    RGB_LED_TYPE_SK6812 = 1,
    RGB_LED_TYPE_APA106 = 2,

    RGB_LED_TYPE_MAX,
};

struct led_color_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct led_strip_t {
    enum rgb_led_type_t rgb_led_type;
    uint32_t led_strip_length;

    // RMT peripheral settings
    rmt_channel_t rmt_channel;

    /*
     * Interrupt table is located in soc.h
     * As of 11/27/16, reccomended interrupts are:
     * 9, 12, 13, 17, 18, 19, 20, 21 or 23
     * Ensure that the same interrupt number isn't used twice
     * across all libraries
     */
    int rmt_interrupt_num;

    gpio_num_t gpio; // Must be less than GPIO_NUM_33

    // Double buffering elements
    bool showing_buf_1;
    struct led_color_t *led_strip_buf_1;
    struct led_color_t *led_strip_buf_2; 

    SemaphoreHandle_t access_semaphore;
};

#define LED_STRIP_TASK_SIZE (2048)
#define LED_STRIP_TASK_PRIORITY (configMAX_PRIORITIES - 1)

#define LED_STRIP_REFRESH_PERIOD_MS (30U) // TODO: add as parameter to led_strip_init

#define LED_STRIP_NUM_RMT_ITEMS_PER_LED (24U) // Assumes 24 bit color for each led

// RMT Clock source is @ 80 MHz. Dividing it by 8 gives us 10 MHz frequency, or 100ns period.
#define LED_STRIP_RMT_CLK_DIV (8)

/****************************
        WS2812 Timing
 ****************************/
#define LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812 9 // 900ns (900ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812 3  // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812 3 // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812 9  // 900ns (900ns +/- 150ns per datasheet)

#define LED_STRIP_LENGTH 8U
#define LED_STRIP_RMT_INTR_NUM 19U

class LedWS2812BDrv : public ILedDrv
{
private:
    struct led_strip_t m_led_strip;
    bool m_led_strip_ok;

public:
    LedWS2812BDrv(struct led_strip_t *led_strip);

    bool led_strip_init() override;

    bool led_strip_set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue) override;

    bool led_strip_show() override;

    bool led_strip_clear() override;

    ~LedWS2812BDrv();

private:

    //static void led_strip_fill_item_level(rmt_item32_t *item, int high_ticks, int low_ticks);

    //static void led_strip_fill_rmt_items_ws2812(struct led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length);

    //static void led_strip_task(void *arg);

    //bool led_strip_init_rmt();
};

typedef void (*led_fill_rmt_items_fn)(struct led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length);

#endif