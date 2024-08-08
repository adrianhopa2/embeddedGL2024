#ifndef _ENV_SENS_BME280_DRV_H
#define _ENV_SENS_BME280_DRV_H

#include "env_sens_drv_interface.hpp"
#include <cstring>

namespace env_sens
{

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
    private:
        bme280_config_t m_bme280_config;
        i2c_master_bus_handle_t m_bus_handle;
        i2c_master_dev_handle_t m_dev_handle;

    public:
        EnvSensBME280Drv(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle);

        bool init() override;
        bool startContinuousMeasurements() override; // enter normal mode
        bool startSingleMeasurement() override;      // enter forced mode
        bool stopMeasuring() override;               // enter sleep mode
        void readTemperature(int16_t &temperature) override;
        void readHumidity(uint8_t &humidity) override;
        void readPressure(uint16_t &pressure) override;

        ~EnvSensBME280Drv() {};
    };

}

#endif