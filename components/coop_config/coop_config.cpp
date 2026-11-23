#include "coop_config.h"

#include <cmath>

bool CoopConfig::IsValid() const
{
    // Проверяем количество птиц.
    if (totalBirds < 0)
    {
        return false;
    }

    if (initialInsideBirds < 0 ||
        initialInsideBirds > totalBirds)
    {
        return false;
    }

    // Проверяем координаты начала линии.
    if (!std::isfinite(line.start.x) ||
        !std::isfinite(line.start.y))
    {
        return false;
    }

    // Проверяем координаты конца линии.
    if (!std::isfinite(line.end.x) ||
        !std::isfinite(line.end.y))
    {
        return false;
    }

    // Проверяем, что линия не вырождена.
    const double dx =
        static_cast<double>(line.end.x) - line.start.x;

    const double dy =
        static_cast<double>(line.end.y) - line.start.y;

    if (dx == 0.0 && dy == 0.0)
    {
        return false;
    }

    // Проверяем сторону курятника.
    if (insideSide != Side::Positive &&
        insideSide != Side::Negative)
    {
        return false;
    }

    return true;
}