#include "led_strip_ws2812b_drv.hpp"

LedStripWS2812BDrv::LedStripWS2812BDrv()
{

}

LedStripWS2812BDrv::~LedStripWS2812BDrv()
{
}


static inline void led_strip_fill_item_level(rmt_item32_t *item, int high_ticks, int low_ticks)
{
    item->level0 = 1;
    item->duration0 = high_ticks;
    item->level1 = 0;
    item->duration1 = low_ticks;
}

void LedStripWS2812BDrv::fill_rmt_items(led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length)
{
    uint32_t rmt_items_index = 0;
    for (uint32_t led_index = 0; led_index < led_strip_length; led_index++)
    {
        struct led_color_t led_color = led_strip_buf[led_index];

        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.green >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.red >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--)
        {
            uint8_t bit_set = (led_color.blue >> (bit - 1)) & 1;
            if (bit_set)
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_1_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_1_LOW_WS2812);
            }
            else
            {
                led_strip_fill_item_level((&(rmt_items[rmt_items_index])), LED_STRIP_RMT_TICKS_BIT_0_HIGH_WS2812, LED_STRIP_RMT_TICKS_BIT_0_LOW_WS2812);
            }
            rmt_items_index++;
        }
    }
}

