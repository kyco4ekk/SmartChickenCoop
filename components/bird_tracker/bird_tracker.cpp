
#include "bird_tracker.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace
{
    double DistanceSquared(Point a, Point b)
    {
        double dx = static_cast<double>(a.x) - b.x;
        double dy = static_cast<double>(a.y) - b.y;

        return dx * dx + dy * dy;
    }

    // Венгерский алгоритм.
    // Возвращает индекс назначенного столбца для каждой строки.
    std::vector<int> SolveAssignment(
        const std::vector<std::vector<double>>& costs)
    {
        int n = static_cast<int>(costs.size());

        if (n == 0)
        {
            return {};
        }

        std::vector<double> u(n + 1, 0.0);
        std::vector<double> v(n + 1, 0.0);

        std::vector<int> p(n + 1, 0);
        std::vector<int> way(n + 1, 0);

        const double INF = std::numeric_limits<double>::infinity();

        for (int i = 1; i <= n; i++)
        {
            p[0] = i;

            int j0 = 0;

            std::vector<double> minv(n + 1, INF);
            std::vector<bool> used(n + 1, false);

            do
            {
                used[j0] = true;

                int i0 = p[j0];
                double delta = INF;
                int j1 = 0;

                for (int j = 1; j <= n; j++)
                {
                    if (used[j])
                    {
                        continue;
                    }

                    double current =
                        costs[i0 - 1][j - 1] - u[i0] - v[j];

                    if (current < minv[j])
                    {
                        minv[j] = current;
                        way[j] = j0;
                    }

                    if (minv[j] < delta)
                    {
                        delta = minv[j];
                        j1 = j;
                    }
                }

                for (int j = 0; j <= n; j++)
                {
                    if (used[j])
                    {
                        u[p[j]] += delta;
                        v[j] -= delta;
                    }
                    else
                    {
                        minv[j] -= delta;
                    }
                }

                j0 = j1;

            } while (p[j0] != 0);

            do
            {
                int j1 = way[j0];

                p[j0] = p[j1];
                j0 = j1;

            } while (j0 != 0);
        }

        std::vector<int> assignment(n, -1);

        for (int j = 1; j <= n; j++)
        {
            if (p[j] != 0)
            {
                assignment[p[j] - 1] = j - 1;
            }
        }

        return assignment;
    }
}

BirdTracker::BirdTracker(
    float maxDistance,
    int maxMissedFrames
)
    : nextId(1),
      maxDistance(
          std::isfinite(maxDistance) && maxDistance >= 0.0f
              ? maxDistance
              : 0.0f
      ),
      maxMissedFrames(
          maxMissedFrames < 0
              ? 0
              : maxMissedFrames
      )
{
}

std::vector<TrackUpdate> BirdTracker::Update(
    const std::vector<Point>& detections)
{
    std::vector<Point> validDetections;

    for (const Point& point : detections)
    {
        if (std::isfinite(point.x) && std::isfinite(point.y))
        {
            validDetections.push_back(point);
        }
    }

    const int trackCount = static_cast<int>(tracks.size());
    const int detectionCount =
        static_cast<int>(validDetections.size());

    std::vector<TrackUpdate> updates(detectionCount);

    std::vector<bool> matchedTracks(trackCount, false);
    std::vector<bool> matchedDetections(detectionCount, false);

    // Строим матрицу стоимости.
    //
    // Строки:
    //   старые треки + фиктивные строки для новых обнаружений.
    //
    // Столбцы:
    //   новые обнаружения + фиктивные столбцы
    //   для отсутствующих треков.

    const int matrixSize = trackCount + detectionCount;

    if (matrixSize > 0)
    {
        const double maxDistanceSquared =
            static_cast<double>(maxDistance) * maxDistance;

        const int maxMatches =
            std::min(trackCount, detectionCount);

        // Стоимость пропуска должна быть больше
        // возможной суммарной стоимости реальных совпадений.
        const double unmatchedCost =
            (maxMatches + 1.0) * (maxDistanceSquared + 1.0);

        const double invalidCost =
            unmatchedCost * (matrixSize + 1.0);

        std::vector<std::vector<double>> costs(
            matrixSize,
            std::vector<double>(matrixSize, 0.0)
        );

        // Старые треки -> новые обнаружения.
        for (int i = 0; i < trackCount; i++)
        {
            for (int j = 0; j < detectionCount; j++)
            {
                double distance = DistanceSquared(
                    tracks[i].position,
                    validDetections[j]
                );

                costs[i][j] =
                    distance <= maxDistanceSquared
                        ? distance
                        : invalidCost;
            }
        }

        // Старые треки -> фиктивные обнаружения.
        // Стоимость означает, что трек не был обнаружен.
        for (int i = 0; i < trackCount; i++)
        {
            for (int j = detectionCount;
                 j < matrixSize;
                 j++)
            {
                costs[i][j] = unmatchedCost;
            }
        }

        // Фиктивные треки -> новые обнаружения.
        // Стоимость означает создание нового трека.
        for (int i = trackCount; i < matrixSize; i++)
        {
            for (int j = 0; j < detectionCount; j++)
            {
                costs[i][j] = unmatchedCost;
            }
        }

        // Фиктивные треки -> фиктивные обнаружения.
        // Стоимость остаётся равной нулю.

        std::vector<int> assignment = SolveAssignment(costs);

        // Применяем только реальные совпадения.
        for (int i = 0; i < trackCount; i++)
        {
            int detectionIndex = assignment[i];

            if (detectionIndex < 0 ||
                detectionIndex >= detectionCount)
            {
                continue;
            }

            Track& track = tracks[i];

            double distance = DistanceSquared(
                track.position,
                validDetections[detectionIndex]
            );

            if (distance > maxDistanceSquared)
            {
                continue;
            }

            updates[detectionIndex] = {
                track.id,
                track.position,
                validDetections[detectionIndex],
                false,
                track.missedFrames == 0
            };

            track.position = validDetections[detectionIndex];
            track.missedFrames = 0;

            matchedTracks[i] = true;
            matchedDetections[detectionIndex] = true;
        }
    }

    // Учитываем кадры, в которых старые птицы не обнаружились.
    for (int i = 0; i < trackCount; i++)
    {
        if (!matchedTracks[i])
        {
            tracks[i].missedFrames++;
        }
    }

    // Удаляем слишком долго отсутствующие треки.
    tracks.erase(
        std::remove_if(
            tracks.begin(),
            tracks.end(),
            [this](const Track& track)
            {
                return track.missedFrames > maxMissedFrames;
            }
        ),
        tracks.end()
    );

    // Создаём новые треки для необработанных обнаружений.
    for (int i = 0; i < detectionCount; i++)
    {
        if (matchedDetections[i])
        {
            continue;
        }

        int id = nextId++;

        tracks.push_back({
            id,
            validDetections[i],
            0
        });

        updates[i] = {
            id,
            validDetections[i],
            validDetections[i],
            true,
            false
        };
    }

    return updates;
}

int BirdTracker::GetTrackCount() const
{
    return static_cast<int>(tracks.size());
}

std::vector<int> BirdTracker::GetActiveTrackIds() const
{
    std::vector<int> ids;
    ids.reserve(tracks.size());

    for (const Track& track : tracks)
    {
        ids.push_back(track.id);
    }

    return ids;
}
