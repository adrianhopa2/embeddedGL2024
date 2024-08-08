#include "env_sens_bme280_drv.hpp"

env_sens::EnvSensBME280Drv::EnvSensBME280Drv(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle)
{
    m_bme280_config = {
        .standby_time = bme280_config->standby_time,
        .filter_coefficient = bme280_config->filter_coefficient,
        .pressure_oversampling = bme280_config->pressure_oversampling,
        .temperature_oversampling = bme280_config->temperature_oversampling,
        .humidity_oversampling = bme280_config->humidity_oversampling,
    };

    m_bus_handle = bus_handle;
}

bool env_sens::EnvSensBME280Drv::init()
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x76,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {
            .disable_ack_check = false,
        },
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(m_bus_handle, &dev_cfg, &m_dev_handle));

    uint8_t buffer_config[2];
    uint8_t buffer_ctrl_meas[2];
    uint8_t buffer_ctrl_hum[2];

    buffer_config[0] = 0xF5;
    buffer_config[1] = ((m_bme280_config.standby_time << 5) | (m_bme280_config.filter_coefficient << 2));
    ESP_ERROR_CHECK(i2c_master_transmit(m_dev_handle, buffer_config, 2, -1));

    buffer_ctrl_hum[0] = 0xF2;
    buffer_ctrl_hum[1] = ((m_bme280_config.temperature_oversampling << 5) | (m_bme280_config.pressure_oversampling << 2));
    ESP_ERROR_CHECK(i2c_master_transmit(m_dev_handle, buffer_ctrl_hum, 2, -1));

    buffer_ctrl_meas[0] = 0xF4;
    buffer_ctrl_meas[1] = m_bme280_config.humidity_oversampling;
    ESP_ERROR_CHECK(i2c_master_transmit(m_dev_handle, buffer_ctrl_meas, 2, -1));
}