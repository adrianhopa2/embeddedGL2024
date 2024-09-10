#ifndef _LED_STRIP_DRV_INTERFACE_H
#define _LED_STRIP_DRV_INTERFACE_H

#include "rmt_wrapper.hpp"

struct led_strip_t {
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

struct led_color_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class ILedStripDrv
{
public:

    virtual void fill_rmt_items(led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length) = 0;

    virtual ~ILedStripDrv() {};
};

#endif