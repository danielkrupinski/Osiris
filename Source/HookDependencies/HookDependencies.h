#pragma once

#include "HookDependenciesMask.h"
#include <FeatureHelpers/EntityFromHandleFinder.h>
#include <FeatureHelpers/EntityListWalker.h>

struct HookDependenciesBuilder {
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

        if (!gameClassImplementations.entitySystem.thisptr || !gameClassImplementations.entitySystem.entityListOffset)
            return {};

        const auto entitySystem = *gameClassImplementations.entitySystem.thisptr;
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

struct HookDependencies {
    HookDependencies(HookDependenciesBuilder builder) noexcept
        : gameClassImplementations{builder.gameClassImplementations}
        , featureHelpers{builder.featureHelpers}
    {
        presentDependencies |= builder.getLocalPlayerController(&localPlayerController);
        presentDependencies |= builder.getEntityList(&entityList, &highestEntityIndex);
        presentDependencies |= HookDependenciesMask{}.set<EntitiesVMTs>();

        if (gameClassImplementations.playerController.offsetToPlayerPawnHandle)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToPlayerPawnHandle>();

        if (gameClassImplementations.entity.offsetToGameSceneNode)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToGameSceneNode>();

        if (gameClassImplementations.entity.offsetToHealth)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToHealth>();

        if (gameClassImplementations.entity.offsetToTeamNumber)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToTeamNumber>();

        if (gameClassImplementations.gameSceneNode.offsetToAbsOrigin)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToAbsOrigin>();

        if (featureHelpers.worldtoClipSpaceConverter)
            presentDependencies |= HookDependenciesMask{}.set<WorldToClipSpaceConverter>();

        presentDependencies |= HookDependenciesMask{}.set<HudInWorldPanelContainer>();
        presentDependencies |= HookDependenciesMask{}.set<HudProvider>();
        presentDependencies |= HookDependenciesMask{}.set<PanelConfigurator>();
        presentDependencies |= HookDependenciesMask{}.set<PanoramaTransformFactory>();
    }

    [[nodiscard]] bool hasDependencies(HookDependenciesMask requiredDependencies) const noexcept
    {
        return presentDependencies.contains(requiredDependencies);
    }

    template <typename Dependency>
    [[nodiscard]] bool hasDependency() const noexcept
    {
        return presentDependencies.has<Dependency>();
    }

    template <typename Dependency>
    [[nodiscard]] decltype(auto) getDependency() const noexcept
    {
        assert(hasDependency<Dependency>());

        if constexpr (std::is_same_v<Dependency, EntityListWalker>) {
            return EntityListWalker{*entityList, highestEntityIndex};
        } else if constexpr (std::is_same_v<Dependency, EntitiesVMTs>) {
            return (featureHelpers.entitiesVMTs);
        } else if constexpr (std::is_same_v<Dependency, LocalPlayerController>) {
            return localPlayerController;
        } else if constexpr (std::is_same_v<Dependency, EntityFromHandleFinder>) {
            return EntityFromHandleFinder{*entityList};
        } else if constexpr (std::is_same_v<Dependency, OffsetToPlayerPawnHandle>) {
            return gameClassImplementations.playerController.offsetToPlayerPawnHandle;
        } else if constexpr (std::is_same_v<Dependency, OffsetToGameSceneNode>) {
            return gameClassImplementations.entity.offsetToGameSceneNode;
        } else if constexpr (std::is_same_v<Dependency, OffsetToHealth>) {
            return gameClassImplementations.entity.offsetToHealth;
        } else if constexpr (std::is_same_v<Dependency, OffsetToTeamNumber>) {
            return gameClassImplementations.entity.offsetToTeamNumber;
        } else if constexpr (std::is_same_v<Dependency, OffsetToAbsOrigin>) {
            return gameClassImplementations.gameSceneNode.offsetToAbsOrigin;
        } else if constexpr (std::is_same_v<Dependency, WorldToClipSpaceConverter>) {
            return featureHelpers.worldtoClipSpaceConverter;
        } else if constexpr (std::is_same_v<Dependency, HudInWorldPanelContainer>) {
            return (featureHelpers.hudInWorldPanelContainer);
        } else if constexpr (std::is_same_v<Dependency, HudProvider>) {
            return (featureHelpers.hudProvider);
        } else if constexpr (std::is_same_v<Dependency, PanelConfigurator>) {
            return featureHelpers.panelConfigurator();
        } else if constexpr (std::is_same_v<Dependency, PanoramaTransformFactory>) {
            return featureHelpers.transformFactory;
        } else {
            static_assert(!std::is_same_v<Dependency, Dependency>, "Unknown dependency");
        }
    }

private:
    const GameClassImplementations& gameClassImplementations;
    FeatureHelpers& featureHelpers;

    cs2::CCSPlayerController* localPlayerController;
    const cs2::CConcreteEntityList* entityList;
    cs2::CEntityIndex highestEntityIndex{cs2::kMaxValidEntityIndex};
    HookDependenciesMask presentDependencies;
};
