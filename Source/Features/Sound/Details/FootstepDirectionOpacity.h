#pragma once

#include <algorithm>

#include "EnemyFootstepSound.h"
#include "PlayedSound.h"

[[nodiscard]] inline float footstepDirectionOpacity(const PlayedSound& sound, float curtime) noexcept
{
    const auto age = sound.getTimeAlive(curtime);
    if (age > EnemyFootstepSound::kFadeAwayStart + EnemyFootstepSound::kFadeAwayDuration)
        return 0.0f;
    const auto firstAge = curtime - sound.firstSpawnTime;
    if (firstAge < 0.0f)
        return 0.0f;
    const auto smooth = [](float x) { return x * x * (3.0f - 2.0f * x); };
    const auto fadeIn = smooth(std::clamp(firstAge / EnemyFootstepSound::kFadeInDuration, 0.0f, 1.0f));
    const auto fadeOut = 1.0f - smooth(std::clamp((age - EnemyFootstepSound::kFadeAwayStart)
        / EnemyFootstepSound::kFadeAwayDuration, 0.0f, 1.0f));
    return fadeIn * fadeOut;
}
