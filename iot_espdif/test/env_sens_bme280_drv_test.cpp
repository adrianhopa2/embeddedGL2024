#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "env_sens_bme280_drv.hpp"
#include "mock/i2c_master_wrapper_mock.hpp"

using ::testing::_;
using ::testing::Return;

class EnvSensBME280DrvTest : public EnvSensBME280Drv
{
public:
    EnvSensBME280DrvTest(bme280_config_t *bme280_config, i2c_master_bus_handle_t bus_handle, II2cMasterWrapper &ii2cmasterwrapper)
        : EnvSensBME280Drv(bme280_config, bus_handle, ii2cmasterwrapper) {}

    virtual ~EnvSensBME280DrvTest() {};
};

class Bme280DrvTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        i2cmaster = std::make_unique<::testing::StrictMock<MockI2cMasterWrapper>>();

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

        driver = std::make_unique<EnvSensBME280DrvTest>(&bme280_config, bus_handle, *i2cmaster);
    }

    virtual void TearDown()
    {
        i2c_del_master_bus(driver->getBusHandle());
        i2cmaster.reset();
        driver.reset();
    }

protected:
    std::unique_ptr<::testing::StrictMock<MockI2cMasterWrapper>> i2cmaster;
    std::unique_ptr<EnvSensBME280DrvTest> driver;
};

TEST_F(Bme280DrvTest, i2ctest_ok)
{
    EXPECT_CALL(*i2cmaster, bus_add_device(_, _, _))
        .Times(1)
        .WillOnce(Return(ESP_OK));
    EXPECT_CALL(*i2cmaster, transmit(_, _, _, _))
        .Times(3)
        .WillOnce(Return(ESP_OK))
        .WillOnce(Return(ESP_OK))
        .WillOnce(Return(ESP_OK));
    EXPECT_CALL(*i2cmaster, transmit_receive(_, _, _, _, _, _))
        .Times(2)
        .WillOnce(Return(ESP_OK))
        .WillOnce(Return(ESP_OK));
    EXPECT_EQ(driver->init(), idle);
}

TEST_F(Bme280DrvTest, i2ctest_bus_invalidarg)
{
    EXPECT_CALL(*i2cmaster, bus_add_device(_, _, _))
        .Times(1)
        .WillOnce(Return(ESP_ERR_INVALID_ARG));
    EXPECT_EQ(driver->init(), error);
}

TEST_F(Bme280DrvTest, i2ctest_bus_nomem)
{
    EXPECT_CALL(*i2cmaster, bus_add_device(_, _, _))
        .Times(1)
        .WillOnce(Return(ESP_ERR_NO_MEM));
    EXPECT_EQ(driver->init(), error);
}

TEST_F(Bme280DrvTest, i2ctest_transmit_invalidarg)
{
    EXPECT_CALL(*i2cmaster, bus_add_device(_, _, _))
        .Times(1)
        .WillOnce(Return(ESP_OK));
    EXPECT_CALL(*i2cmaster, transmit(_, _, _, _))
        .Times(::testing::AtLeast(1))
        .WillOnce(Return(ESP_ERR_INVALID_ARG));
    EXPECT_EQ(driver->init(), error);
}