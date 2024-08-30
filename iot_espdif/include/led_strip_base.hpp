#ifndef _LED_STRIP_BASE_H
#define _LED_STRIP_BASE_H

#include "led_strip_interface.hpp"
#include "led_strip_drv_interface.hpp"

#include <string.h>

#define LED_STRIP_TASK_SIZE (2048)
#define LED_STRIP_TASK_PRIORITY (configMAX_PRIORITIES - 1)

#define LED_STRIP_REFRESH_PERIOD_MS (30U) // TODO: add as parameter to led_strip_init

#define LED_STRIP_NUM_RMT_ITEMS_PER_LED (24U) // Assumes 24 bit color for each led

// RMT Clock source is @ 80 MHz. Dividing it by 8 gives us 10 MHz frequency, or 100ns period.
#define LED_STRIP_RMT_CLK_DIV (8)

#define LED_STRIP_LENGTH 8U
#define LED_STRIP_RMT_INTR_NUM 19U



class LedStrip : public ILedStrip
{
private:
    struct led_strip_t ledStripConfig;
    bool m_led_strip_ok;
    ILedStripDrv& ledStripDriver;

public:
    LedStrip(ILedStripDrv& driver, struct led_strip_t *config);

    bool init() override;

    bool set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue) override;

    bool show() override;

    bool clear() override;

    led_strip_t* getStrip();

    void fill_rmt_items_buf1(rmt_item32_t *rmt_items);

    void fill_rmt_items_buf2(rmt_item32_t *rmt_items);

    ~LedStrip() {};
};

#endif