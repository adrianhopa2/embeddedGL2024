#ifndef _I2C_MASTER_WRAPPER_H
#define _I2C_MASTER_WRAPPER_H

#include <driver/i2c_master.h>

class I2cMasterWrapper
{
public:
    virtual esp_err_t bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle)
    {
        return i2c_master_bus_add_device(bus_handle, dev_config, ret_handle);
    }

    virtual esp_err_t transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms)
    {
        return i2c_master_transmit(i2c_dev, write_buffer, write_size, xfer_timeout_ms);
    }

    virtual esp_err_t transmit_receive(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
    {
        return i2c_master_transmit_receive(i2c_dev, write_buffer, write_size, read_buffer, read_size, xfer_timeout_ms);
    }

    virtual esp_err_t new_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle)
    {
        return i2c_new_master_bus(bus_config, ret_bus_handle);
    }

    virtual ~I2cMasterWrapper() {};
};
#endif