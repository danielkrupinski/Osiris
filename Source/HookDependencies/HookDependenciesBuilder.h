#pragma once

#include <FeatureHelpers/ConVarFinder.h>
#include <GameDependencies/GameDependencies.h>

#include "HookDependenciesMask.h"

struct HookDependenciesBuilder {
    [[nodiscard]] HookDependenciesMask getFileSystem(cs2::CBaseFileSystem** fileSystem) const noexcept
    {
        if (requiredDependencies.has<FileSystem>() && gameDependencies.fileSystem) {
            if ((*fileSystem = *gameDependencies.fileSystem) != nullptr)
                return HookDependenciesMask{}.set<FileSystem>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getSoundChannels(cs2::SoundChannels** soundChannels) const noexcept
    {
        if (requiredDependencies.has<SoundChannels>() && gameDependencies.soundChannels) {
            if ((*soundChannels = *gameDependencies.soundChannels) != nullptr)
                return HookDependenciesMask{}.set<SoundChannels>();
        }
        return {};
    }

    [[nodiscard]] HookDependenciesMask getEntityList(const cs2::CConcreteEntityList** entityList, cs2::CEntityIndex* highestEntityIndex) const noexcept
    {
        if (!requiredDependencies.has<EntityListWalker>() && !requiredDependencies.has<EntityFromHandleFinder>())
            return {};

        if (!gameDependencies.entitySystemDeps.entitySystem || !gameDependencies.entitySystemDeps.entityListOffset)
            return {};

        const auto entitySystem = *gameDependencies.entitySystemDeps.entitySystem;
        if (!entitySystem)
            return {};

        if (requiredDependencies.has<EntityListWalker>() && gameDependencies.entitySystemDeps.highestEntityIndexOffset) {
            *highestEntityIndex = *gameDependencies.entitySystemDeps.highestEntityIndexOffset.of(entitySystem).get();
            if (!highestEntityIndex->isValid())
                *highestEntityIndex = cs2::kMaxValidEntityIndex;
        }

        *entityList = gameDependencies.entitySystemDeps.entityListOffset.of(entitySystem).get();
        return HookDependenciesMask{}.set<EntityListWalker>().set<EntityFromHandleFinder>();
    }

    HookDependenciesMask requiredDependencies;
    GameDependencies& gameDependencies;
};
