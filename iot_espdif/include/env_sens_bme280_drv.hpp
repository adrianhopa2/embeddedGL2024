#ifndef _ENV_SENS_BME280_DRV_H
#define _ENV_SENS_BME280_DRV_H

#include "i2c_master_wrapper.hpp"
#include "env_sens_drv_interface.hpp"

typedef struct bme280_config_t
{
    uint8_t standby_time;
    uint8_t filter_coefficient;
    uint8_t pressure_oversampling;
    uint8_t temperature_oversampling;
    uint8_t humidity_oversampling;
} bme280_config_t;

struct bme280_calib_data
{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    uint8_t dig_h1;
    int16_t dig_h2;
    uint8_t dig_h3;
    int16_t dig_h4;
    int16_t dig_h5;
    int8_t dig_h6;
    int32_t t_fine;
};

class EnvSensBME280Drv : public IEnvSensDrv
{
private:
    i2c_master_bus_handle_t m_bus_handle;
    i2c_master_dev_handle_t m_dev_handle;
    bme280_config_t m_bme280_config;
    bme280_calib_data m_calib_data;
    return_code m_state;
    int32_t m_temperature;
    uint32_t m_pressure;
    uint32_t m_humidity;
    I2cMasterWrapper m_i2cmasterwrapper;

public:
    EnvSensBME280Drv(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle, I2cMasterWrapper &i2cmasterwrapper);

    return_code init() override;
    return_code startContinuousMeasurements() override;
    return_code startSingleMeasurement() override;
    return_code stopMeasuring() override;
    return_code readTemperature(int32_t &temperature) override;
    return_code readHumidity(uint32_t &humidity) override;
    return_code readPressure(uint32_t &pressure) override;
    return_code getMeasurementState() override;
    void process() override;

    ~EnvSensBME280Drv();
};

#endif