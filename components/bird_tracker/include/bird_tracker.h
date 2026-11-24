
#pragma once

#include <vector>
#include "direction_detector.h"

struct TrackUpdate
{
    int id;
    Point previousPosition;
    Point currentPosition;
    bool isNew;
    bool isContinuous;
};

class BirdTracker
{
public:
    BirdTracker(float maxDistance = 30.0f,
                int maxMissedFrames = 5);

    std::vector<TrackUpdate> Update(
        const std::vector<Point>& detections
    );

    int GetTrackCount() const;

    std::vector<int> GetActiveTrackIds() const;

private:
    struct Track
    {
        int id;
        Point position;
        int missedFrames;
    };

    std::vector<Track> tracks;

    int nextId;
    float maxDistance;
    int maxMissedFrames;
};
