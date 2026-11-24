
#include <gtest/gtest.h>

#include "bird_movement_processor.h"

const Line PROCESSOR_TEST_LINE = {
    {50.0f, 0.0f},
    {50.0f, 100.0f}
};

// Курятник находится справа от линии.
const Side PROCESSOR_INSIDE_SIDE = Side::Negative;

TEST(BirdMovementProcessorTests, CountsBirdEntering)
{
    BirdTracker tracker;
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    // Первый кадр: птица снаружи.
    processor.Update({
        {40.0f, 50.0f}
    });

    // Второй кадр: птица пересекла линию.
    auto events = processor.Update({
        {60.0f, 50.0f}
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 11);
    EXPECT_EQ(counter.GetOutside(), 9);
}

TEST(BirdMovementProcessorTests, CountsBirdExiting)
{
    BirdTracker tracker;
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    // Первый кадр: птица внутри.
    processor.Update({
        {60.0f, 50.0f}
    });

    // Второй кадр: птица вышла.
    auto events = processor.Update({
        {40.0f, 50.0f}
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Exited);

    EXPECT_EQ(counter.GetInside(), 9);
    EXPECT_EQ(counter.GetOutside(), 11);
}

TEST(BirdMovementProcessorTests, DoesNotCountNewBirdInside)
{
    BirdTracker tracker;
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    // Птица сразу появилась внутри кадра.
    auto events = processor.Update({
        {60.0f, 50.0f}
    });

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdMovementProcessorTests, DoesNotCountMovementWithoutCrossing)
{
    BirdTracker tracker;
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    processor.Update({
        {20.0f, 50.0f}
    });

    auto events = processor.Update({
        {30.0f, 50.0f}
    });

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdMovementProcessorTests, DoesNotCountAfterTemporaryDisappearance)
{
    BirdTracker tracker(30.0f, 2);
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    processor.Update({
        {40.0f, 50.0f}
    });

    // Птица пропала на один кадр.
    processor.Update({});

    // Появилась уже по другую сторону линии.
    auto events = processor.Update({
        {60.0f, 50.0f}
    });

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdMovementProcessorTests, CountsMultipleBirdsCrossingInSameFrame)
{
    BirdTracker tracker;
    BirdCounter counter(20, 10);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    processor.Update({
        {40.0f, 30.0f},
        {60.0f, 70.0f}
    });

    // Первая птица входит, вторая выходит.
    auto events = processor.Update({
        {60.0f, 30.0f},
        {40.0f, 70.0f}
    });

    ASSERT_EQ(events.size(), 2);

    EXPECT_EQ(counter.GetInside(), 10);
    EXPECT_EQ(counter.GetOutside(), 10);

    EXPECT_NE(events[0].birdId, events[1].birdId);
}

TEST(BirdMovementProcessorTests, DoesNotCountMoreBirdsThanOutsideCount)
{
    BirdTracker tracker;
    BirdCounter counter(2, 1);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    processor.Update({
        {40.0f, 30.0f},
        {40.0f, 70.0f}
    });

    auto events = processor.Update({
        {60.0f, 30.0f},
        {60.0f, 70.0f}
    });

    // Оба события обнаружены, но войти может только одна птица.
    ASSERT_EQ(events.size(), 2);

    EXPECT_EQ(counter.GetInside(), 2);
    EXPECT_EQ(counter.GetOutside(), 0);

    EXPECT_EQ(
        counter.GetInside() + counter.GetOutside(),
        counter.GetTotal()
    );
}

TEST(BirdMovementProcessorTests, DoesNotCountWithInvalidCounter)
{
    BirdTracker tracker;
    BirdCounter counter(-1, 0);

    BirdMovementProcessor processor(
        tracker,
        counter,
        PROCESSOR_TEST_LINE,
        PROCESSOR_INSIDE_SIDE
    );

    processor.Update({
        {40.0f, 50.0f}
    });

    auto events = processor.Update({
        {60.0f, 50.0f}
    });

    EXPECT_TRUE(events.empty());
    EXPECT_FALSE(counter.IsValid());

    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 0);
}


TEST(BirdMovementProcessorTests, CountsBirdEnteringAfterStoppingOnLine)
{
    BirdCounter counter(10, 0);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица снаружи.
    processor.Update({{5, -2}});

    // Птица дошла до линии.
    auto events1 = processor.Update({{5, 0}});

    // Птица остановилась на линии.
    auto events2 = processor.Update({{5, 0}});

    // Птица продолжила движение внутрь.
    auto events3 = processor.Update({{5, 2}});

    EXPECT_TRUE(events1.empty());
    EXPECT_TRUE(events2.empty());

    ASSERT_EQ(events3.size(), 1);
    EXPECT_EQ(events3[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 1);
    EXPECT_EQ(counter.GetOutside(), 9);
}

TEST(BirdMovementProcessorTests, DoesNotCountWhenBirdTouchesLineAndReturns)
{
    BirdCounter counter(10, 0);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица снаружи.
    processor.Update({{5, -2}});

    // Дотронулась до линии.
    processor.Update({{5, 0}});

    // Вернулась обратно наружу.
    auto events = processor.Update({{5, -2}});

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), 10);
}

TEST(BirdMovementProcessorTests, CountsBirdExitingAfterStoppingOnLine)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица находится внутри.
    processor.Update({{5, 2}});

    // Птица дошла до линии.
    auto events1 = processor.Update({{5, 0}});

    // Птица остановилась на линии.
    auto events2 = processor.Update({{5, 0}});

    // Птица продолжила движение наружу.
    auto events3 = processor.Update({{5, -2}});

    EXPECT_TRUE(events1.empty());
    EXPECT_TRUE(events2.empty());

    ASSERT_EQ(events3.size(), 1);
    EXPECT_EQ(events3[0].direction, CrossingDirection::Exited);

    EXPECT_EQ(counter.GetInside(), 4);
    EXPECT_EQ(counter.GetOutside(), 6);
}

TEST(BirdMovementProcessorTests, CountsRepeatedCrossingsBySameBird)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица начинает снаружи.
    processor.Update({{5, -2}});

    // Входит.
    auto enterEvents = processor.Update({{5, 2}});

    ASSERT_EQ(enterEvents.size(), 1);
    EXPECT_EQ(enterEvents[0].direction, CrossingDirection::Entered);

    // Выходит.
    auto exitEvents = processor.Update({{5, -2}});

    ASSERT_EQ(exitEvents.size(), 1);
    EXPECT_EQ(exitEvents[0].direction, CrossingDirection::Exited);

    // Снова входит.
    auto secondEnterEvents = processor.Update({{5, 2}});

    ASSERT_EQ(secondEnterEvents.size(), 1);
    EXPECT_EQ(secondEnterEvents[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);

    EXPECT_EQ(enterEvents[0].birdId, exitEvents[0].birdId);
    EXPECT_EQ(exitEvents[0].birdId, secondEnterEvents[0].birdId);
}

TEST(BirdMovementProcessorTests, DoesNotCountCrossingBeyondLineStart)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица находится ниже линии, за её началом.
    processor.Update({{-2, -2}});

    // Пересекает продолжение линии, но не сам отрезок.
    auto events = processor.Update({{-2, 2}});

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdMovementProcessorTests, DoesNotCountCrossingBeyondLineEnd)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица находится ниже линии, за её концом.
    processor.Update({{12, -2}});

    // Пересекает продолжение линии, но не сам отрезок.
    auto events = processor.Update({{12, 2}});

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdMovementProcessorTests, CountsCrossingAtLineStartEndpoint)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица находится снаружи.
    processor.Update({{-2, -2}});

    // Пересекает линию точно в её начальной точке.
    auto events = processor.Update({{2, 2}});

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);
}

TEST(BirdMovementProcessorTests, CountsCrossingWhenAnotherBirdReturns)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Первая птица снаружи, вторая внутри.
    processor.Update({
        {5, -2},
        {8, 2}
    });

    // Первая входит, вторая касается линии.
    auto events = processor.Update({
        {5, 2},
        {8, 0}
    });

    // Первая остаётся внутри, вторая возвращается.
    auto returnEvents = processor.Update({
        {5, 3},
        {8, 2}
    });

    // Засчитывается только вход первой птицы.
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);

    EXPECT_TRUE(returnEvents.empty());

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);
}

TEST(BirdMovementProcessorTests, CountsTwoBirdsEnteringIndependently)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Обе птицы снаружи.
    processor.Update({
        {3, -2},
        {7, -2}
    });

    // Обе птицы входят.
    auto events = processor.Update({
        {3, 2},
        {7, 2}
    });

    ASSERT_EQ(events.size(), 2);

    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);
    EXPECT_EQ(events[1].direction, CrossingDirection::Entered);

    EXPECT_NE(events[0].birdId, events[1].birdId);

    EXPECT_EQ(counter.GetInside(), 7);
    EXPECT_EQ(counter.GetOutside(), 3);
}


TEST(BirdMovementProcessorTests, DoesNotCountAfterTrackExpiresAndBirdReappears)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 1);

    Line line = {{50, 0}, {50, 100}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Negative
    );

    // Птица снаружи.
    processor.Update({{40, 50}});

    // Первый пропущенный кадр.
    processor.Update({});

    // Второй пропущенный кадр: старый трек удаляется.
    processor.Update({});

    EXPECT_EQ(tracker.GetTrackCount(), 0);

    // Птица появляется уже внутри, но получает новый ID.
    auto events = processor.Update({{60, 50}});

    EXPECT_TRUE(events.empty());

    EXPECT_EQ(tracker.GetTrackCount(), 1);
    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdMovementProcessorTests, CountsCrossingAtLineEndEndpoint)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{0, 0}, {10, 0}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Positive
    );

    // Птица снаружи.
    processor.Update({{8, -2}});

    // Пересекает линию точно в конечной точке.
    auto events = processor.Update({{12, 2}});

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);
}

TEST(BirdMovementProcessorTests, DoesNotCountNewBirdWhileAnotherBirdCrosses)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{50, 0}, {50, 100}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Negative
    );

    // Первая птица снаружи.
    processor.Update({
        {40, 30}
    });

    // Первая птица входит, вторая впервые появляется внутри.
    auto events = processor.Update({
        {60, 30},
        {60, 70}
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].direction, CrossingDirection::Entered);

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);
}

TEST(BirdMovementProcessorTests, MaintainsCorrectDirectionsWhenDetectionOrderChanges)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{50, 0}, {50, 100}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Negative
    );

    // Первая птица снаружи, вторая внутри.
    processor.Update({
        {40, 30},
        {60, 70}
    });

    // Порядок обнаружений изменился.
    // Первая птица входит, вторая выходит.
    auto events = processor.Update({
        {40, 70},
        {60, 30}
    });

    ASSERT_EQ(events.size(), 2);

    int enteredCount = 0;
    int exitedCount = 0;

    for (const auto& event : events)
    {
        if (event.direction == CrossingDirection::Entered)
        {
            ++enteredCount;
        }
        else if (event.direction == CrossingDirection::Exited)
        {
            ++exitedCount;
        }
    }

    EXPECT_EQ(enteredCount, 1);
    EXPECT_EQ(exitedCount, 1);

    EXPECT_NE(events[0].birdId, events[1].birdId);

    EXPECT_EQ(counter.GetInside(), 5);
    EXPECT_EQ(counter.GetOutside(), 5);
}

TEST(BirdMovementProcessorTests, DoesNotEmitRepeatedEventsWhileBirdRemainsInside)
{
    BirdCounter counter(10, 5);
    BirdTracker tracker(30.0f, 5);

    Line line = {{50, 0}, {50, 100}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Negative
    );

    // Птица снаружи.
    processor.Update({{40, 50}});

    // Птица входит.
    auto enterEvents = processor.Update({{60, 50}});

    ASSERT_EQ(enterEvents.size(), 1);
    EXPECT_EQ(enterEvents[0].direction, CrossingDirection::Entered);

    // Птица продолжает находиться внутри.
    auto events1 = processor.Update({{65, 50}});
    auto events2 = processor.Update({{70, 50}});
    auto events3 = processor.Update({{75, 50}});

    EXPECT_TRUE(events1.empty());
    EXPECT_TRUE(events2.empty());
    EXPECT_TRUE(events3.empty());

    EXPECT_EQ(counter.GetInside(), 6);
    EXPECT_EQ(counter.GetOutside(), 4);
}


TEST(BirdMovementProcessorTests, SimulatesOneHundredFramesWithMultipleBirds)
{
    constexpr int TOTAL_BIRDS = 3;
    constexpr int FRAME_COUNT = 100;

    BirdCounter counter(TOTAL_BIRDS, 0);
    BirdTracker tracker(30.0f, 2);

    Line line = {{50, 0}, {50, 120}};

    BirdMovementProcessor processor(
        tracker,
        counter,
        line,
        Side::Negative
    );

    const float birdY[] = {20.0f, 60.0f, 100.0f};

    // Начальное положение: все птицы снаружи.
    auto initialEvents = processor.Update({
        {40.0f, birdY[0]},
        {40.0f, birdY[1]},
        {40.0f, birdY[2]}
    });

    EXPECT_TRUE(initialEvents.empty());
    EXPECT_EQ(tracker.GetTrackCount(), TOTAL_BIRDS);

    int totalEntered = 0;
    int totalExited = 0;
    int expectedInside = 0;

    for (int frame = 1; frame <= FRAME_COUNT; ++frame)
    {
        std::vector<Point> detections;

        // Вторая птица пропускает 50-й кадр.
        const bool secondBirdMissing = (frame == 50);

        // Меняем порядок обнаружений каждый кадр.
        for (int i = TOTAL_BIRDS - 1; i >= 0; --i)
        {
            if (i == 1 && secondBirdMissing)
            {
                continue;
            }

            // Нечётные кадры: внутри.
            // Чётные кадры: снаружи.
            const float x = (frame % 2 == 1) ? 60.0f : 40.0f;

            detections.push_back({
                x,
                birdY[i]
            });
        }

        auto events = processor.Update(detections);

        // На 50-м кадре одна птица отсутствует.
        // На 51-м она восстанавливается без учёта
        // перемещения за время отсутствия.
        int expectedEvents = TOTAL_BIRDS;

        if (frame == 50 || frame == 51)
        {
            expectedEvents = TOTAL_BIRDS - 1;
        }

        ASSERT_EQ(events.size(), expectedEvents)
            << "Unexpected event count at frame " << frame;

        const CrossingDirection expectedDirection =
            (frame % 2 == 1)
                ? CrossingDirection::Entered
                : CrossingDirection::Exited;

        for (const auto& event : events)
        {
            EXPECT_GT(event.birdId, 0);

            EXPECT_EQ(event.direction, expectedDirection)
                << "Unexpected direction at frame " << frame;

            if (event.direction == CrossingDirection::Entered)
            {
                ++totalEntered;
                ++expectedInside;
            }
            else if (event.direction == CrossingDirection::Exited)
            {
                ++totalExited;
                --expectedInside;
            }
        }

        // Проверяем счётчик после каждого кадра.
        EXPECT_EQ(counter.GetInside(), expectedInside)
            << "Incorrect inside count at frame " << frame;

        EXPECT_EQ(
            counter.GetOutside(),
            TOTAL_BIRDS - expectedInside
        ) << "Incorrect outside count at frame " << frame;

        EXPECT_EQ(
            counter.GetInside() + counter.GetOutside(),
            counter.GetTotal()
        ) << "Total count invariant violated at frame " << frame;

        EXPECT_EQ(tracker.GetTrackCount(), TOTAL_BIRDS)
            << "Unexpected track count at frame " << frame;
    }

    EXPECT_EQ(totalEntered, 149);
    EXPECT_EQ(totalExited, 149);

    EXPECT_EQ(counter.GetInside(), 0);
    EXPECT_EQ(counter.GetOutside(), TOTAL_BIRDS);
}

