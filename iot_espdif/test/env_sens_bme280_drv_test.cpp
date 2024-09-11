#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "env_sens_bme280_drv.hpp"
#include "mock/i2c_master_wrapper_mock.hpp"

using ::testing::_;

TEST(bme280test, i2cbustest)
{
    MockI2cMasterWrapper i2cmastermock;

    EXPECT_CALL(i2cmastermock, bus_add_device(_, _, _));
}