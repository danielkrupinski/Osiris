#pragma once

#include <algorithm>
#include <limits>
#include <span>

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/Vector.h>
#include <CS2/Constants/EntityHandle.h>

struct FootstepSource {
    cs2::Vector origin;
    cs2::CEntityHandle handle;
    bool enemy;
};

// Sound channels do not expose an emitter in the currently supported layout.
// Match only at sound collection time, include allies in the search, and reject
// ambiguous matches rather than attributing an ally's footstep to an enemy.
[[nodiscard]] inline cs2::CEntityHandle matchEnemyFootstepSource(
    const cs2::Vector& origin, std::span<const FootstepSource> sources) noexcept
{
    constexpr cs2::CEntityHandle invalid{cs2::INVALID_EHANDLE_INDEX};
    const FootstepSource* closest = nullptr;
    float best = 48.0f * 48.0f;
    float second = (std::numeric_limits<float>::max)();
    for (const auto& source : sources) {
        const auto delta = origin - source.origin;
        if (delta.z < -48.0f || delta.z > 48.0f)
            continue;
        const auto distance = delta.x * delta.x + delta.y * delta.y;
        if (distance < best) {
            if (closest)
                second = best;
            best = distance;
            closest = &source;
        } else {
            second = (std::min)(second, distance);
        }
    }
    if (!closest || !closest->enemy || second <= best * 1.5f + 64.0f)
        return invalid;
    return closest->handle;
}
