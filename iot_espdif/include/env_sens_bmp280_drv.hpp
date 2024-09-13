#ifndef _ENV_SENS_BMP280_DRV_H
#define _ENV_SENS_BMP280_DRV_H

#include "i2c_master_wrapper.hpp"
#include "env_sens_drv_interface.hpp"

typedef struct bmp280_config_t
{
    uint8_t standby_time;
    uint8_t filter_coefficient;
    uint8_t pressure_oversampling;
    uint8_t temperature_oversampling;
} bmp280_config_t;

struct bmp280_calib_data
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
    int32_t t_fine;
};

class EnvSensBMP280Drv : public IEnvSensDrv
{
private:
    i2c_master_bus_handle_t m_bus_handle;
    i2c_master_dev_handle_t m_dev_handle;
    bmp280_config_t m_bmp280_config;
    bmp280_calib_data m_calib_data;
    return_code m_state;
    int32_t m_temperature;
    uint32_t m_pressure;
    II2cMasterWrapper &m_ii2cmasterwrapper;

public:
    EnvSensBMP280Drv(bmp280_config_t *bmp280_config, i2c_master_bus_handle_t bus_handle, II2cMasterWrapper &ii2cmasterwrapper);

    return_code init() override;
    return_code startContinuousMeasurements() override;
    return_code startSingleMeasurement() override;
    return_code stopMeasuring() override;
    return_code readTemperature(int32_t &temperature) override;
    return_code readPressure(uint32_t &pressure) override;
    return_code getMeasurementState() override;
    void process() override;

    ~EnvSensBMP280Drv();
};

#endif