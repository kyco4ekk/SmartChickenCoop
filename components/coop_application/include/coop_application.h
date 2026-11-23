
#pragma once

#include <vector>

#include "coop_config.h"
#include "bird_counter.h"
#include "bird_tracker.h"
#include "bird_movement_processor.h"

class CoopApplication
{
public:
    explicit CoopApplication(const CoopConfig& config);

    bool IsValid() const;

    std::vector<BirdMovementEvent> Update(
        const std::vector<Point>& detections
    );

    const CoopConfig& GetConfig() const;

    const BirdCounter& GetCounter() const;

private:
    CoopConfig config;

    BirdCounter counter;
    BirdTracker tracker;
    BirdMovementProcessor movementProcessor;
};
