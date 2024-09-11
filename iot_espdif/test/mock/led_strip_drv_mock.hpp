#include "gmock/gmock.h"

#include "led_strip_drv_interface.hpp"

class MockLedStripDrv : public ILedStripDrv
{
    public:
    MOCK_METHOD(void, fill_rmt_items, (led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length), (override));
};