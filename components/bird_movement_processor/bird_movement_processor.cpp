
#include "bird_movement_processor.h"

BirdMovementProcessor::BirdMovementProcessor(
    BirdTracker& tracker,
    BirdCounter& counter,
    Line line,
    Side insideSide
)
    : tracker(tracker),
      counter(counter),
      line(line),
      insideSide(insideSide)
{
}

std::vector<BirdMovementEvent> BirdMovementProcessor::Update(
    const std::vector<Point>& detections
)
{
    std::vector<BirdMovementEvent> events;

    if (!counter.IsValid())
    {
        return events;
    }

    auto updates = tracker.Update(detections);

    for (const TrackUpdate& update : updates)
    {
        int currentSide = DirectionDetector::GetSignedSide(
            update.currentPosition,
            line
        );

        // Новый трек: запоминаем начальное положение.
        if (update.isNew)
        {
            birdStates[update.id] = {
                currentSide,
                update.currentPosition
            };

            continue;
        }

        // Восстановленный трек:
        // перемещение за время отсутствия не учитываем.
        if (!update.isContinuous)
        {
            birdStates[update.id] = {
                currentSide,
                update.currentPosition
            };

            continue;
        }

        auto stateIt = birdStates.find(update.id);

        if (stateIt == birdStates.end())
        {
            birdStates[update.id] = {
                currentSide,
                update.currentPosition
            };

            continue;
        }

        BirdState& state = stateIt->second;

        // Птица находится на линии.
        // Сохраняем последнюю стабильную позицию.
        if (currentSide == 0)
        {
            continue;
        }

        // Проверяем переход между сторонами.
        if (state.lastStableSide != 0 &&
            state.lastStableSide != currentSide)
        {
            CrossingDirection direction =
                DirectionDetector::Detect(
                    state.lastStablePosition,
                    update.currentPosition,
                    line,
                    insideSide
                );

            if (direction == CrossingDirection::Entered)
            {
                counter.BirdEntered();
            }
            else if (direction == CrossingDirection::Exited)
            {
                counter.BirdExited();
            }

            if (direction != CrossingDirection::None)
            {
                events.push_back({
                    update.id,
                    direction
                });
            }
        }

        // Обновляем последнюю стабильную позицию.
        state.lastStableSide = currentSide;
        state.lastStablePosition = update.currentPosition;
    }

    RemoveInactiveStates();

    return events;
}

void BirdMovementProcessor::RemoveInactiveStates()
{
    const auto activeIds = tracker.GetActiveTrackIds();

    for (auto it = birdStates.begin(); it != birdStates.end();)
    {
        bool isActive = false;

        for (int id : activeIds)
        {
            if (id == it->first)
            {
                isActive = true;
                break;
            }
        }

        if (!isActive)
        {
            it = birdStates.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
