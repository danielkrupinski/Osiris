#pragma once

#include <cstdint>

#include <CS2/Classes/Vector.h>

struct GrenadeLaunchSnapshot {
    [[nodiscard]] static constexpr bool isFresh(bool stashed, bool hasCurtime, float current, float stashTime) noexcept
    {
        const float delta = current - stashTime;
        return stashed && hasCurtime && delta > 0.0f && delta <= 0.2f;
    }

    [[nodiscard]] static constexpr bool shouldCaptureForLifecycle(bool hasTopEvent, const void* eventWeapon, std::uint32_t eventSequence,
        const void* activeWeapon, std::uint32_t activeSequence, bool hasPendingThrowTime, bool legacyCycle) noexcept
    {
        if (hasTopEvent)
            return eventWeapon == activeWeapon && eventSequence == activeSequence;
        return hasPendingThrowTime || legacyCycle;
    }

    void capture(float newStashTime, cs2::Vector newShootAngles, cs2::Vector newThrowPosition, cs2::Vector newVelocity, const void* newWeapon, std::uint32_t newSequence) noexcept
    {
        stashTime = newStashTime;
        shootAngles = newShootAngles;
        throwPosition = newThrowPosition;
        velocity = newVelocity;
        weapon = newWeapon;
        sequence = newSequence;
        valid = true;
    }

    [[nodiscard]] bool isOwnedBy(const void* expectedWeapon, std::uint32_t expectedSequence) const noexcept
    {
        return valid && weapon == expectedWeapon && sequence == expectedSequence;
    }

    void invalidate() noexcept
    {
        valid = false;
        weapon = nullptr;
        sequence = 0;
    }

    float stashTime{};
    cs2::Vector shootAngles{};
    cs2::Vector throwPosition{};
    cs2::Vector velocity{};
    const void* weapon{};
    std::uint32_t sequence{};
    bool valid{};
};
