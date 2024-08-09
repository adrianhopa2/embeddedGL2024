#include "env_sens_bme280_drv.hpp"

EnvSensBME280Drv::EnvSensBME280Drv(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle)
{
    m_bme280_config = {
        .standby_time = bme280_config->standby_time,
        .filter_coefficient = bme280_config->filter_coefficient,
        .pressure_oversampling = bme280_config->pressure_oversampling,
        .temperature_oversampling = bme280_config->temperature_oversampling,
        .humidity_oversampling = bme280_config->humidity_oversampling,
    };

    m_bus_handle = bus_handle;

    m_humidity = 0;
    m_temperature = 0;
    m_pressure = 0;
}

bool EnvSensBME280Drv::init()
{
    m_state = idle;

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
    buffer_ctrl_meas[1] = m_bme280_config.humidity_oversampling;
    ESP_ERROR_CHECK(i2c_master_transmit(m_dev_handle, buffer_ctrl_hum, 2, -1));

    buffer_ctrl_meas[0] = 0xF4;
    buffer_ctrl_hum[1] = ((m_bme280_config.temperature_oversampling << 5) | (m_bme280_config.pressure_oversampling << 2));
    ESP_ERROR_CHECK(i2c_master_transmit(m_dev_handle, buffer_ctrl_meas, 2, -1));

    uint8_t reg_addr[1] = {0x88};
    uint8_t reg_data[26] = {0};

    if (i2c_master_transmit_receive(m_dev_handle, reg_addr, 1, reg_data, 26, -1) != ESP_OK)
    {
        m_state = error;
    }

    if (m_state == idle)
    {
        calib_data.dig_t1 = (((uint16_t)reg_data[1] << 8) | (uint16_t)reg_data[0]);
        calib_data.dig_t2 = (int16_t)(((uint16_t)reg_data[3] << 8) | (uint16_t)reg_data[2]);
        calib_data.dig_t3 = (int16_t)(((uint16_t)reg_data[5] << 8) | (uint16_t)reg_data[4]);
        calib_data.dig_p1 = (((uint16_t)reg_data[7] << 8) | (uint16_t)reg_data[6]);
        calib_data.dig_p2 = (int16_t)(((uint16_t)reg_data[9] << 8) | (uint16_t)reg_data[8]);
        calib_data.dig_p3 = (int16_t)(((uint16_t)reg_data[11] << 8) | (uint16_t)reg_data[10]);
        calib_data.dig_p4 = (int16_t)(((uint16_t)reg_data[13] << 8) | (uint16_t)reg_data[12]);
        calib_data.dig_p5 = (int16_t)(((uint16_t)reg_data[15] << 8) | (uint16_t)reg_data[14]);
        calib_data.dig_p6 = (int16_t)(((uint16_t)reg_data[17] << 8) | (uint16_t)reg_data[16]);
        calib_data.dig_p7 = (int16_t)(((uint16_t)reg_data[19] << 8) | (uint16_t)reg_data[18]);
        calib_data.dig_p8 = (int16_t)(((uint16_t)reg_data[21] << 8) | (uint16_t)reg_data[20]);
        calib_data.dig_p9 = (int16_t)(((uint16_t)reg_data[23] << 8) | (uint16_t)reg_data[22]);
        calib_data.dig_h1 = reg_data[25];
        reg_addr[0] = 0xE1;

        if (i2c_master_transmit_receive(m_dev_handle, reg_addr, 1, reg_data, 26, -1) != ESP_OK)
        {
            m_state = error;
        }

        if (m_state == idle)
        {
            int16_t dig_h4_lsb;
            int16_t dig_h4_msb;
            int16_t dig_h5_lsb;
            int16_t dig_h5_msb;

            calib_data.dig_h2 = (int16_t)(((uint16_t)reg_data[1] << 8) | (uint16_t)reg_data[0]);
            calib_data.dig_h3 = reg_data[2];
            dig_h4_msb = (int16_t)(int8_t)reg_data[3] * 16;
            dig_h4_lsb = (int16_t)(reg_data[4] & 0x0F);
            calib_data.dig_h4 = dig_h4_msb | dig_h4_lsb;
            dig_h5_msb = (int16_t)(int8_t)reg_data[5] * 16;
            dig_h5_lsb = (int16_t)(reg_data[4] >> 4);
            calib_data.dig_h5 = dig_h5_msb | dig_h5_lsb;
            calib_data.dig_h6 = (int8_t)reg_data[6];
        }
    }

    return true;
}

// enter normal mode
bool EnvSensBME280Drv::startContinuousMeasurements()
{
    uint8_t buffer[2];
    uint8_t mode = 0b11;

    buffer[0] = 0xF4;
    buffer[1] = ((m_bme280_config.temperature_oversampling << 5) | (m_bme280_config.pressure_oversampling << 2) | mode);

    if (i2c_master_transmit(m_dev_handle, buffer, 2, -1) == ESP_OK)
    {
        pending = true;
        return true;
    }
    else
    {
        return false;
    }
}

// enter forced mode
bool EnvSensBME280Drv::startSingleMeasurement()
{
    uint8_t buffer[2];
    uint8_t mode = 0b10;

    buffer[0] = 0xF4;
    buffer[1] = ((m_bme280_config.temperature_oversampling << 5) | (m_bme280_config.pressure_oversampling << 2) | mode);

    if (i2c_master_transmit(m_dev_handle, buffer, 2, -1) == ESP_OK)
    {
        pending = true;
        return true;
    }
    else
    {
        return false;
    }
}

// enter sleep mode
bool EnvSensBME280Drv::stopMeasuring()
{
    uint8_t buffer[2];
    uint8_t mode = 0b00;

    buffer[0] = 0xF4;
    buffer[1] = ((m_bme280_config.temperature_oversampling << 5) | (m_bme280_config.pressure_oversampling << 2) | mode);

    if (i2c_master_transmit(m_dev_handle, buffer, 2, -1) == ESP_OK)
    {
        pending = false;
        return true;
    }
    else
    {
        return false;
    }
}

return_code EnvSensBME280Drv::readTemperature(int32_t &temperature)
{
    if (m_state == ok)
    {
        temperature = m_temperature;
    }
    return m_state;
}

return_code EnvSensBME280Drv::readHumidity(uint16_t &humidity)
{
    if (m_state == ok)
    {
        humidity = m_humidity;
    }
    return m_state;
}

return_code EnvSensBME280Drv::readPressure(uint32_t &pressure)
{
    if (m_state == ok)
    {
        pressure = m_pressure;
    }
    return m_state;
}

return_code EnvSensBME280Drv::getMeasurementStatus()
{
    uint8_t addr[1] = {0xF3};
    uint8_t buffer[1];

    i2c_master_transmit_receive(m_dev_handle, addr, 1, buffer, 1, -1);

    if (buffer[0] & 0b00001000 || buffer[0] & 0b00000001)
    {
        return busy;
    }

    return m_state;
}

void EnvSensBME280Drv::process()
{

    if (m_state != error && getMeasurementStatus() != busy && pending)
    {
        m_state = busy;

        uint8_t addr[1] = {0xF7};
        uint8_t buffer[8];

        if (i2c_master_transmit_receive(m_dev_handle, addr, 1, buffer, 8, -1) != ESP_OK)
        {
            m_state = error;
        }

        uint32_t press_msb = (uint32_t)buffer[0] << 12;
        uint32_t press_lsb = (uint32_t)buffer[1] << 4;
        uint32_t press_xlsb = (uint32_t)buffer[2] << 4;
        uint32_t temp_msb = (uint32_t)buffer[3] << 12;
        uint32_t temp_lsb = (uint32_t)buffer[4] << 4;
        uint32_t temp_xlsb = (uint32_t)buffer[5] << 4;
        uint16_t hum_msb = (uint16_t)buffer[6] << 8;
        uint16_t hum_lsb = (uint16_t)buffer[7];

        uint32_t uncompensated_pressure = press_msb | press_lsb | press_xlsb;
        uint32_t uncompensated_temperature = temp_msb | temp_lsb | temp_xlsb;
        uint16_t uncompensated_humidity = hum_msb | hum_lsb;

        int32_t var1, var2;
        var1 = (int32_t)((uncompensated_temperature / 8) - ((int32_t)calib_data.dig_t1 * 2));
        var1 = (var1 * ((int32_t)calib_data.dig_t2)) / 2048;
        var2 = (int32_t)((uncompensated_temperature / 16) - ((int32_t)calib_data.dig_t1));
        var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data.dig_t3)) / 16384;
        calib_data.t_fine = var1 + var2;
        m_temperature = (calib_data.t_fine * 5 + 128) / 256;

        int64_t var3, var4, p;
        var3 = ((int64_t)calib_data.t_fine) - 128000;
        var4 = var3 * var3 * (int64_t)calib_data.dig_p6;
        var4 = var4 + ((var3 * (int64_t)calib_data.dig_p5) << 17);
        var4 = var4 + (((int64_t)calib_data.dig_p4) << 35);
        var3 = ((var3 * var3 * (int64_t)calib_data.dig_p3) >> 8) + ((var3 * (int64_t)calib_data.dig_p2) << 12);
        var3 = (((((int64_t)1) << 47) + var3)) * ((int64_t)calib_data.dig_p1) >> 33;
        if (var3 == 0)
        {
            m_pressure = 0;
        }
        else
        {
            p = 1048576 - uncompensated_pressure;
            p = (((p << 31) - var4) * 3125) / var3;
            var3 = (((int64_t)calib_data.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
            var4 = (((int64_t)calib_data.dig_p8) * p) >> 19;
            p = ((p + var3 + var4) >> 8) + (((int64_t)calib_data.dig_p7) << 4);
            m_pressure = (uint32_t)p;
        }

        int32_t v_x1_u32r;
        v_x1_u32r = (calib_data.t_fine - ((int32_t)76800));
        v_x1_u32r = (((((uncompensated_humidity << 14) - (((int32_t)calib_data.dig_h4) << 20) - (((int32_t)calib_data.dig_h5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                     (((((((v_x1_u32r * ((int32_t)calib_data.dig_h6)) >> 10) * (((v_x1_u32r * ((int32_t)calib_data.dig_h3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)calib_data.dig_h2) + 8192) >> 14));
        v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)calib_data.dig_h1)) >> 4));
        v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
        v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
        m_humidity = (uint32_t)(v_x1_u32r >> 12);

        m_state = ok;
    }
}

EnvSensBME280Drv::~EnvSensBME280Drv()
{
}
