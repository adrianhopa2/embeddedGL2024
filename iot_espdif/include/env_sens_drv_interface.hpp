#ifndef _ENV_SENS_DRV_INTERFACE_H
#define _ENV_SENS_DRV_INTERFACE_H

#include <cstdint>
#include <driver/i2c_master.h>

class IEnvSensDrv
{

public:
    IEnvSensDrv() {};

    virtual bool init() = 0;
    virtual void startContinuousMeasurements() = 0;
    virtual void makeSingleMeasurement() = 0;
    virtual void stopMeasuring() = 0;
    virtual void configure() = 0;
    virtual void readTemperature(int16_t &temperature) = 0;
    virtual void readHumidity(uint8_t &humidity) = 0;
    virtual void readPressure(uint16_t &pressure) = 0;

    virtual ~IEnvSensDrv() {};
};

#endif