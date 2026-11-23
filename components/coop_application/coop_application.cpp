
#include "coop_application.h"

CoopApplication::CoopApplication(const CoopConfig& config)
    : config(config),
      counter(
          config.totalBirds,
          config.initialInsideBirds
      ),
      tracker(),
      movementProcessor(
          tracker,
          counter,
          config.line,
          config.insideSide
      )
{
}

bool CoopApplication::IsValid() const
{
    return config.IsValid() && counter.IsValid();
}

std::vector<BirdMovementEvent> CoopApplication::Update(
    const std::vector<Point>& detections
)
{
    if (!IsValid())
    {
        return {};
    }

    return movementProcessor.Update(detections);
}

const CoopConfig& CoopApplication::GetConfig() const
{
    return config;
}

const BirdCounter& CoopApplication::GetCounter() const
{
    return counter;
}
