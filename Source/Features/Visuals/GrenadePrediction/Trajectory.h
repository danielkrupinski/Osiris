#pragma once

#include <CS2/Classes/Vector.h>

struct Trajectory {
    int pointsCount{0};
    cs2::Vector points[500];
    int bouncesCount{0};
    cs2::Vector bounces[20];
    cs2::Vector endPos;
    bool valid;
    // For Molotov/Incendiary: true only if the grenade hit a flat surface (impactDetonate).
    // False means it expired mid-air — fire won't reach ground, suppress the orange marker.
    bool validLanding{true};
};
