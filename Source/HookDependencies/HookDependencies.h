#pragma once

#include "HookDependenciesBuilder.h"
#include "HookDependenciesMask.h"
#include <FeatureHelpers/EntityFromHandleFinder.h>
#include <FeatureHelpers/EntityListWalker.h>
#include <GameClasses/FileSystem.h>
#include <GameClasses/PlantedC4.h>

struct HookDependencies {
    HookDependencies(const GameClassImplementations& gameClassImplementations, FeatureHelpers& featureHelpers) noexcept
        : gameClassImplementations{gameClassImplementations}
        , featureHelpers{featureHelpers}
    {
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

        if (gameClassImplementations.playerPawn.offsetToPlayerPawnImmunity)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToPlayerPawnImmunity>();

        if (featureHelpers.worldtoClipSpaceConverter)
            presentDependencies |= HookDependenciesMask{}.set<WorldToClipSpaceConverter>();

        presentDependencies |= HookDependenciesMask{}.set<HudInWorldPanelContainer>();
        presentDependencies |= HookDependenciesMask{}.set<HudProvider>();
        presentDependencies |= HookDependenciesMask{}.set<PanelConfigurator>();
        presentDependencies |= HookDependenciesMask{}.set<PanoramaTransformFactory>();

        if (gameClassImplementations.panoramaLabel.constructor && gameClassImplementations.panoramaLabel.size)
            presentDependencies |= HookDependenciesMask{}.set<PanoramaLabelFactory>();
    }

    [[nodiscard]] bool requestDependencies(HookDependenciesMask requiredDependencies) noexcept
    {
        prepareDependencies(requiredDependencies.difference(presentDependencies));
        return presentDependencies.contains(requiredDependencies);
    }

    template <typename Dependency>
    [[nodiscard]] bool requestDependency() noexcept
    {
        if (!hasDependency<Dependency>())
            prepareDependencies(HookDependenciesMask{}.set<Dependency>());
        return hasDependency<Dependency>();
    }

    template <typename Dependency>
    [[nodiscard]] decltype(auto) getDependency() noexcept
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
            return (featureHelpers.transformFactory);
        } else if constexpr (std::is_same_v<Dependency, FovScale>) {
            return fovScale;
        } else if constexpr (std::is_same_v<Dependency, CurTime>) {
            return curTime;
        } else if constexpr (std::is_same_v<Dependency, PlantedC4>) {
            return PlantedC4{*plantedC4, gameClassImplementations.plantedC4};
        } else if constexpr (std::is_same_v<Dependency, SoundChannels>) {
            return (*soundChannels);
        } else if constexpr (std::is_same_v<Dependency, FileSystem>) {
            return FileSystem{*fileSystem, gameClassImplementations.fileSystem};
        } else if constexpr (std::is_same_v<Dependency, PanoramaLabelFactory>) {
            return PanoramaLabelFactory{gameClassImplementations.panoramaLabel.constructor, gameClassImplementations.panoramaLabel.size};
        } else if constexpr (std::is_same_v<Dependency, OffsetToPlayerPawnImmunity>) {
            return gameClassImplementations.playerPawn.offsetToPlayerPawnImmunity;
        } else if constexpr (std::is_same_v<Dependency, ConVarAccessor>) {
            return ConVarAccessor{*featureHelpers.conVars, gameClassImplementations.conVar, conVarAccessorState};
        } else {
            static_assert(!std::is_same_v<Dependency, Dependency>, "Unknown dependency");
        }
    }

    [[nodiscard]] const GameClassImplementations& offsets() const noexcept
    {
        return gameClassImplementations;
    }

private:
    template <typename Dependency>
    [[nodiscard]] bool hasDependency() const noexcept
    {
        return presentDependencies.has<Dependency>();
    }

    void prepareDependencies(HookDependenciesMask requiredDependencies) noexcept
    {
        const HookDependenciesBuilder builder{requiredDependencies, gameClassImplementations, featureHelpers};

        presentDependencies |= builder.getLocalPlayerController(&localPlayerController);
        presentDependencies |= builder.getEntityList(&entityList, &highestEntityIndex);
        presentDependencies |= builder.getFovScale(&fovScale);
        presentDependencies |= builder.getCurTime(&curTime);
        presentDependencies |= builder.getPlantedC4(&plantedC4);
        presentDependencies |= builder.getSoundChannels(&soundChannels);
        presentDependencies |= builder.getFileSystem(&fileSystem);
        presentDependencies |= builder.getConVarAccessor();
    }

    const GameClassImplementations& gameClassImplementations;
    FeatureHelpers& featureHelpers;

    cs2::CCSPlayerController* localPlayerController;
    const cs2::CConcreteEntityList* entityList;
    cs2::CEntityIndex highestEntityIndex{cs2::kMaxValidEntityIndex};
    float fovScale;
    float curTime;
    cs2::CPlantedC4* plantedC4;
    cs2::SoundChannels* soundChannels;
    cs2::CBaseFileSystem* fileSystem;
    ConVarAccessorState conVarAccessorState;
    HookDependenciesMask presentDependencies;
};
