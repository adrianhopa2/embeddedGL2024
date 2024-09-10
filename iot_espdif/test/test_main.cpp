#include <gtest/gtest.h>
// uncomment line below if you plan to use GMock
#include <gmock/gmock.h>

// TEST(...)
// TEST_F(...)

TEST(DummyTest, ShouldPass)
{
    EXPECT_EQ(1, 1);
}

extern "C"
{
    int app_main(void)
    {
        //::testing::InitGoogleTest(&argc, argv);
        // if you plan to use GMock, replace the line above with
        ::testing::InitGoogleMock();

        if (RUN_ALL_TESTS())
        {
            ;
        }

        // Always return zero-code and allow PlatformIO to parse results
        return 0;
    }
}