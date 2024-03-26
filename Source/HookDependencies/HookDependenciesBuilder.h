#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include <GameClasses/Implementation/GameClassImplementations.h>

#include "HookDependenciesMask.h"

struct HookDependenciesBuilder {
    [[nodiscard]] HookDependenciesMask getFovScale(float* fovScale) const noexcept
    {
        if (requiredDependencies.has<FovScale>()) {
            *fovScale = featureHelpers.viewToProjectionMatrix.getFovScale();
            return HookDependenciesMask{}.set<FovScale>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getLocalPlayerController(cs2::CCSPlayerController** localPlayerController) const noexcept
    {
        if (requiredDependencies.has<LocalPlayerController>() && featureHelpers.localPlayerController) {
            *localPlayerController = *featureHelpers.localPlayerController;
            if (*localPlayerController)
                return HookDependenciesMask{}.set<LocalPlayerController>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getEntityList(const cs2::CConcreteEntityList** entityList, cs2::CEntityIndex* highestEntityIndex) const noexcept
    {
        if (!requiredDependencies.has<EntityListWalker>() && !requiredDependencies.has<EntityFromHandleFinder>())
            return {};

        if (!gameClassImplementations.entitySystem.entitySystem || !gameClassImplementations.entitySystem.entityListOffset)
            return {};

        const auto entitySystem = *gameClassImplementations.entitySystem.entitySystem;
        if (!entitySystem)
            return {};

        if (requiredDependencies.has<EntityListWalker>() && gameClassImplementations.entitySystem.highestEntityIndexOffset) {
            *highestEntityIndex = *gameClassImplementations.entitySystem.highestEntityIndexOffset.of(entitySystem).get();
            if (!highestEntityIndex->isValid())
                *highestEntityIndex = cs2::kMaxValidEntityIndex;
        }

        *entityList = gameClassImplementations.entitySystem.entityListOffset.of(entitySystem).get();
        return HookDependenciesMask{}.set<EntityListWalker>().set<EntityFromHandleFinder>();
    }

    HookDependenciesMask requiredDependencies;
    const GameClassImplementations& gameClassImplementations;
    const FeatureHelpers& featureHelpers;
};
