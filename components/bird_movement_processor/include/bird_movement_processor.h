#pragma once

#include <unordered_map>
#include <vector>

#include "bird_counter.h"
#include "bird_tracker.h"
#include "direction_detector.h"

struct BirdMovementEvent
{
    int birdId;
    CrossingDirection direction;
};

class BirdMovementProcessor
{
public:
    BirdMovementProcessor(
        BirdTracker& tracker,
        BirdCounter& counter,
        Line line,
        Side insideSide
    );

    std::vector<BirdMovementEvent> Update(
        const std::vector<Point>& detections
    );

private:
    struct BirdState
    {
        int lastStableSide;
        Point lastStablePosition;
    };

    BirdTracker& tracker;
    BirdCounter& counter;

    Line line;
    Side insideSide;

    std::unordered_map<int, BirdState> birdStates;

    void RemoveInactiveStates();
};