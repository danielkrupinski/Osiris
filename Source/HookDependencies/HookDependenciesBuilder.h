#pragma once

#include <FeatureHelpers/ConVarFinder.h>
#include <GameDependencies/GameDependencies.h>

#include "HookDependenciesMask.h"

struct HookDependenciesBuilder {
    [[nodiscard]] HookDependenciesMask getSoundChannels(cs2::SoundChannels** soundChannels) const noexcept
    {
        if (requiredDependencies.has<SoundChannels>() && gameDependencies.soundChannels) {
            if ((*soundChannels = *gameDependencies.soundChannels) != nullptr)
                return HookDependenciesMask{}.set<SoundChannels>();
        }
        return {};
    }

    HookDependenciesMask requiredDependencies;
    GameDependencies& gameDependencies;
};
