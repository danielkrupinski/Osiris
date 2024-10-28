#pragma once

#include <FeatureHelpers/ConVarFinder.h>
#include <GameDependencies/GameDependencies.h>

#include "HookDependenciesMask.h"

template <typename HookContext>
struct HookDependenciesBuilder {
    [[nodiscard]] HookDependenciesMask getSoundChannels(cs2::SoundChannels** soundChannels) const noexcept
    {
        if (requiredDependencies.has<SoundChannels>() && hookContext.soundSystemPatternSearchResults().template get<SoundChannelsPointer>()) {
            if ((*soundChannels = *hookContext.soundSystemPatternSearchResults().template get<SoundChannelsPointer>()) != nullptr)
                return HookDependenciesMask{}.set<SoundChannels>();
        }
        return {};
    }

    HookDependenciesMask requiredDependencies;
    HookContext& hookContext;
};
