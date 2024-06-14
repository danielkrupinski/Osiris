#pragma once

#include "HookDependenciesBuilder.h"
#include "HookDependenciesMask.h"
#include <FeatureHelpers/EntityFromHandleFinder.h>
#include <FeatureHelpers/EntityListWalker.h>
#include <GameClasses/FileSystem.h>
#include <GameClasses/PlantedC4.h>
#include <GameClasses/GlobalVars/GlobalVarsOptional.h>

struct HookDependencies {
    HookDependencies(GameDependencies& gameDependencies, FeatureHelpers& featureHelpers) noexcept
        : _gameDependencies{gameDependencies}
        , featureHelpers{featureHelpers}
    {
        presentDependencies |= HookDependenciesMask{}.set<EntitiesVMTs>();

        if (gameDependencies.playerControllerDeps.offsetToPlayerPawnHandle)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToPlayerPawnHandle>();

        if (gameDependencies.entityDeps.offsetToGameSceneNode)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToGameSceneNode>();

        if (gameDependencies.entityDeps.offsetToHealth)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToHealth>();

        if (gameDependencies.entityDeps.offsetToTeamNumber)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToTeamNumber>();

        if (gameDependencies.gameSceneNodeDeps.offsetToAbsOrigin)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToAbsOrigin>();

        if (gameDependencies.playerPawnDeps.offsetToPlayerPawnImmunity)
            presentDependencies |= HookDependenciesMask{}.set<OffsetToPlayerPawnImmunity>();

        if (gameDependencies.worldToProjectionMatrix)
            presentDependencies |= HookDependenciesMask{}.set<WorldToClipSpaceConverter>();

        presentDependencies |= HookDependenciesMask{}.set<HudInWorldPanelContainer>();
        presentDependencies |= HookDependenciesMask{}.set<PanelConfigurator>();
        presentDependencies |= HookDependenciesMask{}.set<PanoramaTransformFactory>();

        if (gameDependencies.panoramaLabelDeps.constructor && gameDependencies.panoramaLabelDeps.size)
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
            return (_gameDependencies.entitiesVMTs);
        } else if constexpr (std::is_same_v<Dependency, LocalPlayerController>) {
            return localPlayerController;
        } else if constexpr (std::is_same_v<Dependency, EntityFromHandleFinder>) {
            return EntityFromHandleFinder{*entityList};
        } else if constexpr (std::is_same_v<Dependency, OffsetToPlayerPawnHandle>) {
            return _gameDependencies.playerControllerDeps.offsetToPlayerPawnHandle;
        } else if constexpr (std::is_same_v<Dependency, OffsetToGameSceneNode>) {
            return _gameDependencies.entityDeps.offsetToGameSceneNode;
        } else if constexpr (std::is_same_v<Dependency, OffsetToHealth>) {
            return _gameDependencies.entityDeps.offsetToHealth;
        } else if constexpr (std::is_same_v<Dependency, OffsetToTeamNumber>) {
            return _gameDependencies.entityDeps.offsetToTeamNumber;
        } else if constexpr (std::is_same_v<Dependency, OffsetToAbsOrigin>) {
            return _gameDependencies.gameSceneNodeDeps.offsetToAbsOrigin;
        } else if constexpr (std::is_same_v<Dependency, WorldToClipSpaceConverter>) {
            return WorldToClipSpaceConverter{_gameDependencies.worldToProjectionMatrix};
        } else if constexpr (std::is_same_v<Dependency, HudInWorldPanelContainer>) {
            return (featureHelpers.hudInWorldPanelContainer);
        } else if constexpr (std::is_same_v<Dependency, PanelConfigurator>) {
            return _gameDependencies.panelConfigurator();
        } else if constexpr (std::is_same_v<Dependency, PanoramaTransformFactory>) {
            return PanoramaTransformFactory{_gameDependencies.transformTranslate3dVmt, _gameDependencies.transformScale3dVmt};
        } else if constexpr (std::is_same_v<Dependency, FovScale>) {
            return fovScale;
        } else if constexpr (std::is_same_v<Dependency, PlantedC4>) {
            return PlantedC4{*plantedC4, _gameDependencies.plantedC4Deps, globalVars()};
        } else if constexpr (std::is_same_v<Dependency, SoundChannels>) {
            return (*soundChannels);
        } else if constexpr (std::is_same_v<Dependency, FileSystem>) {
            return FileSystem{*fileSystem, _gameDependencies.fileSystemDeps};
        } else if constexpr (std::is_same_v<Dependency, PanoramaLabelFactory>) {
            return PanoramaLabelFactory{_gameDependencies.panoramaLabelDeps.constructor, _gameDependencies.panoramaLabelDeps.size};
        } else if constexpr (std::is_same_v<Dependency, OffsetToPlayerPawnImmunity>) {
            return _gameDependencies.playerPawnDeps.offsetToPlayerPawnImmunity;
        } else if constexpr (std::is_same_v<Dependency, ConVarAccessor>) {
            return ConVarAccessor{*_gameDependencies.conVars, _gameDependencies.conVarDeps, conVarAccessorState};
        } else {
            static_assert(!std::is_same_v<Dependency, Dependency>, "Unknown dependency");
        }
    }

    [[nodiscard]] const GameDependencies& gameDependencies() const noexcept
    {
        return _gameDependencies;
    }

    [[nodiscard]] auto hud() noexcept
    {
        return HudOptional{_gameDependencies.hud, hudCache};
    }

    [[nodiscard]] auto globalVars() noexcept
    {
        return GlobalVarsOptional{_gameDependencies.globalVars, globalVarsCache};
    }

private:
    template <typename Dependency>
    [[nodiscard]] bool hasDependency() const noexcept
    {
        return presentDependencies.has<Dependency>();
    }

    void prepareDependencies(HookDependenciesMask requiredDependencies) noexcept
    {
        const HookDependenciesBuilder builder{requiredDependencies, _gameDependencies};

        presentDependencies |= builder.getLocalPlayerController(&localPlayerController);
        presentDependencies |= builder.getEntityList(&entityList, &highestEntityIndex);
        presentDependencies |= builder.getFovScale(&fovScale);
        presentDependencies |= builder.getPlantedC4(&plantedC4);
        presentDependencies |= builder.getSoundChannels(&soundChannels);
        presentDependencies |= builder.getFileSystem(&fileSystem);
        presentDependencies |= builder.getConVarAccessor();
    }

    GameDependencies& _gameDependencies;
    FeatureHelpers& featureHelpers;

    cs2::CCSPlayerController* localPlayerController;
    const cs2::CConcreteEntityList* entityList;
    cs2::CEntityIndex highestEntityIndex{cs2::kMaxValidEntityIndex};
    float fovScale;
    cs2::CPlantedC4* plantedC4;
    cs2::SoundChannels* soundChannels;
    cs2::CBaseFileSystem* fileSystem;
    ConVarAccessorState conVarAccessorState;
    HookDependenciesMask presentDependencies;
    HudCache hudCache;
    GlobalVarsCache globalVarsCache;
};
