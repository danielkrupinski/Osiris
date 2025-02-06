#pragma once

template <typename HookContext>
struct KillfeedPreserverContext {
    KillfeedPreserverContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) deathNotices() const noexcept
    {
        return hookContext.hud().deathNotices();
    }

    [[nodiscard]] auto preserveDeathNotice() const noexcept
    {
        return [](auto&& deathNotice)
        {
            if (deathNotice.isLocalPlayerKiller() && deathNotice.wasSpawnedThisRound().valueOr(false))
                deathNotice.markAsJustSpawned();
        };
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().hudFeaturesStates.killfeedPreserverState;
    }
    
private:
    HookContext& hookContext;
};
