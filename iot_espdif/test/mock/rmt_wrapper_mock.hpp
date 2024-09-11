#include <gmock/gmock.h>

#include "rmt_wrapper.hpp"

class MockRmtWrapper : public RmtWrapper
{
public:
    MOCK_METHOD(esp_err_t, config, (const rmt_config_t *rmt_param), (override));
    MOCK_METHOD(esp_err_t, driver_install, (rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags), (override));
    MOCK_METHOD(esp_err_t, write_items, (rmt_channel_t channel, const rmt_item32_t *rmt_item, int item_num, bool wait_tx_done), (override));
    MOCK_METHOD(esp_err_t, wait_tx_done, (rmt_channel_t channel, TickType_t wait_time), (override));
};