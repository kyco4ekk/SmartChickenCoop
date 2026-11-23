#include <gtest/gtest.h>

#include <limits>

#include "coop_config.h"

namespace
{
    CoopConfig CreateValidConfig()
    {
        CoopConfig config;

        config.totalBirds = 20;
        config.initialInsideBirds = 15;

        config.line = {
            {50.0f, 0.0f},
            {50.0f, 100.0f}
        };

        config.insideSide = Side::Negative;

        return config;
    }
}

TEST(CoopConfigTests, ValidConfigReturnsTrue)
{
    const CoopConfig config = CreateValidConfig();

    EXPECT_TRUE(config.IsValid());
}

TEST(CoopConfigTests, AllowsZeroBirds)
{
    CoopConfig config = CreateValidConfig();

    config.totalBirds = 0;
    config.initialInsideBirds = 0;

    EXPECT_TRUE(config.IsValid());
}

TEST(CoopConfigTests, AllowsAllBirdsInside)
{
    CoopConfig config = CreateValidConfig();

    config.totalBirds = 20;
    config.initialInsideBirds = 20;

    EXPECT_TRUE(config.IsValid());
}

TEST(CoopConfigTests, AllowsNoBirdsInside)
{
    CoopConfig config = CreateValidConfig();

    config.initialInsideBirds = 0;

    EXPECT_TRUE(config.IsValid());
}

TEST(CoopConfigTests, RejectsNegativeTotalBirds)
{
    CoopConfig config = CreateValidConfig();

    config.totalBirds = -1;

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsNegativeInitialInsideBirds)
{
    CoopConfig config = CreateValidConfig();

    config.initialInsideBirds = -1;

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsInitialInsideBirdsGreaterThanTotal)
{
    CoopConfig config = CreateValidConfig();

    config.totalBirds = 10;
    config.initialInsideBirds = 11;

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsNaNCoordinates)
{
    CoopConfig config = CreateValidConfig();

    config.line.start.x =
        std::numeric_limits<float>::quiet_NaN();

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsInfiniteCoordinates)
{
    CoopConfig config = CreateValidConfig();

    config.line.end.y =
        std::numeric_limits<float>::infinity();

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsDegenerateLine)
{
    CoopConfig config = CreateValidConfig();

    config.line = {
        {50.0f, 50.0f},
        {50.0f, 50.0f}
    };

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, RejectsInvalidInsideSide)
{
    CoopConfig config = CreateValidConfig();

    config.insideSide = static_cast<Side>(999);

    EXPECT_FALSE(config.IsValid());
}

TEST(CoopConfigTests, AllowsHorizontalLine)
{
    CoopConfig config = CreateValidConfig();

    config.line = {
        {0.0f, 50.0f},
        {100.0f, 50.0f}
    };

    config.insideSide = Side::Positive;

    EXPECT_TRUE(config.IsValid());
}

TEST(CoopConfigTests, AllowsNegativeCoordinates)
{
    CoopConfig config = CreateValidConfig();

    config.line = {
        {-100.0f, -50.0f},
        {-100.0f, 50.0f}
    };

    EXPECT_TRUE(config.IsValid());
}