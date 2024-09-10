#ifndef _RMT_WRAPPER_H
#define _RMT_WRAPPER_H

#include <driver/rmt.h>

class RmtWrapper
{
public:
    esp_err_t config(const rmt_config_t *rmt_param)
    {
        return rmt_config(rmt_param);
    }

    esp_err_t driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags)
    {
        return rmt_driver_install(channel, rx_buf_size, intr_alloc_flags);
    }

    esp_err_t write_items(rmt_channel_t channel, const rmt_item32_t *rmt_item, int item_num, bool wait_tx_done)
    {
        return rmt_write_items(channel, rmt_item, item_num, wait_tx_done);
    }

    esp_err_t wait_tx_done(rmt_channel_t channel, TickType_t wait_time)
    {
        return rmt_wait_tx_done(channel, wait_time);
    }
};

#endif