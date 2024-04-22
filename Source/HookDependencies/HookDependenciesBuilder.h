#pragma once

#include <FeatureHelpers/ConVarFinder.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <GameClasses/Implementation/GameClassImplementations.h>

#include "HookDependenciesMask.h"

struct HookDependenciesBuilder {
    [[nodiscard]] HookDependenciesMask getConVarAccessor() const noexcept
    {
        if (requiredDependencies.has<ConVarAccessor>()) {
            if (featureHelpers.conVars.has_value())
                return HookDependenciesMask{}.set<ConVarAccessor>();

            if (gameClassImplementations.cvar.cvar && gameClassImplementations.cvar.offsetToConVarList) {
                if (const auto cvar = *gameClassImplementations.cvar.cvar) {
                    featureHelpers.conVars.emplace(ConVarFinder{*gameClassImplementations.cvar.offsetToConVarList.of(cvar).get()});
                    return HookDependenciesMask{}.set<ConVarAccessor>();
                }
            }
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getFileSystem(cs2::CBaseFileSystem** fileSystem) const noexcept
    {
        if (requiredDependencies.has<FileSystem>() && featureHelpers.fileSystem) {
            if ((*fileSystem = *featureHelpers.fileSystem) != nullptr)
                return HookDependenciesMask{}.set<FileSystem>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getSoundChannels(cs2::SoundChannels** soundChannels) const noexcept
    {
        if (requiredDependencies.has<SoundChannels>() && featureHelpers.soundChannels) {
            if ((*soundChannels = *featureHelpers.soundChannels) != nullptr)
                return HookDependenciesMask{}.set<SoundChannels>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getPlantedC4(cs2::CPlantedC4** plantedC4) const noexcept
    {
        if (requiredDependencies.has<PlantedC4>() && featureHelpers.plantedC4s && featureHelpers.plantedC4s->size > 0) {
            if ((*plantedC4 = featureHelpers.plantedC4s->memory[0]) != nullptr)
                return HookDependenciesMask{}.set<PlantedC4>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getCurTime(float* curTime) const noexcept
    {
        if (requiredDependencies.has<CurTime>() && featureHelpers.globalVars) {
            if (const auto globalVars = *featureHelpers.globalVars) {
                *curTime = globalVars->curtime;
                return HookDependenciesMask{}.set<CurTime>();
            }
        }
        return {};
    }

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
    FeatureHelpers& featureHelpers;
};
