#ifndef _LED_STRIP_WS2812B_DRV_H
#define _LED_STRIP_WS2812B_DRV_H

#include "led_strip_drv_interface.hpp"

#include <driver/gpio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/****************************
        WS2812 Timing
 ****************************/
#define LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812 9 // 900ns (900ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812 3  // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812 3 // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812 9  // 900ns (900ns +/- 150ns per datasheet)



class LedStripWS2812BDrv : public ILedStripDrv
{

public:
    LedStripWS2812BDrv();

    void fill_rmt_items(led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length) override;

    ~LedStripWS2812BDrv();

};



#endif