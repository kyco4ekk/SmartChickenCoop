
#include <gtest/gtest.h>

#include "coop_application.h"

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

        config.insideSide = Side::Positive;

        return config;
    }
}

TEST(CoopApplicationTests, ValidConfigCreatesValidApplication)
{
    CoopApplication application(CreateValidConfig());

    EXPECT_TRUE(application.IsValid());
}

TEST(CoopApplicationTests, InitializesBirdCounterFromConfig)
{
    CoopApplication application(CreateValidConfig());

    const BirdCounter& counter = application.GetCounter();

    EXPECT_EQ(counter.GetTotal(), 20);
    EXPECT_EQ(counter.GetInside(), 15);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(CoopApplicationTests, ReturnsOriginalConfig)
{
    const CoopConfig config = CreateValidConfig();

    CoopApplication application(config);

    EXPECT_EQ(
        application.GetConfig().totalBirds,
        config.totalBirds
    );

    EXPECT_EQ(
        application.GetConfig().initialInsideBirds,
        config.initialInsideBirds
    );
}

TEST(CoopApplicationTests, EmptyDetectionsDoNotChangeCounter)
{
    CoopApplication application(CreateValidConfig());

    const auto events = application.Update({});

    EXPECT_TRUE(events.empty());

    const BirdCounter& counter = application.GetCounter();

    EXPECT_EQ(counter.GetInside(), 15);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(CoopApplicationTests, InvalidConfigCreatesInvalidApplication)
{
    CoopConfig config = CreateValidConfig();

    config.totalBirds = -1;

    CoopApplication application(config);

    EXPECT_FALSE(application.IsValid());
}

TEST(CoopApplicationTests, InvalidApplicationDoesNotProcessDetections)
{
    CoopConfig config = CreateValidConfig();

    config.initialInsideBirds = 25;

    CoopApplication application(config);

    EXPECT_FALSE(application.IsValid());

    const auto events = application.Update({});

    EXPECT_TRUE(events.empty());
}
