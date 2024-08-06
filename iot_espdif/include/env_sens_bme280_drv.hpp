#ifndef _ENV_SENS_BME280_DRV_H
#define _ENV_SENS_BME280_DRV_H

#include "env_sens_drv_interface.hpp"

typedef struct 
{
    uint8_t standby_time;
    uint8_t filter_coefficient;
    uint8_t pressure_oversampling;
    uint8_t temperature_oversampling;
    uint8_t humidity_oversampling;
} bme280_config_t;

class EnvSensBME280Drv : public IEnvSensDrv
{

public:
    EnvSensBME280Drv() {};

    bool init(i2c_master_bus_handle_t bus_handle);
    void startContinuousMeasurements(); //enter normal mode
    void makeSingleMeasurement();   //enter forced mode
    void stopMeasuring();   //enter sleep mode
    void configure(const bme280_config_t *bme280_config);
    void readTemperature(int16_t& temperature);
    void readHumidity(uint8_t& humidity);
    void readPressure(uint16_t& pressure);

    ~EnvSensBME280Drv() {};
};

#endif