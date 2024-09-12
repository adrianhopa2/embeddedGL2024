#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock/i2c_master_wrapper_mock.hpp"

using ::testing::_;
using ::testing::Return;

class Bme280I2cTest : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        i2cmaster = std::make_unique<::testing::StrictMock<MockI2cMasterWrapper>>();
    }

    virtual void TearDown()
    {
        i2cmaster.reset();
    }

protected:
    std::unique_ptr<::testing::StrictMock<MockI2cMasterWrapper>> i2cmaster;
};

TEST_F(Bme280I2cTest, i2cbustest)
{
    EXPECT_CALL(*i2cmaster, transmit(_,_,_,_)).Times(3).WillOnce(Return(ESP_OK));
}