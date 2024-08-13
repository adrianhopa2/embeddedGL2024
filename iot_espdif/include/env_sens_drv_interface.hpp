#ifndef _ENV_SENS_DRV_INTERFACE_H
#define _ENV_SENS_DRV_INTERFACE_H

enum return_code
{
    not_initialized, //init() has not been called yet
    idle,   //no measurement started after init()
    busy,   //after measurement started until the first measurement is done
    ok,     //after the first measurement  is done
    error,  //when i2c or sensor returns error during process() call, when "error" happen, then the next measurement will not start, it will be in "error" state until the init() is called, and the init() does the reinit of i2c and sensor
    not_supported,  //not supported by the sensor
};

class IEnvSensDrv
{

public:
    virtual return_code init() = 0;
    virtual return_code startContinuousMeasurements() = 0;
    virtual return_code startSingleMeasurement() = 0;
    virtual return_code stopMeasuring() = 0;
    virtual return_code readTemperature(int32_t &temperature) = 0;
    virtual return_code readHumidity(uint32_t &humidity) = 0;
    virtual return_code readPressure(uint32_t &pressure) = 0;
    virtual return_code getMeasurementState() = 0;
    virtual void process() = 0;

    virtual ~IEnvSensDrv() {};
};

#endif