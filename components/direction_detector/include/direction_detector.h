
#pragma once

struct Point
{
    float x;
    float y;
};

struct Line
{
    Point start;
    Point end;
};

enum class Side
{
    Positive,
    Negative
};

enum class CrossingDirection
{
    None,
    Entered,
    Exited
};

class DirectionDetector
{
public:
    static CrossingDirection Detect(
        Point previous,
        Point current,
        Line line,
        Side insideSide
    );
    static int GetSignedSide(Point point, Line line);
};
