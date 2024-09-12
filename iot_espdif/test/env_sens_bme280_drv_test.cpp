#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "env_sens_bme280_drv.hpp"
#include "mock/i2c_master_wrapper_mock.hpp"

using ::testing::_;
using ::testing::Return;

class EnvSensBME280DrvTest : public EnvSensBME280Drv
{
public:
    EnvSensBME280DrvTest(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle, I2cMasterWrapper &i2cmasterwrapper) : EnvSensBME280Drv(bme280_config, bus_handle, i2cmasterwrapper) {}

    virtual ~EnvSensBME280DrvTest() {};
};

TEST(bme280test, i2cbustest)
{
    ::testing::StrictMock<MockI2cMasterWrapper> i2cmaster;

    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = -1,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    i2c_new_master_bus(&i2c_bus_config, &bus_handle);

    bme280_config_t bme280_config = {
        .standby_time = 0b011,
        .filter_coefficient = 0b000,
        .pressure_oversampling = 0b001,
        .temperature_oversampling = 0b001,
        .humidity_oversampling = 0b001,
    };

    EnvSensBME280DrvTest driver(&bme280_config, bus_handle, i2cmaster);

    EXPECT_CALL(i2cmaster, transmit(_, _, _, _)).Times(3).WillOnce(Return(ESP_OK));
    EXPECT_EQ(driver.init(), idle);
}