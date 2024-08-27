#ifndef _LED_DRV_INTERFACE_H
#define _LED_DRV_INTERFACE_H

#include <cstdint>

class ILedDrv
{

public:
    virtual bool led_strip_init() = 0;

    virtual bool led_strip_set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue) = 0;

    virtual bool led_strip_show() = 0;
    
    virtual bool led_strip_clear() = 0;

    virtual ~ILedDrv() {};
};

#endif