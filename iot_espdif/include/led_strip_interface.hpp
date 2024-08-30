#ifndef _LED_STRIP_INTERFACE_H
#define _LED_STRIP_INTERFACE_H

#include <cstdint>

class ILedStrip
{

public:
    virtual bool init() = 0;

    virtual bool set_pixel_rgb(uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue) = 0;

    virtual bool show() = 0;
    
    virtual bool clear() = 0;

    virtual ~ILedStrip() {};
};

#endif