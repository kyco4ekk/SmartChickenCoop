
#include <gtest/gtest.h>
#include "direction_detector.h"

const Line TEST_LINE = {
    {50.0f, 0.0f},
    {50.0f, 100.0f}
};

// Справа от линии находится курятник.
const Side INSIDE_SIDE = Side::Negative;

TEST(DirectionDetectorTests, DetectsBirdEntering)
{
    auto result = DirectionDetector::Detect(
        {40.0f, 50.0f},
        {60.0f, 50.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, DetectsBirdExiting)
{
    auto result = DirectionDetector::Detect(
        {60.0f, 50.0f},
        {40.0f, 50.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::Exited);
}

TEST(DirectionDetectorTests, DoesNotDetectMovementWithoutCrossing)
{
    auto result = DirectionDetector::Detect(
        {20.0f, 20.0f},
        {30.0f, 30.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, IgnoresCrossingOutsideLineSegment)
{
    auto result = DirectionDetector::Detect(
        {40.0f, 120.0f},
        {60.0f, 120.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, DetectsNoMovement)
{
    auto result = DirectionDetector::Detect(
        {40.0f, 50.0f},
        {40.0f, 50.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::None);
}


TEST(DirectionDetectorTests, IgnoresMovementAlongLine)
{
    auto result = DirectionDetector::Detect(
        {50.0f, 10.0f},
        {50.0f, 90.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, IgnoresTouchingLineWithoutCrossing)
{
    auto result = DirectionDetector::Detect(
        {40.0f, 50.0f},
        {50.0f, 50.0f},
        TEST_LINE,
        INSIDE_SIDE
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, DetectsCrossingOfDiagonalLine)
{
    const Line diagonalLine = {
        {0.0f, 0.0f},
        {100.0f, 100.0f}
    };

    auto result = DirectionDetector::Detect(
        {60.0f, 40.0f},
        {40.0f, 60.0f},
        diagonalLine,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, DetectsCrossingInReverseDirection)
{
    const Line diagonalLine = {
        {0.0f, 0.0f},
        {100.0f, 100.0f}
    };

    auto result = DirectionDetector::Detect(
        {40.0f, 60.0f},
        {60.0f, 40.0f},
        diagonalLine,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Exited);
}

TEST(DirectionDetectorTests, IgnoresZeroLengthLine)
{
    const Line invalidLine = {
        {50.0f, 50.0f},
        {50.0f, 50.0f}
    };

    auto result = DirectionDetector::Detect(
        {40.0f, 50.0f},
        {60.0f, 50.0f},
        invalidLine,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::None);
}


TEST(DirectionDetectorTests, DetectsShortCrossing)
{
    Line line = {{0, 0}, {10, 0}};

    auto result = DirectionDetector::Detect(
        {5, -0.001f},
        {5, 0.001f},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, DetectsLongCrossing)
{
    Line line = {{0, 0}, {10, 0}};

    auto result = DirectionDetector::Detect(
        {5, -100},
        {5, 100},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, IgnoresMovementFarFromLine)
{
    Line line = {{0, 0}, {10, 0}};

    auto result = DirectionDetector::Detect(
        {2, -5},
        {8, -5},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, DetectsCrossingAtLineEndpoint)
{
    Line line = {{0, 0}, {10, 0}};

    auto result = DirectionDetector::Detect(
        {-1, -1},
        {1, 1},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, IgnoresMovementBeyondLineEndpoint)
{
    Line line = {{0, 0}, {10, 0}};

    auto result = DirectionDetector::Detect(
        {-2, -1},
        {-2, 1},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, DetectsCrossingWithReversedLineOrientation)
{
    Line line = {{10, 0}, {0, 0}};

    auto result = DirectionDetector::Detect(
        {5, 1},
        {5, -1},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::Entered);
}

TEST(DirectionDetectorTests, IgnoresMovementAlongDiagonalLine)
{
    Line line = {{0, 0}, {10, 10}};

    auto result = DirectionDetector::Detect(
        {2, 2},
        {8, 8},
        line,
        Side::Positive
    );

    EXPECT_EQ(result, CrossingDirection::None);
}

TEST(DirectionDetectorTests, DetectsRepeatedIndependentCrossings)
{
    Line line = {{0, 0}, {10, 0}};

    auto first = DirectionDetector::Detect(
        {5, -1}, {5, 1}, line, Side::Positive
    );

    auto second = DirectionDetector::Detect(
        {5, 1}, {5, -1}, line, Side::Positive
    );

    EXPECT_EQ(first, CrossingDirection::Entered);
    EXPECT_EQ(second, CrossingDirection::Exited);
}


