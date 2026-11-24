
#include "bird_counter.h"

BirdCounter::BirdCounter(int totalCount, int initialInside)
    : total(0), inside(0), outside(0), valid(false)
{
    if (totalCount < 0 || initialInside < 0 || initialInside > totalCount)
    {
        return;
    }

    total = totalCount;
    inside = initialInside;
    outside = totalCount - initialInside;
    valid = true;
}

bool BirdCounter::IsValid() const
{
    return valid;
}

int BirdCounter::GetTotal() const
{
    return total;
}

int BirdCounter::GetInside() const
{
    return inside;
}

int BirdCounter::GetOutside() const
{
    return outside;
}

void BirdCounter::BirdEntered()
{
    if (valid && outside > 0)
    {
        inside++;
        outside--;
    }
}

void BirdCounter::BirdExited()
{
    if (valid && inside > 0)
    {
        inside--;
        outside++;
    }
}

void BirdCounter::SetInside(int count)
{
    if (valid && count >= 0 && count <= total)
    {
        inside = count;
        outside = total - count;
    }
}

void BirdCounter::SetOutside(int count)
{
    if (valid && count >= 0 && count <= total)
    {
        outside = count;
        inside = total - count;
    }
}
