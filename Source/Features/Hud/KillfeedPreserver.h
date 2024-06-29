#pragma once

#include <FeatureHelpers/FeatureToggle.h>

#include "States/KillfeedPreserverState.h"

struct KillfeedPreserveToggle : FeatureToggle<KillfeedPreserveToggle> {
    explicit KillfeedPreserveToggle(KillfeedPreserverState& state) noexcept
        : state{state}
    {
    }

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    KillfeedPreserverState& state;
};

class KillfeedPreserver {
public:
    KillfeedPreserver(KillfeedPreserverState& state, HookDependencies& dependencies) noexcept
        : state{state}
        , dependencies{dependencies}
    {
    }

    void run() noexcept
    {
        if (!state.enabled)
            return;

        dependencies.hud().deathNotices().forEach([](const auto& deathNotice) {
            if (deathNotice.isLocalPlayerKiller() && deathNotice.wasSpawnedThisRound())
                deathNotice.markAsJustSpawned();
        });
    }

private:
    KillfeedPreserverState& state;
    HookDependencies& dependencies;
};
