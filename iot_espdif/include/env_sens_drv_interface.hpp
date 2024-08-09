#ifndef _ENV_SENS_DRV_INTERFACE_H
#define _ENV_SENS_DRV_INTERFACE_H

enum return_code
{
    idle,   //no measurement started after init()
    busy,   //after measurement started until the first measurement is done
    ok,     //after the first measurement  is done
    error,  //when i2c or sensor returns error during process() call, when "error" happen, then the next measurement will not start, it will be in "error" state until the init() is called, and the init() does the reinit of i2c and sensor
    not_supported,
};

class IEnvSensDrv
{

public:
    virtual bool init() = 0;
    virtual bool startContinuousMeasurements() = 0;
    virtual bool startSingleMeasurement() = 0;
    virtual bool stopMeasuring() = 0;
    virtual return_code readTemperature(int32_t &temperature) = 0;
    virtual return_code readHumidity(uint16_t &humidity) = 0;
    virtual return_code readPressure(uint32_t &pressure) = 0;
    virtual return_code getMeasurementStatus() = 0;
    virtual void process() = 0;

    virtual ~IEnvSensDrv() {};
};

#endif