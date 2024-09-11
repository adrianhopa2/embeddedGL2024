#include <gmock/gmock.h>

#include "i2c_master_wrapper.hpp"

class MockI2cMasterWrapper : public I2cMasterWrapper
{
public:
    MOCK_METHOD(esp_err_t, bus_add_device, (i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle), (override));
    MOCK_METHOD(esp_err_t, transmit, (i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms), (override));
    MOCK_METHOD(esp_err_t, transmit_receive, (i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms), (override));
    MOCK_METHOD(esp_err_t, new_bus, (const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle), (override));
};