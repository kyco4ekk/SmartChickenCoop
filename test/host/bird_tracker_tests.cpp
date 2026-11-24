#include <limits>
#include <gtest/gtest.h>
#include "bird_tracker.h"

TEST(BirdTrackerTests, CreatesNewTracks)
{
    BirdTracker tracker;

    auto updates = tracker.Update({
        {20.0f, 40.0f},
        {80.0f, 40.0f}
    });

    ASSERT_EQ(updates.size(), 2);

    EXPECT_TRUE(updates[0].isNew);
    EXPECT_TRUE(updates[1].isNew);

    EXPECT_NE(updates[0].id, updates[1].id);
    EXPECT_EQ(tracker.GetTrackCount(), 2);
}

TEST(BirdTrackerTests, KeepsIdsWhenBirdsMove)
{
    BirdTracker tracker;

    auto first = tracker.Update({
        {20.0f, 40.0f},
        {80.0f, 40.0f}
    });

    auto second = tracker.Update({
        {25.0f, 42.0f},
        {75.0f, 43.0f}
    });

    EXPECT_EQ(first[0].id, second[0].id);
    EXPECT_EQ(first[1].id, second[1].id);

    EXPECT_FALSE(second[0].isNew);
    EXPECT_TRUE(second[0].isContinuous);
}

TEST(BirdTrackerTests, CreatesNewTrackForUnmatchedDetection)
{
    BirdTracker tracker;

    tracker.Update({
        {20.0f, 40.0f}
    });

    auto updates = tracker.Update({
        {22.0f, 40.0f},
        {150.0f, 150.0f}
    });

    ASSERT_EQ(updates.size(), 2);

    EXPECT_FALSE(updates[0].isNew);
    EXPECT_TRUE(updates[1].isNew);

    EXPECT_EQ(tracker.GetTrackCount(), 2);
}

TEST(BirdTrackerTests, KeepsTrackWhenBirdTemporarilyDisappears)
{
    BirdTracker tracker(30.0f, 2);

    auto first = tracker.Update({
        {20.0f, 40.0f}
    });

    tracker.Update({});
    
    auto updates = tracker.Update({
        {25.0f, 40.0f}
    });

    ASSERT_EQ(updates.size(), 1);

    EXPECT_EQ(first[0].id, updates[0].id);
    EXPECT_FALSE(updates[0].isNew);
    EXPECT_FALSE(updates[0].isContinuous);

    EXPECT_EQ(tracker.GetTrackCount(), 1);
}

TEST(BirdTrackerTests, RemovesTrackAfterTooManyMissedFrames)
{
    BirdTracker tracker(30.0f, 2);

    tracker.Update({
        {20.0f, 40.0f}
    });

    tracker.Update({});
    tracker.Update({});

    EXPECT_EQ(tracker.GetTrackCount(), 1);

    tracker.Update({});

    EXPECT_EQ(tracker.GetTrackCount(), 0);
}


TEST(BirdTrackerTests, KeepsIdsWhenDetectionOrderChanges)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}, {100, 0}});
    auto updates = tracker.Update({{101, 0}, {1, 0}});

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 2);
    EXPECT_EQ(updates[1].id, 1);
}

TEST(BirdTrackerTests, KeepsIdsWhenBirdsCrossTrajectories)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}, {20, 0}});

    auto updates = tracker.Update({{8, 0}, {12, 0}});

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_EQ(updates[1].id, 2);

    updates = tracker.Update({{16, 0}, {4, 0}});

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 2);
    EXPECT_EQ(updates[1].id, 1);
}

TEST(BirdTrackerTests, KeepsIdsWhenBirdsMoveCloseTogether)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}, {20, 0}});

    auto updates = tracker.Update({{9, 0}, {11, 0}});

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_EQ(updates[1].id, 2);
}

TEST(BirdTrackerTests, KeepsIdsDuringSmallMovements)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{10, 10}, {50, 50}});

    auto updates = tracker.Update({{11, 11}, {49, 49}});

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_EQ(updates[1].id, 2);
}

TEST(BirdTrackerTests, CreatesMultipleNewTracks)
{
    BirdTracker tracker(30.0f, 5);

    auto updates = tracker.Update({
        {0, 0},
        {100, 0},
        {200, 0}
    });

    ASSERT_EQ(updates.size(), 3);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_EQ(updates[1].id, 2);
    EXPECT_EQ(updates[2].id, 3);

    EXPECT_TRUE(updates[0].isNew);
    EXPECT_TRUE(updates[1].isNew);
    EXPECT_TRUE(updates[2].isNew);
}

TEST(BirdTrackerTests, RestoresMultipleTemporarilyMissingBirds)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}, {100, 0}});

    tracker.Update({});
    tracker.Update({});

    auto updates = tracker.Update({
        {102, 0},
        {2, 0}
    });

    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].id, 2);
    EXPECT_EQ(updates[1].id, 1);

    EXPECT_FALSE(updates[0].isNew);
    EXPECT_FALSE(updates[1].isNew);

    EXPECT_FALSE(updates[0].isContinuous);
    EXPECT_FALSE(updates[1].isContinuous);
}

TEST(BirdTrackerTests, RemovesOnlyTracksThatExceedMissedFrameLimit)
{
    BirdTracker tracker(30.0f, 2);

    tracker.Update({{0, 0}, {100, 0}});

    tracker.Update({{100, 0}});
    tracker.Update({{100, 0}});
    tracker.Update({{100, 0}});

    auto ids = tracker.GetActiveTrackIds();

    ASSERT_EQ(ids.size(), 1);
    EXPECT_EQ(ids[0], 2);
}

TEST(BirdTrackerTests, HandlesEmptyDetectionFrames)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}, {100, 0}});

    auto updates = tracker.Update({});

    EXPECT_TRUE(updates.empty());
    EXPECT_EQ(tracker.GetTrackCount(), 2);
}

TEST(BirdTrackerTests, HandlesDuplicateDetections)
{
    BirdTracker tracker(30.0f, 5);

    auto updates = tracker.Update({
        {10, 10},
        {10, 10}
    });

    ASSERT_EQ(updates.size(), 2);

    EXPECT_NE(updates[0].id, updates[1].id);
    EXPECT_EQ(tracker.GetTrackCount(), 2);
}

TEST(BirdTrackerTests, ProducesDeterministicAssignments)
{
    BirdTracker first(30.0f, 5);
    BirdTracker second(30.0f, 5);

    std::vector<Point> initial = {
        {0, 0},
        {20, 0}
    };

    std::vector<Point> next = {
        {8, 0},
        {12, 0}
    };

    first.Update(initial);
    second.Update(initial);

    auto firstUpdates = first.Update(next);
    auto secondUpdates = second.Update(next);

    ASSERT_EQ(firstUpdates.size(), secondUpdates.size());

    for (size_t i = 0; i < firstUpdates.size(); ++i)
    {
        EXPECT_EQ(firstUpdates[i].id, secondUpdates[i].id);
    }
}


TEST(BirdTrackerTests, IgnoresNaNDetection)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{10, 10}});

    float nan = std::numeric_limits<float>::quiet_NaN();

    auto updates = tracker.Update({
        {nan, 10}
    });

    EXPECT_TRUE(updates.empty());
    EXPECT_EQ(tracker.GetTrackCount(), 1);

    updates = tracker.Update({
        {11, 10}
    });

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].id, 1);
}

TEST(BirdTrackerTests, IgnoresInfiniteDetection)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{10, 10}});

    float infinity = std::numeric_limits<float>::infinity();

    auto updates = tracker.Update({
        {infinity, 10}
    });

    EXPECT_TRUE(updates.empty());
    EXPECT_EQ(tracker.GetTrackCount(), 1);

    updates = tracker.Update({
        {11, 10}
    });

    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].id, 1);
}

TEST(BirdTrackerTests, MatchesDetectionAtExactMaximumDistance)
{
    BirdTracker tracker(30.0f, 5);

    tracker.Update({{0, 0}});

    auto updates = tracker.Update({
        {30, 0}
    });

    ASSERT_EQ(updates.size(), 1);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_FALSE(updates[0].isNew);
    EXPECT_TRUE(updates[0].isContinuous);
}

TEST(BirdTrackerTests, HandlesLargeNumberOfBirds)
{
    BirdTracker tracker(30.0f, 5);

    std::vector<Point> initial;
    std::vector<Point> moved;

    for (int i = 0; i < 50; ++i)
    {
        initial.push_back({
            static_cast<float>(i * 100),
            0
        });

        moved.push_back({
            static_cast<float>(i * 100 + 5),
            0
        });
    }

    auto initialUpdates = tracker.Update(initial);

    ASSERT_EQ(initialUpdates.size(), 50);
    EXPECT_EQ(tracker.GetTrackCount(), 50);

    auto updates = tracker.Update(moved);

    ASSERT_EQ(updates.size(), 50);
    EXPECT_EQ(tracker.GetTrackCount(), 50);

    for (int i = 0; i < 50; ++i)
    {
        EXPECT_EQ(updates[i].id, i + 1);
        EXPECT_FALSE(updates[i].isNew);
        EXPECT_TRUE(updates[i].isContinuous);
    }
}


TEST(BirdTrackerTests, RejectsNegativeMaxDistance)
{
    BirdTracker tracker(-30.0f, 5);

    tracker.Update({{0, 0}});

    auto updates = tracker.Update({{10, 0}});

    ASSERT_EQ(updates.size(), 1);
    EXPECT_TRUE(updates[0].isNew);
    EXPECT_EQ(updates[0].id, 2);
}

TEST(BirdTrackerTests, RejectsNonFiniteMaxDistance)
{
    float nan = std::numeric_limits<float>::quiet_NaN();

    BirdTracker tracker(nan, 5);

    tracker.Update({{0, 0}});

    auto updates = tracker.Update({{10, 0}});

    ASSERT_EQ(updates.size(), 1);
    EXPECT_TRUE(updates[0].isNew);
    EXPECT_EQ(updates[0].id, 2);
}

TEST(BirdTrackerTests, RejectsNegativeMaxMissedFrames)
{
    BirdTracker tracker(30.0f, -1);

    tracker.Update({{0, 0}});
    tracker.Update({});

    EXPECT_EQ(tracker.GetTrackCount(), 0);
}

TEST(BirdTrackerTests, RejectsInfiniteMaxDistance)
{
    float infinity = std::numeric_limits<float>::infinity();

    BirdTracker tracker(infinity, 5);

    tracker.Update({{0, 0}});

    auto updates = tracker.Update({{10, 0}});

    ASSERT_EQ(updates.size(), 1);
    EXPECT_TRUE(updates[0].isNew);
    EXPECT_EQ(updates[0].id, 2);
}



TEST(BirdTrackerTests, GreedyMatchingProducesSuboptimalAssignment)
{
    BirdTracker tracker(20.0f, 5);

    tracker.Update({
        {0, 0},
        {10, 0}
    });

    auto updates = tracker.Update({
        {4, 0},
        {-5, 0}
    });

    ASSERT_EQ(updates.size(), 2);

    // Глобально оптимальное назначение:
    // ID 2 -> {4, 0}, расстояние 6.
    // ID 1 -> {-5, 0}, расстояние 5.

    EXPECT_EQ(updates[0].id, 2);
    EXPECT_EQ(updates[1].id, 1);

    EXPECT_FALSE(updates[0].isNew);
    EXPECT_FALSE(updates[1].isNew);
}


TEST(BirdTrackerTests, CreatesNewTracksWhenNoMatchesArePossible)
{
    BirdTracker tracker(5.0f, 2);

    auto first = tracker.Update({
        {0.0f, 0.0f},
        {100.0f, 0.0f}
    });

    ASSERT_EQ(first.size(), 2);

    auto second = tracker.Update({
        {50.0f, 0.0f},
        {200.0f, 0.0f}
    });

    ASSERT_EQ(second.size(), 2);

    EXPECT_EQ(second[0].id, 3);
    EXPECT_EQ(second[1].id, 4);

    EXPECT_TRUE(second[0].isNew);
    EXPECT_TRUE(second[1].isNew);
}

TEST(BirdTrackerTests, MatchesOneDetectionToClosestTrack)
{
    BirdTracker tracker(20.0f, 2);

    tracker.Update({
        {0.0f, 0.0f},
        {10.0f, 0.0f}
    });

    auto updates = tracker.Update({
        {8.0f, 0.0f}
    });

    ASSERT_EQ(updates.size(), 1);

    EXPECT_EQ(updates[0].id, 2);
    EXPECT_FALSE(updates[0].isNew);
    EXPECT_TRUE(updates[0].isContinuous);
}

TEST(BirdTrackerTests, CreatesNewTrackForExtraDetection)
{
    BirdTracker tracker(20.0f, 2);

    tracker.Update({
        {0.0f, 0.0f}
    });

    auto updates = tracker.Update({
        {2.0f, 0.0f},
        {8.0f, 0.0f}
    });

    ASSERT_EQ(updates.size(), 2);

    bool existingTrackFound = false;
    bool newTrackFound = false;

    for (const auto& update : updates)
    {
        if (update.id == 1)
        {
            existingTrackFound = true;
            EXPECT_FALSE(update.isNew);
            EXPECT_FLOAT_EQ(update.currentPosition.x, 2.0f);
        }

        if (update.id == 2)
        {
            newTrackFound = true;
            EXPECT_TRUE(update.isNew);
            EXPECT_FLOAT_EQ(update.currentPosition.x, 8.0f);
        }
    }

    EXPECT_TRUE(existingTrackFound);
    EXPECT_TRUE(newTrackFound);
}

TEST(BirdTrackerTests, MaximizesMatchingCardinality)
{
    BirdTracker tracker(10.0f, 2);

    tracker.Update({
        {0.0f, 0.0f},
        {10.0f, 0.0f}
    });

    // ID 1 может сопоставиться с обеими detections.
    // ID 2 может сопоставиться только с первой.
    //
    // Оптимальное назначение:
    // ID 1 -> -9 (расстояние 9)
    // ID 2 -> 0  (расстояние 10)
    //
    // Это лучше, чем одно совпадение ID 1 -> 0
    // с нулевой дистанцией.

    auto updates = tracker.Update({
        {0.0f, 0.0f},
        {-9.0f, 0.0f}
    });

    ASSERT_EQ(updates.size(), 2);

    bool firstTrackMatched = false;
    bool secondTrackMatched = false;

    for (const auto& update : updates)
    {
        EXPECT_FALSE(update.isNew);

        if (update.id == 1)
        {
            firstTrackMatched = true;
            EXPECT_FLOAT_EQ(update.currentPosition.x, -9.0f);
        }

        if (update.id == 2)
        {
            secondTrackMatched = true;
            EXPECT_FLOAT_EQ(update.currentPosition.x, 0.0f);
        }
    }

    EXPECT_TRUE(firstTrackMatched);
    EXPECT_TRUE(secondTrackMatched);
}

TEST(BirdTrackerTests, RestoresTrackWithinMissedFrameLimit)
{
    BirdTracker tracker(20.0f, 2);

    tracker.Update({
        {0.0f, 0.0f}
    });

    tracker.Update({});
    tracker.Update({});

    auto updates = tracker.Update({
        {3.0f, 0.0f}
    });

    ASSERT_EQ(updates.size(), 1);

    EXPECT_EQ(updates[0].id, 1);
    EXPECT_FALSE(updates[0].isNew);
    EXPECT_FALSE(updates[0].isContinuous);
    EXPECT_FLOAT_EQ(updates[0].currentPosition.x, 3.0f);
}

TEST(BirdTrackerTests, ProducesDeterministicAssignmentsForEqualDistances)
{
    BirdTracker firstTracker(20.0f, 2);
    BirdTracker secondTracker(20.0f, 2);

    const std::vector<Point> initialDetections = {
        {0.0f, 0.0f},
        {10.0f, 0.0f}
    };

    const std::vector<Point> equalDistanceDetections = {
        {5.0f, 0.0f},
        {5.0f, 0.0f}
    };

    firstTracker.Update(initialDetections);
    secondTracker.Update(initialDetections);

    auto firstUpdates = firstTracker.Update(equalDistanceDetections);
    auto secondUpdates = secondTracker.Update(equalDistanceDetections);

    ASSERT_EQ(firstUpdates.size(), 2);
    ASSERT_EQ(secondUpdates.size(), 2);

    for (size_t i = 0; i < firstUpdates.size(); ++i)
    {
        EXPECT_EQ(firstUpdates[i].id, secondUpdates[i].id);
        EXPECT_FLOAT_EQ(
            firstUpdates[i].currentPosition.x,
            secondUpdates[i].currentPosition.x
        );
        EXPECT_FLOAT_EQ(
            firstUpdates[i].currentPosition.y,
            secondUpdates[i].currentPosition.y
        );
    }
}


TEST(BirdTrackerTests, DetectionOrderDoesNotChangeTrackAssignments)
{
    BirdTracker firstTracker(20.0f, 2);
    BirdTracker secondTracker(20.0f, 2);

    const std::vector<Point> initialDetections = {
        {0.0f, 0.0f},
        {10.0f, 0.0f},
        {30.0f, 0.0f}
    };

    firstTracker.Update(initialDetections);
    secondTracker.Update(initialDetections);

    auto firstUpdates = firstTracker.Update({
        {2.0f, 0.0f},
        {12.0f, 0.0f},
        {28.0f, 0.0f}
    });

    auto secondUpdates = secondTracker.Update({
        {28.0f, 0.0f},
        {2.0f, 0.0f},
        {12.0f, 0.0f}
    });

    ASSERT_EQ(firstUpdates.size(), 3);
    ASSERT_EQ(secondUpdates.size(), 3);

    for (const auto& first : firstUpdates)
    {
        bool matchingTrackFound = false;

        for (const auto& second : secondUpdates)
        {
            if (first.id == second.id)
            {
                matchingTrackFound = true;

                EXPECT_FLOAT_EQ(
                    first.currentPosition.x,
                    second.currentPosition.x
                );

                EXPECT_FLOAT_EQ(
                    first.currentPosition.y,
                    second.currentPosition.y
                );

                break;
            }
        }

        EXPECT_TRUE(matchingTrackFound);
    }
}

TEST(BirdTrackerTests, RemovesTrackAfterMissedFrameLimitExceeded)
{
    BirdTracker tracker(20.0f, 2);

    auto first = tracker.Update({
        {0.0f, 0.0f}
    });

    ASSERT_EQ(first.size(), 1);
    EXPECT_EQ(first[0].id, 1);

    tracker.Update({});
    tracker.Update({});

    // Трек ещё существует после двух пропущенных кадров.
    EXPECT_EQ(tracker.GetTrackCount(), 1);

    // Третий пропущенный кадр превышает maxMissedFrames.
    tracker.Update({});

    EXPECT_EQ(tracker.GetTrackCount(), 0);

    // Птица появляется снова, но получает новый ID.
    auto restored = tracker.Update({
        {1.0f, 0.0f}
    });

    ASSERT_EQ(restored.size(), 1);

    EXPECT_EQ(restored[0].id, 2);
    EXPECT_TRUE(restored[0].isNew);
    EXPECT_FALSE(restored[0].isContinuous);
}

TEST(BirdTrackerTests, HandlesOneHundredTracksWithReversedDetectionOrder)
{
    constexpr int BIRD_COUNT = 100;

    BirdTracker tracker(10.0f, 2);

    std::vector<Point> initialDetections;
    initialDetections.reserve(BIRD_COUNT);

    for (int i = 0; i < BIRD_COUNT; ++i)
    {
        initialDetections.push_back({
            static_cast<float>(i * 100),
            0.0f
        });
    }

    auto initialUpdates = tracker.Update(initialDetections);

    ASSERT_EQ(initialUpdates.size(), BIRD_COUNT);
    EXPECT_EQ(tracker.GetTrackCount(), BIRD_COUNT);

    std::vector<Point> reversedDetections;
    reversedDetections.reserve(BIRD_COUNT);

    for (int i = BIRD_COUNT - 1; i >= 0; --i)
    {
        reversedDetections.push_back({
            static_cast<float>(i * 100 + 1),
            0.0f
        });
    }

    auto updates = tracker.Update(reversedDetections);

    ASSERT_EQ(updates.size(), BIRD_COUNT);
    EXPECT_EQ(tracker.GetTrackCount(), BIRD_COUNT);

    for (const auto& update : updates)
    {
        EXPECT_FALSE(update.isNew);
        EXPECT_TRUE(update.isContinuous);

        const int expectedId =
            static_cast<int>((update.currentPosition.x - 1.0f) / 100.0f) + 1;

        EXPECT_EQ(update.id, expectedId);
    }
}

