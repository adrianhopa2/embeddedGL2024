#ifndef _ENV_SENS_DRV_INTERFACE_H
#define _ENV_SENS_DRV_INTERFACE_H

#include <driver/i2c_master.h>

namespace env_sens
{

    class IEnvSensDrv
    {

    public:
        virtual bool init() = 0;
        virtual bool startContinuousMeasurements() = 0;
        virtual bool startSingleMeasurement() = 0;
        virtual bool stopMeasuring() = 0;
        virtual void readTemperature(int16_t &temperature) = 0;
        virtual void readHumidity(uint8_t &humidity) = 0;
        virtual void readPressure(uint16_t &pressure) = 0;

        virtual ~IEnvSensDrv() {};
    };

}

#endif