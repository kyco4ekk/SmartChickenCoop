
#include "direction_detector.h"
#include <cmath>

namespace
{
    constexpr float EPSILON = 0.00001f;

    float Cross(Point a, Point b)
    {
        return a.x * b.y - a.y * b.x;
    }

    Point Subtract(Point a, Point b)
    {
        return {a.x - b.x, a.y - b.y};
    }
}

CrossingDirection DirectionDetector::Detect(
    Point previous,
    Point current,
    Line line,
    Side insideSide)
{
    Point movement = Subtract(current, previous);
    Point lineDirection = Subtract(line.end, line.start);

    float denominator = Cross(movement, lineDirection);

    // Параллельное движение или вырожденная линия.
    if (std::fabs(denominator) < EPSILON)
    {
        return CrossingDirection::None;
    }

    Point offset = Subtract(line.start, previous);

    float t = Cross(offset, lineDirection) / denominator;
    float u = Cross(offset, movement) / denominator;

    // Проверяем пересечение именно отрезков.
    if (t < 0.0f || t > 1.0f ||
        u < 0.0f || u > 1.0f)
    {
        return CrossingDirection::None;
    }

    float previousSide = Cross(
        lineDirection,
        Subtract(previous, line.start)
    );

    float currentSide = Cross(
        lineDirection,
        Subtract(current, line.start)
    );

    // Птица должна оказаться по другую сторону линии.
    if (previousSide * currentSide >= 0.0f)
    {
        return CrossingDirection::None;
    }

    bool isInsidePositive =
        insideSide == Side::Positive;

    bool currentIsPositive = currentSide > 0.0f;

    if (currentIsPositive == isInsidePositive)
    {
        return CrossingDirection::Entered;
    }

    return CrossingDirection::Exited;
}

int DirectionDetector::GetSignedSide(Point point, Line line)
{
    Point lineDirection = Subtract(line.end, line.start);
    Point offset = Subtract(point, line.start);

    float cross = Cross(lineDirection, offset);

    if (std::fabs(cross) < EPSILON)
    {
        return 0;
    }

    return cross > 0.0f ? 1 : -1;
}
