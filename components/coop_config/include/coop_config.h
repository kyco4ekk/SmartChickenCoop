#pragma once

#include "direction_detector.h"

struct CoopConfig
{
    int totalBirds = 0;
    int initialInsideBirds = 0;

    Line line{};

    Side insideSide = Side::Positive;

    bool IsValid() const;
};